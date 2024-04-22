#include "Arduino.h"
#include "MyStepper.h"

MyStepper::MyStepper(int motorPin1, int motorPin2, int motorPin3, int motorPin4, int stepsLimit) {
  this->motorPin1 = motorPin1;
  this->motorPin2 = motorPin2;
  this->motorPin3 = motorPin3;
  this->motorPin4 = motorPin4;
  this->stepsLimit = stepsLimit;
  this->stepNo = 0;
  this->stepNoTotal = 0;

  pinMode(this->motorPin1, OUTPUT);
  pinMode(this->motorPin2, OUTPUT);
  pinMode(this->motorPin3, OUTPUT);
  pinMode(this->motorPin4, OUTPUT);

  digitalWrite(this->motorPin1, LOW);
  digitalWrite(this->motorPin2, LOW);
  digitalWrite(this->motorPin3, LOW);
  digitalWrite(this->motorPin4, LOW);
}


int MyStepper::motorSpinning(int dir, bool autoWork){
  if(autoWork == true && (this->stepNoTotal > this->stepsLimit)) {
    dir = 0;
  }
  if(dir == 1) { 
    MyStepper::oneStepMotor(1, autoWork);   //spin clockwise
    delay(2);
  }
  else if(dir == -1) { 
    MyStepper::oneStepMotor(-1, autoWork);   //spin counter clockwise
    delay(2);
  }
  else {
    MyStepper::oneStepMotor(0, false);  // stop spinning
    this->stepNoTotal = 0;
  }
  return dir;
}


void MyStepper::oneStepMotor(int dir, bool autoWork){
  if(autoWork){
    this->stepNoTotal++;
  }
  if(dir == 1){
    switch(this->stepNo){
      case 0:
        digitalWrite(motorPin1, HIGH);
        digitalWrite(motorPin2, LOW);
        digitalWrite(motorPin3, LOW);
        digitalWrite(motorPin4, LOW);
        break;
      case 1:
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, HIGH);
        digitalWrite(motorPin3, LOW);
        digitalWrite(motorPin4, LOW);
        break;
      case 2:
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
        digitalWrite(motorPin3, HIGH);
        digitalWrite(motorPin4, LOW);
        break;
      case 3:
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
        digitalWrite(motorPin3, LOW);
        digitalWrite(motorPin4, HIGH);
        break;
    } 
  } else if(dir == -1) {
    switch(this->stepNo){
      case LOW:
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
        digitalWrite(motorPin3, LOW);
        digitalWrite(motorPin4, HIGH);
        break;
      case 1:
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, LOW);
        digitalWrite(motorPin3, HIGH);
        digitalWrite(motorPin4, LOW);
        break;
      case 2:
        digitalWrite(motorPin1, LOW);
        digitalWrite(motorPin2, HIGH);
        digitalWrite(motorPin3, LOW);
        digitalWrite(motorPin4, LOW);
        break;
      case 3:
        digitalWrite(motorPin1, HIGH);
        digitalWrite(motorPin2, LOW);
        digitalWrite(motorPin3, LOW);
        digitalWrite(motorPin4, LOW);
        break;
    } 
  } else {  //stop spinning
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
  }
  
  if(dir == 1 || dir == -1){
    this->stepNo++;
    if(this->stepNo > 3){
      this->stepNo = 0;
    }
  }
}
