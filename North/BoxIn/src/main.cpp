#include <Arduino.h>
#include "control/LoRa.h"
#include "control/Sensor.h"
#include "control/Display.h"

#define BTN_ACTIVE_LCD 25
#define BTN_CHANGE_MODE 35

RTC_DATA_ATTR boolean debugMode = false;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int state = 1;

int periodSentData = 0;
boolean statusSentData;
unsigned long btnLcdPressed = 0;
unsigned long btnModePressed = 0;
String sensorData = "";
unsigned long previousMillisSw = 0;

void showDisplay()
{
    if (lcdState != 1)
    {
        lcdState++;
        lcdFirstPage("0", Lux, t_in_b, t_in_b);
    }
    else
    {
        lcdState = 1;
        lcdSecondPage(t_in_a, h_in_a, h_in_s);
    }
}

IRAM_ATTR void btnActiveLcdIsPressed()
{

    btnLcdPressed = millis();
    statusSentData = false;
    btnState = 11;
    Serial.println("btn lcd is pressed");
}

IRAM_ATTR void btnChangeModeIsPressed()
{
    btnModePressed = millis();
    statusSentData = false;
    btnState = 12;
    Serial.println("btn mode is pressed");
}

void setMode(boolean mode)
{
    if (!mode)
    {
        esp_sleep_enable_timer_wakeup(2 * 60 * 1000 * 1000);
    }
    else
    {
        esp_sleep_enable_timer_wakeup(10 * 1000 * 1000);
    }
}

void setup()
{
    Serial.begin(115200);
    initLoRa();
    lcd.init();
    setMode(debugMode);
    pinMode(BTN_ACTIVE_LCD, INPUT_PULLUP);
    pinMode(BTN_CHANGE_MODE, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN_ACTIVE_LCD), btnActiveLcdIsPressed, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_CHANGE_MODE), btnChangeModeIsPressed, FALLING);
    Serial.println("ESP Active");
}

void loop()
{
    if (btnState == 11)
    {
        if (millis() - btnLcdPressed >= 400)
        {
            // Serial.println("button lcd pressed");
            Serial.println(lcdState);
            if (lcdState == 0)
            {
                lcdActive();
                lcdState = 1;
                // timer = timerBegin(0, 80, true);                 // Timer 0, divider 80
                // timerAttachInterrupt(timer, &showDisplay, true); // Attach callback function
                // timerAlarmWrite(timer, 6000000, true);           // Set period to 0.5 second (500,000 microseconds)
                // timerAlarmEnable(timer);                         // Enable the timer
            }
            else if (lcdState == 1)
            {
                lcdState = 2;
                lcdFirstPage("0", Lux, t_in_b, t_in_b);
            }
            else
            {
                lcdState = 1;
                lcdSecondPage(t_in_a, h_in_a, h_in_s);
            }
            btnState = 0;
        }
        else
        {
            btnState = 0;
        }
    }
    if (btnState == 12)
    {
        if (millis() - btnModePressed >= 400)
        {
            // Serial.println("button mode pressed");
            Serial.println("Change mode");
            debugMode = !debugMode;
            setMode(debugMode);
            Serial.println(debugMode);
            showMode(debugMode);
            btnState = 0;
        }
        else
        {
            btnState = 0;
        }
    }
    if (state == 1)
    {
        sensorData = readSensorAll();
        // Serial.print("state : ");
        // Serial.println(state);
        state++;
    }
    if (state == 2)
    {
        // sentLoRa(0x00, 0xff, sensorData);
        // Serial.print("state : ");
        // Serial.println(state);
        state++;
    }
    if (state == 3)
    {
        statusSentData = receiveLoRa();
        Serial.print("state : ");
        Serial.println(btnState);
        if (btnState == 11)
        {
            return;
        }
        if (statusSentData)
        {
            // Serial.println("LoRa sent success");
            statusSentData = 0;
            state++;
        }
        else if (!statusSentData && periodSentData <= 5)
        {
            // Serial.print("LoRa sent fail resent ");
            // Serial.println(periodSentData);
            periodSentData++;
            statusSentData = 0;
            if (state == 3)
            {
                state--;
            }
        }
        else
        {
            // Serial.println("LoRa Sent Fail");
            state++;
        }
    }
    if (state == 4)
    {
        Serial.println("ESP Sleep");
        esp_deep_sleep_start();
    }
}