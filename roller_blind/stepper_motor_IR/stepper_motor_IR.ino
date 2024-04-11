// made by Kajetan Walawski
#include <IRremote.h>

#define FOTORES_PIN A5
#define FOTORES_THR_NIGHT 68
#define FOTORES_THR_DAY 165

// all digital PINS plugged in
const int irPin = 2; 
const int motorPin1 = 8;
const int motorPin2 = 9;
const int motorPin3 = 10;
const int motorPin4 = 11;
const int greenLedPin = 12;
const int redLedPin = 13;

// recorded values of signals from the IR pilot
const int leftArrow = 0xFF22DD;  //0xC8EE0693;
const int rightArrow = 0xFF02FD; //0xDD332AF;
const int upArrow = 0xFFA857;    //0x2F7D0DEF;
const int downArrow = 0xFFE01F;  //0x47E58AD3;
const int zeroKey = 0xFF6897;
const int oneKey = 0xFF30CF;
const int twoKey = 0xFF18E7;
const int threeKey = 0xFF7A85;
const int infoKey = 0xFF906F;
const int stopKey = 0xFFC23D; 

IRrecv irrecv(irPin);
decode_results results;

// global variables
int stepNo = 0;
int stepNoTotal = 0;
int stepsLimit = 25000;
int receivedVal = 0;
bool spinCW = false;
bool spinCCW = false;
bool isUp = false;
bool isDown = false;
bool isAutoWork = false;
boolean redLedOn = false;
boolean greenLedOn = false;

void setup() {
  // Serial.begin(9600); //UART - for debbuging
  irrecv.enableIRIn(); //enable receiving data

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

  if(isAutoWork) {
    AutoWork();
    LedLight(false, false);
  } 
  else {
    LedLight(false, true);
  }
}


void IRSignalFun(){
  if (irrecv.decode(&results)) {  // when IR signal received
    receivedVal = results.value;
    switch(receivedVal){
      case leftArrow:   // spin clockwise
        spinCW = true; 
        spinCCW = false;
        break;
      case rightArrow:  // spin counter clockwise
        spinCW = false;
        spinCCW = true;
        break;
      case stopKey:      // turn off motor spinning
        spinCW = false;
        spinCCW = false;
        break;
      /* //just for debbuging
      case infoKey:
        Serial.print("Light = ");
        Serial.print(analogRead(FOTORES_PIN));
        Serial.print(" ; isUp = ");
        Serial.print(isUp);
        Serial.print(" ; isDown = ");
        Serial.print(isDown);
        Serial.print(" ; AUTO = ");
        Serial.print(isAutoWork); 
        Serial.print(" ; StepLimit = ");
        Serial.print(stepsLimit); 
        Serial.print(" ; Step = ");
        Serial.println(stepNoTotal);
        break;
      */
      case zeroKey:                  // set UP position
        isUp = true;
        isDown = false;
        LedLight(true, false);       // visualize the change
        delay(250);
        LedLight(false, false);
        break;
      case oneKey:                   // set DOWN position
        isUp = false;
        isDown = true;
        LedLight(true, false);       // visualize the change
        delay(250);
        LedLight(false, false);
        break;
      case twoKey:                   // toggle auto work
        isAutoWork ^= true;
        stepNoTotal = 0;
        break;  
      case threeKey:                 // option to change threshold
        stepsLimit = stepNoTotal;
        LedLight(true, false);
        delay(500);
        LedLight(false, false);
        break;
    }
    irrecv.resume(); // Receive the next value
  }
}

void MotorSpinning(){
  if(spinCW) { 
    OneStepMotor(1); 
    delay(2);
  }
  else if(spinCCW) { 
    OneStepMotor(2);
    delay(2);
  }
  else {
    OneStepMotor(0);  // stop spinning
    // redLedOn = false;
  }
}

void LedLight(bool redLedOn, bool greenLedOn){
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

void OneStepMotor(byte dir){
  if(dir == 1){
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
  } else if(dir == 2) {
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
  } else {  //stop spinning
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
  }
  
  if(dir == 1 || dir == 2){
    stepNo++;
    stepNoTotal++;
    if(stepNo > 3){
      stepNo = 0;
    }
  }
}

void AutoWork() {
  int fotoResVal = analogRead(FOTORES_PIN);

  if(fotoResVal < FOTORES_THR_NIGHT && isUp) {
    spinCW = true; 
    spinCCW = false;
  } 
  if(fotoResVal > FOTORES_THR_DAY && isDown) {
    spinCW = false; 
    spinCCW = true; 
  }
  if(isUp && stepNoTotal > stepsLimit) {
    isUp = false;
    spinCW = false; 
    spinCCW = false;
    stepNoTotal = 0;
    delay(500);
    isDown = true;
  }
  if(isDown && stepNoTotal > stepsLimit) {
    isDown = false;
    spinCW = false; 
    spinCCW = false;
    stepNoTotal = 0;
    delay(500);
    isUp = true;
  }
}
