/*
* ce code envoie des trame toutes les deux secondes avec l'id de trame qui s'incrémente jusqu'à 100 puis revient a 0
*/

#include <VirtualWire.h>

const char ID_ARDUINO[] = "01";  // ID_ARDUINO doit être une chaîne de caractères constante
int ID_TRAME = 0;                 // ID_TRAME doit être un entier, pas un char
int RF_TX_PIN = 2;
int RF_RX_PIN = 4;
const int lenretrans = 10;        // Taille fixe pour la table de retransmission
int ind = 0;
uint8_t retrans[lenretrans][VW_MAX_MESSAGE_LEN]; // Tableau de retransmission

long t = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("START : ");
  vw_set_tx_pin(RF_TX_PIN); // initialisation de la broche d'envoi
  vw_set_rx_pin(RF_RX_PIN);  // initialisation de la broche de réception
  vw_setup(2000); // choix de la vitesse de transmission
  vw_rx_start(); // démarrage du récepteur
  delay(2000);
}

void loop()
{
  char msg[50] = "";

  // Créer la trame avec l'ID_ARDUINO et l'ID_TRAME
  sprintf(msg, "XX:%s:%02d:00:00:00:00:00", ID_ARDUINO, ID_TRAME);
  
  Serial.print("Envoi d'un message : ");
  Serial.println(msg);

  // Envoi du message
  int res = vw_send((uint8_t *)msg, 1 + strlen(msg)); // envoi du message
  if (res)
    Serial.println("Message envoyé avec succès");
  else
    Serial.println("Erreur lors de l'envoi");

  // Incrémenter l'ID_TRAME et gérer le dépassement de 100
  ID_TRAME++;
  if (ID_TRAME == 100) {
    ID_TRAME = 0; // Réinitialisation à 0 lorsqu'on atteint 100
  }

  // Attendre 2 secondes avant de renvoyer la prochaine trame
  delay(2000);

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  // Si on reçoit un message
  if (vw_get_message(buf, &buflen)) // non-blocking I/O
  {
    Serial.print((char) buf[0]);
    Serial.print((char) buf[1]);

    // Si le message vient du serveur
    if ((char) buf[0] == '0' && (char) buf[1] == '0')
    {
      Serial.println("Message du serveur");
      Serial.println("Exécution du code serveur");
    }

    // Si le message ne vient pas du serveur
    if ((char) buf[0] != '0' || (char) buf[1] != '0')
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
      Serial.println();

      // Test de présence de la trame dans la table de retransmission
      bool flag = false;
      for (int j = 0; j < lenretrans; j++)
      {
        if (memcmp(rec, retrans[j], buflen) == 0) // Comparaison des messages
        {
          Serial.println("Message dans la table de retransmission");
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
        vw_send((uint8_t *) rec, buflen);   // Retransmission du message
        Serial.println("Message retransmis avec succès");
      }
    }
  }
}
