#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "switches.h"
#include "rocket.h"

char blue = 31, green = 0, red = 31;

// axis zero for col, axis 1 for row

short redrawScreen = 1;
u_int controlFontColor = COLOR_GREEN;
int score = 0;
char string[20];

void wdt_c_handler()
{
  static int secCount = 0;
  secCount++;
  static int step = 0;
  if (secCount >= 10) {		/* 10/sec */
    rocketMain();
    secCount = 0;
    redrawScreen = 1;
    step++;
    if (step >= 25) {
      if (score < 10000) score += 100;
      itoa(score,string,10);
      step = 0;
    }
  }
}

int sound = 15000;
short soundstep = 0;
void main()
{ 
  P1DIR |= LED;		/**< Green led on when CPU on */
  P1OUT |= LED;
  configureClocks();
  lcd_init();
  switch_init();
  buzzer_init();
  buzzer_set_period(0);
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */
  
  clearScreen(COLOR_BLACK);
  while (1) {			/* forever */
    if (redrawScreen) {
      redrawScreen = 0;
      update_shape();
    }
    if (thrust) {
      soundstep++;
      if (soundstep >= 2 && sound > 11000) {
	sound -= 1000;
	soundstep = 0;
      }
      buzzer_set_period(sound);
    }
    else {
      soundstep ++;
      if (soundstep >= 1 && sound < 15000) {
	sound += 1000;
	soundstep = 0;
	buzzer_set_period(sound);
      }
      if (sound == 15000) buzzer_set_period(0);
    }
    P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    P1OUT |= LED;	/* led on */
  }
}

void update_shape();

void
update_shape()
{
  screen_update_Rocket();
  drawString5x7(20,5, "SCORE: ", COLOR_WHITE, COLOR_BLACK);
  drawString5x7(55,5, string, COLOR_WHITE,COLOR_BLACK);
}
