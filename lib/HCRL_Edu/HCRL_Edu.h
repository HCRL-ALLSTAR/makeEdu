/*
    Lib Index

    MQTT
    * void Begin(const char *Server, int Port, MQTT_CALLBACK_SIGNATURE);
    * void StartSubscribe(const char *Topic);
    * void Publish(const char *Topic, const char *Payload);
    * void SetUser(const char *UserName, const char *Password);
    * void PrintSubscribeTopic();
    * void PrintPublishTopic();
    * boolean getStatus();
    * char *GetServer();
    * int GetPort();
    
    WiFi
    * void Begin(const char *SSID, const char *PASSWORD);
    * boolean getStatus();
    * char *getSSID();
    * char *getPASSWORD();
    
    Battery
    *  void Begin();
    * int getLevel();
    
    Angle 
    * void Begin();
    * int GetValue();
    
    ENV
    * void Begin();
    * float GetTemp();
    * float GetHumi();
    * float GetPressure();
    
    LedRGB 3 Leds
    * void Begin();
    * void setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    * void SetBrightness(int Value);
    
    RGB Strip 10 Leds
    * void Begin();
    * void setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    * void SetBrightness(int Value);
    
    Motion
    * void Begin();
    * int GetValue();
*/

#ifndef HCRL_Edu_H
#define HCRL_Edu_H
#include <Arduino.h>
#include "UI.h"
#include "MQTT/MqttTask.h"
#include "WiFi/WiFiTask.h"
#include "Sensors/ENVs/ENVs.h"
#include "Sensors/Angle/AngleClass.h"
#include "Sensors/MotionClass/MotionClass.h"
#include "Sensors/RGBStrip/RGBStripClass.h"
#include "Sensors/LedRGBClass/LedRGBClass.h"
#include "System/SystemDefaults.hpp"
#include "System/SystemMacros.hpp"
#include "Baterry/baterry.h"
#include "jsonwrapper/jsonwrapper.hpp"
#include "jsonwrapper/ArduinoJson.h"

class HCRL_Edu
{
private:
    /* data */
public:
    HCRL_Edu(/* args */);
    ~HCRL_Edu();
    /* 
    Access to UI
     */
    UI Ui;
    /* 
    Access to ENV Sensor
     */
    ENVs ENV;
    /* 
    Access to Angle Sensor (potentiometer)
    */
    AngleClass Angle;
    /* 
    Access to control 3 led rgbs
    */
    LedRGBClass LedRGB;
    /* 
    Access to PIR or Montion Sensor
    */
    MotionClass Motion;
    /* 
    Access to control 10 rgb strip around m5
    */
    RGBStrip StripRGB;

    /* 
    Access to MQTT connection methods
    */
    MqttTask MQTT;
    /* 
    Access to wifi connection methods
    */
    WiFiTask WiFi;

    /* 
    Access to JSON Method
    */
    json JSON;

    void update();
};

HCRL_Edu::HCRL_Edu(/* args */)
{
}

HCRL_Edu::~HCRL_Edu()
{
}

/*
*** Important ***
for delay loop task and give other task running */
void HCRL_Edu::update()
{
    Ui.set_wifi_ssid(WiFi.getSSID());
    Ui.set_wifi_status(WiFi.getStatus());
    Ui.set_mqtt_ip(MQTT.getServer());
    Ui.set_mqtt_status(MQTT.getStatus());
    Ui.set_temp(ENV.getTemp());
    Ui.set_humid(ENV.getHumi());
    Ui.set_pa(ENV.getPressure());
    Ui.set_motion(Motion.getValue());
    Ui.update();
    TaskDelay(delay_Time);
}

#endif