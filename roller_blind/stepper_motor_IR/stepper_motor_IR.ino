#include <IRremote.h>   //official library
#include <MyStepper.h>  //created my library
#include "irPilotSignals.h"

#define FOTORES_PIN A5
#define FOTORES_THR_NIGHT 68
#define FOTORES_THR_DAY 165

// all digital PINS plugged in
const int irPin = 2; 
const int greenLedPin = 12;
const int redLedPin = 13;
int motorPin1 = 8;
int motorPin2 = 9;
int motorPin3 = 10;
int motorPin4 = 11;
int stepsLimit = 25000;

MyStepper stepper(motorPin1, motorPin2, motorPin3, motorPin4, stepsLimit);

IRrecv irrecv(irPin);
decode_results results;

bool isAutoWork = false;
int spinDir = 0;
int prevSpinDir = 0;
bool isUp = false;
bool isDown = false;

void setup() {
  irrecv.enableIRIn(); //enable receiving data
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);

  digitalWrite(redLedPin, LOW);     // default low power on LEDs 
  digitalWrite(greenLedPin, LOW);
}

void loop() {
  // function call to get signal value from IR pilot
  IRSignalFun();

  if(isAutoWork && spinDir != 2) {
    AutoWork();
    digitalWrite(greenLedPin, LOW);
  } 
  else {
    digitalWrite(greenLedPin, HIGH);
  }

  spinDir = stepper.motorSpinning(spinDir, isAutoWork);
}


void IRSignalFun(){
  if (irrecv.decode(&results)) {  // when IR signal received
    switch(results.value){
      case goDown:   // spin clockwise manually, with no limits
        spinDir = -1;
        break;
      case goUp:  // spin counter clockwise manually, with no limits
        spinDir = 1;
        break;
      case stopKey:      // turn off motor spinning
        spinDir = 2;
        break;
      case setUpPos:                  // set UP position
        isUp = true;
        isDown = false;
        digitalWrite(redLedPin, HIGH);       // visualize the change
        delay(250);
        digitalWrite(redLedPin, LOW);
        break;
      case setDownPos:                   // set DOWN position
        isUp = false;
        isDown = true;
        digitalWrite(redLedPin, HIGH);       // visualize the change
        delay(250);
        digitalWrite(redLedPin, LOW);
        break;
      case switchWork:                   // toggle auto=manual work
        isAutoWork ^= true;
        break;  
      case setTHR:                 // option to change threshold
        stepsLimit = stepNoTotal;
        digitalWrite(redLedPin, HIGH);
        delay(500);
        digitalWrite(redLedPin, LOW);
        break;
    }
    irrecv.resume(); // Receive the next value
  }
}

void AutoWork() {
  int fotoResVal = analogRead(FOTORES_PIN);

  if(spinDir == 0 && prevSpinDir == 1 && isDown) {  //blind was down, now is up
    delay(200);
    isUp = true;
    isDown = false;
    delay(200);
  }
  if(spinDir == 0 && prevSpinDir == -1 && isUp) {  //blind was up, now is down
    delay(200);
    isUp = false;
    isDown = true;
    delay(200);
  }
  if(fotoResVal < FOTORES_THR_NIGHT && isUp) {
    spinDir = -1;
    prevSpinDir = -1;
  } 
  if(fotoResVal > FOTORES_THR_DAY && isDown) {
    spinDir = 1;
    prevSpinDir = 1;
  }
}
