#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"

// WARNING: LCD DISPLAY USES P1.0.  Do not touch!!! 

#define LED BIT6		/* note that bit zero req'd for display */

#define SW1 1
#define SW2 2
#define SW3 4
#define SW4 8

#define SWITCHES 15

char blue = 31, green = 0, red = 31;
unsigned char step = 0;

static char 
switch_update_interrupt_sense()
{
  char p2val = P2IN;
  /* update switch interrupt to detect changes from current buttons */
  P2IES |= (p2val & SWITCHES);	/* if switch up, sense down */
  P2IES &= (p2val | ~SWITCHES);	/* if switch down, sense up */
  return p2val;
}

void 
switch_init()			/* setup switch */
{  
  P2REN |= SWITCHES;		/* enables resistors for switches */
  P2IE |= SWITCHES;		/* enable interrupts from switches */
  P2OUT |= SWITCHES;		/* pull-ups for switches */
  P2DIR &= ~SWITCHES;		/* set switches' bits for input */
  switch_update_interrupt_sense();
}

int switches = 0;

void
switch_interrupt_handler()
{
  char p2val = switch_update_interrupt_sense();
  switches = ~p2val & SWITCHES;
}


// axis zero for col, axis 1 for row

short drawPos[2] = {screenWidth/2,screenHeight/2}, controlPos[2] = {screenWidth/2, screenHeight/2-1};
short velocity = 0, maxVelocity = 6, colLimits[2] = {1, screenWidth-1}, rowLimits[2] = {1, screenHeight-1};
char direction = 'w';
short thrust = 0;

void
draw_ball(int col, int row, int thrust, unsigned short color)
{
  fillRectangle(col-1, row-1, 3, 3, color);
  switch (direction) {
  case 'w':
    drawPixel(col,row-2,color);
    if (thrust) {
      fillRectangle(col-1,row+2, 3,1, COLOR_RED);
      drawPixel(col,row+3,COLOR_RED);
    }
    break;
  case 'a':
    drawPixel(col-2,row,color);
    if (thrust) {
      fillRectangle(col+2,row-1, 1,3, COLOR_RED);
      drawPixel(col+3,row,COLOR_RED);
    }
    break; 
  case 's':
    drawPixel(col,row+2,color);
    if (thrust) {
      fillRectangle(col-1,row-2, 3,1, COLOR_RED);
      drawPixel(col,row-3,COLOR_RED);
    }
    break;
  case 'd':
    drawPixel(col+2,row,color);
    if (thrust) {
      fillRectangle(col-2,row-1, 1,3, COLOR_RED);
      drawPixel(col-3,row,COLOR_RED);
    }
    break;
  default:
    drawPixel(col,row,color);
    break;
  }
}

void
screen_update_ball()
{
  for (char axis = 0; axis < 2; axis ++) 
    if (drawPos[axis] != controlPos[axis]) /* position changed? */
      goto redraw;
  return;			/* nothing to do */
 redraw:
  fillRectangle(drawPos[0]-3,drawPos[1]-3, 7,7, COLOR_BLACK); /* erase */
  for (char axis = 0; axis < 2; axis ++) 
    drawPos[axis] = controlPos[axis];
  draw_ball(drawPos[0], drawPos[1], thrust, COLOR_WHITE); /* draw */
}
  

short redrawScreen = 1;
u_int controlFontColor = COLOR_GREEN;

void wdt_c_handler()
{
  static int secCount = 0;
  short oldCol = controlPos[0];
  short newCol = oldCol;
  short oldRow = controlPos[1];
  short newRow = oldRow;
  secCount ++;
  if (secCount >= 25) {		/* 10/sec */
    /* Moves forward and accelerates */
    if (switches & SW2) {
      accelerate(&newCol,&newRow,&velocity);
      thrust = 1;
    }
    /* Moves forward and decelerates */
    if (!(switches & SW2) && velocity >= 0) {
      accelerate(&newCol,&newRow,&velocity);
      thrust = 0;
    }
    if(switches & SW1) turnLeft();
    if(switches & SW3) turnRight();
    {
      if (step <= 2)
	step ++;
      else {
	step = 0;
	if ((switches & SW2) && velocity <= maxVelocity) velocity++;
	if (!(switches & SW2) && velocity > 0) velocity--;
      }
      secCount = 0;
    }
    redrawScreen = 1;
  }

  if (newCol <= colLimits[0] || newCol >= colLimits[1]) newCol = oldCol;
  if (newRow <= rowLimits[0] || newRow >= rowLimits[1]) newRow = oldRow;
  else {
    controlPos[0] = newCol;
    controlPos[1] = newRow;
  }
}

void accelerate(short *newCol, short *newRow, short *velocity) {
  switch (direction) {
  case 'w':
    *newRow -= *velocity;
    break;
  case 'a':
    *newCol -= *velocity;
    break;
  case 's':
    *newRow += *velocity;
    break;
  case 'd':
    *newCol += *velocity;
    break;
  default:
    velocity = velocity;
    break;
  }
}

void turnLeft() {
  switch (direction) {
  case 'w':
    direction = 'a';
    break;
  case 'a':
    direction = 's';
    break;
  case 's':
    direction = 'd';
    break;
  case 'd':
    direction = 'w';
    break;
  default:
    direction = direction;
    break;
  }
}

void turnRight() {
  switch (direction) {
  case 'w':
    direction = 'd';
    break;
  case 'a':
    direction = 'w';
    break;
  case 's':
    direction = 'a';
    break;
  case 'd':
    direction = 's';
    break;
  default:
    direction = direction;
    break;
  }
}

void update_shape();

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
      screen_update_ball();
    }
    P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    P1OUT |= LED;	/* led on */
  }
}

    
void
update_shape()
{
  screen_update_ball();
}

void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}
