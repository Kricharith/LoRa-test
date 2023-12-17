#include <EasyButton.h>

#include <LiquidCrystal_I2C.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_ADS1X15.h>
#include "Wire.h"
#include "DHT.h"

const unsigned long timeShowLcd = 30000;

#define SW 25
#define LED 23

#define DHTPIN 15
#define DHTTYPE DHT22
#define address 0x23  //I2C Address 0x23 เซนเซอร์แสง

DHT dht(DHTPIN, DHTTYPE);
Adafruit_ADS1115 ads;
Adafruit_AHTX0 aht;

LiquidCrystal_I2C lcd(0x27, 16, 2);
EasyButton button(SW);

unsigned long startTime = 0;
unsigned long currentTime = 0;

float t_in_a, h_in_a, t_in_b, h_in_b;
int h_in_s;
bool status_sw = false;
sensors_event_t humidity, temp;

uint8_t buf[4] = { 0 };
uint16_t data, data1;
float Lux;

void am2315() {
  if (!aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    t_in_a = 0;
    h_in_a = 0;
    // while (1) delay(10);
  } else {
    aht.getEvent(&humidity, &temp);  // วัดค่าอุณหภูมิและความชื้น
    t_in_a = temp.temperature;
    h_in_a = humidity.relative_humidity;
    Serial.print(t_in_a);
    Serial.print("                ");
    Serial.println(h_in_a);
  }
  //ถ้า addr+vcc = 0x49
}

void ads1115() {
  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    h_in_s = 0;
  } else {
    int16_t adc3;
    float volts3;
    adc3 = ads.readADC_SingleEnded(3);
    h_in_s = map(adc3, 11400, 14212, 100, 0);
    Serial.println(adc3);
    Serial.print("hins: ");
    Serial.println(h_in_s);
  }
}

void Lightsensor() {
  int checkLightsensor = readReg(0x10, buf, 2);  //Register Address 0x10
  if (checkLightsensor == 0) {
    Serial.println("ไม่พบเซนเซอร์แสง");
    Lux = 0;

  } else {
    data = buf[0] << 8 | buf[1];
    Lux = (((float)data) / 1.2);
    Serial.print("LUX:");
    Serial.print(Lux);
    Serial.print("lx");
    Serial.print("\n");
    // delay(10);
  }
}

uint8_t readReg(uint8_t reg, const void* pBuf, size_t size) {
  if (pBuf == NULL) {
    Serial.println("pBuf ERROR!! : null pointer");
  }
  uint8_t* _pBuf = (uint8_t*)pBuf;
  Wire.beginTransmission(address);
  Wire.write(&reg, 1);
  if (Wire.endTransmission() != 0) {
    return 0;
  }
  // delay(10);
  Wire.requestFrom(address, (uint8_t)size);
  for (uint16_t i = 0; i < size; i++) {
    _pBuf[i] = Wire.read();
  }
  return size;
}

void dht22() {
  float h = dht.readHumidity();
  h_in_b = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // t_in_b = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    h_in_b = 0;
    t_in_b = 0;
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  t_in_b = dht.computeHeatIndex(t, h, false);
  // Serial.print(F("Humidity: "));
  Serial.print(hic);
  Serial.print("            ");
  // Serial.print(F("%  Temperature: "));
  Serial.println(h);
  // Serial.print(F(" C "));
  // Serial.print(f);
  // Serial.print(F(" F  Heat index: "));
  // Serial.print(hic);
  // Serial.print(F(" C "));
  // Serial.print(hif);
  // Serial.println(F(" F"));
}

void display() {
  Serial.println("This is function display!!!");
  lcd.display();  // เปิดการแสดงตัวอักษร
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T in Air : ");
  lcd.setCursor(11, 0);
  lcd.print(t_in_a);
  lcd.setCursor(0, 1);
  lcd.print("H in Air : ");
  lcd.setCursor(11, 1);
  lcd.print(h_in_a);
  // lcd.clear();  // ล้างหน้าจอ
  // lcd.setCursor(0, 0);
  // lcd.print("T in Box : ");
  // lcd.setCursor(11, 0);
  // lcd.print(t_in_b);
  // lcd.setCursor(0, 1);
  // lcd.print("H in Box : ");
  // lcd.setCursor(11, 1);
  // lcd.print(h_in_b);
  // delay(4000);  // หน่วงเวลา 0.5 วินาที
  // lcd.clear();  // ล้างหน้าจอ
  // lcd.setCursor(0, 0);
  // lcd.print("LUX:");
  // lcd.setCursor(4, 0);
  // lcd.print(Lux);
  // lcd.setCursor(9, 0);
  // lcd.print(" lx");
  // lcd.setCursor(0, 1);
  // lcd.print("H in S:");
  // lcd.setCursor(10, 1);
  // lcd.print(h_in_s);
  // delay(4000);  // หน่วงเวลา 0.5 วินาที
  // lcd.clear();  // ล้างหน้าจอ
  // lcd.noDisplay();
  // lcd.noBacklight();
  // delay(5000);
  startTime = currentTime;
}

void readSensor() {
  am2315();
  ads1115();
  Lightsensor();
  dht22();
  Serial.println("This is function readSensor");
  // display();
}

void resetSys() {
  Serial.println("this is function resetSys!!");
  esp_restart();
}

void activeLcd(){
  readSensor();
  Serial.println("this is function activeLcd!!");
  display();
}

void setup() {
  pinMode(SW, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  lcd.begin();
  lcd.clear();
  lcd.noDisplay();
  lcd.noBacklight();
  Serial.begin(115200);
  Wire.begin();
  dht.begin();
  button.begin();
  button.onPressedFor(5000, resetSys);
  button.onPressed(activeLcd);
}

void loop() {
  button.read();
  // Serial.println("Hello loop");

  currentTime = millis();
  if(currentTime - startTime >= timeShowLcd){
    lcd.clear();
    lcd.noDisplay();
    lcd.noBacklight();
  }
  // if (status_sw == true) {
  //   scanI2C();
  //   status_sw = false;
  // }
}
