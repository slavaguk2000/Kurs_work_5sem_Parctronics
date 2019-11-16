#define gnd_dat1 4
#define echo_dat1 5
#define trig_dat1 6
#define vcc_dat1 7
#define MS_LENGTH 5

#include <SPI.h>
#include <math.h>
#include "Ucglib.h"
Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ 8, /*cs=*/ 10, /*reset=*/ 9);
const int HEIGHT = 160;
const int WIDTH = 128;
const int LINES_HEIGHT = 30;

enum side{
  LEFT = 0,
  RIGHT = 1,
  FRONT = 2, 
  BACK = 3
};
void setup(void)
{
  pinMode(gnd_dat1, OUTPUT);
  pinMode(echo_dat1, INPUT);
  pinMode(trig_dat1, OUTPUT);
  pinMode(vcc_dat1, OUTPUT);
  digitalWrite(gnd_dat1, LOW);
  digitalWrite(vcc_dat1, HIGH);
  

  
  delay(1000);
  ucg.begin(UCG_FONT_MODE_SOLID);
//  ucg.setRotate0();
  ucg.clearScreen();
  ucg.setFont(ucg_font_ncenR10_tr);
  ucg.setPrintPos(0,25);
  ucg.setColor(255, 255, 255);
  Serial.begin(9600);
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

int oldLength[4] = {0, 0, 0, 0};
bool clearCheck(int length, int index)
{
  bool answer = false;
  for(int i = 100; i > 1; i /= 10)
    if (oldLength[index] >= i && length < i) 
    {
      answer = true;
      break;
    }
  Serial.print(oldLength[index]);
  Serial.print(" ");
  Serial.print(length);
  Serial.println(answer?" true":" false");
  oldLength[index] = length;
  return answer;
}

void printLinesLength(int length, side horizontalSize, side verticalSide)
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
  if (clearCheck(length, horizontalSize*(verticalSide-2)))
  {
    ucg.setColor(0, 0, 0);
    printLength(888, startPointX, startPointY);  
  }
  ucg.setColor(100, 100, 255);
  printLength(length, startPointX, startPointY);  
}

void printRedLine(side hSide)
{
  for(int i = 0; i < 7; i++)
  {
    int y = 20+i*i;
    ucg.drawVLine(hSide==LEFT?i:WIDTH-1-i, y, HEIGHT - 2*y);    
  }
}

void printLeftButtom(int length)
{
  printLinesLength(length, LEFT, BACK);
}
void printLeftFront(int length)
{
  printLinesLength(length, LEFT, FRONT);
}
void printRightButtom(int length)
{
  printLinesLength(length, RIGHT, BACK);
}
void printRightFront(int length)
{
  printLinesLength(length, RIGHT, FRONT);
}

int getDistance(int trig, int echo)
{
  digitalWrite(trig, HIGH); 
  delayMicroseconds(2); 
  digitalWrite(trig, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trig, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trig, LOW); 
  return pulseIn(echo, HIGH)/58;
}
int middle = 0;
int comp (const int *i, const int *j)
{
  return abs(*i - middle) - abs(*j-middle);
}
int getTrueDistance(int trig, int echo)
{
  int ms[MS_LENGTH];
  int sum = 0;
  for (int i = 0; i < MS_LENGTH; i++)
  {
    ms[i] = getDistance(trig, echo);
    sum += ms[i];
  }
  middle = sum / MS_LENGTH;
  qsort(ms, MS_LENGTH, sizeof(int), (int(*) (const void *, const void *)) comp);
  int new_ms_length = (MS_LENGTH * 2) / 3;
  sum = 0;
  for (int i = 0; i < new_ms_length; i++)
      sum += ms[i];
  int resoult = sum /new_ms_length;
  return resoult;  
}

void loop(void)
{ 
  bool flag = false;
  if (i == 9 || i == 99) flag = true; 
  printLeftFront(getTrueDistance(trig_dat1, echo_dat1));
  printRightFront(i/2);
  printLeftButtom(i/4);
  printRightButtom(i/8);
  ucg.setColor(255, 0, 0);
  if (i == 205) printRedLine(LEFT);
  if (i == 195) printRedLine(RIGHT);
  
 
  i--;
  //ucg.setColor(255, 255, i);
  //i++;
  //delay(100);  
  //ucg.setPrintPos(0,25);
  //ucg.drawBox(2,3,50,60);
  //ucg.setPrintPos(0,75);
  //ucg.print((int)i);
  
}
