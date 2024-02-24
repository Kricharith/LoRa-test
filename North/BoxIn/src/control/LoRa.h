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

#define ADDR_SOUCE 0xAA
#define ADDR_DEST 0xFF

unsigned long loraTime = 0;

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

int checkDataLoRa()
{
    int lengthData = 4;
    String loraData[lengthData];
    String receiveData;
    int startIndex = 0;
    int endIndex = 0;
    int i = 0, count = 0;

    while (LoRa.available())
    {
        receiveData = LoRa.readString();
        Serial.print("Receive : ");
        Serial.println(receiveData);
    }

    // print RSSI of packet
    int rssi = LoRa.packetRssi();
    int snr = LoRa.packetSnr();
    Serial.print("Data RSSI : ");
    Serial.println(rssi);
    Serial.print("Data SNR : ");
    Serial.println(snr);

    for (i = 0; i < receiveData.length(); i++)
    {
        if (receiveData[i] == ',')
        {
            count++;
            // Serial.println("Receive data");
            // return 11;
        }
    }
    if (count == lengthData-1)
    {
        for (int i = 0; i < lengthData; i++)
        {
            endIndex = receiveData.indexOf(',', startIndex);
            if (endIndex == -1)
            {
                endIndex = receiveData.length();
            }
            loraData[i] = receiveData.substring(startIndex, endIndex);
            startIndex = endIndex + 1;
        }
    }
    else{
        return 3;
        Serial.println("Receive data is not 0-9 or \",\"");
    }

    // Serial.print("loraData[0]");
    // Serial.println(loraData[0]);
    // Serial.print("loraData[1]");
    // Serial.println(loraData[1]);
    // Serial.print("loraData[2]");
    // Serial.println(loraData[2]);
    // return true;
    if (loraData[0].equals(String(ADDR_DEST, DEC)) && loraData[1].equals(String(ADDR_SOUCE, DEC))) // Addr true
    {
        Serial.println("Check LoRa");
        if (loraData[2].equals("1") && loraData[3].equals("0")) // Hard reset == 0
        {
            Serial.println("Check LoRa return 1");
            return 1;
        }
        else if (loraData[3].equals("1")) // Hard reset == 1
        {
            return 2;
        }
    }
    else // Addr false
    {
        return 3;
    }
}

int receiveLoRa()
{
    int packetSize = LoRa.parsePacket();
    return packetSize;
}

void sentLoRa(byte souce, byte destination, String LoRaData)
{
    // Serial.println("This is fanction sentLoRa!!");
    Serial.println("LoRa sent");
    LoRa.beginPacket();
    // LoRaData = String(t_in_a) + "," + String(h_in_a);
    LoRa.write(souce);
    LoRa.write(destination);
    LoRa.write(LoRaData.length());
    LoRa.print(LoRaData);
    LoRa.endPacket();
    Serial.print("LoRa packet sent. : ");
    String LoRaDataTest = souce + destination + LoRaData;
    Serial.println(LoRaData);
    LoRaData = "";
    // onReceive();
}