#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <EasyButton.h>
#include <sensorRead.h>
// #include <ezButton.h>
#define DEBOUNCE_TIME 50 // the debounce time in millisecond, increase this time if it still chatters

#define pump 4
#define ledPump 32
#define ledLoRa 16
// ezButton button(4);


#define SW 4
#define LED 5
bool ledstate = 0;

#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define SIX 6
#define SEVEN 7
int state = 1;
// IRAM_ATTR void handleInterrupt() {
//   state = !state;
// }

void setup() {
  Serial.begin(115200);
  pinMode(SW,INPUT_PULLUP);
  pinMode(ledPump,OUTPUT);
  pinMode(pump,OUTPUT);
  digitalWrite(ledPump, LOW);
  digitalWrite(pump, LOW);
  //attachInterrupt(SW, handleInterrupt, FALLING);
  //RISING,CHANGE FALLING
  // button.setDebounceTime(DEBOUNCE_TIME); // set debounce time to 50 milliseconds
  
  // touchAttachInterrupt(T9, [](){ }, 40); // กำหนดใช้เซ็นเซอร์สำผัสช่อง T3 และให้มีค่าเกิน 40 จึงเกิดอินเตอร์รัพท์
  // esp_sleep_enable_touchpad_wakeup(); // กำหนดการตื่นด้วยเซ็นเซอร์สัมผัส
  // กำหนดขาที่ต่อกับเซ็นเซอร์และกำหนด Active เป็น HIGH
  //esp_sleep_enable_ext0_wakeup((gpio_num_t)33, HIGH);
  // หรือถ้าคุณต้องการให้ตื่นเมื่อครบ 20 วินาที ให้ใช้ฟังก์ชันดังนี้
  // esp_sleep_enable_timer_wakeup(20 * 1000 * 1000);
  
  // bool status;
  // status = bme.begin(0x76);  
  // if (!status) {
  //   Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //   while (1);
  // }
  delay(1000);
}
void pumpOn(bool status){
  if(status){
    digitalWrite(pump,HIGH);
    digitalWrite(ledPump, HIGH);
    Serial.println("Pume ON");
  }else{
    digitalWrite(pump,LOW);
    digitalWrite(ledPump, LOW);
    Serial.println("Pume OFF");
  }
}
void loop() {
  if(state == ONE){                   //อ่านค่าเซ็นเซอร์
    Serial.println(state);
    state = TWO;
  }else if(state == TWO){             //ส่งข้อมูลไปเกตเวย์พร้อมขอสถานะปั้มน้ำและค่าconfigต่างๆ
    Serial.println(state);
    state = THREE;
  }else if(state == THREE){            //รอรับการยืนยันความถูกต้องของข้อมูลจากเกตเวย์
    Serial.println(state);
    state = FOUR;
  }else if(state == FOUR){            //ส่งสถานะปั้มน้ำปัจจุบันไปยังเกตเวย์
    Serial.println(state);
    state = FIVE;
  }else if(state == FIVE){            //รอรับการยืนยันความถูกต้องของข้อมูลจากเกตเวย์
    Serial.println(state);
    state = SIX;
  }else if(state == SIX){             //บันทึกข้อมูลลงEEPROM
    Serial.println(state);
    state = SEVEN;
  }else if(state == SEVEN){           // เข้าสู่โหมด Deep Sleep
    Serial.print("state:");
    Serial.println(state);
    // esp_deep_sleep_start(); // เข้าสู่โหมด Deep Sleep
    state = ONE;
  }
  // delay(1000);
}
