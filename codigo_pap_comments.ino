// This line includes the library for the I2C communication protocol
#include <Wire.h>
// This line includes the library for the I2C Liquid Crystal display
#include <LiquidCrystal_I2C.h>
// This line includes the library for controlling servo motors
#include <Servo.h>
// This line includes the library for the MFRC522 RFID reader
#include <MFRC522.h>
// Declare constant variables for the pin assignments for the red LED, green LED, servo motor and piezo buzzer
const int redLedPin = 4;
const int greenLedPin = 2;
const int servoPin = 6;
const int piezoPin = 8;
// Declare a Servo object named "myservo"
Servo myservo;
// Declare variables for the door state and switch state
bool Estado_Porta = false;
bool switchState = false;
// Create an instance of the I2C Liquid Crystal display, with the address 0x27, 16 columns and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Define constants for the reset pin and slave select pin for the MFRC522 RFID reader
#define RST_PIN 9
#define SS_PIN 10
// Define constants for the limit switch state (active or inactive)
#define LIMIT_ACTIVE HIGH
#define LIMIT_INACTIVE LOW
// Define a constant for the switch button pin
#define switchButton 3
// Create an instance of the MFRC522 class for the RFID reader with the defined reset pin and slave select pin
MFRC522 mfrc522(SS_PIN, RST_PIN);
// Declare a function to verify an RFID card using the provided key, block and length
bool RFIDVerify(MFRC522::MIFARE_Key key, byte block, byte len, MFRC522::StatusCode status);
// Declare a function to open the door
void AbrirPorta();
// Declare a function to close the door
void FecharPorta();
// Declare a function to indicate that the door is open
void PortaAberta();
// Declare a function to indicate that the card was not accepted
void CartaoNegado();
//initial setup
void setup() {
// Initialize the pins for red LED, green LED and the switch button
pinMode(redLedPin, OUTPUT);
pinMode(greenLedPin, OUTPUT);
pinMode(switchButton, INPUT);
// Attach the servo motor to the pin specified for the servo
myservo.attach(servoPin);
// Start serial communication at 9600 baud rate
Serial.begin(9600);
// Initialize the LCD display
lcd.begin();
// Initialize SPI communication
SPI.begin();
// Initialize MFRC522 instance
mfrc522.PCD_Init();
// Blink both LEDs 3 times and display "A Testar..." on the LCD
lcd.print("A Testar...");
for (int i = 0; i < 3; i++) {
digitalWrite(redLedPin, HIGH);
digitalWrite(greenLedPin, HIGH);
delay(500);
digitalWrite(redLedPin, LOW);
digitalWrite(greenLedPin, LOW);
delay(500);
}
// Clear the LCD and display "Operacional"
lcd.clear();
lcd.print("Operacional");
delay(1000);
// Clear the LCD and display "Passe o cartao"
lcd.clear();
lcd.print("Passe o cartao");
}
//Main loop
void loop() {
// Get the state of the switch button
switchState = digitalRead(switchButton);
// Print the state of the switch button to the serial monitor
Serial.println(switchState);
// Prepare key for the MFRC522 library
// All keys are set to FFFFFFFFFFFFh at chip delivery from the factory
MFRC522::MIFARE_Key key;
for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
// Declare variables to be used later in the code
byte block;
byte len;
MFRC522::StatusCode status;
// Check if there is a new card present on the reader
if ( ! mfrc522.PICC_IsNewCardPresent()) {
  // If there is no new card, exit the loop
  return false;
}
// Select one of the cards if there is a new card present
if ( ! mfrc522.PICC_ReadCardSerial()) {
  // If a card cannot be selected, exit the loop
  return false;
}
// Print a message indicating that a card has been detected
Serial.println(F("**Card Detected:**"));
// Check if the card is verified and the door state
// If the card is verified and the door is closed, open the door
if ( RFIDVerify(key, block, len, status) && (Estado_Porta == false)) {
  AbrirPorta();
} 
// If the card is verified and the door is open and the switch is off, close the door
else if ( RFIDVerify(key, block, len, status) && (Estado_Porta == true) && (switchState == false) ) {
  FecharPorta();
} 
// If the card is verified and the door is open and the switch is on, keep the door open
else if ( RFIDVerify(key, block, len, status) && (Estado_Porta == true) && (switchState == true) ) {
  PortaAberta();
} 
// If the card is not verified, deny access
else {
  CartaoNegado();
}
// Halt the selected card
mfrc522.PICC_HaltA();
// Stop the crypto module
mfrc522.PCD_StopCrypto1();
}

// This function is used to verify an RFID card
bool RFIDVerify(MFRC522::MIFARE_Key key, byte block, byte len, MFRC522::StatusCode status) {
// Print "Name: " on the serial monitor
Serial.print(F("Name: "));
byte buffer1[18];
// Set the value of block to 4
block = 4;
// Set the length of the buffer to 18
len = 18;

// Authenticate the card
status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
// If the authentication fails, print an error message on the serial monitor and return false
if (status != MFRC522::STATUS_OK) {
Serial.print(F("Authentication failed: "));
Serial.println(mfrc522.GetStatusCodeName(status));
return false;
}

// Read data from the card
status = mfrc522.MIFARE_Read(block, buffer1, &len);
// If the read operation fails, print an error message on the serial monitor and return false
if (status != MFRC522::STATUS_OK) {
Serial.print(F("Reading failed: "));
Serial.println(mfrc522.GetStatusCodeName(status));
return false;
}

// Create a String object to store the name read from the card
String Nome;
// Print the first name on the serial monitor
for (uint8_t i = 0; i < 16; i++) {
// Check if the current character is not a space
if (buffer1[i] != 32) {
// Print the character on the serial monitor
Serial.write(buffer1[i]);
// Add the character to the name string
Nome += String((char)buffer1[i]);
}
}
// Print a space after the first name
Serial.print(" ");
// Trim the name string
Nome.trim();
// Print "End Reading" on the serial monitor
Serial.println(F("\nEnd Reading\n"));
// Check if the name is equal to "Alexandru"
if (Nome == "Alexandru") {
// Return true if the name is equal to "Alexandru"
return true;
} else {
// Return false if the name is not equal to "Alexandru"
return false;
}
}

void AbrirPorta() {
// This function opens the door
// Turn on green LED
digitalWrite(greenLedPin, HIGH);
// Clear the LCD display
lcd.clear();
// Set cursor position on the first line, first column
lcd.setCursor(0, 0);
// Print message "Acesso garantido" on the display
lcd.print("Acesso garantido");
// Play a tone on the piezo speaker with frequency 1000Hz for 500 milliseconds
tone(piezoPin, 1000, 500);
// Move servo to 0 degrees position
myservo.write(0);
// Delay for 225 milliseconds
delay(225);
// Move servo to 90 degrees position
myservo.write(90);
// Delay for 750 milliseconds
delay(750);
// Turn off green LED
digitalWrite(greenLedPin, LOW);
// Clear the LCD display
lcd.clear();
// Set cursor position on the first line, first column
lcd.setCursor(0, 0);
// Print message "Feche a Porta e" on the first line of the display
lcd.print("Feche a Porta e");
// Set cursor position on the second line, first column
lcd.setCursor(0, 1);
// Print message "Passe o cartao!" on the second line of the display
lcd.print("Passe o cartao!");
// Set the state of the door to open
Estado_Porta = true;
}

void FecharPorta() {
//Turn on green LED
digitalWrite(greenLedPin, HIGH);
//Turn on red LED
digitalWrite(redLedPin, HIGH);
//Clear the LCD screen
lcd.clear();
//Set the cursor to the first line, first column
lcd.setCursor(0, 0);
//Print "A Fechar Porta" on the first line of the LCD screen
lcd.print("A Fechar Porta");
//Play a tone on the piezoPin at a frequency of 1000 Hz for 200 milliseconds
tone(piezoPin, 1000, 200);
//Rotate the servo to 180 degrees
myservo.write(180);
//Delay for 250 milliseconds
delay(250);
//Rotate the servo back to 90 degrees
myservo.write(90);
//Turn off green LED
digitalWrite(greenLedPin, LOW);
//Turn off red LED
digitalWrite(redLedPin, LOW);
//Clear the LCD screen
lcd.clear();
//Set the cursor to the first line, first column
lcd.setCursor(0, 0);
//Print "Porta Fechada!" on the first line of the LCD screen
lcd.print("Porta Fechada!");
//Update the state of the door to "closed"
Estado_Porta = false;
//Delay for 750 milliseconds
delay(750);
//Clear the LCD screen
lcd.clear();
//Set the cursor to the first line, first column
lcd.setCursor(0, 0);
//Print "Passe o cartao" on the first line of the LCD screen
lcd.print("Passe o cartao");
}

void PortaAberta() {
// Turn on the red LED
digitalWrite(redLedPin, HIGH);
// Clear the LCD screen
lcd.clear();
// Set cursor position on the first line and first column
lcd.setCursor(0, 0);
// Print message "Feche a Porta e" on the first line
lcd.print("Feche a Porta e");
// Set cursor position on the second line and first column
lcd.setCursor(0, 1);
// Print message "Passe o cartao" on the second line
lcd.print("Passe o cartao");
// Play a tone on the piezo pin for 200 milliseconds
tone(piezoPin, 1000, 200);
// Wait for 250 milliseconds
delay(250);
// Play another tone on the piezo pin for 200 milliseconds
tone(piezoPin, 1000, 200);
// Wait for 250 milliseconds
delay(250);
// Play another tone on the piezo pin for 200 milliseconds
tone(piezoPin, 1000, 200);
// Turn off the red LED
digitalWrite(redLedPin, LOW);
}

void CartaoNegado() {
//Turns on the red LED
digitalWrite(redLedPin, HIGH);
//Clears the LCD screen
lcd.clear();
//Sets the cursor position on the LCD screen to row 0, column 0
lcd.setCursor(0, 0);
//Prints "Acesso negado" on the LCD screen
lcd.print("Acesso negado");
//Outputs a tone from the piezo speaker with a frequency of 1000 and a duration of 200 milliseconds
tone(piezoPin, 1000, 200);
//Waits for 250 milliseconds
delay(250);
//Outputs another tone from the piezo speaker with a frequency of 1000 and a duration of 200 milliseconds
tone(piezoPin, 1000, 200);
//Waits for another 250 milliseconds
delay(250);
//Outputs yet another tone from the piezo speaker with a frequency of 1000 and a duration of 200 milliseconds
tone(piezoPin, 1000, 200);
//Waits for 500 milliseconds
delay(500);
//Clears the LCD screen
lcd.clear();
//Sets the cursor position on the LCD screen to row 0, column 0
lcd.setCursor(0, 0);
//Prints "Passe o cartao" on the LCD screen
lcd.print("Passe o cartao");
//Turns off the red LED
digitalWrite(redLedPin, LOW);
}
