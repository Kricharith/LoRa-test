#include <Arduino.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
extern Adafruit_BME280 bme;

// #define SEALEVELPRESSURE_HPA (1013.25)
unsigned char data[4] = {};
unsigned long previousMillis1 = 0;
float temp = 0;
float hum = 0;

void updateSensorTemp_Hum(){
  unsigned long currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 >= 1000) {
    previousMillis1 = currentMillis1;
    temp = bme.readTemperature();
    hum = bme.readHumidity();
    Serial.print(temp);
    Serial.print("        ");
    Serial.print(hum);
    Serial.println("        ");
  }
}
