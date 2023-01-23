#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Declare variables
const int redLedPin = 13;
const int redButtonPin = 2;
const int greenLedPin = 12;
const int greenButtonPin = 4;
const int servoPin = 9;

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// Initial setup
void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(redButtonPin, INPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(greenButtonPin, INPUT);
  
  myservo.attach(servoPin);
  
  lcd.begin(16, 2);
  
  // Blink both LEDs 3 times
  for (int i = 0; i < 3; i++) {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, HIGH);
    delay(500);
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    delay(500);
  }
  
  lcd.clear();
  lcd.print("Operacional");
  delay(2000);
  lcd.clear();
  lcd.print("Passe o cartao");
}

// Main loop
void loop() {
  if (digitalRead(greenButtonPin) == HIGH) {
    digitalWrite(greenLedPin, HIGH);
    lcd.clear();
    lcd.print("Acesso garantido");
    myservo.write(0);
    delay(225);
    myservo.write(90);
    delay(210);
    myservo.write(180);
    delay(250);
    myservo.write(90);
    digitalWrite(greenLedPin, LOW);
    lcd.clear();
    lcd.print("Passe o cartao");
  }
  else if (digitalRead(redButtonPin) == HIGH) {
    digitalWrite(redLedPin, HIGH);
    lcd.clear();
    lcd.print("Acesso negado");
    delay(3000);
    lcd.clear();
    lcd.print("Passe o cartao");
    digitalWrite(redLedPin, LOW);
  }
}
