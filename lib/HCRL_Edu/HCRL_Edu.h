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
    Ui.wifi_ssid_set(WiFi.getSSID());
    Ui.wifi_status_set(WiFi.getStatus());
    Ui.mqtt_ip_set(MQTT.getServer());
    Ui.mqtt_status_set(MQTT.getStatus());
    Ui.temp_set(ENV.getTemp());
    Ui.humid_set(ENV.getHumi());
    Ui.pa_set(ENV.getPressure());
    Ui.motion_set(Motion.getValue());

    TaskDelay(delay_Time);
}

#endif