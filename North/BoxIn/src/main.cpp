#include <Arduino.h>
#include "control/LoRa.h"
#include "control/Sensor.h"
#include "control/Display.h"

#define BTN_ACTIVE_LCD 25
#define BTN_CHANGE_MODE 35
#define GPIO_LCD GPIO_NUM_25
#define MODE_GPIO 0x800000000
#define SW_DEBOUNCE_TIME 400
#define PRESSED_RESET_TIME 5000
#define LCD_WORKING_TIME 30000

RTC_DATA_ATTR boolean debugMode = false;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int state = 1;
int btnState = 0;

int periodSentData = 0;
boolean statusSentData;
boolean statusLCD = false;
boolean lcdFlag = false;
unsigned long btnLcdPressed = 0;
unsigned long btnModePressed = 0;
String sensorData = "";
unsigned long previousMillisSw = 0;
unsigned long timeToPressedBTN = 0;
unsigned long timeToActiveLCD = 0;

void setMode(boolean mode)
{
    if (!mode)
    {
        esp_sleep_enable_timer_wakeup(5 * 60 * 1000 * 1000);
    }
    else
    {
        esp_sleep_enable_timer_wakeup(10 * 1000 * 1000);
    }
}

void changeMode()
{
    statusLCD = true;
    Serial.println("Change mode");
    debugMode = !debugMode;
    setMode(debugMode);
    Serial.println(debugMode);
    timeToActiveLCD = millis();
    showMode(debugMode);
    btnState = 0;
}

void changeDisplay()
{
    portENTER_CRITICAL_ISR(&timerMux);
    lcdFlag = true;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void enableTimeInterrupt()
{
    timer = timerBegin(0, 80, true);                   // Timer 0, divider 80
    timerAttachInterrupt(timer, &changeDisplay, true); // Attach callback function
    timerAlarmWrite(timer, 5000000, true);             // Set period to 0.5 second (500,000 microseconds)
    timerAlarmEnable(timer);                           // Enable the timer
}

void showDisplay()
{
    if (lcdState == 0 || lcdState == 1)
    {
        timeToActiveLCD = millis();
        lcdState = 2;
        lcdFirstPage("0", Lux, t_in_b, t_in_b);
    }
    else if (lcdState == 2)
    {
        timeToActiveLCD = millis();
        lcdState = 1;
        lcdSecondPage(t_in_a, h_in_a, h_in_s);
    }
}

void checkWakeupReason()
{
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("Wakeup by GPIO25");
        Serial.print("Show LCD");
        timeToActiveLCD = millis();
        statusLCD = true;
        enableTimeInterrupt();
        showDisplay();
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        Serial.println("Wakeup by GPIO35");
        Serial.print("Change mode");
        changeMode();
        break;
    }
}

IRAM_ATTR void btnActiveLcdIsPressed()
{
    if (millis() - btnLcdPressed >= SW_DEBOUNCE_TIME)
    {
        // statusSentData = false;
        btnState = 11;
        timeToPressedBTN = millis();
        // Serial.println("btn lcd is pressed");
    }
    btnLcdPressed = millis();
}

IRAM_ATTR void btnChangeModeIsPressed()
{
    if (millis() - btnLcdPressed >= SW_DEBOUNCE_TIME)
    {
        // statusSentData = false;
        btnState = 12;
        // Serial.println("btn lcd is pressed");
    }
    btnLcdPressed = millis();
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
    esp_sleep_enable_ext0_wakeup(GPIO_LCD, 0);
    esp_sleep_enable_ext1_wakeup(MODE_GPIO, ESP_EXT1_WAKEUP_ALL_LOW);
    checkWakeupReason();
    Serial.println("ESP Active");
}

void loop()
{
    if (lcdFlag)
    {
        if (lcdState == 1)
        {
            lcdState = 2;
            lcdFlag = false;
            lcdFirstPage("0", Lux, t_in_b, t_in_b);
        }
        else
        {
            lcdState = 1;
            lcdFlag = false;
            lcdSecondPage(t_in_a, h_in_a, h_in_s);
        }
    }
    if (millis() - timeToActiveLCD >= LCD_WORKING_TIME && statusLCD)
    {
        Serial.println("lcd off");
        statusLCD = false;
        lcdShutdown();
        timerAlarmDisable(timer);
    }
    if (btnState == 11)
    {
        statusLCD = true;
        Serial.println(lcdState);
        if (lcdState == 0)
        {
            timeToActiveLCD = millis();
            lcdActive();
            lcdState = 1;
            enableTimeInterrupt();
        }
        else
        {
            showDisplay();
        }
        while (digitalRead(BTN_ACTIVE_LCD) == 0)
        {
            if (millis() - timeToPressedBTN >= PRESSED_RESET_TIME)
            {
                Serial.println("Reset ESP");
                esp_restart();
            }
        }
        btnState = 0;
    }
    if (btnState == 12)
    {
        changeMode();
    }
    if (state == 1)
    {
        sensorData = readSensorAll();
        Serial.print("state : ");
        Serial.println(state);
        state++;
    }
    if (state == 2)
    {
        Serial.print("state : ");
        Serial.println(state);
        sentLoRa(ADDR_SOUCE, 0xff, sensorData);
        state++;
    }
    if (state == 3)
    {
        Serial.print("state : ");
        Serial.println(state);
        loraTime = millis();
        while (true)
        {
            int packetSize = LoRa.parsePacket();
            if (packetSize)
            {
                statusSentData = checkDataLoRa();
                if (statusSentData)
                {
                    Serial.println("LoRa sent success");
                    state = 4;
                    break;
                }
                else
                {
                    periodSentData++;
                    Serial.print("LoRa sent fail resent : ");
                    Serial.println(periodSentData);
                    state = 2;
                    break;
                }
            }
            else if (millis() - loraTime >= 5000)
            {
                periodSentData++;
                Serial.print("LoRa sent fail resent : ");
                Serial.println(periodSentData);
                state = 2;
                break;
            }
            else if (periodSentData >= 5)
            {
                Serial.println("LoRa sent fail");
                state = 4;
                break;
            }
            else if (btnState == 11 || btnState == 12)
            {
                break;
            }
        }
    }
    if (state == 4 && !statusLCD)
    {
        Serial.print("state : ");
        Serial.println(state);
        Serial.println("ESP Sleep");
        esp_deep_sleep_start();
    }
}