Small Game Collection on 20x04 LCD
==================================================
:Author: Yicheng Qian
:Course: ECE387
:Project: Final Individual Project

Introduction
------------

This project is a collection of LCD games on Arduino that served as a kid toy. Remember what I played with as a child, I came up with this idea because I spent a lot of time playing those games on the old Nokia when I was a child. We didn't have smartphones back then, but it was really fun to play those games. That is my precious memory, and I still miss that time. The I2C 20x04 LCD module will be used as the display. The design will have a menu that allows users to select one of those classic games. They can also go back to the menu when they finish one game. All games would need a joystick or a few buttons to control.
Intial thought on what games will be included (It depends on how much time I have):

- Snake (Absolutely classic, this only needs a joystick)
- Space Impact (this needs a joystick)
- Flappy Bird (this only needs a joystick)
- and more (probably a game that two players can play together)

In the end, I only have time for building two games. One is the Snake, another is the Space Impact. There is also an menu to choose the game the user want to play.

Prototype Setup
---------------
Proposed Materials
~~~~~~~~~~~~~~~~~~
- I2C 20x04 LCD Display Module x 1
- Joystick x 1
- Button x 2
- Resistor x 2


Description of Components
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**I2C LCD:**

An I2C LCD is an LCD display screen with an I2C interface. It can be used to display a range of sizes. There are usually two typical sizes: 16x02 and 20x04(16 characters on 2 lines or 20 characters on 4 lines). The reason we need such an LCD with I2C is a normal LCD would need over 10 wires to connect to an Arduino while an I2C LCD would only use 4 wires to connect to an Arduino. 2 data pins are used to control the LCD. Adding an I2C interface reduces complexity and difficulty. In addition, more port resources are saved. An I2C LCD is made up of a PCF8574T port expander and an HD44780 LCD. 

**Joystick:**

The joystick is a position sensor that returns two simulated values representing its X and Y positions. It can act as an interface to navigate menus or control the direction or speed of an object. It often appears on joysticks in video games, mock-ups of remote controls, and even on dashboards of industrial machines.

The home position of the joystick is (x, y :511,511). If the joystick is moved from one end of the X-axis to the other, the x-value will change from 0 to 1023, and a similar situation will occur when moving along the Y-axis.

**Button:**

A pull-up resistor is similar to a pull-down resistor and is a way of directing current or signals to a specific location based on specific conditions. The most important thing to remember is that electricity reaches the ground as quickly as possible.

There are two ways to connect the button. Voltage can take two paths. The first path is from the voltage source to the digital pin, which causes a high signal, or it can be pushed to the ground, which causes the microcontroller to read a low signal.

Here we are using pull-down resistors for our buttons.

How to Use Each Component
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**I2C LCD:**

Connection:

| Analog Pin 4 -> SDA
| Analog pin 5 -> SCL
| 5V           -> VCC
GND          -> GND



| Basic code to use the I2C LCD: 
::

  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(0x27, 20, 4);
  void setup() {
    lcd.init();
    lcd.clear();
    lcd.backlight();
  }
  void loop() {
    lcd.setCursor(0,0);
    lcd.print("Hello World");
  }
  
Note: If you are using a 16x02 I2C LCD, you should change the second and third argument to 20 and 4 respectively.
  

  

**Joystick:**

Connection:

| VCC -> 5V
| GND -> GND
| VRx -> A0
| VRy -> A1
SW  -> Pin 2

| Basic code to use a Joystick: 
::

  #define joyX A0
  #define joyY A1
  void setup() {
    Serial.begin(9600);
  }
  void loop() {
    xValue = analogRead(joyX);
    yValue = analogRead(joyY);
    Serial.print(xValue);
    Serial.print("\t");
    Serial.println(yValue);
  }

**Button:**

::
    
  int buttonPin = 2;
  int buttonState = 0; 
  void setup() {
    pinMode(buttonPin, INPUT);
  }
  void loop() {
    buttonState = digitalRead(buttonPin);
  }

Implementation
--------------

Discussion and Demonstration
----------------------------

Conclusion
----------

Reference
----------

