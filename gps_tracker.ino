#include <SoftwareSerial.h>


#define RX_PIN 7
#define TX_PIN 8

SoftwareSerial swSerial(RX_PIN, TX_PIN);

void setup()
{
  swSerial.begin(9600);
  Serial.begin(115200);

  Serial.println("Setup complete");
}

void loop()
{
  unsigned n = 0;
  unsigned t = 0;

  do {
    while (swSerial.available())
    {
      char x = swSerial.read();
      Serial.print(x);
      n++;
    }    
  } while ((millis() - t) < 1000);

  Serial.println("");
  Serial.print("Got characters: ");
  Serial.println(n);
}
