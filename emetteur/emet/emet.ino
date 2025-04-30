/*
* ce code envoie des trame toutes les deux secondes avec l'id de trame qui s'incrémente jusqu'à 100 puis revient a 0
*/

#include <VirtualWire.h>
#include <LiquidCrystal.h>
#include <BME280I2C.h>
#include <Wire.h>

// écran cristaux liquide
const int rs = 12, en = 11, d4 = 37, d5 = 35, d6 = 33, d7 = 31;
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

void setup()
{
  Serial.begin(9600); // initialisation du port serie
  Serial.println("ok");

  lcd.begin(16, 2); // initialisation de l'écran
  lcd.setCursor(0,0);
  lcd.print("initialisation");
  delay(1000);
  lcd.clear();

  // while(!Serial) {} // Wait
  Wire.begin();
  while(!bme.begin())
  {
    lcd.print("capteur-NF");
    delay(1000);
  }
  switch(bme.chipModel())
  {
    case BME280::ChipModel_BME280:
      lcd.print("BME280-OK");
      delay(1000);
      lcd.clear();
      break;
    case BME280::ChipModel_BMP280:
      lcd.print("BME280-OK-noH");
      delay(1000);
      lcd.clear();
      break;
    default:
      lcd.print("Ukn-sens");
      delay(1000);
      lcd.clear();
  }
  delay(1000);
  // - - - Liaison radio - - -
  vw_set_tx_pin(RF_TX_PIN); // initialisation de la broche d'envoi
  vw_set_rx_pin(RF_RX_PIN);  // initialisation de la broche de réception
  vw_setup(2000); // choix de la vitesse de transmission rf
  vw_rx_start(); // démarrage du récepteur rf
  lcd.print("rf-ok");
  delay(1000);
  lcd.clear();
}
/*

void send(char msg[50]){
  // Envoi du message
  int res = vw_send((uint8_t *)msg, 1 + strlen(msg)); // envoi du message
  if (res){
    Serial.print(msg);
    Serial.println("Message envoyé avec succès");
  }
  else {
    Serial.println("Erreur lors de l'envoi");
  }
}

*/

void loop()
{


      // relève les constante de la pièce -  -

  // lecture et enregistrement des valeurs ...
  float temp(NAN), hum(NAN), pres(NAN);
  bme.read(pres, temp, hum, bme.TempUnit_Celsius, bme.PresUnit_hPa);

  // Affichage sur l'écran lcd
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("oC");
  lcd.setCursor(4,0);
  lcd.print(" hPa");
  lcd.setCursor(9,0);
  lcd.print("%RH");

  lcd.setCursor(2,1);
  lcd.print((int)temp);
  lcd.setCursor(5,1);
  lcd.print((int)pres);
  lcd.setCursor(9,1);
  lcd.print((int)hum);

  // Envoie des trames toutes les 15 secondes
  if(millis() >= current + 15000){ // Delai non-bloquant (15 secondes)

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
    // 4 ,5        -  ID_TR     ->  id de la trame
    // 6 ,7        -  Temp      ->  température
    // 8 ,9        -  Hum       ->  Humidité
    // 10,11,12    -  Pres      ->  Pression atmosphérique


    sprintf(msg, "XX:%s:%02d:%d:%d:%d:%df:ff", ID_ARDUINO, ID_TRAME, (int)temp, (int)hum, (int)pres/10, (int)pres%10);
    // Serial.println((int)pres/10);
    // Serial.println((int)pres%10);
    Serial.println("envoie ... ");
    // send(msg);

    // Envoi du message
    int res = vw_send((uint8_t *)msg, 1 + strlen(msg)); // envoi du message
    if (res){
      Serial.println(msg);
      Serial.println("Message envoyé avec succès");
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
  
  // A COMPLETER

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
