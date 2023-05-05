#include <msp430.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"

short drawPos[2] = {screenWidth/2,screenHeight/2}, controlPos[2] = {screenWidth/2, screenHeight/2-1};
short colLimits[2] = {-2, screenWidth+2}, rowLimits[2] = {-2, screenHeight+2};

short maxVelocity = 3, velocity[4] = {0,0,0,0};
short direction = 0;
short thrust = 0;

unsigned char step = 0;
char left = 0;
char right = 0;

void rocketMain(){
  short oldCol = controlPos[0];
  short newCol = oldCol;
  short oldRow = controlPos[1];
  short newRow = oldRow;
  thrust = (switches & SW2);	/* Allows draw_Rocket to add engine thrust */
  
  accelerate(&newRow, &newCol);
  if (step <= 1) step ++;
  else {
    step = 0;
    if(switches & SW1) turnLeftS();
    if(switches & SW3) turnRightS();
    
    if (switches & SW2) {
      if (direction <= 1 && (velocity[direction] >= (maxVelocity*-1))) velocity[direction]--;
      if (direction >= 2 && (velocity[direction] <= maxVelocity)) velocity[direction]++;
    }
    decelerate();
  }
  updatePos(&newRow,&newCol);
}

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

/* Checks if new Pos is within limits, teleports to opposite edge if not */
void updatePos(short *newRow, short *newCol) {
  if (*newCol <= colLimits[0]) *newCol = colLimits[1]-1;
  else if (*newCol >= colLimits[1]) *newCol = colLimits[0]+1;
  
  if (*newRow <= rowLimits[0]) *newRow = rowLimits[1]-1;
  else if (*newRow >= rowLimits[1]) *newRow = rowLimits[0]+1;
  
  controlPos[0] = *newCol;
  controlPos[1] = *newRow;
}

/* Updates x,y according to velocity vectors */
void accelerate(short *newRow, short *newCol) {
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

void turnLeftS();

void turnRightS();
