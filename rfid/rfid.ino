#include <SPI.h>
#include <MFRC522.h>

// Définir les pins pour le module RC522
#define SS_PIN 10    // Pin de sélection de périphérique (SDA)
#define RST_PIN 9    // Pin de réinitialisation

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Créer une instance du lecteur RFID

void setup() {
  // Initialiser la communication série et le lecteur RFID
  Serial.begin(9600);  // Initialiser le moniteur série à 9600 bauds
  SPI.begin();         // Initialiser la communication SPI
  mfrc522.PCD_Init();  // Initialiser le module RFID
  Serial.println("Approchez une carte RFID du lecteur...");
}

void loop() {
  // Vérifier si une carte est présente
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Si une nouvelle carte est détectée, lire l'UID
    if (mfrc522.PICC_ReadCardSerial()) {
      Serial.print("UID de la carte : ");
      
      // Afficher l'UID de la carte
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      // Terminer la lecture de la carte
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    }
  }
}