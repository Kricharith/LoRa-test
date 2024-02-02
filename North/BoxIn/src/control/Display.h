#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

int lcdState = 0;

void lcdActive()
{
    Serial.println("lcdActive");
    lcd.display(); // เปิดการแสดงตัวอักษร
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please wait");
    // Serial.println("LCD Active please wait");
}

void showMode(bool mode)
{
    lcd.display();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    if (mode)
    {
        lcd.print("Debug Mode");
    }
    else
    {
        lcd.print("Normal Mode");
    }
}

void lcdFirstPage(String batt, float lux, float temp, float humi)
{
    Serial.println("lcdFirstPage");
    lcd.display();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Batterry :");
    lcd.setCursor(11, 0);
    lcd.print(batt);
    lcd.setCursor(0, 1);
    lcd.print("Lux      :");
    lcd.setCursor(11, 1);
    lcd.print(lux);
    lcd.setCursor(0, 2);
    lcd.print("T in Box :");
    lcd.setCursor(11, 2);
    lcd.print(temp);
    lcd.setCursor(0, 3);
    lcd.print("H in Box :");
    lcd.setCursor(11, 3);
    lcd.print(humi);
}

void lcdSecondPage(float tempA, float humiA, float humiS)
{
    Serial.println("lcdSecondPage");
    lcd.display();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T in Air :");
    lcd.setCursor(11, 0);
    lcd.print(tempA);
    lcd.setCursor(0, 1);
    lcd.print("H in Air :");
    lcd.setCursor(11, 1);
    lcd.print(humiA);
    lcd.setCursor(0, 2);
    lcd.print("H in Sol :");
    lcd.setCursor(11, 2);
    lcd.print(humiS);
}