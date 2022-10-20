#include <MsTimer2.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int horse1 = 2;  //stepper motor pin
const int horse2 = 3;
const int horse3 = 4;

const int dirPin1 = 5;  //rotation direction
const int dirPin2 = 6;
const int dirPin3 = 7;

int randomStep[3] = {0, 0, 0};

int srtBtn = 28;
int srtVal = 1;

const int btnPin1 = 22;
const int btnPin2 = 24;
const int btnPin3 = 26;
int b1val = 1;
int b2val = 1;
int b3val = 1;

int stopper = 0;
int i = 0;
int res = 4;  //게임 결과
int ans = 0;  //답변 했는지 여부
int guess = 3;  //플레이어 예상

unsigned long horse1Distance = 0;
unsigned long horse2Distance = 0;
unsigned long horse3Distance = 0;

void gameStarting();
void randStep();
void stopCountPush();
void race();
void calcWinner();
void gameReset();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  pinMode(srtBtn, INPUT_PULLUP);
  pinMode(btnPin1, INPUT_PULLUP);
  pinMode(btnPin2, INPUT_PULLUP);
  pinMode(btnPin3, INPUT_PULLUP);

  pinMode(horse1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(horse2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(horse3, OUTPUT);
  pinMode(dirPin3, OUTPUT);

  MsTimer2::set(200, stopCountPush);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(dirPin1, HIGH);  //direction
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, HIGH);

  srtVal = digitalRead(srtBtn);
//  Serial.println(srtVal);

  lcd.setCursor(5, 0);
  lcd.print("Racing");
  
  if (srtVal == 0) {
    gameStarting();

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Choose Number!");
    Serial.println("choose number");
    
    while(ans == 0) {
       b1val = digitalRead(btnPin1);
       b2val = digitalRead(btnPin2);
       b3val = digitalRead(btnPin3);
       delay(1);
               
       if (((b1val == 0) || (b2val == 0)) || (b3val == 0)) {
           ans = 1;
           lcd.clear();
       }
        
//        Serial.println("waiting");
    }
    lcd.setCursor(5, 0);
    lcd.print("Racing");
    
    for (;;) {
      MsTimer2::start();
      for (;;) {
        
        randStep();
        if (stopper == 1) {
          MsTimer2::stop();  //1 sec
          break;
        }
      }
      race();  //moving belt
      stopper = 0;
      i++;
      if (i == 10) {  //stop game after 0.2*20 sec
        i = 0;
        break;
      }
    }

    if (b1val == 0) {
        guess = 0;
    } else if (b2val == 0) {
        guess = 1;
    } else if (b3val == 0) {
        guess = 2;
    } else {
        guess = 3;
    }
    
    calcWinner();  //calculate winner
    Serial.println(res);
    delay(1000);
    gameReset();  //game auto reset
  }
}

void gameStarting() {
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print(3);
  delay(1000);
  lcd.print(2);
  delay(1000);
  lcd.print(1);
  delay(1000);
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("Racing");
}

void randStep() {
  for (int i=0; i<3; i++) {
    randomStep[i] = getTrueRotateRandomByte() % 5;  //0 1 2 3 4
  }
}

void stopCountPush() {
  stopper = 1;
}

void race() {
  for (int i=0; i<randomStep[0]*12; i++) {
    digitalWrite(horse1, HIGH);
    delayMicroseconds(800);
    digitalWrite(horse1, LOW);
    delayMicroseconds(800);
  }
  for (int i=0; i<randomStep[1]*12; i++) {
    digitalWrite(horse2, HIGH);
    delayMicroseconds(800);
    digitalWrite(horse2, LOW);
    delayMicroseconds(800);
  }
  for (int i=0; i<randomStep[2]*12; i++) {
    digitalWrite(horse3, HIGH);
    delayMicroseconds(800);
    digitalWrite(horse3, LOW);
    delayMicroseconds(800);
  }

  horse1Distance = horse1Distance + (randomStep[0]*12);
  horse2Distance = horse2Distance + (randomStep[1]*12);
  horse3Distance = horse3Distance + (randomStep[2]*12);

  Serial.println(horse1Distance);
  Serial.println(horse2Distance);
  Serial.println(horse3Distance);
  Serial.println();

  for (int i=0; i<3; i++) {
    randomStep[i] = 0;
  }
}

void calcWinner() {
  if ((horse1Distance >= horse2Distance) && (horse1Distance >= horse3Distance)) {
    //horse1 win
    res = 0;
  } else if ((horse2Distance >= horse1Distance) && (horse2Distance >= horse3Distance)) {
    //horse2 win
    res = 1;
  } else if ((horse3Distance >= horse2Distance) && (horse3Distance >= horse1Distance)) {
    //horse3 win
    res = 2;
  }

    if (guess == res) {
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Correct!");
        Serial.println("Correct!");
        delay(3000);
    } else {
        lcd.clear();
        lcd.setCursor(5, 0);
        lcd.print("Wrong!");
        Serial.println("Wrong!");
        delay(3000);
    }
    Serial.println(guess);
}

void gameReset() {
  lcd.clear();
  stopper = 0;
  i = 0;
  res = 4;
  ans = 0;
  guess = 3;
  lcd.setCursor(1, 0);
  lcd.print("Game Resetting");

  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, LOW);
  digitalWrite(dirPin3, LOW);

  for (int j=0; j<horse1Distance; j++) {
    digitalWrite(horse1, HIGH);
    delayMicroseconds(1000);
    digitalWrite(horse1, LOW);
    delayMicroseconds(1000);
  }
    for (int k=0; k<horse2Distance; k++) {
    digitalWrite(horse2, HIGH);
    delayMicroseconds(1000);
    digitalWrite(horse2, LOW);
    delayMicroseconds(1000);
  }
    for (int l=0; l<horse3Distance; l++) {
    digitalWrite(horse3, HIGH);
    delayMicroseconds(1000);
    digitalWrite(horse3, LOW);
    delayMicroseconds(1000);
  }

  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, HIGH);

    horse1Distance = 0;
    horse2Distance = 0;
    horse3Distance = 0;

    delay(2000);
    lcd.clear();
}
