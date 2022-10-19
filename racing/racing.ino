#include <Wire.h>
#include <MsTimer2.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int horse1 = 2;  //stepper motor pin
const int horse2 = 3;
const int player = 4;

const int dirPin1 = 5;  //rotation direction
const int dirPin2 = 6;
const int dirPin3 = 7;

int randomNum[2] = {0, 0};
int randomSpeed[2] = {0, 0};

int srtBtn = 22;
int srtVal = 1;

const int btnPin = 24;
int buttonState = 0;
int lastButtonState = 0;
int buttonPushCounter = 0;

int stopper = 0;
int i = 0;
int MotorDel;
int res = 4;

unsigned long horse1Distance = 0;
unsigned long horse2Distance = 0;
unsigned long playerDistance = 0;

void gameStarting();
void randSpeed();
void stopCountPush();
void race(int cnt);
int btnCnt();  //count button input
void calcWinner();
void showResult();
void gameReset();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(srtBtn, INPUT_PULLUP);
  pinMode(btnPin, INPUT_PULLUP);

  pinMode(horse1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(horse2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(player, OUTPUT);
  pinMode(dirPin3, OUTPUT);

  MsTimer2::set(300, stopCountPush);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(dirPin1, HIGH);  //direction
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, HIGH);

  srtVal = digitalRead(srtBtn);

  lcd.setCursor(5, 0);
  lcd.print("Racing");
  
  if (srtVal == 0) {
    gameStarting();
    
    for (;;) {
      int cnt = 0;

      lcd.setCursor(5, 0);
      lcd.print("Racing");
      
      MsTimer2::start();
      for (;;) {
        cnt = btnCnt();  //count button input
        randSpeed();
        if (stopper == 1) {
          MsTimer2::stop();  //1 sec
          break;
        }
      }
      race(cnt);  //moving belt
      buttonPushCounter = 0;
      cnt = 0;
      stopper = 0;
      i++;
      if (i == 20) {  //stop game after 0.3*20 sec
        i = 0;
        break;
      }
    }
    
    calcWinner();  //calculate winner
    showResult();  //show final result
    gameReset();  //game auto reset
  }
}

void gameStarting() {
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Racing");
  delay(2000);
  lcd.clear();
  
  lcd.setCursor(9, 0);
  lcd.print(3);
  delay(1000);
  lcd.print(2);
  delay(1000);
  lcd.print(1);
  delay(1000);
  lcd.clear();
}

int btnCnt() {
  buttonState = digitalRead(btnPin);
  
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      buttonPushCounter++;
    }
  }
  lastButtonState = buttonState;

  return buttonPushCounter;
}

void randSpeed() {
  for (int i=0; i<2; i++) {
    randomNum[i] = getTrueRotateRandomByte() % 6;

    if (randomNum[i] == 0) {
      randomSpeed[i] = 1200;
    } else if (randomNum[i] == 1) {
      randomSpeed[i] = 1000;
    } else if (randomNum[i] == 2) {
      randomSpeed[i] = 800;
    } else if (randomNum[i] == 3) {
      randomSpeed[i] = 700;
    } else if (randomNum[i] == 4) {
      randomSpeed[i] = 600;
    } else if (randomNum[i] == 5) {
      randomSpeed[i] = 500;
    }
  }
}

void stopCountPush() {
  stopper = 1;
}

void race(int cnt) {
  Serial.println(cnt);
  if ((0<=cnt) && (cnt<=2)) {
    MotorDel = 1200;
  } else if ((2<=cnt) && (cnt<=4)) {
    MotorDel = 1000;
  } else if ((4<=cnt) && (cnt<=6)) {
    MotorDel = 800;
  } else if ((6<=cnt) && (cnt<=8)) {
    MotorDel = 700;
  } else if ((8<=cnt) && (cnt<=9)) {
    MotorDel = 600;
  }  else if (cnt > 9) {
    MotorDel = 500;
  }

  for (int i=0; i<50; i++) {
    digitalWrite(horse1, HIGH);
    delayMicroseconds(randomSpeed[0]);
    digitalWrite(horse2, HIGH);
    delayMicroseconds(randomSpeed[1]);
    digitalWrite(player, HIGH);
    delayMicroseconds(MotorDel);
    digitalWrite(horse1, LOW);
    delayMicroseconds(randomSpeed[0]);
    digitalWrite(horse2, LOW);
    delayMicroseconds(randomSpeed[1]);
    digitalWrite(player, LOW);
    delayMicroseconds(MotorDel);
  }

  horse1Distance = horse1Distance + randomSpeed[0];
  horse2Distance = horse2Distance + randomSpeed[1];
  playerDistance = playerDistance + MotorDel;
  Serial.println(horse1Distance);
  Serial.println(horse2Distance);
  Serial.println(playerDistance);
  Serial.println();

  for (int i=0; i<2; i++) {
    randomNum[i] = 0;
    randomSpeed[i] = 0;
  }
  MotorDel = 0;
}

void calcWinner() {
  if ((horse1Distance < horse2Distance) && (horse1Distance < playerDistance)) {
    //horse1 win
    res = 0;
  } else if ((horse2Distance < horse1Distance) && (horse2Distance < playerDistance)) {
    //horse2 win
    res = 1;
  } else if ((playerDistance < horse2Distance) && (playerDistance < horse1Distance)) {
    //player win
    res = 2;
  }
}

void showResult() {
  if (res == 0) {
    lcd.setCursor(4, 1);
    lcd.print("You Lose");
    Serial.print("You Lose");
    delay(1000);
  } else if (res == 1) {
    lcd.setCursor(4, 1);
    lcd.print("You Lose");
    Serial.print("You Lose");
    delay(1000);
  } else if (res == 2) {
    lcd.setCursor(4, 1);
    lcd.print("You Win!");
    Serial.print("You win");
    delay(1000);
  }
}

void gameReset() {
  lcd.clear();
  stopper = 0;
  buttonPushCounter = 0;
  i = 0;
  res = 4;
  horse1Distance = 0;
  horse2Distance = 0;
  playerDistance = 0;
  lcd.setCursor(2, 0);
  lcd.print("Game Resetting..");

  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, LOW);
  digitalWrite(dirPin3, LOW);

  for (int j=0; j<1000; j++) {
    digitalWrite(horse1, HIGH);
    digitalWrite(horse2, HIGH);
    digitalWrite(player, HIGH);
    delayMicroseconds(500);
    digitalWrite(horse1, LOW);
    digitalWrite(horse2, LOW);
    digitalWrite(player, LOW);
    delayMicroseconds(500);
  }

  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, HIGH);

  lcd.clear();
}
