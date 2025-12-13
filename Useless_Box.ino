#include <Servo.h>
#pragma region Distance Sensor config
#include <NewPing.h>

#define TRUE 1
#define FALSE 0


Servo arm;
int storageAngle = 70;
int reachAngle = 180;
int peekAngle = 120;
int armPin = 11;

int switchPin = 12;
int switchState = 0;
int flickNum = 0;

Servo wall;
bool wallWouldbeDisplayed = false;
int wallStorageAngle = 60;
int wallReachAngle = 0;
int wallPin = 8;


int TRIGGER_PIN = 10; // Arduino pin tied to trigger pin on the ultrasonic sensor (HCSR04).
int ECHO_PIN = 9;      // Arduino pin tied to echo pin on the ultrasonic sensor.
int MAX_DISTANCE = 10;  // Max distance in centimeters that the ultrasonic sensor will look for

int distance = 0;
bool detectingHand = false;
#pragma endregion Distance Sensor config
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
// max dist can be increased but further distances can take longer to read which could disrupt balancing

int motorControlPin1 = 6;
int motorControlPin2 = 5;
int enablePin = 3;
int motorEnable = 0;
int motorSpeed= 0;
int motorDirection = 1;
bool startEvading = false;
bool activateMove = false;

int ON = LOW;
int OFF = HIGH; 
int prevState = ON;
bool flipped = false;
bool detectedBeforeFlip = false;
int detectedCounter = 0;
bool handDetected = false;
bool activateWall = false;

void setup() {
  arm.attach(armPin);  
  wall.attach(wallPin);

  pinMode(switchPin, INPUT);  // switch
  pinMode(13, OUTPUT);        //built in light
  pinMode(armPin, OUTPUT);    // arm

  pinMode(wallPin, OUTPUT);

  pinMode(TRIGGER_PIN, INPUT); 
  pinMode(ECHO_PIN, INPUT);
  
  pinMode(motorControlPin1, OUTPUT);
  pinMode(motorControlPin2, OUTPUT);
  pinMode(enablePin, OUTPUT);
  
  Serial.begin(9600);
  digitalWrite(enablePin, LOW);
  arm.write(storageAngle);
  wall.write(wallStorageAngle);
}

void loop() {
  // put your main code here, to run repeatedly:
  switchState = digitalRead(switchPin);
  detectingHand = distanceSensing();



  if (detectingHand == true && flipped == false) {
    handDetected = true;
    // logger("detected");
    detectedCounter = 0;
  } 

  if (detectingHand == false && detectedCounter > 75 && handDetected == true) {
    handDetected = false;
    // logger("no longer detected");
    detectedCounter = 0;
  } else {
    detectedCounter++;
  }

  if (handDetected == false && flipped == true) {
    startEvading = true;
    logger("true");
  }

  // if (checkSwitch() == true) { //switch was flipped
  //   // logger("flip");
  //   flip(flickNum);
  // } else if (checkSwitch() == false){ //switch in correct position
  //   logger("storing");
  //   storeArm(1);
  // }

  if (switchState == OFF){ //switch in the prefered position
    flipped = false;
    if (prevState == ON) {
      prevState = OFF;
      storeArm(1);
      return false;
    }
    
  } else if (switchState == ON){ // switch not in the perfered position
    flipped = true;
    if (prevState == OFF) {
      prevState = ON;
      flip(flickNum);
      return true;
    }
  }

  if (activateWall == true) {
    moveWall();
  }

  if (activateMove == true) {
    move();
  }
  // move();
  
  
  delay(15);
}

void flip(int attempt){
  flickNum++;
  // Serial.println(attempt);
  if(attempt == 0) {
    delay(1000);
    arm.write(reachAngle);
  } else if(attempt > 0 && attempt < 6) {
    reachArm(true); // [][][][][][][][][][][][][][][][]change to true for final
  } else if(attempt > 5 && attempt < 16){
    reachArm(false);
    activateWall = true;
  } else {
    reachArm(false);
    activateWall = true;
    activateMove = true;
    //arm.write(reachAngle);
  }
}

void storeArm(int c) {
  switch (c) {
    case 1: //normal
      arm.write(storageAngle);
      break;
    case 2:
      arm.write(peekAngle);
      delay(1000);
      arm.write(storageAngle);
      break;
  }
}


void reachArm(bool includePeek){
  switch (random(1, 4)) {
    case 1:
    case 2: 
      arm.write(reachAngle);
      break;
    case 3:
      if (includePeek) {
        arm.write(peekAngle);
        delay(1500);
        arm.write(reachAngle);
      } else {
        arm.write(reachAngle);
      }
      break;
  }
}

bool checkSwitch() {
  if (switchState == OFF){ //switch in the prefered position
    flipped = false;
    if (prevState == ON) {
      prevState = OFF;
      storeArm(1);
      return false;
    }
    
  } else if (switchState == ON){ // switch not in the perfered position
    flipped = true;
    if (prevState == OFF) {
      prevState = ON;
      flip(flickNum);
      logger("true");
      return true;
    }
  }
}

int evasiveCounter = 0;
int moveCounter = random(5, 10);
int waitCounter = 0;
int moveType = random(1, 4);
void move() {
  if (startEvading == true) { // evasive maneuvers for 10ish seconds
    if (evasiveCounter < 1000) {
      String s = "evading |";
      logger(s + evasiveCounter + "|" + waitCounter + "|" + moveCounter + "|" + startEvading + "|" + motorDirection);
      evasiveCounter++;
      if (waitCounter == 0) {
        
        if (moveCounter > 0) {
          moveCounter--;
          analogWrite(enablePin, 1000);
        } else {
          moveCounter = random(5, 10);
          waitCounter = random(15, 50);
          analogWrite(enablePin, 0);
          changeMotorDir();
          
        }
      } else {
        
        waitCounter--;
        
      } 
      
    } else {
      startEvading = false;
      evasiveCounter = 0;
    }
  }
}


void changeMotorDir() {
  
  motorDirection = !motorDirection;
  if (motorDirection == 1) {
    digitalWrite(motorControlPin1, HIGH);
    digitalWrite(motorControlPin2, LOW);
  } else {
    digitalWrite(motorControlPin1, LOW);
    digitalWrite(motorControlPin2, HIGH);
  }
}
   
int counter = 0;
int wallWouldBeDisplayedCounter = 0;
int holdWallDisplayCounter = random(1, 4);
void moveWall(){
  counter++;
  if (detectingHand && distance < 15 && switchState == OFF) {
    if (wallWouldbeDisplayed == false){
      wallWouldBeDisplayedCounter++;
        
      if(wallWouldBeDisplayedCounter > holdWallDisplayCounter) {
        logger("wall");  
        wall.write(wallReachAngle);
        wallWouldBeDisplayedCounter = 0;
        holdWallDisplayCounter = random(1, 4);
      }
      wallWouldbeDisplayed = true;
      counter = 0;
    }
  } else if (wallWouldbeDisplayed == true && counter > 50) {
    wall.write(wallStorageAngle); //will be called extra times
    wallWouldbeDisplayed = false;
    logger("hiddingwall");
  }
}

bool distanceSensing(){
  distance = sonar.ping_cm();
  if (distance != 0 && distance < MAX_DISTANCE) {
    return true;
    // Serial.println("hand detecting");
    
  } else {
    return false;
    
  }

}
int logcounter = 0;
void logger (String s) {
  Serial.println(logcounter++ + s);
}
