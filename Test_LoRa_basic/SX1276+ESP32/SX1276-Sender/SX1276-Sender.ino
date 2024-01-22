//ESP32 + SX1276
#include <SPI.h>
#include <LoRa.h>

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 923E6

//packet counter
int counter = 0;

void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  Serial.println("LoRa Sender Test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  LoRa.setPins(SS, RST, DIO0);
  while (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
  }
  Serial.println("LoRa Initializing OK!");
  Serial.println("LORA SENDER");
}

void loop() {
   
  // Serial.print("Sending packet: ");
  // Serial.println(counter);

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();
  
  Serial.print("LoRa packet sent. Counter:");
  Serial.println(counter);

  counter++;
  
  delay(1000);
}