#include <VirtualWire.h>

int RF_RX_PIN = 4;  // Broche de réception
int RF_TX_PIN = 2;  // Broche d'envoi

unsigned long previousMillis = 0;  // Stocke le temps du dernier événement
const long interval = 8000;         // Intervalle de 500 ms (délai non-bloquant)

// Tableau global pour stocker les trames reçues
#define MAX_TRAMES 10                            // Taille maximale du tableau de trames
uint8_t trames[MAX_TRAMES][VW_MAX_MESSAGE_LEN];  // Tableau pour stocker les trames brutes
uint8_t trames_count = 0;                        // Compteur de trames stockées
uint8_t current_index = 0;                       // Indice pour ajouter/mettre à jour la trame dans le tableau


// Valeurs des données
char temperature[] = "FF";
char humidite[] = "FF";
char presence[] = "FF";
char lumiere[] = "FF";


void setup() {
  Serial.begin(9600);
  Serial.println("setup");
  vw_set_rx_pin(RF_RX_PIN);  // Initialisation de la broche de réception
  vw_set_tx_pin(RF_TX_PIN);  // Initialisation de la broche d'envoi
  vw_setup(2000);            // Choix de la vitesse de transmission
  vw_rx_start();             // Démarrage du récepteur
  pinMode(12, OUTPUT);       // LED pour indiquer la réception
  // Serial.println("Message envoyé");
  // char msg[] = "XX:00:01:12:34:56:78:99";
  // vw_send((uint8_t *)msg, 1 + strlen(msg));
  Serial.println("- - - - - - - - - - - -");
  Serial.println("");
}

void loop() {

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  unsigned long currentMillis = millis();  // Obtenir le temps actuel

  // Gestion du délai non-bloquant
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(12, LOW);  // Éteindre la LED
    // Serial.println("Message envoyé");
    // char msg[] = "XX:00:01:12:34:56:78:99";
    // vw_send((uint8_t *)msg, 1 + strlen(msg));

    afficher_menu(String(temperature), String(humidite), String(presence), String(lumiere));

  }

  // Si un message est reçu
  if (vw_get_message(buf, &buflen)) {
    digitalWrite(12, HIGH);  // Allumer la LED pour indiquer la réception
    analyser_trame(buf, buflen);

  } else {
    //analyser_trame(buf, buflen);
    // Si la trame est déjà dans le tableau, ne rien afficher
  }
}

void analyser_trame(uint8_t *buf, uint8_t &buflen) {
  // Vérifier que la trame a exactement 23 caractères (octets)
  if (buflen != 24) {
    return;  // Sortir de la fonction sans faire d'autres traitements
  }

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

    // Vérifier que ça commence par XX
    char entete1 = (char)buf[0];
    char entete2 = (char)buf[1];

    char entete[3];  // Tableau pour stocker le résultat, taille de 3 pour 2 caractères + '\0' (caractère de fin de chaîne)
    entete[0] = entete1;
    entete[1] = entete2;
    entete[2] = '\0';  // N'oublie pas de terminer la chaîne par un '\0'
    
    Serial.println("Entete : ");
    Serial.println(entete);  // Doit afficher XX;

    // Récupérer l'ID de l'arduino
    char idArduino1 = (char)buf[3];
    char idArduino2 = (char)buf[4];

    char idArduino[3];  // Tableau pour stocker le résultat, taille de 3 pour 2 caractères + '\0' (caractère de fin de chaîne)
    idArduino[0] = idArduino1;
    idArduino[1] = idArduino2;
    idArduino[2] = '\0'; 
    
    Serial.println("ID Arduino: ");
    Serial.println(idArduino);  // Doit afficher l'id;

    // Récupérer l'id de la trame
    char idTrame1 = (char)buf[6];
    char idTrame2 = (char)buf[7];

    char idTrame[3];  // Tableau pour stocker le résultat, taille de 3 pour 2 caractères + '\0' (caractère de fin de chaîne)
    idTrame[0] = idTrame1;
    idTrame[1] = idTrame2;
    idTrame[2] = '\0';  // N'oublie pas de terminer la chaîne par un '\0'
    
    Serial.println("ID Trame : ");
    Serial.println(idTrame);  // Doit afficher XX;

    // if(String(idTrame) != "XX\0") {
    //   return;
    // }
    
    char temp1 = (char)buf[9];
    char temp2 = (char)buf[10];

    temperature[0] = temp1;
    temperature[1] = temp2;
    temperature[2] = '\0';

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
    Serial.println("");
  }
}


void afficher_menu(String t, String h, String p, String l) {
  Serial.println("- - - - - - - - Menu - - - - - - - - -");
  Serial.println("Temp   |   humi   |   pres   |   lumi");
  Serial.println(" " + t+ "    |    " + h + "    |    " + p + "    |    " + l);
  Serial.println("- - - - - - - - - - - - - - - - - - -");

}
