#ifndef HCRL_EDU_H
#define HCRL_EDU_H

#include <Arduino.h>
#include "WiFi/wifiWrapper.h"
#include "MQTT/mqttWrapper.h"
#include "SENSORS/ANGLE/angleClass.h"
#include "SENSORS/ENV/envClass.h"
#include "SENSORS/PIR/pirClass.h"
#include "SENSORS/LED_RGB/rgbClass.h"
#include "millisDelay/millisDelay.h"
#include "SENSORS/LED_STRIP/stripClass.h"
#include "JSON/ArduinoJson.h"
#include "UI.h"
#include "UI.cpp"
#include "JSON/jsonwrapper.hpp"

using jsonWrapper = json;
using uiWrapper = UI;
using angleWrapper = angleClass;
using pirWrapper = pirClass;
using envWrapper = envClass;
using rgbLedWrapper = rgbLedClass;
using rgbStripWrapper = rgbStripClass;

class HCRL_EDU
{
private:
    /* data */
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
    rgbLedWrapper RGB_LED;
    rgbStripWrapper RGB_STRIP;
    void update();
};

HCRL_EDU::HCRL_EDU(/* args */)
{
}

HCRL_EDU::~HCRL_EDU()
{
}

void HCRL_EDU::update()
{
    Ui.set_wifi_ssid(WiFi.getSSID());
    Ui.set_wifi_status(WiFi.getStatus());
    Ui.set_mqtt_ip(MQTT.getServer());
    Ui.set_mqtt_status(MQTT.getStatus());
    Ui.set_temp(ENV.getTemp());
    Ui.set_humid(ENV.getHumi());
    Ui.set_pa(ENV.getPressure());
    Ui.set_motion(MOTION.getValue());
    Ui.update();
    RGB_LED.update();
    RGB_STRIP.update();
    MQTT.update();
}

#endif