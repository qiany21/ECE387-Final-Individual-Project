#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);


/* Joystick */
#define BTNS  2
#define JoyX A0
#define JoyY A1

/* buttons */
const int button1 = 7;
const int button2 = 8; 

/********************* Space Impact ************************/
#define BLANK 0
#define SPACESHIP 1
#define OBSTACLE 2
#define SPACESHIP_OBSTACLE_COLLISION 3
#define BULLET 4
#define BULLET_OBSTACLE_COLLISION 6
#define TRIPLE_COLLISION 10
// the 2D array to hold the status of each spot on LCD
uint8_t gameRegion[4][20];

// LCD custom characters for space impact
byte ship[8]     = {B11100, B11100, B01111, B01111, B11100, B11100, B00000, B00000,};
byte obstacle[8] = {B11100, B10100, B10100, B10100, B10100, B10100, B11100, B00000,};
byte fire[8]     = {B00100, B00100, B10010, B10010, B01000, B01000, B00000, B00000,};

/* basic setup variables for Space Impact*/
int waitTime = 400;
int riskValue = 10;
int fireValue = 0;
int conValue = 9;
int lifeCounter = 3;

// Space Impact game result values
unsigned long result = 0;
unsigned long counter = 0;
bool startGameStatus = false;

// Joystick reading values for space impact
int xValue , yValue;

/********************* Snake game ************************/
#define RIGHT 0
#define LEFT  1
#define UP    2
#define DOWN  3

int snakeLength = 2;

// default snake position
int headX = 4;
int headY = 1;
// position for food
int foodX;
int foodY;
// arrays to store snake position
int x[80];
int y[80];
// default direction
int snakeDirection = RIGHT;
// flag for food and Snake game result
bool foodStatus = true;
bool startGameStatusSnake = false;
// score
int score = 0;
// the speed of snake's move
int snakeSpeed = 200;



// custom characters for snake
byte body[] = {B00000, B00000, B00100, B01110, B11111, B01110, B00100, B00000};
byte food[] = {B00000, B11111, B11111, B11111, B11111, B11111, B11111, B00000};


// game selct
int gameSelect;

/*
 * Game Menu that shows two options
 */
void menu() {
  gameSelect = 0;
  lcd.setCursor(0, 0);
  lcd.print("Game Collection");
  lcd.setCursor(0, 1);
  lcd.print("Btn1:Snake");
  lcd.setCursor(0, 2);
  lcd.print("Btn2:Space Impact");
  while (gameSelect == 0) {
    // button 1 is to select Snake game
    // button 2 is to select Space Impact
    if (digitalRead(button1)) gameSelect = 1;
    if (digitalRead(button2)) gameSelect = 2;
  }
}

/*
 * initialize the LCD
 */
void initLcd() {
  lcd.init();
  lcd.backlight();
  lcd.begin(20, 4);
  lcd.createChar(0, ship);
  lcd.createChar(1, obstacle);
  lcd.createChar(2, fire);
  lcd.createChar(3, body);
  lcd.createChar(4, food);
}

/*
 * initialize the joystick
 */
void initHardware() {
  pinMode(BTNS, INPUT_PULLUP);
  pinMode(JoyX , INPUT);
  pinMode(JoyY , INPUT);
}

void setup()
{
  // reset arrays that store the postion of a snake
  memset(x, 0x00, 80);
  memset(y, 0x00, 80);
  randomSeed(analogRead(3)); 
  // setup all components
  Serial.begin(9600);
  initLcd();
  initHardware();
}


void loop()
{
  menu();
  if (gameSelect == 1) {
    welcomeSnake();
    resetGameSnake();
    while(1) {
      lcd.clear();
      Serial.println(startGameStatusSnake);
      if (startGameStatusSnake)
      {
        lcd.clear();
        drawResult();
        break;
      } else {
        joystick();
        snakeMove();
        drawGame();
      }
      delay(snakeSpeed);
    }
  }
  else if (gameSelect == 2) {
    dispIntro();
    dispRules();
    resetGame();
    startGame();
  }
}

/*************************** Snake ***********************************/
/*
 * display the introduction messgae to the player
 */
void welcomeSnake() {
  lcd.clear();
  lcd.setCursor(7, 1);
  lcd.print("Snake");
  lcd.setCursor(8, 2);
  lcd.print("Game");
  delay(1000);
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("Push to Start");
  while (1) {
    if (digitalRead(BTNS) == 0){
      lcd.clear();
      break;
    }
  }
}

/*
 * joystick function for the Snake game specifically
 */
void joystick() {
  xValue = analogRead(JoyX);
  yValue = analogRead(JoyY);
//  Serial.println(xValue);
//  Serial.println(yValue);
  if (xValue > 850 && xValue < 1023) {
    if (snakeDirection != DOWN)
      snakeDirection = UP;
  }
  else if (xValue > 0 && xValue < 200) {
    if (snakeDirection != UP)
      snakeDirection = DOWN; 
  }
  else if (yValue > 800 && yValue < 1023) {
    if (snakeDirection != LEFT)
      snakeDirection = RIGHT;
  }
  else if (yValue > 0 && yValue < 200) {
    if (snakeDirection != RIGHT)
      snakeDirection = LEFT;
  }
}

/*
 * output the snake piece to the physical LCD screen
 */
void drawSnake(int x, int y) {
  // defensive code
  if (x < 0 || y >= 20) delay(5000);
  lcd.setCursor(x, y);
  lcd.write(3);
}

/*
 * output logical screen to the physical LCD screen
 */
void drawGame() {
  lcd.clear();
  for (int i = 0; i < snakeLength; i++) {
    drawSnake(x[i], y[i]);
  }
  drawFood();
}

/*
 * generate random food
 */
void drawFood(void) {
  int food_out = 0;
  if (foodStatus) {
    while (food_out == 0) {
      food_out = 1;
      foodX = random(0, 20);
      foodY = random(0, 4);
      for (int i = snakeLength - 1; i > 0; i--) {
        if (foodX == x[i] && foodY == y[i]) {
          food_out = 0;
        }
      }
    }
  }
  lcd.setCursor(foodX, foodY);
  lcd.write(4);
  foodStatus = false;
}

/*
 * update the direction of the snake and its postion
 */
void snakeMove(void) {
  // update coordinate
  switch (snakeDirection) {
    case RIGHT:
     headX += 1;
      break;
    case UP:
     headY -= 1;
      break;
    case LEFT:
     headX -= 1;
      break;
    case DOWN:
     headY += 1;
      break;
  }
  // check if the food has been eaten
  if ((headX == foodX) && (headY == foodY)) {
    foodStatus = true; 
    snakeLength++;
    score++;
  }
  // update snake 
  for (int i = snakeLength - 1; i > 0; i--) {
    x[i] = x[i - 1];
    y[i] = y[i - 1];
  }
  x[0] = headX;
  y[0] = headY;
  // check if the snake dies or not
  checkDie();
}

/*
 * Check if the snake goes over the wall or hit itself. If so, end the game
 */
void checkDie(void) {
  // touch walls, die
  if (headX < 0 || headX > 19 || headY < 0 || headY > 3) {
    startGameStatusSnake = true;
  }
  // touch itself, die
  for (int i = 1; i < snakeLength; i++) {
    if (headX == x[i] && headY == y[i]) {
      startGameStatusSnake = true;
    }
  }
}

/*
 * show the result of the Snake game
 */
void drawResult() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(0, 1);
  lcd.print("SCORE:");
  lcd.print(score);
  while (1) {
    if (digitalRead(BTNS) == 0){
      lcd.clear();
      break;
    }
  }
}

/*
 * Reset the Snake game
 */
void resetGameSnake() {
  snakeLength = 2;

  // default snake position
  headX = 4;
  headY = 1;
  // arrays to store snake position
  memset(x, 0x00, 80);
  memset(y, 0x00, 80);
  // default direction
  snakeDirection = RIGHT;
  // flag for food and game result
  foodStatus = true;
  startGameStatusSnake = false;
  // score
  score = 0;
}

/******************************* Space Impact ********************************/
void startGame() {
  lcd.setCursor(0, 1);
  lcd.print(lifeCounter);
  lcd.setCursor(0, 2);
  lcd.print(fireValue);
  resetRegion();
  startGameStatus = true;
  while (startGameStatus) {
    updateRegion(1);
    readButton();
    updateFireStatus();
    resetRegion();
    customDelay(waitTime);
    readDirection();
    updateRegion(2);
    resetRegion();
    customDelay(waitTime);
    updateGame();
  }
}

/*
 * this custom delay is for firing the bullet
 */
void customDelay(unsigned long timer) {
  unsigned long tx = millis();
  while (millis() - tx < timer) {
    readButton();
  }
}

void updateGame() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 19; j++) {
      if (gameRegion[i][j] == SPACESHIP_OBSTACLE_COLLISION) {
        gameRegion[i][j] = SPACESHIP;
        lifeCounter--;
        lcd.setCursor(0, 1);
        lcd.print(lifeCounter);
        // if there is no life, game over
        if (lifeCounter == 0) {
          startGameStatus = false;
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print("GAME OVER");
          lcd.setCursor(0, 2);
          lcd.print("SCORE:" + String(result));
          delay(1000);
          moveShip();
        }
      } else if (gameRegion[i][j] > BULLET) {
        // bingo!
        result += 10;
        // reset back to blank
        gameRegion[i][j] -= BULLET_OBSTACLE_COLLISION;
      }
    }
  }
  // update the score
  // based on the score we have, the game difficulty increases
  result++;
  counter++;
  if (counter % 100 == 0) {
    waitTime -= 5;
    riskValue += 3;
    conValue--;
  }
}

/*
 * calculate and output the correct weapon load to the LCD
 */
void updateFireStatus() {
  if (fireValue < 9) {
    fireValue++;
    lcd.setCursor(0, 2);
    lcd.print(fireValue);
  }
}

/*
 * update the postion of the spaceship based on the input of the joystick
 * reset the current postion to 0, and change the target location to 1
 */
void readDirection() {
  byte spaceshipFlag = 0;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 19; j++) {
      if (gameRegion[i][j] == SPACESHIP) {
        spaceshipFlag = 1;
        xValue = analogRead(JoyX);
        yValue = analogRead(JoyY);
        // for testing purpose
        //Serial.println(String(analogRead(A0)) + " " + String(analogRead(A1)));
        if (yValue > 570 && j > 0) {
          gameRegion[i][j] = BLANK;
          gameRegion[i][j - 1] += SPACESHIP;
        } else if ((yValue < 530) && j < 14) {
          if (gameRegion[i][j + 1] != BULLET) {
            gameRegion[i][j] = BLANK;
            gameRegion[i][j + 1] += SPACESHIP;
          }
        } else if (xValue < 540 && i > 0) {
          gameRegion[i][j] = BLANK;
          gameRegion[i - 1][j] += SPACESHIP;
        } else if (xValue > 580 && i < 3) {
          gameRegion[i][j] = BLANK;
          gameRegion[i + 1][j] += SPACESHIP;
        }
      }
      if (spaceshipFlag == 1) {
        break;
      }
    }
    if (spaceshipFlag == 1) {
      break;
    }
  }
}

/*
 * update either the bullets and obstacles on the screen
 */
void updateRegion(int x) {
  if (x == 1) {
    // update bullets, move bullets forward, 4 means bullets
    for (int i = 0; i < 4; i++) {
      for (int j = 19; j > 0; j--) {
        if (gameRegion[i][j - 1] == BULLET) {
          gameRegion[i][j - 1] = BLANK;
          if (j < 19) {
            gameRegion[i][j] += BULLET;
          }
        }
      }
    }
  }
  // update obstacles, 2 means obstacles
  else if (x == 2) {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 19; j++) {
        if (gameRegion[i][j] == OBSTACLE) {
          gameRegion[i][j] = BLANK;
          if (j > 0) {
            gameRegion[i][j - 1] += OBSTACLE;
          }
        }
      }
    }
    // based on the current score, generate obstacles
    for (int i = 0; i < 4; i++) {
      if (random(100) < riskValue) {
        gameRegion[i][18] += OBSTACLE;
      }
    }
  }
}

/*
 * read the input from the joystick button and fire
 */
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

/*
 * reset region removes all the characters from your screen and updates to new character based on the gameregion array 
 * for example if game region is 1 at that index it checks if the next row is 0 than it displays ship in that row and no ship in other rows. 
 * It does similar checks for obstalce and bullet. 
 * use https://www.hackster.io/john-bradnam/tiny-space-impact-game-ab1d77 as a reference
 */
void resetRegion() {
  for (int i = 0; i < 4; i += 2) {
    for (int j = 0; j < 19; j++) {
      // skip the first column
      lcd.setCursor(j + 1, i);
      if (gameRegion[i][j] == SPACESHIP) {
        // 1 at that index it checks if the next row is 0 than it displays ship in that row and no ship in other rows. It also lets user to control ship via joystick
        if (gameRegion[i + 1][j] == BLANK) {
          lcd.print(char(0));
          lcd.setCursor(j + 1, i + 1);
          lcd.print(" ");
        } else if (gameRegion[i + 1][j] == OBSTACLE || gameRegion[i + 1][j] == BULLET_OBSTACLE_COLLISION || gameRegion[i + 1][j] == TRIPLE_COLLISION) {
          lcd.print(char(0));
          lcd.setCursor(j + 1, i + 1);
          lcd.print(char(1));
        }
      // if it is 2 than it checks 
      // if next column is 0 than it only shows box in that row index and next is empty else 
      // if next is 1 than it shows box in first row and ship in next 
      // if next is 2 than it shows box in both
      // else if it 4 , 6 or 10 it shows box and then fire
      } else if (gameRegion[i][j] == OBSTACLE || gameRegion[i][j] == BULLET_OBSTACLE_COLLISION || gameRegion[i][j] == TRIPLE_COLLISION) {
        if (gameRegion[i + 1][j] == BLANK) {
          lcd.print(char(1));
          lcd.setCursor(j + 1, i + 1);
          lcd.print(" ");
        } else if (gameRegion[i + 1][j] == SPACESHIP) {
          lcd.print(char(1));
          lcd.setCursor(j + 1, i + 1);
          lcd.print(char(0));
        } else if (gameRegion[i + 1][j] == OBSTACLE) {
          lcd.print(char(1));
          lcd.setCursor(j + 1, i + 1);
          lcd.print(char(1));
        } else if (gameRegion[i + 1][j] == BULLET || gameRegion[i + 1][j] == BULLET_OBSTACLE_COLLISION || gameRegion[i + 1][j] == TRIPLE_COLLISION) {
          lcd.print(char(1));
          lcd.setCursor(j + 1, i + 1);
          lcd.print(char(2));
        }
        // update bullets, if the position is a bullet
      } else if (gameRegion[i][j] == BULLET) {
        if (gameRegion[i + 1][j] == BLANK) {
          lcd.print(char(2));
          lcd.setCursor(j + 1, i + 1);
          lcd.print(" ");
        } else if (gameRegion[i + 1][j] == OBSTACLE || gameRegion[i + 1][j] == BULLET_OBSTACLE_COLLISION || gameRegion[i + 1][j] == TRIPLE_COLLISION) {
          // update bulltes and obstacles
          lcd.print(char(2));
          lcd.setCursor(j + 1, i + 1);
          lcd.print(char(1));
        }
      } else if (gameRegion[i][j] == BLANK) {
        // if there is nothing above and below the current postion, print blank spaces
        if (gameRegion[i + 1][j] == BLANK) {
          lcd.print(" ");
          lcd.setCursor(j + 1, i + 1);
          lcd.print(" ");
        } else if (gameRegion[i + 1][j] == SPACESHIP) {
          lcd.print(" ");
          lcd.setCursor(j + 1, i + 1);
          lcd.print(char(0));
        } else if (gameRegion[i + 1][j] == OBSTACLE || gameRegion[i + 1][j] == BULLET_OBSTACLE_COLLISION || gameRegion[i + 1][j] == TRIPLE_COLLISION) {
          lcd.print(" ");
          lcd.setCursor(j + 1, i + 1);
          lcd.print(char(1));
        } else if (gameRegion[i + 1][j] == BULLET) {
          lcd.print(" ");
          lcd.setCursor(j + 1, i + 1);
          lcd.print(char(2));
        }
      } else {
        lcd.print(" ");
        lcd.setCursor(j + 1, i + 1);
        lcd.print(" ");
      }
    }
  }
}

/*
 * show the introduction messgae of the Space Impact game
 */
void dispIntro() {
  lcd.clear();
  lcd.setCursor(0 , 1);
  lcd.print("      S P A C E     ");
  lcd.setCursor(0 , 2);
  lcd.print("     I M P A C T    ");
  moveShip();
}

/*
 * Show the rule of the Space Impact game
 */
void dispRules() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Life points = 3");
  lcd.setCursor(3, 1);
  lcd.print("Weapon Load = 9");
  lcd.setCursor(3, 2);
  lcd.print("Press Button to");
  lcd.setCursor(8, 3);
  lcd.print("START");
  while (digitalRead(BTNS)) {
  }
  moveShip();
  lcd.clear();
}


/*
 * Reset the whole Space Impact game, set everything back to default
 */
void resetGame() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 20; j++) {
      gameRegion[i][j] = BLANK;
    }
  }
  gameRegion[0][0] = 1;
  waitTime = 100;
  riskValue = 10;
  fireValue = 0;
  conValue = 9;
  result = 0;
  lifeCounter = 3;
  counter = 0;
}

/*
 * scroll the spaceship on the LCD
 */
void moveShip() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 20; j++) {
      lcd.setCursor(j, i);
      lcd.print(char(0));
      delay(50);
      lcd.setCursor(j, i);
      lcd.print(" ");
    }
  }
  delay(500);
}
