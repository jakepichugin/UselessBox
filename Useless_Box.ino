#include <Servo.h>
#pragma region Distance Sensor config
#include <NewPing.h>



Servo arm;
int storageAngle = 70;
int reachAngle = 180;
int peekAngle = 120;
int armPin = 11;

int switchPin = 12;
int switchState = 0;
int flickNum = 0;

Servo wall;
bool wallHidden = true;
int wallStorageAngle = 60;
int wallReachAngle = 0;
int wallPin = 8;

int TRIGGER_PIN = 10; // Arduino pin tied to trigger pin on the ultrasonic sensor (HCSR04).
int ECHO_PIN = 9;      // Arduino pin tied to echo pin on the ultrasonic sensor.
int MAX_DISTANCE = 20;  // Max distance in centimeters that the ultrasonic sensor will look for

int distance = 0;
bool detectingHand = false;
#pragma endregion Distance Sensor config
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
// max dist can be increased but further distances can take longer to read which could disrupt balancing


int ON = LOW;
int OFF = HIGH; 
int prevState = ON;

void setup() {
  arm.attach(armPin);  
  wall.attach(wallPin);

  pinMode(switchPin, INPUT);  // switch
  pinMode(13, OUTPUT);        //built in light
  pinMode(armPin, OUTPUT);    // arm

  pinMode(wallPin, OUTPUT);

  pinMode(TRIGGER_PIN, INPUT); 
  pinMode(ECHO_PIN, INPUT);
  

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  switchState = digitalRead(switchPin);
  distanceSensing();

  if (switchState == OFF){ //switch in the prefered position
    moveWall();
    if (prevState == ON) {
      prevState = OFF;
      arm.write(storageAngle);
    }
    
  } else { // switch not in the perfered position
   
    if (prevState == OFF) {
      reachAction(flickNum);
      flickNum++;
      prevState = ON;
      // Serial.print("flicked: ");
      // Serial.println(flickNum);
    }
    digitalWrite(13, LOW); // turning on built in light

    
  }
  
  delay(15);
}

void reachAction(int attempt){
  if(attempt == 0) {
    delay(1000);
    arm.write(reachAngle);
  } else if(attempt < 20) {
      arm.write(reachAngle);
  } else {
    arm.write(reachAngle);
  }
}

int counter = 0;
void moveWall(){
  counter++;
  if (detectingHand && distance < 15 && switchState == OFF) {
    if (wallHidden == true){
      logger("wall extending");
      wall.write(wallReachAngle);
      wallHidden = false;
      counter = 0;
    }
  } else {
    if (wallHidden == false && counter > 25) {
      wall.write(wallStorageAngle);
      wallHidden = true;
    }
  }
}

void distanceSensing(){
  distance = sonar.ping_cm();
  if (distance != 0 && distance < MAX_DISTANCE) {
    detectingHand = true;
    // Serial.println("hand detecting");
    
  } else {
    detectingHand = false;
    
  }

}
int logcounter = 0;
void logger (String s) {
  Serial.println(logcounter++ + s);
}
