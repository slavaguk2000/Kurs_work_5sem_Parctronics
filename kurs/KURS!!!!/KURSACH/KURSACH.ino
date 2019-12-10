#define MS_LENGTH 5
#define SENSORS_COUNT 6
#define DEBUG_DEL 5

#include <SPI.h>
#include <math.h>
#include "Ucglib.h"

const int CD_SCREEN_PIN = 8, CS_SCREEN_PIN = 10, RESET_SCREEN_PIN = 9,
              ECHO_PIN[6] = {A6, A4, A0, 5, 3, 6}, TRIG_PIN[6] = {A5, A2, A1, 4, 2, 7},
              SOUND_PIN = 12;

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
void printLinesLength(int length, side horizontalSize, side verticalSide)
{
  int startPointX = WIDTH/4;
  if (horizontalSize == RIGHT) startPointX *= 3;
  int startPointX_ = startPointX;
  int startPointY = verticalSide == FRONT?50:125;
  int startPointY_ = startPointY;  
  startPointX -= 5;
  if (length < 0) return;
  ucg.setColor(100, 100, 255);
  ucg.setFont(ucg_font_ncenR14_tr);  
  if(length > 200)
  {
    length = 200;
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
  if(length == 200) ucg.setColor(255, 255, 255);
  else ucg.setColor(0, 0, 0);
  ucg.setPrintPos(startPointX_ - 25, startPointY_ - 2);
  ucg.print('>');
}

void printFlangLine(side hSide, int distance)
{
 ucg.setColor(distance<100 && distance > 0?255:0, 0, 0); 
  for(int i = 0; i < 7; i++)
  {
    int y = 20+i*i;
    ucg.drawVLine(hSide==LEFT?i:WIDTH-1-i, y, HEIGHT - 2*y);    
  }
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
  return (pulseIn(echo, HIGH)/(58.0/DEBUG_DEL));
}
int middle = 0;
int comp (const int *i, const int *j)
{
  return abs(*i - middle) - abs(*j-middle);
}
int counterBips = 0;
int timeBetweenBips;
void bip()
{
  if (!counterBips)
  {
    tone(SOUND_PIN, 400);    
    counterBips = timeBetweenBips;
  }
  counterBips--;
    
}
void loop(void)
{ 
  tone(SOUND_PIN, 400, 2000); 
  int trueDistance[SENSORS_COUNT] = {};
  while(1)
  {
    int minDistance = 0;
    for (int i = 1; i < SENSORS_COUNT; i++)
    {
      bip();
      trueDistance[i] = getDistance(TRIG_PIN[i], ECHO_PIN[i]);
      if (i != 0 && i != 5 && trueDistance[i] < minDistance) minDistance = trueDistance[i];
    }
    timeBetweenBips = minDistance/10;
    printFlangLine(LEFT, trueDistance[0]);
    printLinesLength(trueDistance[1], LEFT, FRONT);
    printLinesLength(trueDistance[2], LEFT, BACK);
    printLinesLength(trueDistance[3], RIGHT, BACK);
    printLinesLength(trueDistance[4], RIGHT, FRONT);
    printFlangLine(RIGHT, trueDistance[5]);
  }
}





//
//int getTrueDistance(int* trueDistance)
//{
//  int ms[SENSORS_COUNT][MS_LENGTH];
//  for (int i = 0; i < MS_LENGTH; i++)
//  {
//    for (int j = 0; j < SENSORS_COUNT; j++)
//    {
//      ms[j][i] = getDistance(TRIG_PIN[j], ECHO_PIN[j]);
//    }
//  }
//  int new_ms_length = (MS_LENGTH * 2) / 3;
//  for (int i = 0; i < SENSORS_COUNT; i++)
//  {
//    qsort(ms, MS_LENGTH, sizeof(int), (int(*) (const void *, const void *)) comp);
//    int sum = 0;
//    for (int j = 0; j < new_ms_length; j++)
//      sum += ms[i][j];
//    trueDistance[i] = sum /new_ms_length;
//    Serial.print("Sensor ");
//    Serial.print(i);
//    Serial.print(": ");
//    Serial.println(trueDistance[i]);
//  }
//}
