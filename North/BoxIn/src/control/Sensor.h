#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_ADS1X15.h>
#include <DHT.h>

#define address 0x23
#define DHTPIN 15
#define DHTTYPE DHT22

float t_in_a, h_in_a, t_in_b, h_in_b, Lux;
int h_in_s;
Adafruit_AHTX0 aht;
sensors_event_t humidity, temp;
Adafruit_ADS1115 ads;

uint8_t buf[4] = {0};
DHT dht(DHTPIN, DHTTYPE);

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
        // Serial.print(t_in_a);
        // Serial.print("                ");
        // Serial.println(h_in_a);
    }
}

void readHumiInSoil()
{
    if (!ads.begin())
    {
        // Serial.println("Failed to initialize ADS.");
        h_in_s = 0;
    }
    else
    {
        int16_t adc3;
        float volts3;
        adc3 = ads.readADC_SingleEnded(3);
        h_in_s = map(adc3, 11400, 14212, 100, 0);
        // Serial.println(adc3);
        // Serial.print("hins: ");
        // Serial.println(h_in_s);
    }
}

uint8_t readRegister(uint8_t reg, const void *pBuf, size_t size)
{
    if (pBuf == NULL)
    {
        // Serial.println("pBuf ERROR!! : null pointer");
    }
    uint8_t *_pBuf = (uint8_t *)pBuf;
    Wire.beginTransmission(address);
    Wire.write(&reg, 1);
    if (Wire.endTransmission() != 0)
    {
        return 0;
    }
    Wire.requestFrom(address, size);
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
        // Serial.println("ไม่พบเซนเซอร์แสง");
        Lux = 0;
    }
    else
    {
        uint16_t data = buf[0] << 8 | buf[1];
        Lux = (((float)data) / 1.2);
        // Serial.print("LUX:");
        // Serial.print(Lux);
        // Serial.print("lx");
        // Serial.print("\n");
    }
}

void readTempInBox()
{
    dht.begin();
    h_in_b = dht.readHumidity();
    t_in_b = dht.readTemperature();

    if (isnan(h_in_b) || isnan(t_in_b))
    {
        h_in_b = 0;
        t_in_b = 0;
        // Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    // Serial.print("temp in box : ");
    // Serial.print(t_in_b);
    // Serial.print(" Humi in box : ");
    // Serial.println(h_in_b);
}

String readSensorAll()
{
    readTempInAir();
    readHumiInSoil();
    readLight();
    readTempInBox();
    return String(t_in_b) +","+ String(h_in_b) +","+ String(t_in_a) +","+ String(h_in_a) +","+ String(h_in_s) +","+ String(Lux);
}