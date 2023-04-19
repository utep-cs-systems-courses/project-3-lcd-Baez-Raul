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
short maxVelocity = 3, velocity[4] = {0,0,0,0};
short colLimits[2] = {-2, screenWidth+2}, rowLimits[2] = {-2, screenHeight+2};
short direction = 0;
short thrust = 0;

void
draw_Rocket(int col, int row, int thrust, unsigned short color)
{
  fillRectangle(col-1, row-1, 3, 3, color);
  switch (direction) {
  case 0:
    drawPixel(col,row-2,color);
    if (thrust) {
      fillRectangle(col-1,row+2, 3,1, COLOR_RED);
      drawPixel(col,row+3,COLOR_RED);
    }
    break;
  case 1:
    drawPixel(col-2,row,color);
    if (thrust) {
      fillRectangle(col+2,row-1, 1,3, COLOR_RED);
      drawPixel(col+3,row,COLOR_RED);
    }
    break; 
  case 2:
    drawPixel(col,row+2,color);
    if (thrust) {
      fillRectangle(col-1,row-2, 3,1, COLOR_RED);
      drawPixel(col,row-3,COLOR_RED);
    }
    break;
  case 3:
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
screen_update_Rocket()
{
  for (char axis = 0; axis < 2; axis ++) 
    if (drawPos[axis] != controlPos[axis]) /* position changed? */
      goto redraw;
  return;			/* nothing to do */
 redraw:
  fillRectangle(drawPos[0]-3,drawPos[1]-3, 7,7, COLOR_BLACK); /* erase */
  for (char axis = 0; axis < 2; axis ++) 
    drawPos[axis] = controlPos[axis];
  draw_Rocket(drawPos[0], drawPos[1], thrust, COLOR_WHITE); /* draw */
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

/* Checks if new Pos is within limits, teleports to opposite edge if not */
void updatePos(short *oldCol, short *newCol, short *oldRow, short *newRow) {
  if (*newCol <= colLimits[0]) *newCol = colLimits[1]-1;
  else if (*newCol >= colLimits[1]) *newCol = colLimits[0]+1;
  
  if (*newRow <= rowLimits[0]) *newRow = rowLimits[1]-1;
  else if (*newRow >= rowLimits[1]) *newRow = rowLimits[0]+1;
  
  controlPos[0] = *newCol;
  controlPos[1] = *newRow;
}

/* Updates x,y according to velocity vectors */
void accelerate(short *newCol, short *newRow, short *velocity) {
  *newRow += (velocity[0] + velocity[2]);
  *newCol += (velocity[1] + velocity[3]);
}

/* Reduces velocity in direction which the rocket is not currently accelerating */
void decelerate() {
  if (!(switches & SW2) && velocity[direction] != 0) {
    if (direction <= 1) velocity[direction]++;
    else velocity[direction]--;
  }
  if (direction != 0 && velocity[0] != 0) velocity[0]++;
  if (direction != 1 && velocity[1] != 0) velocity[1]++;
  if (direction != 2 && velocity[2] != 0) velocity[2]--;
  if (direction != 3 && velocity[3] != 0) velocity[3]--;
}

void turnLeft() {
  if (direction >= 3) direction = 0;
  else direction++;
}

void turnRight() {
  if (direction <= 0) direction = 3;
  else direction--;
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
      screen_update_Rocket();
    }
    P1OUT &= ~LED;	/* led off */
    or_sr(0x10);	/**< CPU OFF */
    P1OUT |= LED;	/* led on */
  }
}

    
void
update_shape()
{
  screen_update_Rocket();
}

void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}
