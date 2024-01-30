#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

Adafruit_BME280 bme;

// #define SEALEVELPRESSURE_HPA (1013.25)

unsigned long previousMillis1 = 0;
extern float temp = 0;
extern float hum = 0;

void updateSensorTemp_Hum(){
  unsigned long currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 >= 1000) {
    previousMillis1 = currentMillis1;
    temp = bme.readTemperature();
    hum = bme.readHumidity();
    // Serial.print(temp);
    // Serial.print("        ");
    // Serial.print(hum);
    // Serial.println("        ");
  }
}
