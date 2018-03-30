/*
 * main.c
 *
 *  Created on: Mar 7, 2018
 *      Author: zzp
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
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
char channel='0';
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

void UARTIntHandler(void)
{
    char get='0';
    uint32_t ui32Status;
    char words[100]={0};
    int i;
    ui32Status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, ui32Status);
    while(UARTCharsAvail(UART1_BASE))
    {
        if(UARTCharGetNonBlocking(UART1_BASE)=='$')
        {
            i=0;
            get=UARTCharGetNonBlocking(UART1_BASE);
            while(get!='#')
            {
                words[i]=get;
                get=UARTCharGetNonBlocking(UART1_BASE);
                i++;
            }
            words[i+1]='\0';
            UARTprintf("%s",words);
            strcpy(Commands,words);
        }
    }
}

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
    while(ui32Count--)
    {
        UARTCharPutNonBlocking(UART1_BASE, *pui8Buffer++);
        SysCtlDelay(SysCtlClockGet()/300);//make sure UART work smoothly,delay some time
    }
}

void WIFISend(void)
{
    char* strings="AT+CIPSEND=0,12\r\n";
    UARTSend((uint8_t *)(strings),strlen(strings));
    strings="test";
}

int main(void)
{
    char* string="AT\r\n";
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);//40mhz
    FPUEnable();
    FPULazyStackingEnable();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    IntMasterEnable();
    GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE,GPIO_PIN_4|GPIO_PIN_5);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART1);
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
    ConfigureUART0();
    UARTprintf("hello world ! \n");
    string="AT+CIPMUX=1\r\n";
    UARTSend((uint8_t *)(string),strlen(string));
    string="AT+CIPSERVER=1,6666\r\n";
    UARTSend((uint8_t *)(string),strlen(string));
    //WIFISend();
    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
        SysCtlDelay(SysCtlClockGet() / 6);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
        SysCtlDelay(SysCtlClockGet() /6);
        UARTprintf("%s",Commands);
    }
}



