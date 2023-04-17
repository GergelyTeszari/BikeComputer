//Adjustable variables
bool isMetricUnit = true;
int sensorPin = 2;
int blinker = 13;
float wheelCircumference = 2.04; //in meters or feets
//SpeedoMeter variables
bool prevState = HIGH;
float currentSpeed;
int timer;
float longTimer;
float odo;
float currentCorrectedSpeed;
float longAvg;
int avgDivider = 2;
int outPrintRequest;

//LCD initialization
#include <LiquidCrystal.h>
int rs=7;
int en=8;
int d4=9;
int d5=10;
int d6=11;
int d7=12;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

void setup(){
 				lcd.begin(16,2);
 				pinMode(sensorPin, INPUT);
 				digitalWrite(sensorPin,LOW);
 				pinMode(blinker, OUTPUT);
 				digitalWrite(blinker,LOW);
    if (isMetricUnit == false){
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
    if (isMetricUnit){
        lcd.print("m, Km/h");
    }
    else{
        lcd.print("ft, mi/h ");
    }
    delay(2000);
}

void loop(){
    if (digitalRead(sensorPin) == LOW && prevState == HIGH){ //Magnet triggered sensor
        prevState = LOW;
        digitalWrite(blinker, HIGH);
    }
				
    if (digitalRead(sensorPin) == HIGH && prevState == LOW){ //Magnet left the sensor
        prevState = HIGH;
        digitalWrite(blinker, LOW);
        odo += wheelCircumference/1000;
        currentSpeed = (3600.0 * wheelCircumference) / timer;
        if (isMetricUnit == false){
            currentSpeed /= 1.609344;
        }
        if (currentSpeed < 100.0){
            currentCorrectedSpeed += currentSpeed;
            currentCorrectedSpeed /= 2;
        }
        print();
        timer=0;
				}
    
    else { //Sensor is waiting
        if (timer > 2000){
            currentSpeed = 0;
            currentCorrectedSpeed = 0;
            timer = 0;
            print();
        }
				}
    if (longTimer >= 30.0){
        longAvg += currentCorrectedSpeed;
        longAvg /= avgDivider;
        avgDivider++;
        longTimer = 0;
    }
		 		delay(10); //Counting time for
		 		timer += 10; //speed calculations
    longTimer += 0.01;
}

void print(){ //Printing values to the LCD
    outPrintRequest++;
    if (currentCorrectedSpeed*0.1 <= outPrintRequest || currentCorrectedSpeed == 0){
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
}/*
0123456789123456
Speed  ODO   AVG
--.-  --.-  --.-*/