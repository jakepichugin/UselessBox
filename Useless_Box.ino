#include <Servo.h>

Servo arm;

int storageAngle = 70;
int reachAngle = 180;
int peekAngle = 120;
int armPin = 11;

int switchPin = 12;
int switchState = 0;
int flickNum = 0;

String prevState = "ON";

void setup() {
  arm.attach(armPin);  

  pinMode(switchPin, INPUT);  // switch
  pinMode(13, OUTPUT);        //built in light
  pinMode(armPin, OUTPUT);    // arm

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  switchState = digitalRead(switchPin);

  if (switchState == HIGH){ //switch in the prefered position
    digitalWrite(13, HIGH);
    arm.write(storageAngle);
    if (prevState == "ON") {
      prevState = "OFF";
    }
    
  } else { // switch not in the perfered position
   
    if (prevState == "OFF") {
      reachAction(flickNum);
      flickNum++;
      prevState = "ON";
      Serial.print("flicked: ");
      Serial.println(flickNum);
    }
    digitalWrite(13, LOW); // turning on built in light

    
  }
  
  delay(15);
}

void reachAction(int attempt){
  if(attempt == 0) {
    delay(3000);
    arm.write(reachAngle);
  } else if(attempt < 20) {
    switch (random(0, 7)) {
      case 1 & 4 & 5 & 6 & 7:
        arm.write(reachAngle);
        break;
      case 3:
        Serial.print("Peaking");
        arm.write(peekAngle);
        delay(2000);
        arm.write(reachAngle);
        break;
      case 2:
      Serial.print("Edging");
        for (int i = 0; i < 90; i++) {
          arm.write(storageAngle + i);
          delay(15);
        }
        arm.write(reachAngle);
      default:
        arm.write(reachAngle);
        break;
    }
  } else {
    arm.write(reachAngle);
  }
}
