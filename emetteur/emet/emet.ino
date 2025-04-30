/*
* ce code envoie des trame toutes les deux secondes avec l'id de trame qui s'incrémente jusqu'à 100 puis revient a 0
*/
#include <SPI.h>
#include <VirtualWire.h>
#include <LiquidCrystal.h>
#include <BME280I2C.h>
#include <Wire.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 53
// -- objet MFRC / RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

// écran cristaux liquide
const int rs = 43, en = 41, d4 = 37, d5 = 35, d6 = 33, d7 = 31;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// capteur pression, température, humidité
BME280I2C bme;  // Default : forced mode, standby time = 1000 ms
                // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,

const char ID_ARDUINO[] = "01";  // ID_ARDUINO doit être une chaîne de caractères constante
int ID_TRAME = 0;                 // ID_TRAME doit être un entier, pas un char
int RF_TX_PIN = 2;
int RF_RX_PIN = 4;
const int lenretrans = 10;        // Taille fixe pour la table de retransmission
int ind = 0;
uint8_t retrans[lenretrans][VW_MAX_MESSAGE_LEN]; // Tableau de retransmission
long current = millis();
long t = 0;

String data[2] = {"37", "22"};
String noms[] = {"clement", "matheo"};
int index = 2;

void setup()
{
  lcd.begin(16, 2); // initialisation de l'écran
  
  lcd.setCursor(0,0);
  lcd.print("initialisation");
  delay(700);
  lcd.clear();
  
  Serial.begin(9600); // initialisation du port serie
  SPI.begin();

  lcd.setCursor(0,0);
  lcd.print("serial : OK");
  delay(500);
  lcd.clear();

  mfrc522.PCD_Init();

  lcd.setCursor(0,0);
  lcd.print("RFID : OK");
  delay(500);
  lcd.clear();

  // while(!Serial) {} // Wait
  Wire.begin();
  while(!bme.begin())
  {
    lcd.print("CAPTEUR : NF");
    delay(500);
  }
  switch(bme.chipModel())
  {
    case BME280::ChipModel_BME280:
      lcd.print("BME280-OK");
      delay(500);
      lcd.clear();
      break;
    case BME280::ChipModel_BMP280:
      lcd.print("BME280-OK-noH");
      delay(500);
      lcd.clear();
      break;
    default:
      lcd.print("Ukn-sens");
      delay(500);
      lcd.clear();
  }
  // - - - Liaison radio - - -
  vw_set_tx_pin(RF_TX_PIN); // initialisation de la broche d'envoi
  vw_set_rx_pin(RF_RX_PIN);  // initialisation de la broche de réception
  vw_setup(2000); // choix de la vitesse de transmission rf
  vw_rx_start(); // démarrage du récepteur rf
  lcd.print("RF : OK");
  delay(500);
  lcd.clear();
}

void loop()
{
  // lecture et enregistrement des valeurs ...
  float temp(NAN), hum(NAN), pres(NAN);
  bme.read(pres, temp, hum, bme.TempUnit_Celsius, bme.PresUnit_hPa);

  // Affichage sur l'écran lcd
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("oC");
  lcd.setCursor(4,0);
  lcd.print(" hPa");
  lcd.setCursor(11,0);
  lcd.print("%RH");

  lcd.setCursor(1,1);
  lcd.print((int)temp);
  lcd.setCursor(5,1);
  lcd.print((int)pres);
  lcd.setCursor(11,1);
  lcd.print((int)hum);

  // Envoie des trames toutes les 15 secondes
  if(millis() >= current + 5000){ // Delai non-bloquant (15 secondes)

    // Incrémenter l'ID_TRAME et gérer le dépassement de 100
    ID_TRAME++;
    if (ID_TRAME == 100) {
      ID_TRAME = 0; // Réinitialisation à 0 lorsqu'on atteint 100
    }
    // préparation de la trame à envoyer
    char msg[50] = "";

    // Créer la trame avec l'ID_ARDUINO et l'ID_TRAME
    // 0 ,1        -  XX        ->  identifiant du protocole
    // 2 ,3        -  ID_AR     ->  id de l'arduino
    // 4 ,5        -  TP_AR     ->  id de l'arduino (00 -> variable environnementale | 01 -> contrôle d'accés)
    // 6 ,7        -  ID_TR     ->  id de la trame
    // 8 ,9        -  Temp      ->  température
    // 10 ,11        -  Hum       ->  Humidité
    // 12,13,14    -  Pres      ->  Pression atmosphérique

    sprintf(msg, "XX:%s:00:%02d:%d:%d:%02d:%02d", ID_ARDUINO, ID_TRAME, (int)temp, (int)hum, (int)pres/100, (int)pres%100);
    // Serial.println((int)pres/10);
    // Serial.println((int)pres%10);
    Serial.print("trame des constantes : ");
    // send(msg);

    // Envoi du message
    int res = vw_send((uint8_t *)msg, 1 + strlen(msg)); // envoi du message
    if (res){
      Serial.println(msg);
      lcd.setCursor(15,1);
      lcd.print("*");
      delay(100);
      lcd.setCursor(15,1);
      lcd.print(" ");
      
    }
    else {
      Serial.println("Erreur lors de l'envoi");
    }

    current = millis();
  }
  

  // TEST RFID -------------------------------------
  
  if (mfrc522.PICC_IsNewCardPresent()) {  // Vérifie si une carte est présente
    if (mfrc522.PICC_ReadCardSerial()) { // Si la carte est lue
      Serial.print("trame d'accés : ");
      int code;
      char msg[50] = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        if(i == 0){
          code = mfrc522.uid.uidByte[i];
          Serial.println(code);
          ID_TRAME++; // incrémente l'id de la trame
          if (ID_TRAME == 100) {
            ID_TRAME = 0; // Réinitialisation à 0 lorsqu'on atteint 100
          }

          

          String codeStr = String(code);  // Convertir le code int en String
          int id = -1;
          for (int i = 0; i < index; i++) {
            if (codeStr == data[i]) {
              id = i;
              break; // Pas besoin de continuer, on a trouvé
            }
          }

          if(id == -1){
            lcd.clear();
            lcd.setCursor(3,0);
            lcd.print("badge N/A");
            delay(1000);
            lcd.clear();
          } else {
            lcd.clear();
            lcd.setCursor(5,0);
            lcd.print("RFID");
            lcd.setCursor(7,1);
            lcd.print("OK");
            delay(1000);
            lcd.clear();

            sprintf(msg, "XX:%s:01:%02d:%d:ff:ff:ff", ID_ARDUINO, ID_TRAME, code);

            // Envoi du message
            int res = vw_send((uint8_t *)msg, 1 + strlen(msg)); // envoi du message
            if (res){
              Serial.println(msg);
              lcd.setCursor(15,1);
              lcd.print("*");
              delay(200);
              lcd.setCursor(15,1);
              lcd.print(" ");
              
            }
            else {
              Serial.println("Erreur lors de l'envoi de l'accés");
            }
          }
        }
      }
      mfrc522.PICC_HaltA(); // Arrête la lecture de la carte
    }
  }

  // -----------------------------------------------

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  // Si on reçoit un message
  if (vw_get_message(buf, &buflen)) // non-blocking I/O
  {
    Serial.print("id : ");
    Serial.print((char) buf[0]); // X
    Serial.print((char) buf[1]); // X
    Serial.print((char) buf[2]); // :
    Serial.print((char) buf[3]); // X id trame 1
    Serial.print((char) buf[4]); // X id trame 2
    Serial.print((char) buf[5]); // :
    Serial.print((char) buf[6]); // X id arduino 1
    Serial.println((char) buf[7]); // X id arduino 2

    // le message vient du serveur ?
    if ((char) buf[6] == '0' && (char) buf[7] == '0')
    {
      Serial.print("Message du serveur | ");
      Serial.println("Exécution du code serveur");
    }
    // Si le message ne vient pas du serveur
    if ((char) buf[6] != '0' || (char) buf[7] != '0')
    {
      int i;
      uint8_t rec[buflen];
      // stockage du message reçu à retransmettre
      Serial.print("Reçu : ");
      for (i = 0; i < buflen; i++)
      {
        rec[i] = buf[i];
        Serial.print((char)rec[i]);
      }
      

      // Test de présence de la trame dans la table de retransmission
      bool flag = false;
      for (int j = 0; j < lenretrans; j++)
      {
        if (memcmp(rec, retrans[j], buflen) == 0) // Comparaison des messages
        {
          Serial.println("Message déjà dans la table de retransmission");
          flag = true;
          break;
        }
      }

      if (!flag)
      {
        // Ajouter le message à la table de retransmission
        ind++;
        if (ind == lenretrans)
        {
          ind = 0; // Remise à zéro pour créer un effet circulaire de la liste
        }
        memcpy(retrans[ind], rec, buflen); // Stockage du message
        int res = vw_send((uint8_t *) rec, buflen);   // Retransmission du message
        // if(res)Serial.println("message envoyé");

        Serial.print("Message retransmis avec succès : ");
        Serial.println(res);
      }
    }
  }
}
