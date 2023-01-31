#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <MFRC522.h>

// Declare variables
const int redLedPin = 4;
const int greenLedPin = 2;
const int servoPin = 6;
const int piezoPin = 8;


Servo myservo;
bool Estado_Porta = false;
bool switchState = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
#define LIMIT_ACTIVE HIGH
#define LIMIT_INACTIVE LOW
#define switchButton 3

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

bool RFIDVerify(MFRC522::MIFARE_Key key, byte block, byte len, MFRC522::StatusCode status);
void AbrirPorta();
void FecharPorta();
void PortaAberta();
void CartaoNegado();

// Initial setup
void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(switchButton, INPUT);
  myservo.attach(servoPin);
  Serial.begin(9600);

  lcd.begin();
  SPI.begin();
  mfrc522.PCD_Init();
  switchState = digitalRead(switchButton);

  // Blink both LEDs 3 times
  lcd.print("A Testar...");
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
  delay(1000);
  lcd.clear();
  lcd.print("Passe o cartao");
}

// Main loop
void loop() {
  switchState = digitalRead(switchButton);
  Serial.println(switchState);

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  //some variables we need
  byte block;
  byte len;
  MFRC522::StatusCode status;

  //-------------------------------------------

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return false;
  }

  Serial.println(F("**Card Detected:**"));

  if ( RFIDVerify(key, block, len, status) && (Estado_Porta == false) && (switchState == false)) {
    AbrirPorta();
  } else if ( RFIDVerify(key, block, len, status) && (Estado_Porta == true) && (switchState == false) ) {
    FecharPorta();
  } else if ( RFIDVerify(key, block, len, status) && (Estado_Porta == true) && (switchState == true) ) {
    PortaAberta();
  } else {
    CartaoNegado();
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

bool RFIDVerify(MFRC522::MIFARE_Key key, byte block, byte len, MFRC522::StatusCode status) {
  Serial.print(F("Name: "));

  byte buffer1[18];

  block = 4;
  len = 18;

  //------------------------------------------- GET FIRST NAME
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid)); //line 834 of MFRC522.cpp file
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }

  String Nome;
  //PRINT FIRST NAME
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer1[i] != 32)
    {
      Serial.write(buffer1[i]);
      Nome += String((char)buffer1[i]);
    }
  }
  Serial.print(" ");
  Nome.trim();

  Serial.println(F("\n**End Reading**\n"));

  if (Nome == "Alexandru") {
    return true;
  } else {
    return false;
  }
}

void AbrirPorta() {
  digitalWrite(greenLedPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Acesso garantido");
  tone(piezoPin, 1000, 500);
  myservo.write(0);
  delay(225);
  myservo.write(90);
  delay(750); //Delay para display & led
  digitalWrite(greenLedPin, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Feche a Porta e");
  lcd.setCursor(0, 1);
  lcd.print("Passe o cartao!");
  Estado_Porta = true;
}

void FecharPorta() {
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A Fechar Porta");
  tone(piezoPin, 1000, 200);
  myservo.write(180);
  delay(250);
  myservo.write(90);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Porta Fechada!");
  Estado_Porta = false;
  delay(750); //Delay para display & led
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Passe o cartao");
}

void PortaAberta() {
  digitalWrite(redLedPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Feche a Porta e");
  lcd.setCursor(0, 1);
  lcd.print("Passe o cartao");
  tone(piezoPin, 1000, 200);
  delay(250);
  tone(piezoPin, 1000, 200);
  delay(250);
  tone(piezoPin, 1000, 200);
  digitalWrite(redLedPin, LOW);

}

void CartaoNegado() {
  digitalWrite(redLedPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Acesso negado");
  tone(piezoPin, 1000, 200);
  delay(250);
  tone(piezoPin, 1000, 200);
  delay(250);
  tone(piezoPin, 1000, 200);
  delay(500); //Delay para display & led
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Passe o cartao");
  digitalWrite(redLedPin, LOW);
}
