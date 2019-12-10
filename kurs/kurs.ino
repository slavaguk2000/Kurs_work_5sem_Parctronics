#define MS_LENGTH 5
#define SENSORS_COUNT 6

#include <SPI.h>
#include <math.h>
#include "Ucglib.h"

const int CD_SCREEN_PIN = 8, CS_SCREEN_PIN = 10, RESET_SCREEN_PIN = 9,
              ECHO_PIN[6] = {A6, A4, A0, 5, 3, 6}, TRIG_PIN[6] = {A5, A2, A1, 4, 2, 7};

Ucglib_ST7735_18x128x160_HWSPI ucg(/*cd=*/ CD_SCREEN_PIN, /*cs=*/ CS_SCREEN_PIN , /*reset=*/ RESET_SCREEN_PIN);
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
  ucg.begin(UCG_FONT_MODE_SOLID);
  ucg.clearScreen();
  ucg.setFont(ucg_font_ncenR10_tr);
  ucg.setPrintPos(0,25);
  ucg.setColor(255, 255, 255);
  for (int i = 0; i < SENSORS_COUNT; i++)
  {
    pinMode(ECHO_PIN[i], INPUT);
    pinMode(TRIG_PIN[i], OUTPUT);
  }
  Serial.begin(9600);
}
int i = 210;
uint16_t w = 0;
uint16_t v = 0;
//
void printLength(int length, int pos, int startPointY)
{
  if (length > 9) pos -= 5;
  if (length > 99) pos -= 5;
  ucg.setColor(0, 0, 0);  
  ucg.drawBox(pos-10, startPointY-20, 40, 20);
  ucg.setColor(255, 255, 255);  
  ucg.setPrintPos(pos, startPointY);
  ucg.print(length);
}
//
//int oldLength[4] = {0, 0, 0, 0};

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
  printLength(length, startPointX, startPointY);  
}

//void printRedLine(side hSide)
//{
//  for(int i = 0; i < 7; i++)
//  {
//    int y = 20+i*i;
//    ucg.drawVLine(hSide==LEFT?i:WIDTH-1-i, y, HEIGHT - 2*y);    
//  }
//}

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

int getTrueDistance(int* trueDistance)
{
  int ms[SENSORS_COUNT][MS_LENGTH];
  for (int i = 0; i < MS_LENGTH; i++)
  {
    for (int j = 0; j < SENSORS_COUNT; j++)
    {
      ms[j][i] = getDistance(TRIG_PIN[j], ECHO_PIN[j]);
    }
  }
  int new_ms_length = (MS_LENGTH * 2) / 3;
  for (int i = 0; i < SENSORS_COUNT; i++)
  {
    qsort(ms, MS_LENGTH, sizeof(int), (int(*) (const void *, const void *)) comp);
    int sum = 0;
    for (int j = 0; j < new_ms_length; j++)
      sum += ms[i][j];
    trueDistance[i] = sum /new_ms_length;
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(trueDistance[i]);
  }
}

void printLines(int* trueDistance)
{
  printLinesLength(trueDistance[0], LEFT, FRONT);
  printLinesLength(trueDistance[5], RIGHT, FRONT);
  printLinesLength(trueDistance[2], LEFT, BACK);
  printLinesLength(trueDistance[3], RIGHT, BACK);  
}

void loop(void)
{ 
  int trueDistance[SENSORS_COUNT];
  printLength(getDistance(TRIG_PIN[0], ECHO_PIN[0]), 20, 50);
  printLength(getDistance(TRIG_PIN[1], ECHO_PIN[1]), 20, 100);
  printLength(getDistance(TRIG_PIN[2], ECHO_PIN[2]), 20, 150);
  printLength(getDistance(TRIG_PIN[3], ECHO_PIN[3]), 100, 150);
  printLength(getDistance(TRIG_PIN[4], ECHO_PIN[4]), 100, 100);
  printLength(getDistance(TRIG_PIN[5], ECHO_PIN[5]), 100, 50);
  Serial.println(getDistance(7,6));
  printLength(getDistance(7,6), 80, 50);
//  while(1)
//  {
//    bool flag = false;
//    getTrueDistance(trueDistance);
//    printLines(trueDistance);
//    ucg.setColor(255, 0, 0);
//    if (trueDistance[1] < 100) printRedLine(LEFT);
//    if (trueDistance[4] < 100) printRedLine(RIGHT);
//  }
}
