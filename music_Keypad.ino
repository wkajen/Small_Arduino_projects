#include "pitches.h"
#include <Keypad.h>
// #define BUTTON 5
#define BUZZ 11

const byte ROWS = 4;
const byte COLS = 4;

char KeyNotes[ROWS][COLS] = {
                            {'1', '2', '3', '4'},
                            {'5', '6', '7', '8'},
                            {'A', 'B', 'C', 'D'},
                            {'E', 'F', 'G', 'H'}
                            };
byte colPins[ROWS] = {7, 8, 9, 10};
byte rowPins[COLS] = {6, 5, 4, 3};

//initialize an instance of class Keypad
Keypad customKeypad = Keypad(makeKeymap(KeyNotes), rowPins, colPins, ROWS, COLS); 

// notes in the welcomeMelody:
int welcomeMelody[] = {NOTE_C5, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, 0, NOTE_B4, NOTE_C5};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurationStart[] = {4, 8, 8, 4, 4, 4, 4, 4};

// int myMelody[] = {NOTE_G4, NOTE_E4, NOTE_E4, NOTE_F4, NOTE_D4, NOTE_D4, NOTE_C4, NOTE_E4, NOTE_G4, 0};
// int myMelodyDuration[] = {4,4,4,4,4,4,8,8,2,2};
// int musicNo = 0;

void setup() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000 / noteDurationStart[thisNote];
    tone(BUZZ, welcomeMelody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(BUZZ);
  }
}

void loop() {
  int customKey = customKeypad.getKey();
  KeypadNotes2(customKey);
}

void KeypadNotes(int customKey){
  if (customKey == '1') {
    tone(BUZZ, NOTE_C4, 200);
  }
  if (customKey == '2') {
    tone(BUZZ, NOTE_D4, 200);
  }
  if (customKey == '3') {
    tone(BUZZ, NOTE_E4, 200);
  }
  if (customKey == '4') {
    tone(BUZZ, NOTE_F4, 200);
  }
  if (customKey == '5') {
    tone(BUZZ, NOTE_G4, 200);
  }
  if (customKey == '6') {
    tone(BUZZ, NOTE_A4, 200);
  }
  if (customKey == '7') {
    tone(BUZZ, NOTE_B4, 200);
  }
  if (customKey == '8') {
    tone(BUZZ, NOTE_C5, 200);
  }
  if (customKey == 'A') {
    tone(BUZZ, NOTE_C5, 200);
  }
  if (customKey == 'B') {
    tone(BUZZ, NOTE_D5, 200);
  }
  if (customKey == 'C') {
    tone(BUZZ, NOTE_E5, 200);
  }
  if (customKey == 'D') {
    tone(BUZZ, NOTE_F5, 200);
  }
  if (customKey == 'E') {
    tone(BUZZ, NOTE_G5, 200);
  }
  if (customKey == 'F') {
    tone(BUZZ, NOTE_A5, 200);
  }
  if (customKey == 'G') {
    tone(BUZZ, NOTE_B5, 200);
  }
  if (customKey == 'H') {
    tone(BUZZ, NOTE_C6, 200);
  }
  
  delay(200);
  noTone(BUZZ);
}
