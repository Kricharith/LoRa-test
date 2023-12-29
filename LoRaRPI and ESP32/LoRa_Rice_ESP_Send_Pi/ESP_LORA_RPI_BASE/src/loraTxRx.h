#include <Arduino.h>
#include <LoRa.h>

#ifndef loratx_rx
#define loratx_rx
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26
#define BAND 923E6
#endif

extern const int csPin = 7;          // LoRa radio chip select
extern const int resetPin = 6;       // LoRa radio reset
extern const int irqPin = 1;         // change for your board; must be a hardware interrupt pin

byte localAddress = 0xF1;     // address of this device
byte destination = 0xBB;      // destination to send to
extern bool sendSuccess = false;
extern bool resendData = false;
unsigned long previousMillis = 0;

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it                          // increment message ID
  Serial.print(" destination :");
  Serial.println(destination);
  Serial.print(" localAddress :");
  Serial.println(localAddress);
  Serial.print(" outgoing.length :");
  Serial.println(outgoing.length());
  Serial.print(" outgoing :");
  Serial.println(outgoing);
}

// void onReceive(int packetSize) {
//   int count = 0;
//   while (true) {
//     // Serial.println("onReceive");
//     if(packetSize){          // if there's no packet, return
//       Serial.println("packetSize");  
//       String rawPayload = "";
//       for (int i = 0; i < packetSize; i++) {
//         char currentChar = (char)LoRa.read();
//         if (currentChar == ' ') {
//           rawPayload += ",";
//         }else {
//           rawPayload += currentChar;
//         }
//       } 
//       Serial.print("rawPayload"); 
//       Serial.println(rawPayload); 
//       // Split the rawPayload into an array using " " (space) as a delimiter
//       int arraySize = 0;
//       String payloadArray[20]; // Assuming a maximum of 20 elements
//       payloadArray[arraySize++] = ""; // Initialize the first element

//       for (int i = 0; i < rawPayload.length(); i++) {
//         char currentChar = rawPayload.charAt(i);
//         if (currentChar == ',') {
//           payloadArray[arraySize++] = ""; // Move to the next element
//         } else {
//           payloadArray[arraySize - 1] += currentChar; // Add the character to the current element
//         }
//       }  
//       // Do something with the array elements if needed
//       String recipient = "0x"+String(payloadArray[0].toInt(), HEX);          // recipient address
//       String sender = "0x"+String(payloadArray[1].toInt(), HEX);            // sender address
//       String incomingLength = payloadArray[2];    // incoming msg length
//       String data = "";
      
//       if(recipient == "0xf1" && sender == "0xbb"){
//         for (int i = 3; i < arraySize; i++) {
//           data += payloadArray[i];
//         }
//         Serial.print("Received from: ");
//         Serial.println(recipient);
//         Serial.print("Sent to: ");
//         Serial.println(sender);
//         Serial.print("Message length: ");
//         Serial.println(incomingLength);
//         Serial.print("data: ");
//         Serial.println(data);
//         Serial.println("RSSI: " + String(LoRa.packetRssi()));
//         Serial.println("Snr: " + String(LoRa.packetSnr()));
//         if(data.equals("CPE")) {
//           Serial.println("CPE");
//           sendSuccess = true;
//         }else{
//           sendSuccess = false;
//         }
//       }
//     }else{
//         unsigned long currentMillis = millis();
//         if (currentMillis - previousMillis >= 1000) {
//           previousMillis = currentMillis;
//           count++;
//         //   Serial.println("count");
//         //   Serial.println(count);
//         }
//         if(sendSuccess){
//         //   Serial.println("sendSuccess");
//           break;
//         }else if(count == 10){
//         //   Serial.println("break");
//         //   Serial.println("This message is not for me.");
//           sendSuccess = false;
//           break;
//         }
//     }
//   }
// }

void onReceive() {
    int count = 0;
    while(true){
        int packetSize = LoRa.parsePacket();
        if (packetSize) {          // if there's no packet, return
            String rawPayload = "";
            for (int i = 0; i < packetSize; i++) {
                char currentChar = (char)LoRa.read();
                if (currentChar == ' ') {
                rawPayload += ",";
                } else {
                rawPayload += currentChar;
                }
            }
            // Split the rawPayload into an array using " " (space) as a delimiter
            int arraySize = 0;
            String payloadArray[20]; // Assuming a maximum of 20 elements
            payloadArray[arraySize++] = ""; // Initialize the first element

            for (int i = 0; i < rawPayload.length(); i++) {
                char currentChar = rawPayload.charAt(i);
                if (currentChar == ',') {
                payloadArray[arraySize++] = ""; // Move to the next element
                } else {
                payloadArray[arraySize - 1] += currentChar; // Add the character to the current element
                }
            }  
            String recipient = "0x"+String(payloadArray[0].toInt(), HEX);          // recipient address
            String sender = "0x"+String(payloadArray[1].toInt(), HEX);            // sender address
            String incomingLength = payloadArray[2];    // incoming msg length
            String data = "";
            Serial.print("data: ");
            // Serial.println(payloadArray[0]);
            if(recipient == "0xf1" && sender == "0xbb"){
                for (int i = 3; i < arraySize; i++) {
                    data += payloadArray[i];
                }
                Serial.println("Read Pass");   
                if(data.equals("CPE")) {
                    Serial.println("CPE Pass");
                    Serial.print("Received from: ");
                    Serial.println(recipient);
                    Serial.print("Sent to: ");
                    Serial.println(sender);
                    Serial.print("Message length: ");
                    Serial.println(incomingLength);
                    Serial.print("data: ");
                    Serial.println(data);
                    Serial.println("RSSI: " + String(LoRa.packetRssi()));
                    Serial.println("Snr: " + String(LoRa.packetSnr()));
                    sendSuccess = true;
                    resendData = false;
                    break;
                }  
            }
        }else {
            unsigned long currentMillis = millis();
            if (currentMillis - previousMillis >= 1000) {
                previousMillis = currentMillis;
                count++;
                Serial.println("count");
                Serial.println(count);
            }
            if(count == 10){
                Serial.println("break");
                //   Serial.println("This message is not for me.");
                sendSuccess = false;
                resendData = true;
                break;
            }
        // }else if(sendSuccess == true && resendData == false){
        //     // break;
        // }
        }
    }
  
}

