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

Each of these characters on the LCD is a grid of 5×8 pixels. We can customize these characters by using LCD Custom Character Generator: https://maxpromer.github.io/LCD-Character-Creator/

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
    int xValue = analogRead(joyX);
    int yValue = analogRead(joyY);
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

Snake
~~~~~~
The default length of the snake is 2 and the snake will move to the right by default. The restriction of a snake's move is that it can not turn in the opposite direction when it is moving in one direction which means for example it can not turn left when it is moving right.

The most important part of Snake is to paint its body. Arrays are used to hold coordinate values for each square of the snake body, and the drawSnake() function is used to draw a custom shape to represent the snake square. Each time the position of the snake head changes, traverse the entire coordinate array from the tail forward, move each body piece position forward, and then traverse the coordinate array again and call the drawSnake() and drawGame() function to redraw the entire snake body.
::

  // arrays to store snake position
  int x[80];
  int y[80];
  void drawSnake(int x, int y) {
    lcd.setCursor(x, y);
    lcd.write(3);
  }
  void drawGame() {
    lcd.clear();
    for (int i = 0; i < snakeLength; i++) {
      drawSnake(x[i], y[i]);
    }
    drawFood();
  }

The random seed is first initialized in setup() by randomSeed() function. random() can then be used to generate the coordinate data, where in addition to specifying the region, we also need to consider excluding the food generated in the snake.
::

  randomSeed(analogRead(3));
  foodX = random(0, 20);
  foodY = random(0, 4);

In this game, the joystick module is used to identify different operations up, down, left and right through the data collected through analog pins, and then change the coordinates of the snake head accordingly. When the snake body is redrawn, the whole snake moves once.

The game ends when the coordinates of the snake head exceed the display area, i.e. the snake hits the wall, or when the coordinates of the snake head position match the coordinates of the other squares of the snake body, i.e. the snake eats itself. This is done by checkDie() function.

This game is fairly easy compared to Space Impact.

Space Impact
~~~~~~~~~~~~
Space Impact is very hard to implement. I spent most of time on making this game.
The life number and weapon load value are displayed at column 1 of the LCD. The spaceship is displayed at column 2, row 1 by default. The spaceship can only fire a bullet when the weapon load is great than or equal to 9. The fire function is done by pressing the joystick.
::
  void readButton() {
    if (digitalRead(BTNS) == LOW && fireValue >= conValue) {
      fireValue -= conValue;
      for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 18; j++) {
          if (gameRegion[i][j] == SPACESHIP) {
            // increaase by 4 means a bullet
            gameRegion[i][j + 1] += BULLET;
          }
        }
      }
    }
  }

Space Impact is controlled through the joystick module just as the Snake. The difference is that I used a game region array that stores different values which indicates what should be displayed at each character. For example, 1 means a SPACESHIP, 2 means a OBSTACLE, 4 means a BULLET. The LCD display will be updated based on the game region array, and the game region array will be modified constantly through a variety of ways.
::
  uint8_t gameRegion[4][20];

The first will be the spaceship movement. The position of the spaceship will be updated every time it takes in values from the joystick module. For example:
::
  // move left
  if (yValue > 570 && j > 0) {
    gameRegion[i][j] = BLANK;
    gameRegion[i][j - 1] += SPACESHIP;
  }
  
There are some objects on LCD that will move automatically. They are those obstacles and bullets that have been fired. They are done in updateRegion() function. The idea is the same as moving the spaceship.

The most important part of this game is the resetRegion() function which will update the game region based on the values that are held in the game Region array. The function traverses the whole game region array and makes changes to the array based on the value that is held at each index. For more details, please refer to my code.

Those functions that will make changes to the game region array will be called in loop() function to achieve a dynamic game.



Discussion and Demonstration
----------------------------

Conclusion
----------

Reference
----------

