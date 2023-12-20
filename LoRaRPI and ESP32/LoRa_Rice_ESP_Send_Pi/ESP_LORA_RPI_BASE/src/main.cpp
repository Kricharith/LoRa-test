#include <Arduino.h>
#include <SPI.h>              
#include <LoRa.h>
#include <sensorRead.h>
#include <loratx_rx.h>

#define ONE 1
#define TWO 2
#define TREE 3
#define FOUR 4
int state = 0;
bool stateSendData = false;
int countSendData = 0;
String Mymessage = "";

void setup() {
  Serial.begin(115200);
  esp_sleep_enable_timer_wakeup(20 * 1000 * 1000);
  state = ONE;
  Serial.println("LoRa Node 1");
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  LoRa.setTxPower(18);
  
  while (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
  }
  Serial.println("LoRa init succeeded.");
  delay(1000);
}

void loop() {
  if(state == ONE){                   //อ่านค่าเซ็นเซอร์
    // onReceive(LoRa.parsePacket());
    //Serial.println(state);
    // state = TWO;
  }else if(state == TWO){        //ส่งข้อมูลไปเกตเวย์
    // Serial.print("state:");
    // Serial.println(state);
    if(Serial.available()>0){
      Mymessage = Serial.readString();
      Mymessage.trim();
      sendMessage(Mymessage);
      Mymessage = "";
      state = TREE;
    }
    sendMessage("Mymessage");
    state = TREE;
  }else if(state == TREE){            //รอรับการยืนยันความถูกต้องของข้อมูลจากเกตเวย์
    // Serial.print("state:");
    // Serial.println(state);
    onReceive();
    if(sendSuccess == true && resendData == false){
      state = FOUR;
    }else if(sendSuccess == false && resendData == true){
      if(countSendData >= 5){
        state = FOUR;
      }else{
        countSendData ++;
        Serial.print("countSendData");
        Serial.println(countSendData);
        state = TWO;
      }
      
    }
  }else if(state == FOUR){           //เข้าโหมดsleep
    Serial.print("state:");
    Serial.println(state);
    esp_deep_sleep_start(); // เข้าสู่โหมด Deep Sleep
  }
  delay(10);
}
