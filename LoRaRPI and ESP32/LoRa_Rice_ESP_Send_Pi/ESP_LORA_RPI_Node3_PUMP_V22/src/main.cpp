#include <Arduino.h>
#include <SPI.h>              
#include <LoRa.h>
#include <sensorRead.h>
#include <loraTxRx.h>
#include <Adafruit_INA219.h>
#include <EasyButton.h>
#include <EEPROM.h>

#define ONE          1
#define TWO          2
#define THREE        3
#define FOUR         4
#define FIVE         5
#define SIX          6
#define SEVEN        7
#define BUTTON_MODE  2
#define BUTTON_PUMP  4
#define LED_PUMP     33
#define LED_STATUS   32
//====================================EEPROM Address==============================
#define addr_pumpMode        0x10
#define addr_mode            0x20
#define addr_pumpState       0x30
#define addr_statusNode      0x40
#define addr_timeNormalMode  0x50
#define addr_timeDebugMode   0x60

gpio_num_t wakeup_gpio_button_mode = GPIO_NUM_2;
int wakeup_gpio_button_pump = 4;
gpio_num_t ledStatus_gpio = GPIO_NUM_33;
// Adafruit_INA219 ina219;
///////////////////////////////////////////////
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
int countPumpMode = 0;
String Mymessage = "";

volatile unsigned long lastDebounceTimeMode = 0; // เวลาล่าสุดที่ debounce
volatile unsigned long debounceDelayMode = 400; // เวลา debounce (milliseconds)
unsigned long previousMillisCount = 0;

void pumpOn(bool status);
void checkStatePump();
bool updateConfig(String receiveData);
void updateEEPROM();
void setMode(bool mode);

ICACHE_RAM_ATTR void handleInterruptMode() {
  // ตรวจสอบ debounce
  if ((millis() - lastDebounceTimeMode) > debounceDelayMode) {
    mode = !mode;
    Serial.print("Modeeeeeeeeeeee ");
    Serial.println(mode);
  }
  lastDebounceTimeMode = millis();
}
ICACHE_RAM_ATTR void handleInterruptSetZeRo() {
  // ตรวจสอบ debounce
  // if ((millis() - lastDebounceTimeSetZeRo) > debounceDelaySetZeRo) {
  //   // stateSetZero = !stateSetZero;
  // }
  // lastDebounceTimeSetZeRo = millis();
}
void check_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 :  Serial.println("Wakeup caused by external signal using RTC_IO");
                                  Serial.print("mode IN");Serial.println(mode); 
                                  mode = !mode;
                                  Serial.print("mode OUT");Serial.println(mode);
                                  setMode(mode); 
                                  break;
    case ESP_SLEEP_WAKEUP_EXT1 :  Serial.println("Wakeup caused by external signal using RTC_CNTL");
                                  Serial.print("STATUS PUMPPPPPPP");Serial.println(mode); 
                                  // mode = !mode;
                                  // Serial.print("mode OUT");Serial.println(mode);
                                  // setMode(mode); 
                                  break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}
void setup() {
  Serial.begin(115200);
  EEPROM.begin(512); // You can change the size based on your requirements
  /////////////////////////////////////////
  // Set data to EEPROM
  // EEPROM.put(addr_pumpMode, pumpMode);
  // EEPROM.put(addr_mode, mode);
  // EEPROM.put(addr_pumpState, pumpState);
  // EEPROM.put(addr_statusNode, statusNode);
  // EEPROM.put(addr_timeNormalMode, timeNormalMode);
  // EEPROM.put(addr_timeDebugMode, timeNormalMode);
  // EEPROM.commit();
  // EEPROM.get(addr_pumpMode, pumpMode);
  // EEPROM.get(addr_mode, mode);
  // EEPROM.get(addr_pumpState, pumpState);
  // EEPROM.get(addr_statusNode, statusNode);
  // EEPROM.get(addr_timeNormalMode, timeNormalMode);
  // EEPROM.get(addr_timeDebugMode, timeNormalMode);
  //////////////////////////////////////////////////////////////////////////////
  pinMode(BUTTON_MODE,INPUT_PULLUP);
  pinMode(BUTTON_PUMP,INPUT_PULLUP);
  pinMode(LED_STATUS,OUTPUT);
  pinMode(LED_PUMP,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_MODE), handleInterruptMode, RISING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PUMP), handleInterruptSetZeRo, RISING);
  check_wakeup_reason();
  esp_sleep_enable_ext0_wakeup(wakeup_gpio_button_mode,0); //1 = High, 0 = Low
  esp_sleep_enable_ext1_wakeup(BIT(wakeup_gpio_button_pump), ESP_EXT1_WAKEUP_ALL_LOW);
  setMode(mode);

  state = ONE;
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  LoRa.setTxPower(18);
  
  while (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    delay(500);
  }
  Serial.println("LoRa init succeeded.");

  while (!Serial) 
  {
    delay(1);
  }
 //////////////////////////////////////////////////////////////
  Wire.begin(21, 22); // SDA = 21, SCL = 22 (เปลี่ยนตามขาของ ESP32)
  bool statusBme;
  statusBme = bme.begin(0x76);  
  if (!statusBme) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  
  // bool statusIna;
  // statusIna = bme.begin(0x76);  
  // if (!statusIna) {
  //   Serial.println("Failed to find INA219 chip");
  //   while (1);
  // }
  // Initialize the INA219.
  // ina219.begin();
  /////////////////////////////////////////////////////////////
  Serial.println("Startttttt");
  delay(1000);
}

void loop() {
  if(pumpState == 0){
    Serial.println(">>>>>>>>>>>>>>>>>>> pumpMode 0 <<<<<<<<<<<<<<<<<<<<<<");
  
    if(state == ONE){                   //อ่านค่าเซ็นเซอร์
      //Serial.println(state);
      updateSensorTemp_Hum();
      state = TWO;
    }else if(state == TWO){             //ส่งข้อมูลไปเกตเวย์พร้อมขอสถานะปั้มน้ำและค่าconfigต่างๆ
      //Serial.println(state); pumpMode  mode pumpState timeNormalMode timeDebugMode
      // temp hum pumpMode  mode  pumpState pumpStatus  statusNode  timeNormalMode  timeDebugMode
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
          statusNode = 0;
          digitalWrite(LED_STATUS,statusNode);
          gpio_hold_dis(ledStatus_gpio);
          Serial.print("OFF LED_STATUS");
          delay(10);
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
        Serial.print("statusNode: ");  Serial.println(statusNode);
        digitalWrite(LED_STATUS,statusNode);
        gpio_hold_en(ledStatus_gpio);
        state = FIVE;
      }else{
        state = TWO;
      }
      // checkStatePump();
      //Serial.println(state);
      
      //state = FIVE;
    }else if(state == FIVE){            //รอรับการยืนยันความถูกต้องของข้อมูลจากเกตเวย์
      //Serial.println(state);
      Serial.println(">>>>>>>>>>>>>>>>>>>Before<<<<<<<<<<<<<<<<<<<<<<");
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
      Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<");
      updateEEPROM();
      state = SIX;
    }else if(state == SIX){             //บันทึกข้อมูลลงEEPROM
      //Serial.println(state);
      setMode(mode);
      Serial.println(">>>>>>>>>>>>>>>>>>>Before Deep Sleep <<<<<<<<<<<<<<<<<<<<<<");
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
      Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
      esp_deep_sleep_start(); // เข้าสู่โหมด Deep Sleep
      state = SEVEN;
    }
  }else if(pumpMode == 1){
    Serial.println(">>>>>>>>>>>>>>>>>>> pumpMode 1 <<<<<<<<<<<<<<<<<<<<<<");
    unsigned long currentMillisCount = millis();
    if (currentMillisCount - previousMillisCount >= 1000) {
      previousMillisCount = currentMillisCount;
      countPumpMode ++;
    }
    if(countPumpMode == 20){
      if(state == ONE){                   //อ่านค่าเซ็นเซอร์
      //Serial.println(state);    
                                          //อ่านค่าเซ็นเซอร์วัดกระแสว่าปั้มทำงานจริงหรือไม่####################ยังไม่ได้ทำนะครับ
      updateSensorTemp_Hum();
      state = TWO;
      }else if(state == TWO){             //ส่งข้อมูลไปเกตเวย์พร้อมขอสถานะปั้มน้ำและค่าconfigต่างๆ
        //Serial.println(state);
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
            statusNode = 0;
            digitalWrite(LED_STATUS,statusNode);
            gpio_hold_dis(ledStatus_gpio);
            Serial.print("OFF LED_STATUS");
            delay(10);
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
          Serial.print("statusNode: ");  Serial.println(statusNode);
          digitalWrite(LED_STATUS,statusNode);
          gpio_hold_en(ledStatus_gpio);
          state = FIVE;
        }else{
          state = TWO;
        }
        // checkStatePump();
        //Serial.println(state);
        
        //state = FIVE;
      }else if(state == FIVE){            //รอรับการยืนยันความถูกต้องของข้อมูลจากเกตเวย์
        //Serial.println(state);
        Serial.println(">>>>>>>>>>>>>>>>>>>Before<<<<<<<<<<<<<<<<<<<<<<");
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
        Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<");
        updateEEPROM();
        state = SIX;
      }else if(state == SIX){             //บันทึกข้อมูลลงEEPROM
        //Serial.println(state);
        setMode(mode);
        Serial.println(">>>>>>>>>>>>>>>>>>>Before Deep Sleep <<<<<<<<<<<<<<<<<<<<<<");
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
        Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
        countPumpMode = 0;
        return;
      }
    }
  }
  delay(10);

}
// void pumpOn(bool status){
//   if(status){
//     // digitalWrite(pump,HIGH);
//     digitalWrite(LEDPUMP, HIGH);
//     Serial.println("Pume ON");
//   }else{
//     // digitalWrite(pump,LOW);
//     digitalWrite(LEDPUMP, LOW);
//     Serial.println("Pume OFF");
//   }
// }
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
void setMode(bool mode){
  if(mode){
    updateEEPROM();
    esp_sleep_enable_timer_wakeup(timeNormalMode * 60 * 1000000);  // ตื่นขึ้นทุก 5 นาที
    Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>NormalMode<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  }else{
    updateEEPROM();
    esp_sleep_enable_timer_wakeup(timeDebugMode * 60 * 1000000);  // ตื่นขึ้นทุก 1 นาที
    Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>DebugMode<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
  }
}
void updateEEPROM(){
  EEPROM.put(addr_pumpMode, pumpMode);
  EEPROM.put(addr_mode, mode);
  EEPROM.put(addr_pumpState, pumpState);
  EEPROM.put(addr_statusNode, statusNode);
  EEPROM.put(addr_timeNormalMode, timeNormalMode);
  EEPROM.put(addr_timeDebugMode, timeNormalMode);
  EEPROM.commit();
  EEPROM.get(addr_pumpMode, pumpMode);
  EEPROM.get(addr_mode, mode);
  EEPROM.get(addr_pumpState, pumpState);
  EEPROM.get(addr_statusNode, statusNode);
  EEPROM.get(addr_timeNormalMode, timeNormalMode);
  EEPROM.get(addr_timeDebugMode, timeNormalMode);
  Serial.println(">>>>>>>>>>>>>>>>>>>updateEEPROM<<<<<<<<<<<<<<<<<<<<<<");
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
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}