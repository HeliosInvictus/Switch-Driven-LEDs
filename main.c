#include "functions.h"

#define GPIO_HBCTL *((unsigned int*)0x400FE06CU)
#define RCGCPIO *((unsigned int *)0x400FE608U)

#define SYSCTL 0x400FE000U //system base
#define GPIO_AHB_F 0x4005D000U // just the base

//#define GPIODATA_AHB_F ((volatile unsigned int *)(GPIO_AHB_F)) //I want to see if I can use array for the same thing.

#define GPIO_AHB_DEN_F (*((unsigned int *)(GPIO_AHB_F + 0x51CU))) //set GPIO_AHB_PORTF to turn on
#define GPIO_AHB_DIR_F (*((unsigned int *)(GPIO_AHB_F + 0x400U))) //Turn on GPIO_AHB_F 
#define GPIODATA_AHB_F ((*(volatile unsigned int *)(GPIO_AHB_F+0x3FCU))) //just easier to write it like this to add new things
#define GPIO_AHB_LOCK_F  (*((unsigned int *)(GPIO_AHB_F + 0x520U)))//This is GPIO AHB F lock which turns pn GPIOCR or "committ register"
#define GPIO_AHB_CR_F (*((unsigned int *)(GPIO_AHB_F + 0x524))) //Allows me to use switch PF0
#define GPIO_AHB_PUR_F (*((unsigned int *)(GPIO_AHB_F + 0x510U))) //Allows me to use a Pullup register

#define GPIO_RCGC_TIMER (*((volatile unsigned int *)(SYSCTL + 0x604U)))  //604(Timer offset)
//#define GPIO_RCGCW_TIMER (*((volatile unsigned int *)(SYSCTL + 0x65CU)))  //65C
#define TIMER0_CTL (*((unsigned int *)0x4003000CU)) //starts the counter
#define TIMER0_GPTMRIS (*((unsigned int *)0x4003001CU)) //raw interrupt status
#define TIMER0_CFG (*((unsigned int *)0x40030000U)) //configure timer 0
#define TIMER0_TAMR (*((unsigned int *)0x40030004U))
#define TIMER0_TAILR (*((unsigned int *)0x40030028U))//Timer A Interval Load register, this device has a clock rate of 16MhZ 
#define TIMER0_ICR (*((unsigned int *)0x40030024U)) //Interrupt CLear
#define TIMER0_IMR (*((unsigned int *)0x40030018U))



#define Red (1U << 1)// 0x010 
#define Blue (1U << 2) // 0x0100
#define Green (1U << 3) // 0x01000
#define Off 0x00U


int main()
{
   
   GPIO_HBCTL |= (1 << 5);
   RCGCPIO = (1 << 5);
   
   GPIO_AHB_LOCK_F  = 0x4C4F434B; //Unlock switch on board
   GPIO_AHB_CR_F = 0xff;  //commit to it
   
   GPIO_RCGC_TIMER |= (1U << 0); 
   TIMER0_CTL &= ~(1U << 0); //reset timer 0
   TIMER0_CFG |= 0x00000000U;  
   TIMER0_TAMR |= (0x2 << 0); // set the timer to be a "periodic timer"
   TIMER0_TAMR &= ~(1 << 4); //set the timer to count down
   TIMER0_TAILR = 0x00F42400U; //0xF42400U


   GPIO_AHB_DEN_F |= 0x1F; //b'0001 1111 
   GPIO_AHB_DIR_F = Blue|Red|Green; // goal:01110
   GPIO_AHB_PUR_F = 0x11;




    while(1)
    {
      switch(GPIODATA_AHB_F & 0x11)
      {
        
      case 0x01: 
      TIMER0_CTL = (1U << 0);
      
        if((TIMER0_GPTMRIS & 0x01U) == 1U) //1 second till green
        {          
              if(GPIODATA_AHB_F == 0x01|Off)
              {
                GPIODATA_AHB_F ^= Green;
              }
              else if(GPIODATA_AHB_F == (0x01|(Green)))
              {
                 GPIODATA_AHB_F &= ~(Green);
                 GPIODATA_AHB_F ^= Red;
                 
              }
              else if(GPIODATA_AHB_F == (0x01|(Red)))
              {
                 GPIODATA_AHB_F &= ~(Red);
                 GPIODATA_AHB_F ^= Blue;                 
              }
              else if(GPIODATA_AHB_F == (0x01|(Blue)))
              {
                 GPIODATA_AHB_F &= ~(Blue);
                 GPIODATA_AHB_F ^= Green;                 
              }
              TIMER0_ICR = (1 << 0); //reset interval load register

        }  
    break;
    
       case 0x10: 
      TIMER0_CTL = (1U << 0);
      
        if((TIMER0_GPTMRIS & 0x00000001U) == 1U) //1 second till green
        {          
              if(GPIODATA_AHB_F == 0x10|Off)
              {
                GPIODATA_AHB_F ^= Red;
                GPIODATA_AHB_F ^= Blue;                
              }
              else if(GPIODATA_AHB_F == (0x10|(Red)|(Blue)))
              {
                 GPIODATA_AHB_F &= ~(Red);
                 GPIODATA_AHB_F |= (Green);
                 
              }
              else if(GPIODATA_AHB_F == (0x10|(Green)|(Blue)))
              {
                 GPIODATA_AHB_F &= ~(Blue);
                 GPIODATA_AHB_F ^= Red;                 
              }
              else if(GPIODATA_AHB_F == (0x10|(Green)|(Red)))
              {
                 GPIODATA_AHB_F &= ~(Green); 
                 GPIODATA_AHB_F ^= (Blue);
                 
              }
              TIMER0_ICR = (1 << 0); //reset interval load register
        }  
    break;
    
    case 0x00:
        GPIODATA_AHB_F |= (Red|Green|Blue);
  
    break;
    
    default:
        GPIODATA_AHB_F = Off;
    break;
    

        }
        

    }
      
  return 0;
}



