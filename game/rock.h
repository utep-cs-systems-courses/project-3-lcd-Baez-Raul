#include <msp430.h>
#include <stdlib.h>
#include "lcdutils.h"
#include "lcddraw.h"

short drawPosR[2] = {0,0}, controlPosR[2] = {1,1};
short colLimitsR[2] = {-2, screenWidth+2}, rowLimitsR[2] = {-2, screenHeight+2};

short maxVelocityR = 3, velocityR[4] = {0,0,0,0};
short directionR = 0;
short onScreen = 0;

void rockMain() {
  short oldColR = controlPosR[0];
  short newColR = oldColR;
  short oldRowR = controlPosR[1];
  short newRowR = oldRowR;

  directionR = 0;
  if (!onScreen) {
    spawnRock();
    onScreen = 1;
  }
  else {
    accelerateRock(&newRowR,&newColR);
    updatePosR(&newRowR,&newColR);
  }
  accelerateRock(&newRowR,&newColR);
  updatePosR(&newRowR,&newColR);
}

void spawnRock() {
  switch (directionR) {
  case 0:
    controlPosR[0] = screenWidth/3 + (rand()%(screenWidth/3));
    controlPosR[1] = 0;
    velocityR[2] = maxVelocityR;
  case 1:
    controlPosR[0] = 0;
    controlPosR[1] = screenHeight/3 + (rand()%(screenHeight/3));
    velocityR[3] = maxVelocityR;
  case 2:
    controlPosR[0] = screenWidth/3 + (rand()%(screenWidth/3));
    controlPosR[1] = screenHeight;
    velocityR[0] = maxVelocityR * -1;
  case 3:
    controlPosR[0] = screenWidth;
    controlPosR[1] = screenHeight/3 + (rand()%(screenHeight/3));
    velocityR[1] = maxVelocityR * -1;
  default:
    controlPosR[0] = controlPosR[0];
    controlPosR[1] = controlPosR[1];
  }
  //drawPosR[0] = controlPosR[0];
  //drawPosR[1] = controlPosR[1];
}

void draw_Rock(int col, int row, unsigned short color) {
  fillRectangle(col-1, row-1, 3, 3, color);
}

void screen_update_Rock() {
  for (char axis = 0; axis < 2; axis ++) 
    if (drawPosR[axis] != controlPosR[axis]) /* position changed? */
      goto redraw;
  return;			/* nothing to do */
 redraw:
  fillRectangle(drawPosR[0]-1,drawPosR[1]-1, 3,3, COLOR_BLACK); /* erase */
  for (char axis = 0; axis < 2; axis ++) 
    drawPosR[axis] = controlPosR[axis];
  draw_Rock(drawPosR[0], drawPosR[1], COLOR_WHITE); /* draw */
}

void updatePosR(short *newRowR, short *newColR) {
  if (*newColR <= colLimitsR[0] || *newColR >= colLimitsR[1]) *newColR = 0;
  if (*newRowR <= rowLimitsR[0] || *newRowR >= rowLimitsR[1]) *newRowR = 0;
  
  controlPosR[0] = *newColR;
  controlPosR[1] = *newRowR;
}

void accelerateRock(short *newRowR, short *newColR) {
  *newRowR += (velocityR[0] + velocityR[2]);
  *newColR += (velocityR[1] + velocityR[3]);
}
