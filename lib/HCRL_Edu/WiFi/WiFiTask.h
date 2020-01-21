#ifndef WiFi_Wrapper_H
#define WiFi_Wrapper_H

#include <Arduino.h>
#include "System/SystemMacros.hpp"
#include "System/SystemDefaults.hpp"
#include "WiFiWrapper/WiFiWrapper.h"

class WiFiTask
{
private:
    WiFiWrapper wrapper;
    TaskHandle_t BeginHandle;
    static void BeginCode(void *pv);
    char *SSID;
    char *PASSWORD;

public:
    WiFiTask(/* args */);
    ~WiFiTask();
    void Begin(const char *SSID, const char *PASSWORD);
    boolean getStatus();
    char *getSSID();
    char *getPASSWORD();
};

WiFiTask::WiFiTask(/* args */)
{
}

WiFiTask::~WiFiTask()
{
}

void WiFiTask::BeginCode(void *pv)
{
    WiFiTask task = *(WiFiTask *)(pv);
    task.wrapper.Begin(task.SSID, task.PASSWORD);
    for (;;)
    {
        TaskDelay(delay_Time);
    }
}

/*
    start connect to wift with ssid and password
    SSID        -> wifi name
    Password    -> wifi password
*/
void WiFiTask::Begin(const char *SSID, const char *PASSWORD)
{
    this->SSID = (char *)SSID;
    this->PASSWORD = (char *)PASSWORD;
    xTaskCreate(BeginCode, "Begin Task", Default_Task_Stack * 2, this, 30, &BeginHandle); // Create Task Wifi Connection

    while (!WiFi.isConnected())
    {
        /* code */
    }
}

/*
    Get wifi status 
    0 -> not connect
    1 -> connected
*/
boolean WiFiTask::getStatus()
{
    return this->wrapper.Connected();
}
/* 
    Get SSID 
*/
char *WiFiTask::getSSID()
{
    return this->SSID;
}
/* 
    Get Password
*/
char *WiFiTask::getPASSWORD()
{
    return this->wrapper.getPASSWORD();
}
#endif
