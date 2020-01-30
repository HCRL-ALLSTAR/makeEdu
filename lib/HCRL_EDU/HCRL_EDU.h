#ifndef HCRL_EDU_H
#define HCRL_EDU_H

#include <Arduino.h>
#include "WiFi/wifiWrapper.h"
#include "MQTT/mqttWrapper.h"
#include "SENSORS/ANGLE/angleClass.h"
#include "SENSORS/ENV/envClass.h"
#include "SENSORS/PIR/pirClass.h"
#include "SENSORS/RGB/RGBLed/RGBLed.h"
#include "SENSORS/RGB/RGBStrip/RGBStrip.h"

#include "millisDelay/millisDelay.h"
#include "JSON/ArduinoJson.h"
#include "UI.h"
#include "UI.cpp"
#include "JSON/jsonwrapper.hpp"

#include <esp_system.h>

using jsonWrapper = json;
using uiWrapper = UI;
using angleWrapper = angleClass;
using pirWrapper = pirClass;
using envWrapper = envClass;

class HCRL_EDU
{
private:
    /* data */
    millisDelay updateDelay;
    hw_timer_t *timer = NULL;

public:
    HCRL_EDU(/* args */);
    ~HCRL_EDU();

    wifiWrapper WiFi;
    mqttWrapper MQTT;
    jsonWrapper JSON;

    uiWrapper Ui;

    angleWrapper ANGLE;
    pirWrapper MOTION;

    envWrapper ENV;

    RGBStrip STRIP;
    RGBLed LED;

    void update();
};

HCRL_EDU::HCRL_EDU(/* args */)
{
    this->updateDelay.start(Sec2MS(3));
    this->timer = timerBegin(0, 80, true); //timer 0, div 80
    timerAttachInterrupt(timer, esp_restart, true);
    timerAlarmWrite(timer, 3000000, false); //set time in us
    timerAlarmEnable(timer);                //enable interrupt
}

HCRL_EDU::~HCRL_EDU()
{
}

void HCRL_EDU::update()
{
    timerWrite(this->timer, 0);
    
    STRIP.update();
    LED.update();
    MQTT.update();
    Ui.set_wifi_ssid(WiFi.getSSID());
    Ui.set_wifi_status(WiFi.getStatus());
    Ui.set_mqtt_ip(MQTT.getServer());
    Ui.set_mqtt_status(MQTT.getStatus());
    Ui.set_temp(ENV.getTemp());
    Ui.set_humid(ENV.getHumi());
    Ui.set_pa(ENV.getPressure());
    Ui.set_motion(MOTION.getValue());
    Ui.update();
}
#endif