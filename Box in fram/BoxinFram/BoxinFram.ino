#include <SPI.h>
#include <LoRa.h>
 
//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
 
#define BAND 923E6
 
//packet counter
int counter = 0;
unsigned long previousMillis = 0;
String LoRaData = "";
bool sendSuccess = false;
 
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
 
void onReceive() {
  int count = 0;
 
  while (true) {
    unsigned long currentMillis = millis();
 
    int packetSize = LoRa.parsePacket();
    // Serial.println("Wait data");
    if (packetSize) {
      //received a packet
      Serial.print("Received packet : ");
 
      //read packet
      while (LoRa.available()) {
        LoRaData = LoRa.readString();
        Serial.println(LoRaData);
      }
 
      //print RSSI of packet
      int rssi = LoRa.packetRssi();
      int snr = LoRa.packetSnr();
      Serial.print("Data RSSI : ");
      Serial.println(rssi);
      Serial.print("Data SNR : ");
      Serial.println(snr);
      counter++;
      if (LoRaData.equals("CPE")) {
        sendSuccess = true;
      }
      count = 0;
      break;
    }
 
    if (currentMillis - previousMillis >= 1000) {
      previousMillis = currentMillis;
      count++;
    }
    if(count == 5){
      break;
    }
  }
}
 
void sendData() {
  LoRa.beginPacket();
  LoRa.print("temp ");
  LoRa.print(counter);
  LoRa.endPacket();
  Serial.print("LoRa packet sent. : temp ");
  Serial.println(counter);
  onReceive();
}
 
void readSensor(){
  //อ่านค่าเซนเซอร์
}
 
void loop() {
  if(Serial.available() )
  if (sendSuccess == true) {
    Serial.println("LoRa send Success");
    sendSuccess = false;
    delay(10000);
  } else {
    sendData();
    delay(1000);
  }
}