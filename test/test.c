#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "switches.h"
#include "rocket.h"

char blue = 31, green = 0, red = 31;
unsigned char step = 0;

// axis zero for col, axis 1 for row
  

short redrawScreen = 1;
u_int controlFontColor = COLOR_GREEN;

void wdt_c_handler()
{
  static int secCount = 0;
  
  secCount ++;
  if (secCount >= 10) {		/* 10/sec */
    
    thrust = (switches & SW2);	/* Allows draw_Rocket to add engine thrust */
    accelerate(&newCol,&newRow,&velocity);

    if (step <= 1)
      step ++;
    else {
      step = 0;
      if(switches & SW1) turnLeft();
      if(switches & SW3) turnRight();
      
      if (switches & SW2) {
	if (direction <= 1 && (velocity[direction] >= (maxVelocity*-1))) velocity[direction]--;
	if (direction >= 2 && (velocity[direction] <= maxVelocity)) velocity[direction]++;
      }
      decelerate();
    }
    secCount = 0;
    redrawScreen = 1;
  }
  updatePos(&oldCol,&newCol,&oldRow,&newRow);
}

void main()
{
  
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  clearScreen(COLOR_BLACK);
  while (1) {			/* forever */
    if (redrawScreen) {
      redrawScreen = 0;
      // update_shape();
      screen_update_Rocket();
    }
    P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    P1OUT |= LED;	/* led on */
  }
}

    



