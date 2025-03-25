// code de l'émetteur
#include <VirtualWire.h>

int RF_TX_PIN = 2;


char msg[] = "11PIci le numéro 1";
long t = 0;
void setup()
{
  Serial.begin(9600);
  Serial.println("setup");
  vw_set_tx_pin(RF_TX_PIN); // initialisation de la broche d'envoi
  vw_setup(2000); // choix de la vitesse de transmission
}

void loop()
{
    Serial.print("envoi d'un message : ");
    Serial.println(3 + msg);
    int res = vw_send((uint8_t *)msg, 1 + strlen(msg));
    Serial.println();
    delay(1000);
  
}
