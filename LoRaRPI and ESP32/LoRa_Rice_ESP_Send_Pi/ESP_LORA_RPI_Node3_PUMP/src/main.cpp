#include <Arduino.h>
#include <SPI.h>
#include <EasyButton.h>
#include <sensorRead.h>
#include <loraTxRx.h>



#define ONE          1
#define TWO          2
#define THREE        3
#define FOUR         4
#define FIVE         5
#define SIX          6
#define SEVEN        7
#define BUTTON_MODE  16
#define BUTTON_PUMP  4
#define LEDPUMP      32
#define LEDLORA      33
//====================================EEPROM Address==============================
#define addr_pumpMode        0x10
#define addr_mode            0x20
#define addr_pumpState       0x30
#define addr_statusNode      0x40
#define addr_timeNormalMode  0x50
#define addr_timeDebugMode   0x60
gpio_num_t wakeup_gpio = GPIO_NUM_16;
gpio_num_t ledStatus_gpio = GPIO_NUM_33;
bool pumpMode = false;
bool mode = false;
bool pumpState = false;
bool pumpStatus = false;
bool statusNode = false;
float timeNormalMode = 5;
float timeDebugMode = 3;

int state = 1;
bool stateSendData = false;
int countSendData = 0;

String Mymessage = "";


void pumpOn(bool status);
void checkStatePump();
bool updateConfig(String receiveData);

void onPressed(){
  pumpState = !pumpState;
  pumpOn(pumpState);
  sendMessage("Mymessage");
  Serial.println("Button pressed");
  delay(20);
  // state = TWO;
}
void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_MODE,INPUT_PULLUP);
  pinMode(LEDPUMP,OUTPUT);
  digitalWrite(LEDPUMP, LOW);

  /////////////////////////////////////////
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  LoRa.setTxPower(18);
  
  while (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
  }
  Serial.println("LoRa init succeeded.");

  /////////////////////////////////////////
  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  delay(1000);
}

void loop() {
  if(state == ONE){                   //อ่านค่าเซ็นเซอร์
    //Serial.println(state);
    updateSensorTemp_Hum();
    state = TWO;
  }else if(state == TWO){             //ส่งข้อมูลไปเกตเวย์พร้อมขอสถานะปั้มน้ำและค่าconfigต่างๆ
    //Serial.println(state);
    //Serial.print(temp);   Serial.print(hum);   Serial.println(pumpState); 
    // pumpMode  mode  pumpState pumpStatus  statusNode  timeNormalMode  timeDebugMode
    Mymessage = String(temp)+","+String(hum)+","+String(pumpMode)+","+String(mode)+","+String(pumpState)+","+String(pumpStatus)+","+String(statusNode)+","+String(timeNormalMode)+","+String(timeDebugMode);
    Serial.println(Mymessage);
    sendMessage(Mymessage);
    state = THREE;
  }else if(state == THREE){            //รอรับการยืนยันความถูกต้องของข้อมูลจากเกตเวย์
    //Serial.println(state);
    onReceive();
    if(sendSuccess == true && resendData == false){
      state = FOUR;
    }else if(sendSuccess == false && resendData == true){
      if(countSendData >= 5){
        state = FIVE;
      }else{
        countSendData ++;
        Serial.print("countSendData");
        Serial.println(countSendData);
        state = TWO;
      }
    }
  }else if(state == FOUR){            //Updateข้อมูลconfigต่างๆจากเกตเวย์
    if(updateConfig(receiveData)){
      state = FIVE;
    }else{
      state = THREE;
    }
    // checkStatePump();
    //Serial.println(state);
    delay(1000);
    esp_deep_sleep_start(); // เข้าสู่โหมด Deep Sleep
    state = ONE;
    //state = FIVE;
  }else if(state == FOUR){            //ส่งสถานะปั้มน้ำปัจจุบันไปยังเกตเวย์
    // Mymessage = String(temp)+","+String(hum)+","+String(pumpState);
    // Serial.println(Mymessage);
    // sendMessage(Mymessage);
    //Serial.println(state);
    //state = FIVE;
  }else if(state == FIVE){            //รอรับการยืนยันความถูกต้องของข้อมูลจากเกตเวย์
    //Serial.println(state);
    state = SIX;
  }else if(state == SIX){             //บันทึกข้อมูลลงEEPROM
    //Serial.println(state);
    state = SEVEN;
  }else if(state == SEVEN){           // เข้าสู่โหมด Deep Sleep
    //Serial.print("state:");
    // Serial.println(state);
    // esp_deep_sleep_start(); // เข้าสู่โหมด Deep Sleep
    state = ONE;
  }
  delay(10);
}
void checkStatePump(){
  if(pumpData == "PUMP_ON"){
    pumpOn(true);
  }else if(pumpData == "PUMP_OFF"){
    pumpOn(false);
  }
}
void pumpOn(bool status){
  if(status){
    // digitalWrite(pump,HIGH);
    digitalWrite(LEDPUMP, HIGH);
    Serial.println("Pume ON");
  }else{
    // digitalWrite(pump,LOW);
    digitalWrite(LEDPUMP, LOW);
    Serial.println("Pume OFF");
  }
}
bool updateConfig(String receiveData){
  String data  = receiveData;
  char delimiter = ',';
  // แบ่งข้อมูลโดยใช้ String::indexOf() และ String::substring()
  String dataValues[9]; // สร้างอาร์เรย์ String มีขนาด 7 สมาชิก
  int startIndex = 0;
  int endIndex;
  for (int i = 0; i < 9; i++) {
      endIndex = data.indexOf(delimiter, startIndex);
      dataValues[i] = data.substring(startIndex, endIndex);
      startIndex = endIndex + 1;
  }
  // แบ่งข้อมูลสุดท้าย
  dataValues[9] = data.substring(startIndex);
  // data = "161,187,10,Pass,0,5,1";
  if(dataValues[3] == "Pass"){
    // pumpMode  mode  pumpState pumpStatus  statusNode  timeNormalMode  timeDebugMode
    pumpMode = dataValues[4].toInt();
    mode = dataValues[5].toFloat();
    pumpState = dataValues[6].toFloat();
    statusNode = dataValues[7].toFloat();
    timeNormalMode = dataValues[8].toFloat();
    timeDebugMode = dataValues[9].toFloat();
    Serial.println("---------------------------------------------");
    Serial.print("pumpMode");
    Serial.println(pumpMode);
    Serial.print("mode");
    Serial.println(mode);
    Serial.print("pumpState");
    Serial.println(pumpState);
    Serial.print("statusNode");
    Serial.println(statusNode);
    Serial.print("timeNormalMode");
    Serial.println(timeNormalMode);
    Serial.print("timeDebugMode");
    Serial.println(timeDebugMode);
    Serial.println("---------------------------------------------");
    return true;
  }else{
     return false;
  }
}