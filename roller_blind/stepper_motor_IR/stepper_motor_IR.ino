// made by Kajetan Walawski
#include <IRremote.h>
#include "arduino-timer.h"

// all connected pins
const int irPin = 2; 
const int leftBtn = 6;
const int rightBtn = 7;
const int motorPin1 = 8;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;
const int greenLedPin = 12;
const int redLedPin = 13;

// decoded hex IR values
const int leftArrow = 0xC8EE0693;
const int rightArrow = 0xDD332AF;
const int upArrow = 0x2F7D0DEF;
const int downArrow = 0x47E58AD3;
const int okKey = 0x1CEDF82B;
const int playKey = 0x6220794F;
const int backKey = 0xA80FD90F;
const int menuKey = 0x20624873;

IRrecv irrecv(irPin);
decode_results irSignal;

auto timer = timer_create_default(); // create a timer with default settings

int stepNo = 0;
boolean spinCW = false;
boolean spinCCW = false;
boolean redLedOn = false;
boolean greenLedOn = false;

void setup() {
  irrecv.enableIRIn(); //enable receiving IR data

  pinMode(leftBtn, INPUT_PULLUP);
  pinMode(rightBtn, INPUT_PULLUP);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  digitalWrite(redLedPin, LOW);     // default low power on LEDs 
  digitalWrite(greenLedPin, LOW);
}

void loop() {
  // function calls
  IRSignalFun();
  MotorSpinning();
  LEDlights();

  // tick the timer
  timer.tick(); 
}

void IRSignalFun(){
  if (irrecv.decode(&irSignal)) {
    int receivedVal = irSignal.value;
    switch(receivedVal){
      case leftArrow:   // turn clockwise
        spinCW = true; 
        spinCCW = false;
        break;
      case rightArrow:  // turn counter clockwise
        spinCW = false;
        spinCCW = true;
        break;
      case backKey:      // turn off motor spinning
        spinCW = false;
        spinCCW = false;
        // timer.cancel();
        break;
      case upArrow:
        redLedOn ^= true;
        break;
      case downArrow:
        greenLedOn ^= true;
        break;
    }
    irrecv.resume();  // wait for new IR signal
  }
}

void MotorSpinning(){
  if(spinCW == true || digitalRead(leftBtn) == LOW) {           //left button pushed down
    OneStepMotor(true);                                         //spin CW
    delay(2);
    timer.every(1000, ToggleLed);
  }
  else if(spinCCW == true || digitalRead(rightBtn) == LOW) {    //right button pushed down
    OneStepMotor(false);                                        //spin CCW
    delay(2);
    timer.every(1000, ToggleLed);
  }
  else {
    timer.cancel();   // turn off all active timer actions 
  }
}

void LEDlights(){
  if(redLedOn == true) { 
    digitalWrite(redLedPin, HIGH);
  } else {
    digitalWrite(redLedPin, LOW);
  }
  if(greenLedOn == true) { 
    digitalWrite(greenLedPin, HIGH);
  } else {
    digitalWrite(greenLedPin, LOW);
  }
}

bool ToggleLed() {
  redLedOn ^= true; // toggle the LED
  return true;      // keep timer active? true
}

void OneStepMotor(bool dir){
  if(dir){
    switch(stepNo){
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
  } else{
    switch(stepNo){
      case 0:
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
  }
  
  stepNo++;
  if(stepNo > 3){
    stepNo = 0;
  }
}