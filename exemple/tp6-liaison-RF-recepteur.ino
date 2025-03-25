// code du récepteur
#include <VirtualWire.h>

int RF_RX_PIN = 4;

void setup()
{
  Serial.begin(9600);
  Serial.println("setup");
  vw_set_rx_pin(RF_RX_PIN);  // initialisation de la broche de reception
  vw_setup(2000); // choix de la vitesse de transmission
  vw_rx_start(); //démarrage du récepteur
}

void loop()
{
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) // non-blocking I/O
  {
    int i;
    // si on a reçu un message
    Serial.print("Reçu : ");
    for (i = 0; i < buflen; i++)
    {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println("");
  }
}
