#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_ADS1X15.h>
#include <DHT.h>
#include "INA219.h"

#define luxSensor 0x23
#define DHTPIN 15
#define DHTTYPE DHT22

float t_in_a, h_in_a, t_in_b, h_in_b, Lux, batt;
int h_in_s;
// int cycleRead;
Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;
Adafruit_ADS1115 humiditySoil;

uint8_t buf[4] = {0};
DHT dht(DHTPIN, DHTTYPE);
INA219 sensorBatt(0x40);

// void initSensorBatt()
// {
//     if (!sensorBatt.begin())
//     {
//         // Serial.println("could not connect INA219. Fix and Reboot");
//         batt = 0;
//     }
// }

// void initSensorTempInAir()
// {
//     if (!aht.begin())
//     {
//         // Serial.println("Could not find AHT? Check wiring");
//         t_in_a = 0;
//         h_in_a = 0;
//     }
// }

// void initSensorHumiInSoil()
// {
//     if (!humiditySoil.begin())
//     {
//         // Serial.println("Failed to initialize ADS.");
//         h_in_s = 0;
//     }
// }

// void initSensorTempInBox()
// {
//     dht.begin();
// }

// void initSensor()
// {
//     initSensorBatt();
//     initSensorTempInAir();
//     initSensorHumiInSoil();
//     initSensorTempInBox();
// }

void readBatt()
{
    if (!sensorBatt.begin())
    {
        Serial.println("could not connect INA219. Fix and Reboot");
        batt = 0;
    }
    else
    {
        sensorBatt.setMaxCurrentShunt(5, 0.002);
        batt = sensorBatt.getBusVoltage();
    }
}

void readTempInAir()
{
    if (!aht.begin())
    {
        // Serial.println("Could not find AHT? Check wiring");
        t_in_a = 0;
        h_in_a = 0;
    }
    else
    {
        aht.getEvent(&humidity, &temp); // วัดค่าอุณหภูมิและความชื้น
        t_in_a = temp.temperature;
        h_in_a = humidity.relative_humidity;
    }
}

void readHumiInSoil()
{
    if (!humiditySoil.begin())
    {
        // Serial.println("Failed to initialize ADS.");
        h_in_s = 0;
    }
    else
    {
        int16_t adc3;
        float volts3;
        adc3 = humiditySoil.readADC_SingleEnded(3);
        h_in_s = map(adc3, 11400, 14212, 100, 0);
    }
}

uint8_t readRegister(uint8_t reg, const void *pBuf, size_t size)
{
    if (pBuf == NULL)
    {
        // Serial.println("pBuf ERROR!! : null pointer");
    }
    uint8_t *_pBuf = (uint8_t *)pBuf;
    Wire.beginTransmission(luxSensor);
    Wire.write(&reg, 1);
    if (Wire.endTransmission() != 0)
    {
        return 0;
    }
    Wire.requestFrom(luxSensor, size);
    for (uint16_t i = 0; i < size; i++)
    {
        _pBuf[i] = Wire.read();
    }
    return size;
}

void readLight()
{
    int checkLightsensor = readRegister(0x10, buf, 2); // Register Address 0x10
    if (checkLightsensor == 0)
    {
        Serial.println("ไม่พบเซนเซอร์แสง");
        Lux = 0;
    }
    else
    {
        uint16_t data = buf[0] << 8 | buf[1];
        Lux = (((float)data) / 1.2);
    }
}

void readTempInBox()
{
    h_in_b = dht.readHumidity();
    t_in_b = dht.readTemperature();

    if (isnan(h_in_b) || isnan(t_in_b))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        h_in_b = 0;
        t_in_b = 0;
        return;
    }
}

String readSensorAll()
{
    readTempInAir();
    readHumiInSoil();
    readLight();
    readTempInBox();
    readBatt();
    return String(t_in_b) + "," + String(t_in_a) + "," + String(h_in_a) + "," + String(h_in_b) + "," + String(h_in_s) + "," + String(Lux) + "," + String(batt);
}