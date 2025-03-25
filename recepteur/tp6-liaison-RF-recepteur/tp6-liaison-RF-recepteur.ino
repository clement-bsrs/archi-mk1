#include <VirtualWire.h>

int RF_RX_PIN = 4;  // Broche de réception
int RF_TX_PIN = 2;  // Broche d'envoi

unsigned long previousMillis = 0;  // Stocke le temps du dernier événement
const long interval = 500;         // Intervalle de 500 ms (délai non-bloquant)

// Tableau global pour stocker les trames reçues
#define MAX_TRAMES 10                            // Taille maximale du tableau de trames
uint8_t trames[MAX_TRAMES][VW_MAX_MESSAGE_LEN];  // Tableau pour stocker les trames brutes
uint8_t trames_count = 0;                        // Compteur de trames stockées
uint8_t current_index = 0;                       // Indice pour ajouter/mettre à jour la trame dans le tableau

void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  vw_set_rx_pin(RF_RX_PIN);  // Initialisation de la broche de réception
  vw_set_tx_pin(RF_TX_PIN);  // Initialisation de la broche d'envoi
  vw_setup(2000);            // Choix de la vitesse de transmission
  vw_rx_start();             // Démarrage du récepteur
  pinMode(12, OUTPUT);       // LED pour indiquer la réception
  Serial.println("Message envoyé");
  char msg[] = "00:01:12:34:56";
  vw_send((uint8_t *)msg, 1 + strlen(msg));
}

void loop() {

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  unsigned long currentMillis = millis();  // Obtenir le temps actuel

  // Gestion du délai non-bloquant
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(12, LOW);  // Éteindre la LED
  }

  // Si un message est reçu
  if (vw_get_message(buf, &buflen)) {
    digitalWrite(12, HIGH);  // Allumer la LED pour indiquer la réception
    analyser_trame(buf, buflen);

  } else {
    // Si la trame est déjà dans le tableau, ne rien afficher
    // Rien à afficher ici, on ne fait rien si la trame est déjà présente
  }
}

void analyser_trame(uint8_t *buf, uint8_t &buflen) {
  // Vérifier si la trame est déjà dans le tableau
  bool is_new = true;
  for (int i = 0; i < trames_count; i++) {
    bool match = true;
    for (int j = 0; j < buflen; j++) {
      if (trames[i][j] != buf[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      is_new = false;  // La trame est déjà présente
      break;
    }
  }

  // Si la trame n'est pas déjà dans le tableau, on l'ajoute ou remplace
  if (is_new) {
    // Ajouter ou remplacer la trame dans le tableau
    for (int i = 0; i < buflen; i++) {
      trames[current_index][i] = buf[i];
    }

    // Si le tableau est plein, on remplace la trame la plus ancienne
    if (trames_count < MAX_TRAMES) {
      trames_count++;  // Incrémenter le compteur de trames si on n'a pas encore atteint la taille maximale
    }

    // Passer à l'indice suivant (circular buffer)
    current_index = (current_index + 1) % MAX_TRAMES;

    // Afficher la trame brute et la traduction en ASCII
    Serial.print("Nouvelle trame reçue : ");
    for (int i = 0; i < buflen; i++) {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");

    Serial.print("Message : ");
    for (int i = 0; i < buflen; i++) {
      if (buf[i] >= 32 && buf[i] <= 126) {  // Vérifier si le caractère est un caractère ASCII imprimable
        Serial.print((char)buf[i]);
      }
    }
    Serial.println();

    //-------------------------
    // Analyser la trame
    int id_arduino[2];
    id_arduino[0] = buf[0];
    id_arduino[1] = buf[1];
    Serial.println("ID de l'arduino : ");
    Serial.print((char)id_arduino[0]);
    Serial.print((char)id_arduino[1]);
    Serial.println();

    //-------------------------

    // Afficher le tableau des trames stockées après l'ajout ou la mise à jour
    Serial.println("Trames stockées :");
    for (int i = 0; i < trames_count; i++) {
      Serial.print("Trame ");
      Serial.print(i);
      Serial.print(": ");
      for (int j = 0; j < VW_MAX_MESSAGE_LEN; j++) {
        if (trames[i][j] != 0) {  // Afficher uniquement les octets non nuls
          Serial.print(trames[i][j], HEX);
          Serial.print(" ");
        }
      }
      Serial.println();
    }
    Serial.println("- - - - - - - - - - - - - - -");
  }
}
