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

Each of these characters on the LCD is a grid of 5??8 pixels. We can customize these characters by using LCD Custom Character Generator: https://maxpromer.github.io/LCD-Character-Creator/

Connection:

| Analog Pin 4 -> SDA
| Analog pin 5 -> SCL
| 5V           -> VCC
GND          -> GND

.. image:: https://github.com/qiany21/ECE387-Final-Individual-Project/blob/main/Wiring-I2C-LCD-Display-with-Arduino.png

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

.. image:: https://github.com/qiany21/ECE387-Final-Individual-Project/blob/main/Joystick_with_Arduino.png

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

.. image:: https://github.com/qiany21/ECE387-Final-Individual-Project/blob/main/button_with_arduino.png

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
  
There are some objects on LCD that will move automatically. They are those obstacles and bullets that have been fired. They are done in updateRegion() function. The idea is the same as moving the spaceship. Obstacles are auto generated based on the risk value, and the risk value is being updated as the game goes on.
::
  // update the score
  // based on the score we have, the game difficulty increases
  result++;
  counter++;
  if (counter % 100 == 0) {
    waitTime -= 5;
    riskValue += 3;
    conValue--;
  }
  for (int i = 0; i < 4; i++) {
    if (random(100) < riskValue) {
      gameRegion[i][18] += OBSTACLE;
    }
  }

The most important part of this game is the resetRegion() function which will update the game region based on the values that are held in the game Region array. The function traverses the whole game region array and makes changes to the array based on the value that is held at each index. For more details, please refer to my code.

Those functions that will make changes to the game region array will be called in loop() function to achieve a dynamic game.

Discussion and Demonstration
----------------------------
Testing and Defensive Code
~~~~~~~~~~~~~~~~~~~~~~~~~~
**Snake:**

Snake game has a constant game speed, and it works smoothly without using millis() so that I did not consider this will be the problem. Snake game has a bunch of void functions that will be placed in loop(). The only function that takes in parameters is drawSnake(), I checked if the input parameters are valid or not. If not, the program will be paused for 5 seconds. Hence, it is very easy to notice whether such an earror exists or not. Another is the snake can hit itself only if the snake length is at least 4. Snake cannot hit itself its length is less than 4. So in checkDie() function, this is the first thing that is needed to be checked. These are crucial parts when I debugged this game. 

For testing, just a couple things that are needed to validate. The first is the game should be over when the snake hits the wall or hits itself, and the snake cannot hit itself if the snake length is less then 4. The second thing to check is the snake cannot turn in the opposite direction. The third is the food should be auto-generated once the current food has been eaten by the snake.

**Space Impact:**

All functions made for Space Impact will not take in any parameters so that there is no need to check if the input and output are valid or not. The only thing that I need to check is if the timing works perfectly or not.
As the movement based on input from the joystick and the refresh of the screen are placed in loop(), the speed of the game is realized by changing the delay time by changing the delay function, so when the game speed is too slow, the joystick button operation becomes insensitive, that is, when I press the joystick to fire a bullet, and it takes longer time to fire. Other than this, the game is smooth. What I did is to try using timers to refresh the screen and vary the speed of the game by changing the timing. It helps the program keep running without pausing.
::
  void customDelay(unsigned long timer) {
    unsigned long tx = millis();
    while (millis() - tx < timer) {
      readButton();
    }
  }
For testing, there are a few things that I need to notice. One is the bullet can be shot when the weapon load value is greater than or equal to 9. THe second is the bullet and the obstacle should vanish when the bullet hits against the obstacle. The third is the spaceship and the obstacle should vanish when the spaceship collides with the obstacle and the spaceship should be appear again at the same place. The number should decrease as well. There is no need to test the joystick because I have successfully used it for Snake.

Demonstration Video Link: 
~~~~~~~~~~~~~~~~~~~~~~~~~
Snake Demo: https://www.youtube.com/shorts/UjCRQ7at4HU

Space Impact Demo: https://www.youtube.com/watch?v=hD0sastdKdM

Conclusion and Reflection
--------------------------

Many of my classmates used the 3D-printing to make the product more aesthetic. I found out I ran out of time to do these, at least the prototype is working perfectly. In addition, I also did not make more games as I proposed early. The reason is that it is actually much harder to make a game and run on Arduino and LCD. I overestimated my ability to build games in Arduino IDE and work along with hardware.

Finally, I think my understanding of embedded systems had a significant deviation between the beginning and the end of the course. Obviously, FPGA is directly related to embedded system. However, most of the embedded systems discussed and designed in class do not require FPGA. Many microcontrollers, or microprocessors, are sufficient to handle those operations. We cannot design an embedded system with high computational intensity and complexity, which is also the reason why FPGA does not appear in this course. Also, I found that even though the FPGA was not present in this course. I also can't seem to bring FPGA into my personal projects. I guess it would be a better choice for me to make a Final Project of ECE287 that I have not completed before because I love FPGA-related project. I must lack the spirit of innovation.

All in all, I seem to have a hard time finding interesting ideas or building interesting embedded systems in life. I feel I am more excited and motivated to build and design complex industrial things in chip(semiconductor) industry. I am also not innovative and creative enough, which should be the direction of my personal development in the future. I think I should read more interesting projects shared by others on the Internet.

Reference
----------
I2C LCD Tutorial: https://lastminuteengineers.com/i2c-lcd-arduino-tutorial/

Joystick Tutorial: https://exploreembedded.com/wiki/Analog_JoyStick_with_Arduino

Button Tutorial: https://www.instructables.com/Arduino-Button-Tutorial/

Space Invader: https://www.hackster.io/john-bradnam/tiny-space-invaders-game-d1f3a8

