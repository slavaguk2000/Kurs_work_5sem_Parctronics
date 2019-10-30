#include <SPI.h>
#include "Ucglib.h"
Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 8, /*cs=*/ 10, /*reset=*/ 9);
const int HEIGHT = 160;
const int WIDTH = 128;
const int LINES_HEIGHT = 30;

enum side{
  LEFT,
  RIGHT,
  FRONT, 
  BACK
};
void setup(void)
{
  delay(1000);
  ucg.begin(UCG_FONT_MODE_SOLID);
//  ucg.setRotate0();
  ucg.clearScreen();
  ucg.setFont(ucg_font_ncenR10_tr);
  ucg.setPrintPos(0,25);
  ucg.setColor(255, 255, 255);
}
int i = 210;
uint16_t w = 0;
uint16_t v = 0;

void printLength(int length, int pos, int startPointY)
{
  if (length > 9) pos -= 5;
  if (length > 99) pos -= 5;
  ucg.setPrintPos(pos, startPointY);
  ucg.print(length);
}

void printLinesLength(int length, side horizontalSize, side verticalSide, bool clear)
{
  int startPointX = WIDTH/4;
  if (horizontalSize == RIGHT) startPointX *= 3;
  int startPointY = verticalSide == FRONT?50:125;
  startPointX -= 5;
  if (length < 0) return;
  ucg.setColor(100, 100, 255);
  ucg.setFont(ucg_font_ncenR14_tr);  
  if(length > 200)
  {
    ucg.setPrintPos(startPointX-20, startPointY);
    ucg.print('>');
    length = 200;
  }
  else
  {
    ucg.setColor(0, 0, 0);
    ucg.setPrintPos(startPointX-20, startPointY);
    ucg.print('>');
  }
    
  for (int i = 0; i < LINES_HEIGHT; i++)
  {    
    int startLineX = LEFT == horizontalSize? i+1:WIDTH /2+1;
    switch(i)
    {
      case 0:
        ucg.setColor(0, 255, 0);
        break;
      case 10:
        ucg.setColor(255, 255, 0);
        break;
      case 20: 
        ucg.setColor(255, 0, 0);
        break;
      default:
        if (i > LINES_HEIGHT - sqrt(length)*2) ucg.setColor(0, 0, 0);
        ucg.drawHLine(startLineX, verticalSide == BACK?HEIGHT-1-i:i, WIDTH /2 - 2 - i);    
        break;
    }   
  }
  if (clear)
  {
    ucg.setColor(0, 0, 0);
    printLength(888, startPointX, startPointY);  
  }
  ucg.setColor(100, 100, 255);
  printLength(length, startPointX, startPointY);  
}

void printLeftButtom(int length, bool clear)
{
  printLinesLength(length, LEFT, BACK, clear);
}
void printLeftFront(int length, bool clear)
{
  printLinesLength(length, LEFT, FRONT, clear);
}
void printRightButtom(int length, bool clear)
{
  printLinesLength(length, RIGHT, BACK, clear);
}
void printRightFront(int length, bool clear)
{
  printLinesLength(length, RIGHT, FRONT, clear);
}
void loop(void)
{ 
  bool flag = false;
  if (i == 9 || i == 99) flag = true; 
  printLeftFront(i, flag);
  printRightFront(i/2, flag);
  printLeftButtom(i/4, flag);
  printRightButtom(i/8, flag);
 
  i--;
  //ucg.setColor(255, 255, i);
  //i++;
  //delay(100);  
  //ucg.setPrintPos(0,25);
  //ucg.drawBox(2,3,50,60);
  //ucg.setPrintPos(0,75);
  //ucg.print((int)i);
  
}
