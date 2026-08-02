/**
 * @file BikeComputer.ino
 *
 * Arduino Nano / Uno bicycle computer prototype.
 *
 * Measures wheel speed, trip distance and average speed using a wheel sensor,
 * then displays the values on a 16x2 LCD.
 *
 * Written for Arduino Nano board.
 *
 * $Author: Gergely Teszári
 * $Date: 2022.08.21.
 * $Revision: 3.1
 *
 * @copyright Gergely Teszári
 */

/**** INCLUDES ***********************************************************************************/

#include <LiquidCrystal.h>
#include "PinChangeInterrupt.h"

/**** END OF INCLUDES ****************************************************************************/

/**** CONSTANTS **********************************************************************************/

/* LCD initialization
 * Pay close attention to the signal pins of the LCD.
 * For my PCB layout it is: RS 10, EN 8, D4 3, D5 2, D6 5, D7 4.
 * Do not use one of the LCD signal ports as the rotation detector input.
 */
const byte LCD_RS = 10;
const byte LCD_EN = 8;
const byte LCD_D4 = 3;
const byte LCD_D5 = 2;
const byte LCD_D6 = 5;
const byte LCD_D7 = 4;

/* Port definitions */
const byte SENSOR_PIN = 12;
const byte STATUS_LED_PIN = 13;

const byte BUTTON_1_PIN = A0;
const byte BUTTON_2_PIN = A1;
const byte BUTTON_3_PIN = A4;
const byte BUTTON_4_PIN = A5;

/* Bike configuration */
const float WHEEL_CIRCUMFERENCE_M = 2.04f;
const float MAX_VALID_SPEED_KMH = 100.0f;
const float SPEED_SMOOTHING_FACTOR = 0.5f;

/* Timing configuration */
const unsigned long STOPPED_TIMEOUT_MS = 2000UL;
const unsigned long AVERAGE_SAMPLE_PERIOD_MS = 30000UL;
const unsigned long MIN_DISPLAY_REFRESH_PERIOD_MS = 250UL;

/**** END OF CONSTANTS ***************************************************************************/

/**** GLOBAL OBJECTS *****************************************************************************/

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

/**** END OF GLOBAL OBJECTS **********************************************************************/

/**** VARIABLES **********************************************************************************/

bool previousSensorState = HIGH;
bool hasLastRotationTime = false;
bool hasCurrentSpeed = false;
bool displayUpdateRequested = true;

unsigned long lastRotationTimeMs = 0;
unsigned long lastAverageSampleTimeMs = 0;
unsigned long lastDisplayRefreshTimeMs = 0;

float currentSpeedKmh = 0.0f;
float displayedSpeedKmh = 0.0f;
float tripDistanceKm = 0.0f;
float averageSpeedKmh = 0.0f;
unsigned int averageSampleCount = 0;

volatile bool button1Pressed = false;
volatile bool button2Pressed = false;
volatile bool button3Pressed = false;
volatile bool button4Pressed = false;

/**** END OF VARIABLES ***************************************************************************/

/**** LOCAL FUNCTION DECLARATIONS ****************************************************************/

void setup();
void loop();

void showWelcomeScreen();
void handleWheelSensor(unsigned long nowMs);
void handleWheelRotation(unsigned long nowMs);
void updateStoppedState(unsigned long nowMs);
void updateAverageSpeed(unsigned long nowMs);
void handleButtons();
void resetTrip();
void printLCD();

void button1Interrupt();
void button2Interrupt();
void button3Interrupt();
void button4Interrupt();

/**** END OF LOCAL FUNCTION DECLARATIONS *********************************************************/

/**** LOCAL FUNCTION DEFINITIONS *****************************************************************/

void setup()
{
  lcd.begin(16, 2);

  pinMode(SENSOR_PIN, INPUT);
  digitalWrite(SENSOR_PIN, LOW);

  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_3_PIN, INPUT_PULLUP);
  pinMode(BUTTON_4_PIN, INPUT_PULLUP);

  attachPCINT(digitalPinToPCINT(BUTTON_1_PIN), button1Interrupt, FALLING);
  attachPCINT(digitalPinToPCINT(BUTTON_2_PIN), button2Interrupt, FALLING);
  attachPCINT(digitalPinToPCINT(BUTTON_3_PIN), button3Interrupt, FALLING);
  attachPCINT(digitalPinToPCINT(BUTTON_4_PIN), button4Interrupt, FALLING);

  lastAverageSampleTimeMs = millis();
  lastDisplayRefreshTimeMs = millis();

  showWelcomeScreen();
  printLCD();
}

void loop()
{
  const unsigned long nowMs = millis();

  handleWheelSensor(nowMs);
  updateStoppedState(nowMs);
  updateAverageSpeed(nowMs);
  handleButtons();

  if (displayUpdateRequested &&
      nowMs - lastDisplayRefreshTimeMs >= MIN_DISPLAY_REFRESH_PERIOD_MS)
  {
    printLCD();
  }
}

void showWelcomeScreen()
{
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Welcome !");
  delay(1500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance unit:");
  lcd.setCursor(0, 1);
  lcd.print("m, km/h");
  delay(1500);
}

void handleWheelSensor(unsigned long nowMs)
{
  const bool sensorState = digitalRead(SENSOR_PIN);

  if (sensorState == LOW && previousSensorState == HIGH)
  {
    /* Magnet triggered the sensor. */
    digitalWrite(STATUS_LED_PIN, HIGH);
  }
  else if (sensorState == HIGH && previousSensorState == LOW)
  {
    /* Magnet left the sensor. Count one full wheel rotation. */
    digitalWrite(STATUS_LED_PIN, LOW);
    handleWheelRotation(nowMs);
  }

  previousSensorState = sensorState;
}

void handleWheelRotation(unsigned long nowMs)
{
  tripDistanceKm += WHEEL_CIRCUMFERENCE_M / 1000.0f;

  if (hasLastRotationTime)
  {
    const unsigned long elapsedMs = nowMs - lastRotationTimeMs;

    if (elapsedMs > 0UL)
    {
      const float measuredSpeedKmh =
        (3600.0f * WHEEL_CIRCUMFERENCE_M) / (float)elapsedMs;

      if (measuredSpeedKmh <= MAX_VALID_SPEED_KMH)
      {
        currentSpeedKmh = measuredSpeedKmh;

        if (hasCurrentSpeed)
        {
          displayedSpeedKmh =
            (SPEED_SMOOTHING_FACTOR * currentSpeedKmh) +
            ((1.0f - SPEED_SMOOTHING_FACTOR) * displayedSpeedKmh);
        }
        else
        {
          displayedSpeedKmh = currentSpeedKmh;
          hasCurrentSpeed = true;
        }
      }
    }
  }

  lastRotationTimeMs = nowMs;
  hasLastRotationTime = true;
  displayUpdateRequested = true;
}

void updateStoppedState(unsigned long nowMs)
{
  if (!hasLastRotationTime)
  {
    return;
  }

  if (nowMs - lastRotationTimeMs > STOPPED_TIMEOUT_MS && hasCurrentSpeed)
  {
    currentSpeedKmh = 0.0f;
    displayedSpeedKmh = 0.0f;
    hasCurrentSpeed = false;
    displayUpdateRequested = true;
  }
}

void updateAverageSpeed(unsigned long nowMs)
{
  if (nowMs - lastAverageSampleTimeMs < AVERAGE_SAMPLE_PERIOD_MS)
  {
    return;
  }

  averageSpeedKmh =
    ((averageSpeedKmh * (float)averageSampleCount) + displayedSpeedKmh) /
    (float)(averageSampleCount + 1U);

  averageSampleCount++;
  lastAverageSampleTimeMs = nowMs;
  displayUpdateRequested = true;
}

void handleButtons()
{
  if (button1Pressed)
  {
    button1Pressed = false;
    resetTrip();
  }

  if (button2Pressed)
  {
    button2Pressed = false;
    /* Reserved for future UI function. */
  }

  if (button3Pressed)
  {
    button3Pressed = false;
    /* Reserved for future UI function. */
  }

  if (button4Pressed)
  {
    button4Pressed = false;
    /* Reserved for future UI function. */
  }
}

void resetTrip()
{
  currentSpeedKmh = 0.0f;
  displayedSpeedKmh = 0.0f;
  tripDistanceKm = 0.0f;
  averageSpeedKmh = 0.0f;
  averageSampleCount = 0;

  hasLastRotationTime = false;
  hasCurrentSpeed = false;
  lastAverageSampleTimeMs = millis();

  displayUpdateRequested = true;
}

/* Screen alignment:
0123456789123456
Speed  ODO  AVG
--.-  --.-- --.-
*/
void printLCD()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Speed  ODO  AVG");

  lcd.setCursor(0, 1);
  lcd.print(displayedSpeedKmh, 1);

  lcd.setCursor(6, 1);
  lcd.print(tripDistanceKm, 2);

  lcd.setCursor(12, 1);
  lcd.print(averageSpeedKmh, 1);

  lastDisplayRefreshTimeMs = millis();
  displayUpdateRequested = false;
}

void button1Interrupt()
{
  button1Pressed = true;
}

void button2Interrupt()
{
  button2Pressed = true;
}

void button3Interrupt()
{
  button3Pressed = true;
}

void button4Interrupt()
{
  button4Pressed = true;
}

/**** END OF LOCAL FUNCTION DEFINITIONS **********************************************************/

/* End of file BikeComputer.ino */
