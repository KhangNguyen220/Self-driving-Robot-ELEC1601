//-----------------------------------------------------------------------------------------------------------//
//                                                                                                           //
//  Slave_ELEC1601_Student_2019_v3                                                                           //
//  The Instructor version of this code is identical to this version EXCEPT it also sets PIN codes           //
//  20191008 Peter Jones                                                                                     //
//                                                                                                           //
//  Bi-directional passing of serial inputs via Bluetooth                                                    //
//  Note: the void loop() contents differ from "capitalise and return" code                                  //
//                                                                                                           //
//  This version was initially based on the 2011 Steve Chang code but has been substantially revised         //
//  and heavily documented throughout.                                                                       //
//                                                                                                           //
//  20190927 Ross Hutton                                                                                     //
//  Identified that opening the Arduino IDE Serial Monitor asserts a DTR signal which resets the Arduino,    //
//  causing it to re-execute the full connection setup routine. If this reset happens on the Slave system,   //
//  re-running the setup routine appears to drop the connection. The Master is unaware of this loss and      //
//  makes no attempt to re-connect. Code has been added to check if the Bluetooth connection remains         //
//  established and, if so, the setup process is bypassed.                                                   //
//                                                                                                           //
//-----------------------------------------------------------------------------------------------------------//

#include <SoftwareSerial.h>   //Software Serial Port
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

//use this for the "waiting" after a button press
int period = 1000; 
unsigned long timeNow = 0;
// ###

#define RxD 7
#define TxD 6
#define ConnStatus A1

#define DEBUG_ENABLED  1

// ##################################################################################
// ### EDIT THE LINES BELOW TO MATCH YOUR SHIELD NUMBER AND CONNECTION PIN OPTION ###
// ##################################################################################

int shieldPairNumber = 13;

// CAUTION: If ConnStatusSupported = true you MUST NOT use pin A1 otherwise "random" reboots will occur
// CAUTION: If ConnStatusSupported = true you MUST set the PIO[1] switch to A1 (not NC)

boolean ConnStatusSupported = true;   // Set to "true" when digital connection status is available on Arduino pin

// #######################################################

// The following two string variable are used to simplify adaptation of code to different shield pairs

String slaveNameCmd = "\r\n+STNA=Slave";   // This is concatenated with shieldPairNumber later

SoftwareSerial blueToothSerial(RxD,TxD);

void setup() {
  
  Serial.begin(9600);
  blueToothSerial.begin(38400);                    // Set Bluetooth module to default baud rate 38400
    
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  pinMode(ConnStatus, INPUT);
  
  //  Check whether Master and Slave are already connected by polling the ConnStatus pin (A1 on SeeedStudio v1 shield)
  //  This prevents running the full connection setup routine if not necessary.

  if(ConnStatusSupported) Serial.println("Checking Slave-Master connection status.");

  if(ConnStatusSupported && digitalRead(ConnStatus)==1)
  {
    Serial.println("Already connected to Master - remove USB cable if reboot of Master Bluetooth required.");
  }
  else
  {
    Serial.println("Not connected to Master.");
        
    setupBlueToothConnection();   // Set up the local (slave) Bluetooth module

    delay(1000);                  // Wait one second and flush the serial buffers
    Serial.flush();
    blueToothSerial.flush();
  }  
  
  // ## OUR OWN SETUP ##
  analogWrite(contrastPin, contrast);
  lcd.begin(16, 2);
  pinMode(analogOne, INPUT);
  pinMode(analogTwo, INPUT);
  pinMode(analogThree, INPUT);
  pinMode(analogFour, INPUT);
  
}

void loop() {
  
  char recvChar;
  
  while(1)
  {
    if(blueToothSerial.available())   // Check if there's any data sent from the remote Bluetooth shield
    {
      recvChar = blueToothSerial.read();
      if (recvChar == 'a') {
        show("IDLE AT", "KITCHEN");
      }
      else if (recvChar == 'b') {
        show("IDLE AT", "TABLE 1");   
            }
      else if (recvChar == 'c') {
        show("IDLE AT", "TABLE 2");               
      }
      else if (recvChar == 'A') {
        show("MOVING TO", "KITCHEN");      
      }
      else if (recvChar == 'B') {
        show("MOVING TO", "TABLE 1");    
      }
      else if (recvChar == 'C') {
        show("MOVING TO", "TABLE 2");    
      }
    }  
  
    int buttonOne = analogRead(analogOne);
    int buttonTwo = analogRead(analogTwo);
    int buttonThree = analogRead(analogThree);
    int buttonFour = analogRead(analogFour);
    
    if (isButtonPressed(buttonOne)) {
          sendSignal('1');
          //wait is in sendSignal() function  
        }
    else if (isButtonPressed(buttonTwo)) {
          sendSignal('2');
        } 
    else if (isButtonPressed(buttonThree)) {
          sendSignal('3');
        } 
    else if (isButtonPressed(buttonFour)) {
          sendSignal('4');
        }
  }
}



// ### THIS FUNCTION NEEDS TESTING ASAP! ####
boolean isButtonPressed(float convertedVoltage) {
  if(convertedVoltage >= 500) {  //this i am unsure of, needs to be tested
    return true;   
  }
  return false;
}

void sendSignal(char command) {
  blueToothSerial.println(command);
  wait();
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
    //do nothing
  }
}

void setupBlueToothConnection() {
  
    Serial.println("Setting up the local (slave) Bluetooth module.");

    slaveNameCmd += shieldPairNumber;
    slaveNameCmd += "\r\n";

    blueToothSerial.print("\r\n+STWMOD=0\r\n");      // Set the Bluetooth to work in slave mode
    blueToothSerial.print(slaveNameCmd);             // Set the Bluetooth name using slaveNameCmd
    blueToothSerial.print("\r\n+STAUTO=0\r\n");      // Auto-connection should be forbidden here
    blueToothSerial.print("\r\n+STOAUT=1\r\n");      // Permit paired device to connect me
    
    //  print() sets up a transmit/outgoing buffer for the string which is then transmitted via interrupts one character at a time.
    //  This allows the program to keep running, with the transmitting happening in the background.
    //  Serial.flush() does not empty this buffer, instead it pauses the program until all Serial.print()ing is done.
    //  This is useful if there is critical timing mixed in with Serial.print()s.
    //  To clear an "incoming" serial buffer, use while(Serial.available()){Serial.read();}

    blueToothSerial.flush();
    delay(2000);                                     // This delay is required

    blueToothSerial.print("\r\n+INQ=1\r\n");         // Make the slave Bluetooth inquirable
    
    blueToothSerial.flush();
    delay(2000);                                     // This delay is required
    
    Serial.println("The slave bluetooth is inquirable!");
}
