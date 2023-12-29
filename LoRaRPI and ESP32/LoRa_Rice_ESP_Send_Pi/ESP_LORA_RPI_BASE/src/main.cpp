#include <Arduino.h>
#include <SPI.h>              
#include <LoRa.h>
#include <sensorRead.h>
#include <loraTxRx.h>
#include <Adafruit_INA219.h>
#include <EasyButton.h>

#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5

#define BUTTON_PIN 4

// Adafruit_INA219 ina219;
EasyButton button(BUTTON_PIN);

int count = 0;
int state = 0;
bool stateSendData = false;
int countSendData = 0;
String Mymessage = "";

unsigned long previousMillisSw = 0;

void onPressed()
{
  zeroDistance = distance;
  Serial.println("Button pressed");
  delay(20);
  // state = TWO;
}
void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  button.begin();
  //button.onPressed(onPressed);
  button.onPressedFor(2000,onPressed);

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
  if (! ina219.begin())
  {
    Serial.println("Failed to find INA219 chip");
    while (1) 
    {
      delay(10);
    }
  }
  /////////////////////////////////////////////////////////////
  delay(1000);
}

void loop() {
  button.read();
  if(state == ONE){                   //อ่านค่าเซ็นเซอร์
    unsigned long currentMillisSw = millis();
    if (currentMillisSw - previousMillisSw >= 1000) {
        previousMillisSw = currentMillisSw;
        updateSensorUltra();
        count++;
        Serial.println(count);
    }
    if(count == 8){
      state = TWO;
    }
    // state = TWO;
  }else if(state == TWO){                   //อ่านค่าเซ็นเซอร์
    updateSensorTemp_Hum();
    updateSensorVoltage();
    updateSensorUltra();
    // Serial.println(state);
    state = THREE;
  }else if(state == THREE){             //ส่งข้อมูลไปเกตเวย์พร้อมขอค่าconfigต่างๆ
    // Serial.print("state:");
    // Serial.println(state);
    Serial.print(temp);   Serial.print(hum);   Serial.print(adjustedDistance);   Serial.println(busvoltage); 
    Mymessage = String(temp)+","+String(hum)+","+String(adjustedDistance)+","+String(busvoltage);
    // Serial.println(Mymessage);
    sendMessage(Mymessage);
    state = FOUR;
  }else if(state == FOUR){            //รอรับการยืนยันความถูกต้องของข้อมูลจากเกตเวย์
    // Serial.print("state:");
    // Serial.println(state);
    onReceive();
    if(sendSuccess == true && resendData == false){
      state = FIVE;
    }else if(sendSuccess == false && resendData == true){
      if(countSendData >= 5){
        state = FIVE;
      }else{
        countSendData ++;
        Serial.print("countSendData");
        Serial.println(countSendData);
        state = THREE;
      }
    }
  }else if(state == FIVE){           //เข้าโหมดsleep
    Serial.print("state:");
    Serial.println(state);
    esp_deep_sleep_start(); // เข้าสู่โหมด Deep Sleep
  }
  delay(10);
}
