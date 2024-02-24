#include <Arduino.h>
#include <EEPROM.h>
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
#define ADDR_MODE 0x10

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

int state = 1;
int btnState = 0;

int periodSentData = 0;
int statusSentData;
boolean statusLCD = false;
boolean lcdFlag = false;
boolean debugMode = false;
unsigned long btnLcdPressed = 0;
unsigned long btnModePressed = 0;
String sensorData = "";
unsigned long previousMillisSw = 0;
unsigned long timeToPressedBTN = 0;
unsigned long timeToActiveLCD = 0;

void updateConfigMode(boolean mode)
{
    EEPROM.put(ADDR_MODE, mode);
    EEPROM.commit();
    EEPROM.get(ADDR_MODE, debugMode);
}

void getConfigMode()
{
    EEPROM.get(ADDR_MODE, debugMode);
}

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
    getConfigMode();
    statusLCD = true;
    Serial.println("Change mode");
    debugMode = !debugMode;
    setMode(debugMode);
    Serial.println(debugMode);
    timeToActiveLCD = millis();
    showMode(debugMode);
    updateConfigMode(debugMode);
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
    sensorData = readSensorAll();
    Serial.println(sensorData);
    Serial.println("in showDisplay : ");
    Serial.print("t_in_b : ");
    Serial.println(t_in_b);
    Serial.print("h_in_b : ");
    Serial.println(h_in_b);
    if (lcdState == 0 || lcdState == 1)
    {
        timeToActiveLCD = millis();
        lcdState = 2;
        lcdFirstPage(batt, Lux, t_in_b, h_in_b);
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
    EEPROM.begin(512);
    Serial.begin(115200);
    initLoRa();
    lcd.init();
    dht.begin();
    pinMode(BTN_ACTIVE_LCD, INPUT_PULLUP);
    pinMode(BTN_CHANGE_MODE, INPUT);

    attachInterrupt(digitalPinToInterrupt(BTN_ACTIVE_LCD), btnActiveLcdIsPressed, FALLING);
    attachInterrupt(digitalPinToInterrupt(BTN_CHANGE_MODE), btnChangeModeIsPressed, FALLING);
    esp_sleep_enable_ext0_wakeup(GPIO_LCD, 0);
    esp_sleep_enable_ext1_wakeup(MODE_GPIO, ESP_EXT1_WAKEUP_ALL_LOW);
    checkWakeupReason();

    Serial.println("Curren mode : ");
    getConfigMode();
    Serial.println(debugMode);
    setMode(debugMode);
    Serial.println("ESP Active");
}

void loop()
{
    if (lcdFlag) // Change the screen every time. When function changeDisplay works.
    {
        sensorData = readSensorAll();
        if (lcdState == 1)
        {
            lcdState = 2;
            lcdFirstPage(batt, Lux, t_in_b, h_in_b);
            lcdFlag = false;
        }
        else
        {
            lcdState = 1;
            lcdSecondPage(t_in_a, h_in_a, h_in_s);
            lcdFlag = false;
        }
    }
    if (millis() - timeToActiveLCD >= LCD_WORKING_TIME && statusLCD) // Control screen to turn off.
    {
        Serial.println("lcd off");
        statusLCD = false;
        lcdShutdown();
        // timerAlarmDisable(timer);
    }
    if (btnState == 11) // Pressed button GPIO25
    {
        sensorData = readSensorAll();
        // Serial.println(sensorData);
        statusLCD = true;
        // Serial.println(lcdState);
        if (lcdState == 0)
        {
            timeToActiveLCD = millis();
            // lcdActive();
            lcdState = 1;
            showDisplay();
            enableTimeInterrupt();
        }
        else
        {
            timeToActiveLCD = millis();
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
    if (btnState == 12) // Pressed button GPIO35
    {
        changeMode();
        btnState = 0;
    }
    if (state == 1) // Read sensor all
    {
        sensorData = readSensorAll();
        Serial.print("state : ");
        Serial.println(state);
        // Serial.println(sensorData);
        state++;
    }
    if (state == 2) // Sent data to geteway
    {
        Serial.print("state : ");
        Serial.println(state);
        sentLoRa(ADDR_SOUCE, ADDR_DEST, sensorData);
        state++;
    }
    if (state == 3) // Receive data from gateway
    {
        Serial.print("state : ");
        Serial.println(state);
        loraTime = millis();
        while (true)
        {
            int packetSize = LoRa.parsePacket();
            if (packetSize) // ESP32 have receives data
            {
                statusSentData = checkDataLoRa();
                if (statusSentData == 1) // data correct
                {
                    Serial.println("LoRa sent success");
                    state = 4;
                    break;
                }
                else if (statusSentData == 2) // Hard reset from wweb server.
                {
                    esp_restart();
                }
                else // data not correct. Send again.
                {
                    periodSentData++;
                    Serial.print("LoRa sent fail resent : ");
                    Serial.println(periodSentData);
                    state = 2;
                    break;
                }
            }
            else if (millis() - loraTime >= 5000) // Data not received within 5 seconds. Send again.
            {
                periodSentData++;
                Serial.print("LoRa sent fail resent : ");
                Serial.println(periodSentData);
                state = 1;
                break;
            }
            else if (periodSentData >= 5) // Data not received within 25 seconds. Send fail.
            {
                Serial.println("LoRa sent fail");
                state = 4;
                break;
            }
            else if (btnState == 11 || btnState == 12) // Button is pressed. Exit loop.
            {
                break;
            }
        }
    }
    if (state == 4 && !statusLCD) // ESP32 sleep
    {
        Serial.print("state : ");
        Serial.println(state);
        Serial.println("ESP Sleep");
        updateConfigMode(debugMode);
        Serial.println(debugMode);
        esp_deep_sleep_start();
    }
}