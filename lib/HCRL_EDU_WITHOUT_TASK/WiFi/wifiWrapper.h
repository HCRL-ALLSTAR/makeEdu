#ifndef WIFI_WRAPPER_H
#define WIFI_WRAPPER_H

#include <Arduino.h>
#include "SYSTEM/macros.hpp"

#ifdef ESP32
#include <WiFi.h>
#endif

class wifiWrapper
{
private:
    /* data */
    char *SSID;
    char *PASSWORD;

public:
    wifiWrapper(/* args */);
    ~wifiWrapper();
    void begin(const char *, const char *);

    char *getSSID();
    boolean getStatus();
};

wifiWrapper::wifiWrapper(/* args */)
{
}

wifiWrapper::~wifiWrapper()
{
}

void wifiWrapper::begin(const char *SSID, const char *PASSWORD)
{
    this->SSID = (char *)SSID;
    this->PASSWORD = (char *)PASSWORD;

    WiFi.begin(this->SSID, this->PASSWORD);
    Sprintln("Connect to : " + String(this->SSID));
    Sprint("Start Connection => [ ");
    int startTime = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Sprint("=");
        TaskDelay(200);
        if (millis() - startTime > 10000)
        {
            ESP.restart();
        }
    }
    Sprintln(" ] 100% ");
    Sprintln("WiFi Connected");
    Sprint("Local IP : ");
    Sprintln(WiFi.localIP());
}

char *wifiWrapper::getSSID()
{
    return this->SSID;
}
boolean wifiWrapper::getStatus()
{
    return WiFi.isConnected();
}

#endif