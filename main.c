/* 
For this project I've programmer a PUR(pull up resistor) on my 
TM4C123GHPM ARM microcontroller. It is programmed to to turn the
red LED on if the right PUR is pressed and blue if the left is pressed.
If Both are pressed it should produce a purple color, if neither are pressed
both LEDs should be turned off.
*/



#include "functions.h"

#define GPIO_HBCTL *((unsigned int*)0x400FE06CU)
#define RCGCPIO *((unsigned int *)0x400FE608U)

#define GPIO_AHB_F 0x4005D000U // just the base

#define GPIO_AHB_DEN_F (*((unsigned int *)(GPIO_AHB_F + 0x51CU))) //set GPIO_AHB_PORTF to turn on
#define GPIO_AHB_DIR_F (*((unsigned int *)(GPIO_AHB_F + 0x400U))) //Turn on GPIO_AHB_F 
#define GPIODATA_AHB_F ((*(volatile unsigned int *)(GPIO_AHB_F+0x3FCU))) //just easier to write it like this to add new things
#define GPIO_AHB_LOCK_F  (*((unsigned int *)(GPIO_AHB_F + 0x520U)))//This is GPIO AHB F lock which turns pn GPIOCR or "committ register"
#define GPIO_AHB_CR_F (*((unsigned int *)(GPIO_AHB_F + 0x524))) //Allows me to use switch PF0
#define GPIO_AHB_PUR_F (*((unsigned int *)(GPIO_AHB_F + 0x510U))) //Allows me to use a Pullup register


#define Red (1U << 1)// 0x010
#define Blue (1U << 2) // 0x0100
#define Green (1U << 3) // 0x01000
#define Off 0x00U
 

int main()
{
   GPIO_HBCTL = 1 << 5;
   RCGCPIO = 1 << 5;
   GPIO_AHB_LOCK_F  = 0x4C4F434B;
   GPIO_AHB_CR_F = 0xff; 
   

   GPIO_AHB_DEN_F |= 0x1F; 
   GPIO_AHB_DIR_F = Blue|Red; 
   GPIO_AHB_PUR_F = 0x11;
   

   unsigned volatile num;

   
   while(1)
   {

      switch(GPIODATA_AHB_F & 0x11) 
      {
          case 0x01:// if the right switch is press I emit Red
              GPIODATA_AHB_F = Red; 
              break;

          case 0x10://if the right switch is press I emit Red
              GPIODATA_AHB_F = Blue; 
              break;              
   
           case 0x00:
              GPIODATA_AHB_F = Red|Blue;// as both of the PURs are pressed I decided to make purple or the combination of blue and red.
              break;
                
           case 0x11:
              GPIODATA_AHB_F = Off; //incase neither PUR are pressed it defaults with no lights
              break;
            
      }
    }

   

  return 0;
}


