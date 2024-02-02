#include <Arduino.h>
#include <LoRa.h>

#ifndef LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 923E6
#endif

unsigned long loraTime = 0;
int btnState = 0;

void initLoRa()
{
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DIO0);
    while (!LoRa.begin(BAND))
    {
        Serial.println("Starting LoRa failed!");
        delay(500);
    }
    Serial.println("LoRa Initializing OK!");
}

bool receiveLoRa()
{
    while (true)
    {
        String LoRaData;
        unsigned long currentMillis = millis();

        int packetSize = LoRa.parsePacket();
        // Serial.println("Wait data");
        if (packetSize)
        {
            // received a packet
            Serial.print("Received packet : ");

            // read packet
            while (LoRa.available())
            {
                LoRaData = LoRa.readString();
                Serial.println(LoRaData);
            }

            // print RSSI of packet
            int rssi = LoRa.packetRssi();
            int snr = LoRa.packetSnr();
            Serial.print("Data RSSI : ");
            Serial.println(rssi);
            Serial.print("Data SNR : ");
            Serial.println(snr);
            if (LoRaData.equals("CPE"))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        // if (btnState == 11)
        // {
        //     return false;
        // }
        if (currentMillis - loraTime >= 5000)
        {
            loraTime = currentMillis;
            return false;
        }
    }
}

void sentLoRa(byte souce, byte destination, String LoRaData)
{
    // Serial.println("This is fanction sentLoRa!!");
    LoRa.beginPacket();
    // LoRaData = String(t_in_a) + "," + String(h_in_a);
    LoRa.print(souce);
    LoRa.print(destination);
    LoRa.print(LoRaData);
    LoRa.endPacket();
    Serial.print("LoRa packet sent. : ");
    String LoRaDataTest = souce + destination + LoRaData;
    Serial.println(LoRaData);
    LoRaData = "";
    // onReceive();
}