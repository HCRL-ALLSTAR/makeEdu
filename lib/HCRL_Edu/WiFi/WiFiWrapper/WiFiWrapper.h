#ifndef WIFI_WRAPPER_H
#define WIFI_WRAPPER_H

#include <WiFi.h>
#include <Arduino.h>
#include "System/SystemMacros.hpp"

class WiFiWrapper
{
private:
    char *SSID;
    char *PASSWORD;

public:
    WiFiWrapper(/* args */);
    ~WiFiWrapper();

    void Begin(const char *SSID, const char *PASSWORD);
    boolean Connected();
    char *getSSID();
    char *getPASSWORD();
};

WiFiWrapper::WiFiWrapper(/* args */)
{
}

WiFiWrapper::~WiFiWrapper()
{
}

void WiFiWrapper::Begin(const char *SSID, const char *PASSWORD)
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

boolean WiFiWrapper::Connected()
{
    return WiFi.isConnected();
}

char *WiFiWrapper::getSSID()
{
    return this->SSID;
}

char *WiFiWrapper::getPASSWORD()
{
    return this->PASSWORD;
}

#endif