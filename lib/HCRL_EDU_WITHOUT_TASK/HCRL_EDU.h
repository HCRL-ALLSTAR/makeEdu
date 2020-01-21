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
#include "UI/UI.h"
#include "UI/UI.cpp"
class HCRL_EDU
{
private:
    /* data */
public:
    HCRL_EDU(/* args */);
    ~HCRL_EDU();

    wifiWrapper WiFi;
    mqttWrapper MQTT;

    UI Ui;

    angleClass ANGLE;
    pirClass MOTION;
    envClass ENV;
    rgbLedClass RGB_LED;
    rgbStripClass RGB_STRIP;
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
    Ui.wifi_ssid_set(WiFi.getSSID());
    Ui.wifi_status_set(WiFi.getStatus());
    Ui.mqtt_ip_set(MQTT.getServer());
    Ui.mqtt_status_set(MQTT.getStatus());
    Ui.temp_set(ENV.getTemp());
    Ui.humid_set(ENV.getHumi());
    Ui.pa_set(ENV.getPressure());
    Ui.motion_set(MOTION.getValue());
    Ui.update();
    RGB_LED.update();
    RGB_STRIP.update();
    MQTT.update();
}

#endif