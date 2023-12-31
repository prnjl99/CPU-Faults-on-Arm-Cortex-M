#include "startup.h"

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

int x,y,z;

int __svc(0x00) compute_Add (int a, int b); /* __svc is a compiler keyword that replaces a function call with an SVC instruction containing the specified number */
int __svc(0x01) compute_sub (int a, int b);
int __svc(0x02) compute_mult (int a, int b);
int __svc(0x03) compute_div (int a, int b);
void SVC_Handler_C(unsigned int *svc_args);

int main(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){};

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    /* Pass arguments using SVC */
    x= 1 ;
    y= 5 ;
    z= compute_Add(x,y);

    x = 9 ;
    y= 2 ;
    z = compute_sub(x,y);

    x=3;
    y=4 ;
    z = compute_mult(x,y);

    x=28;
    y=4 ;
    z = compute_div(x,y);
}

void User_intr_handler(void)
{
    while(1){};
}

__asm void SVC_Handler(void)
{
    TST LR,#4 /* check bit 2 */

    ITE EQ
    MRSEQ R0,MSP                /* if bit 2 is 0 */
    MRSNE R0,PSP               /* if bit 2 is not 0 */
    B __cpp(SVC_Handler_C)

}

void SVC_Handler_C(unsigned int *svc_args)
{
    unsigned int svc_num ;
    /* extract SVC number from PC */
    svc_num = ((char*)svc_args[6])[-2] ;
    switch(svc_num)
    {

        /* AAPCS : Func parameters are stored in R0-R3 & retval in R0, args[0] -> R0 (of caller) & args[1] -> R1 (of caller)  */

        case 0 :
            svc_args[0] = svc_args[1] + svc_args[0] ;
            break ;
        case 1 :
            svc_args[0] = svc_args[0] - svc_args[1] ;
            break ;
        case 2 :
            svc_args[0] = svc_args[1] * svc_args[0] ;
            break ;
        case 3 :
            svc_args[0] = svc_args[0] / svc_args[1] ;
            break ;
        default :
            break ;
    }
}
