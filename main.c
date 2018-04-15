/*
 * main.c
 *
 *  Created on: Mar 7, 2018
 *      Author: zzp
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
char Commands[100]={0};
void ConfigureUART0(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE,GPIO_PIN_0|GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE,UART_CLOCK_PIOSC);
    UARTStdioConfig(0,115200,16000000);
}

void ConfigureUART1(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE,GPIO_PIN_4|GPIO_PIN_5);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
}

void ConfigureUART3(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinConfigure(GPIO_PC7_U3TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE,GPIO_PIN_6|GPIO_PIN_7);
    UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART3);
    UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);
}

void UART1IntHandler(void)
{
    uint32_t ui32Status;
    char words[100]={0};
    int i=0;
    ui32Status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, ui32Status);
    while(UARTCharsAvail(UART1_BASE)&&i<100)
    {
        words[i]=UARTCharGetNonBlocking(UART1_BASE);
        i++;
    }
    words[i+1]='\0';
    strcpy(Commands,words);
}

void UART3IntHandler(void)
{
    uint32_t ui32Status;
    char words[100]={0};
    int i=0;
    ui32Status = UARTIntStatus(UART3_BASE, true);
    UARTIntClear(UART3_BASE, ui32Status);
    while(UARTCharsAvail(UART3_BASE)&&i<100)
    {
        words[i]=UARTCharGetNonBlocking(UART3_BASE);
        i++;
    }
    words[i+1]='\0';
}

void UART1Send(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    while(ui32Count--)
    {
        UARTCharPutNonBlocking(UART1_BASE, *pui8Buffer++);
    }
}

void UART3Send(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    char *stringend="\xff\xff\xff";
    uint32_t count;
    count=strlen(stringend);
    while(ui32Count--)
    {
        UARTCharPutNonBlocking(UART3_BASE, *pui8Buffer++);
    }
    while(count--)
    {
        UARTCharPutNonBlocking(UART3_BASE, *stringend++);
    }
}

int main(void)
{volatile int i;
    char* string="zzpw2\r\n";
    char* string1="page 0";
    volatile char a[7]="page 0";
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);//40mhz
    FPUEnable();
    FPULazyStackingEnable();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_1);
    IntMasterEnable();
    ConfigureUART1();
    ConfigureUART3();
    ConfigureUART0();
    UARTprintf("hello world ! \n");
    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        SysCtlDelay(SysCtlClockGet() / 6);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
        SysCtlDelay(SysCtlClockGet() /6);
        UART1Send((uint8_t *)(string),strlen(string));
        UART3Send((uint8_t *)(string1),strlen(string1));
        //UARTprintf("%s",Commands);
    }
}



