/**
 * @file BikeComputer.ino
 * 
 * Written for Arduino Nano board
 * 
 * $Author: Gergely Teszári
 * $Date: 2022.08.21.
 * $Revision: 3.0
 * 
 * @copyright Gergely Teszári
 * 
*/

/**** INCLUDES ***********************************************************************************/

#include <LiquidCrystal.h>
#include "PinChangeInterrupt.h"

/**** END OF INCLUDES ****************************************************************************/

/**** MACROS *************************************************************************************/

/* LCD initialization 
 *  Pay close attention to the signal pins of the LCD
 *  For my PCB layout it is (rs 10), (en 8), (d4 3), (d5 2), (d6 5), (d7 4)
 *  Also pay close attention NOT to use one of the LCD signal ports as 
 *  the input port from the rotation detecter sensor
*/
#include <LiquidCrystal.h>
#define rs (10)
#define en (8)
#define d4 (3)
#define d5 (2)
#define d6 (5)
#define d7 (4)
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

/* Port definitions */
#define sensorPin (12)
#define blinker (13)
#define isMetricUnit (determineMetricUnit()) 

#define button1 A0
#define button2 A1
#define button3 A4
#define button4 A5

/**** END OF MACROS ******************************************************************************/

/**** TYPE DEFINITIONS ***************************************************************************/

/**** END OF TYPE DEFINITIONS ********************************************************************/

/**** VARIABLES **********************************************************************************/

float wheelCircumference = 2.04; /* defined in meters, overwritten if not metric unit system */

/* SpeedoMeter variables */
bool prevState = HIGH;
float currentSpeed;
int timer;
float longTimer;
float odo;
float currentCorrectedSpeed;
float longAvg;
int avgDivider = 2;
int outPrintRequest;

/**** END OF VARIABLES ***************************************************************************/

/**** LOCAL FUNCTION DECLARATIONS ****************************************************************/

boolean determineMetricUnit();
void setup();
void loop();
void printLCD();
void button1func();
void button2func();
void button3func();
void button4func();

/**** END OF LOCAL FUNCTION DECLARATIONS *********************************************************/

/**** LOCAL FUNCTION DEFINITIONS *****************************************************************/

boolean determineMetricUnit() /* TODO */
{
  return true;
}

void setup()
{
  lcd.begin(16,2);
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin,LOW);
  pinMode(blinker, OUTPUT);
  digitalWrite(blinker,LOW);
  if (false == isMetricUnit)
  {
    wheelCircumference *= 0.3048;
  }
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Welcome !");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Distance unit:");
  lcd.setCursor(0,1);
  if (isMetricUnit)
  {
    lcd.print("m, Km/h");
  }
  else
  {
    lcd.print("ft, mi/h ");
  }
  delay(2000);
  
  pinMode(button1, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(button1), button1func, FALLING);
  pinMode(button2, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(button2), button2func, FALLING);
  pinMode(button3, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(button3), button3func, FALLING);
  pinMode(button4, INPUT_PULLUP);
  attachPCINT(digitalPinToPCINT(button4), button4func, FALLING);
}

void loop()
{
  if (digitalRead(sensorPin) == LOW && prevState == HIGH)
  { /* Magnet triggered sensor */
      prevState = LOW;
      digitalWrite(blinker, HIGH);
  }
      
  else if (digitalRead(sensorPin) == HIGH && prevState == LOW)
  { /* Magnet left the sensor */
    prevState = HIGH;
    digitalWrite(blinker, LOW);
    odo += wheelCircumference/1000;
    currentSpeed = (3600.0 * wheelCircumference) / timer;
    if (false == isMetricUnit)
    {
        currentSpeed /= 1.609344;
    }
    if (currentSpeed < 100.0)
    {
        currentCorrectedSpeed += currentSpeed;
        currentCorrectedSpeed /= 2;
    }
    printLCD();
    timer=0;
  }
  
  else 
  { /* Sensor is waiting */
    if (timer > 2000)
    {
      currentSpeed = 0;
      currentCorrectedSpeed = 0;
      timer = 0;
      printLCD();
    }
  }
  if (longTimer >= 30.0)
  {
    longAvg += currentCorrectedSpeed;
    longAvg /= avgDivider;
    avgDivider++;
    longTimer = 0;
  }
  /* Counting time for speed calculations */
  delay(10);
  timer += 10;
  longTimer += 0.01;
}

/* Screen alignment
0123456789123456
Speed  ODO   AVG
--.-  --.-  --.-
*/
void printLCD()
{
  outPrintRequest++;
  if (currentCorrectedSpeed*0.1 <= outPrintRequest || currentCorrectedSpeed == 0)
  {
    outPrintRequest = 0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Speed  ODO   AVG");
    lcd.setCursor(0,1);
    lcd.print(currentCorrectedSpeed);
    lcd.setCursor(6,1);
    lcd.print(odo);
    lcd.setCursor(12,1);
    lcd.print(longAvg);
  }
}

void button1func()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Button 1 pressed!");
  delay(100);
}

void button2func()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Button 2 pressed!");
  delay(100);
}

void button3func()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Button 3 pressed!");
  delay(100);
}

void button4func()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Button 4 pressed!");
  delay(100);
}

/**** END OF LOCAL FUNCTION DEFINITIONS **********************************************************/

/**** GLOBAL FUNCTION DEFINITIONS ****************************************************************/

/**** END OF GLOBAL FUNCTION DEFINITIONS *********************************************************/

/* End of file BikeComputer.ino */
