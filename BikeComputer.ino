float wheelCircumference = 2.04;
bool prevState = HIGH;
float speed;
int timer;
float odoKm;
int inactiveCounter;

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
				pinMode(2, INPUT);
				digitalWrite(2,LOW);
				pinMode(13, OUTPUT);
				digitalWrite(13,LOW);
}

void loop(){
				if (digitalRead(2) == LOW && prevState == HIGH){
								prevState = LOW;
								digitalWrite(13, HIGH);
				}
				
				if (digitalRead(2) == HIGH && prevState == LOW){
								inactiveCounter = 0;
								prevState = HIGH;
								digitalWrite(13, LOW);
								odoKm += wheelCircumference/1000;
								speed = (3600 * wheelCircumference) / timer;
								print();
								timer=0;
				}
				
				else {
								inactiveCounter++;
								print();
								if (inactiveCounter > 150){
												speed = 0;
												inactiveCounter = 0;
								}
				}
				
				delay(10);
				timer+=10;
}

void print(){
				lcd.clear();
		  	lcd.setCursor(3,0);
		   lcd.print("Speed: ");
				lcd.setCursor(10,0);
				lcd.print(speed);
				lcd.setCursor(4,1);
				lcd.print("ODO: ");
				lcd.setCursor(9,1);
				lcd.print(odoKm);
}