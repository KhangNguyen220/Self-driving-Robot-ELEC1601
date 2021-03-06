#include <LiquidCrystal.h>
int contrast = 20;
int contrastPin = 8;
//One for each button
int analogOne = A2;
int analogTwo = A3;
int analogThree = A4;
int analogFour = A5;
// ###                
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  
  Serial.begin(9600); 
  
  // ## OUR OWN SETUP ##
  analogWrite(contrastPin, contrast);
  lcd.begin(16, 2);
  pinMode(analogOne, INPUT);
  pinMode(analogTwo, INPUT);
  pinMode(analogThree, INPUT);
  pinMode(analogFour, INPUT);
  
}

void loop() {
  
    int buttonOne = analogRead(analogOne);
    int buttonTwo = analogRead(analogTwo);
    int buttonThree = analogRead(analogThree);
    int buttonFour = analogRead(analogFour);

        if (isButtonPressed(buttonOne)) {
          show("1", "PRESSED")
        }
        else if (isButtonPressed(buttonTwo)) {
          show("2", "PRESSED")
        } 
        else if (isButtonPressed(buttonThree)) {
          show("3", "PRESSED")
        } 
        else if (isButtonPressed(buttonFour)) {
          show("4", "PRESSED")
        }
}


// ### THIS FUNCTION NEEDS TESTING ASAP! ####
boolean isButtonPressed(float convertedVoltage) {
  if(convertedVoltage >= 500) {  //this i am unsure of, needs to be tested
    return true;   
  }
  return false;
}

void show(String lineOne, String lineTwo) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(lineOne);
  lcd.setCursor(0,1);
  lcd.print(lineTwo);
}

void wait() {
  timeNow = millis();
  while(millis() < timeNow + period) {
    //do nothing i.e. do not send a signal if one was already sent for a second
  }
}
