#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);
/*
  Joystick Module

  created 18 Dec 2018
  by MisterBotBreak

  This example code can be found:

  https://create.arduino.cc/projecthub/MisterBotBreak/how-to-use-a-joystick-with-serial-monitor-1f04f0
*/


int VRx = A0; //Analog Pin for X Position
int VRy = A1; //Analog Pin for Y Position
int SW = 2;   //Digital Pin for Button

//Store State Variables
int xPosition = 0; 
int yPosition = 0;
int SW_state = 0;

void setup() {
  Serial.begin(9600); 
  lcd.init();
  lcd.backlight();

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 
  
}

void loop() {
  //Read Data
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);

  //Print Data in Serial Monitor - Find it at Tools > Serial Monitor
  Serial.print("X: ");
  Serial.print(xPosition);
  Serial.print(" | Y: ");
  Serial.print(yPosition);
  Serial.print(" | Button: ");
  Serial.println(SW_state);

  if (xPosition > 900 && xPosition < 1023)
    lcd.print("up");
  else if (xPosition > 0 && xPosition < 20)
    lcd.print("down");
  else if (yPosition > 900 && yPosition < 1023)
    lcd.print("right");
  else if (yPosition > 0 && yPosition < 20)
    lcd.print("left");

}
