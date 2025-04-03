#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9  // Pin de Reset (peut varier selon ton montage)
#define SS_PIN 10  // Pin Slave Select (peut varier selon ton montage)

MFRC522 mfrc522(SS_PIN, RST_PIN); // Crée un objet MFRC522

void setup() {
  Serial.begin(9600);  // Démarre la communication série à 9600 bauds
  SPI.begin();         // Démarre la communication SPI
  mfrc522.PCD_Init();  // Initialise le module RFID

  Serial.println("Approchez un tag RFID du lecteur...");
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent()) {  // Vérifie si une carte est présente
    if (mfrc522.PICC_ReadCardSerial()) { // Si la carte est lue
      Serial.print("UID de la carte : ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i], DEC);
        Serial.print(" ");
      }
      Serial.println();
      mfrc522.PICC_HaltA(); // Arrête la lecture de la carte
    }
  }
}