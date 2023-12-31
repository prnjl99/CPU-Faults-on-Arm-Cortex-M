#include <stdint.h>
#include "main.h"


#ifndef COMMON_H
#define COMMON_H

#define PERIPH_BASE           (0x40000000UL) /*!< Peripheral base address */

#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000UL)
#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x08000000UL)

#define RCC_BASE              (AHB1PERIPH_BASE + 0x1000UL)
#define RCC                   ((RCC_TypeDef *) RCC_BASE)
#define RCC_AHB2ENR_GPIOAEN   1UL

#define     __I     volatile const
#define     __O     volatile
#define     __IO    volatile
#define     __OM     volatile
#define     __IM     volatile const
#define     __IOM    volatile

#define BIT_POS(x) (1UL << (x))

typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t ICSCR;
  __IO uint32_t CFGR;
  __IO uint32_t PLLCFGR;
  __IO uint32_t PLLSAI1CFGR;
  __IO uint32_t PLLSAI2CFGR;
  __IO uint32_t CIER;
  __IO uint32_t CIFR;
  __IO uint32_t CICR;
  uint32_t      RESERVED0;
  __IO uint32_t AHB1RSTR;
  __IO uint32_t AHB2RSTR;
  __IO uint32_t AHB3RSTR;
  uint32_t      RESERVED1;
  __IO uint32_t APB1RSTR1;
  __IO uint32_t APB1RSTR2;
  __IO uint32_t APB2RSTR;
  uint32_t      RESERVED2;
  __IO uint32_t AHB1ENR;
  __IO uint32_t AHB2ENR;
  __IO uint32_t AHB3ENR;
  uint32_t      RESERVED3;
  __IO uint32_t APB1ENR1;
  __IO uint32_t APB1ENR2;
  __IO uint32_t APB2ENR;
  uint32_t      RESERVED4;
  __IO uint32_t AHB1SMENR;
  __IO uint32_t AHB2SMENR;
  __IO uint32_t AHB3SMENR;
  uint32_t      RESERVED5;
  __IO uint32_t APB1SMENR1;
  __IO uint32_t APB1SMENR2;
  __IO uint32_t APB2SMENR;
  uint32_t      RESERVED6;
  __IO uint32_t CCIPR;
  uint32_t      RESERVED7;
  __IO uint32_t BDCR;
  __IO uint32_t CSR;
} RCC_TypeDef;

typedef struct __attribute__((packed))
{
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t xpsr;
} stack_frame_t;

typedef enum
{
    BUS_FAULT_PRECISE = 0,
    BUS_FAULT_IMPRECISE,
    BUS_FAULT_IBUSERR,
    MEM_MANAGE_IACCVIOL,
    USAGE_FAULT_INVSTATE,
        USAGE_FAULT_UNDEFSTR,
    USAGE_FAULT_INVPC,
    USAGE_FAULT_DIV_BY_ZERO,
    USAGE_FAULT_UNALIGNED_MEM_ACCESS,
    USAGE_FAULT_NOCP,
    BUS_FAULT_STKERR,
} fault_type_t ;

void _close(void);
void _lseek(void); 
void _read(void);
void _write(void);

extern void *gl_unaligned_buffer;

static void delay(uint32_t ms)
{
    SysTick->CTRL |= (1<<0) | (1<<2) ;
    
    SysTick->LOAD = (SystemCoreClock/1000) - 1  ; 
    
    for (int i=0; i<ms; i++) 
    {
        while(!(SysTick->CTRL & (0x10000))) ;
    }
    
    SysTick->CTRL &= ~(1<<0) ; 
}

 

static void stackdump(stack_frame_t* sp)
{
    delay(100) ; 
    UART0_Tx_s(ESC bRED "\r\nStack Dump: \r\n" ESC_RESET) ; 
    delay(100) ; 
    
    UART0_Tx_s("R0:  ") ;
    UART0_Tx_hex(sp->r0) ;
    UART0_Tx_s("\r\n") ; 
    
    delay(100) ; 
    UART0_Tx_s("R1:  ") ;
    UART0_Tx_hex(sp->r1) ;
    UART0_Tx_s("\r\n") ;
    
    delay(100) ; 
    UART0_Tx_s("R2:  ") ;
    UART0_Tx_hex(sp->r2) ;
    UART0_Tx_s("\r\n") ;
    
    delay(100) ; 
    UART0_Tx_s("R3:  ") ;
    UART0_Tx_hex(sp->r3) ;
    UART0_Tx_s("\r\n") ;
    
    delay(100) ; 
    UART0_Tx_s("R12: ") ;
    UART0_Tx_hex(sp->r12) ;
    UART0_Tx_s("\r\n") ;
    
    delay(100) ; 
    UART0_Tx_s("LR:  ") ;
    UART0_Tx_hex(sp->lr) ;
    UART0_Tx_s("\r\n") ;
    
    delay(100) ; 
    UART0_Tx_s("PC:  ") ;
    UART0_Tx_hex(sp->pc) ;
    UART0_Tx_s("\r\n") ;
    
    delay(100) ; 
    UART0_Tx_s("xPSR:") ;
    UART0_Tx_hex(sp->xpsr) ;
     
}

static void get_fault_source(void)
{
    if(SCB->HFSR    & (1<<30)) 
    {
        if(SCB->CFSR)
        {
            if (SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) 
            {
                delay(100) ; 
                UART0_Tx_s(ESC bRED "\r\nBus Fault: " ESC_RESET) ; 
                
                uint32_t val = (SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) & ~(SCB_CFSR_BFARVALID_Msk)  ;  
                
                switch(val)
                {
                    case SCB_CFSR_IBUSERR_Msk :         
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "IBUSERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_IMPRECISERR_Msk :         
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "IMPRECISERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_PRECISERR_Msk :       
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "PRECISERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_LSPERR_Msk :      
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "LPSERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_STKERR_Msk :      
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "STKERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                
                    case SCB_CFSR_UNSTKERR_Msk :        
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "UNSTKERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    default : break ; 
                
                }
                
            if(SCB->CFSR & SCB_CFSR_BFARVALID_Msk)
                {
                    delay(10); 
                    UART0_Tx_s(ESC bGREEN "BFAR Valid. Faulting Address : "ESC_RESET) ; 
                    UART0_Tx_hex(SCB->BFAR) ;
                }
            }   

            else if (SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk) 
            {
                delay(100) ;
                UART0_Tx_s(ESC bRED "\r\nMemManage Fault: " ESC_RESET) ; 
                
                uint32_t val = (SCB->CFSR & SCB_CFSR_MEMFAULTSR_Msk) & ~(SCB_CFSR_MMARVALID_Msk)  ;  
                
                switch(val)
                {
                    case SCB_CFSR_IACCVIOL_Msk :        
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "IACCVIOL!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_MLSPERR_Msk :         
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "MPSERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_MSTKERR_Msk :         
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "MSTKERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                
                    case SCB_CFSR_MUNSTKERR_Msk :       
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "MUNSTKERR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    default : break ; 
        
                }
                
            if(SCB->CFSR & SCB_CFSR_MMARVALID_Msk)
                {
                    delay(10); 
                    UART0_Tx_s(ESC bGREEN "BFAR Valid. Faulting Address : "ESC_RESET) ; 
                    UART0_Tx_hex(SCB->MMFAR) ;
                }
            }

            else if (SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) 
            {
                UART0_Tx_s(ESC bRED "\r\nUsage Fault: " ESC_RESET) ; 
                delay(100) ; 
                
                uint32_t val = (SCB->CFSR & SCB_CFSR_USGFAULTSR_Msk) ;
                
                switch(val)
                {
                    case SCB_CFSR_DIVBYZERO_Msk :       
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "Div By Zero!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_INVPC_Msk :       
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "INVPC!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_INVSTATE_Msk :        
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "INVSTATE!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                
                    case SCB_CFSR_NOCP_Msk :        
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "NOCP!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                  case SCB_CFSR_UNALIGNED_Msk :         
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "UNALGINED Memory Access!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    case SCB_CFSR_UNDEFINSTR_Msk :      
                    {
                        delay(100) ;
                        UART0_Tx_s(ESC bRED "UNDEFINSTR!\r\n" ESC_RESET) ; 
                        break ; 
                    }
                    
                    default : break ; 
                
                }
            }                           
        }
    }
}

__attribute__ ((naked))void HardFault_Handler(void)
{
    /* Check Faulting SP */ 
        __ASM volatile(                                                          
            "TST lr, #4 \n"
            "ITE EQ \n"
            "MRSEQ r0, MSP\n"
            "MRSNE r0, PSP\n"
            "B Hard_Fault_Handler"
        ) ; 
}   

void Hard_Fault_Handler(stack_frame_t *sp)
{
    /* Stop right here if it's a stacking exception */ 
    if((SCB->CFSR & SCB_CFSR_BUSFAULTSR_Msk) == SCB_CFSR_STKERR_Msk)
    {
        while(1) ; 
    }
    
    delay(100) ; 
    UART0_Tx_s(ESC bCYAN"\r\n\r\nProcessor Fault!!" ESC_RESET) ;
    
    get_fault_source() ; 
    stackdump((stack_frame_t *)sp) ;    

    while(1) ;  
}

void disable_n_access_fpu(void) 
{

  /* Let's disable the FPU */
  __asm volatile(
 
              "ldr r0, =0xE000ED88 \n"
          "mov r1, #0 \n"
          "str r1, [r0] \n"
          "dsb \n"

          /* Try to access FPU */
              "vmov r0, s0 \n"
             );
}

static void set_pending_irq(void) 
{
    
  NVIC->ISER[0] |= (1<<1) ;

  /* Pend an interrupt */ 
  NVIC->ISPR[0] |= (1<<1);
    
}

void bus_fault_stkerr(void) 
{
    
  uint8_t var;
  unsigned long long distance_from_ram_bottom = (uint32_t)&var - (uint32_t)0x20000000;
    
  /* Bloat up the RAM */ 
    
  volatile uint8_t buffer[distance_from_ram_bottom - 8];
    
  for (uint64_t i = 0; i < sizeof(buffer); i++) 
  {
    buffer[i] = i;
  }
  
  set_pending_irq();
}

void enable_qei_interrupts(void)
{
    /* The following sets up the QEI peripheral for the TM4C MCU */ 
    SYSCTL -> RCGCQEI |= (1<<0) ; 
    SYSCTL -> RCGCGPIO |= (1<<3) ;  

    GPIOD -> LOCK = 0x4C4F434B ; 
    GPIOD -> CR |= (1<<7) ; 
    GPIOD -> LOCK = 0x01 ; 
    GPIOD -> DEN |= (1<<7) | (1<<6) ; 
    GPIOD -> AFSEL |= (1<<6) | (1<<7) ; 
    GPIOD -> PCTL |= (1<<25)| (1<<26) | (1<<29) | (1<<30) ; 
        
    
    QEI0->MAXPOS = 0xFFFFFFFF ; 
    QEI0->LOAD = 25000000 ; 
    QEI0->CTL |= (1<<0) | (1<<3) | (1<<5) | (1<<13) ;
    QEI0->INTEN = (1<<1) ; 
    
    /* Enable Interrupt for QEI */
    NVIC_EnableIRQ(QEI0_IRQn) ; 
}


void bus_fault_precise(void)
{
    enter_unpriv_mode() ; 
    
    /* Accessing a "Privileged" register in unprivileged mode fires a bus fault - Joseph Yiu */ 

    NVIC_EnableIRQ(QEI0_IRQn) ;
    
    /** You can uncomment this and comment above 2 lines as well  **/ 
    /* 
    
    volatile uint32_t *a = (volatile uint32_t*)0x10000000 ;
    volatile uint32_t b = 12; 
    
    b = b/(*a) ;
    
    */ 
}

void bus_fault_imprecise(void)
{
     volatile uint64_t *buf = (volatile uint64_t *)0x30000000;
     *buf = 0x123456789ABCDEF ; 
}

void bus_fault_ibuserr(void) 
{
    void (*fptr)(void) = (void(*)(void))0x00040000;     /* Bus Fault :  I-BUS Error */      
    (*fptr)(); 

}

void mem_manage_iaccviol(void)
{
    void (*fptr)(void) = (void(*)(void))0xE0000615;     /* MemManage fault : CPU tried to execute code from a eXecute Never memory region */                                                                     // I-code fetch / Instruction Access violation  
    (*fptr)();

}

void usage_fault_invstate(void) 
{
    void (*fptr)(void) = (void(*)(void))0x00000514; /* usage fault: Invalid state due to T-Bit */ 
    (*fptr)();

}

void usage_fault_undefinstr(void) 
{
    void (*fptr)(void) = (void(*)(void))0x00001001;  /* usage fault: Undefined instruction */
    (*fptr)();
}

void usage_fault_div_by_zero(void) 
{
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk ; 
    volatile uint32_t *a = (volatile uint32_t*)0xE000ED2A ;     /* The Reset value of UFSR(@0xE000ED2A) is 0x00 */ 
    volatile uint32_t b = 12; 

    b = b/(*a) ;  /* Attempt to divide by zero without the compiler knowing */ 

}

void usage_fault_invpc(void) 
{
    enable_qei_interrupts() ; 
}

void usage_fault_unaligned_mem_access(void)
{
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk ; 
    
    uint64_t *buf = gl_unaligned_buffer;
    *buf = 0x123456789ABCDEF;

}

void QEI0_Handler(void)
{
    QEI0->ISC = (1<<1) ; 
    __asm("PUSH {R0}"); 
}


void enter_priv_mode(void)
{
    __asm ("MRS R1, CONTROL  \n"
           "AND R1,R1, #0xFE \n"
           "MSR CONTROL, R1  \n" ) ;
} 


void enter_unpriv_mode(void)
{
    __asm ("MRS R1, CONTROL") ; 
    __asm ("ORR R1,R1, #0x01") ;
    __asm ("MSR CONTROL, R1") ;
}



void trigger_fault (fault_type_t fault_type)
{
    switch(fault_type) 
    {
        case BUS_FAULT_PRECISE: bus_fault_precise(); 
        break ; 
        
        case BUS_FAULT_IMPRECISE: bus_fault_imprecise(); 
        break ; 

        case BUS_FAULT_IBUSERR: bus_fault_ibuserr(); 
        break ; 
        
        case MEM_MANAGE_IACCVIOL: mem_manage_iaccviol() ; 
        break ; 
        
        case USAGE_FAULT_INVSTATE: usage_fault_invstate() ; 
        break ;
    
        case USAGE_FAULT_UNDEFSTR: usage_fault_undefinstr() ; 
        break ;

        case USAGE_FAULT_INVPC: usage_fault_invpc() ; 
        break ;         
        
        case USAGE_FAULT_DIV_BY_ZERO: usage_fault_div_by_zero() ; 
        break ; 
        
        case USAGE_FAULT_UNALIGNED_MEM_ACCESS: usage_fault_unaligned_mem_access() ; 
        break ; 
                        
        case USAGE_FAULT_NOCP: disable_n_access_fpu() ; 
        break ; 
                        
        case BUS_FAULT_STKERR: bus_fault_stkerr() ;  
        break ; 
                        
    }
}

#endif /* COMMON_H */
