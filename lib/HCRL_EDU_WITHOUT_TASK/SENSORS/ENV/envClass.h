#ifndef ENV_CLASS_H
#define ENV_CLASS_H

#include <Arduino.h>
#include <Wire.h>

#include "DHT12/DHT12.h"
#include "DHT12/DHT12.cpp"
#include "Adafruit_BMP280/Adafruit_BMP280.h"
#include "Adafruit_BMP280/Adafruit_BMP280.cpp"
class envClass
{
private:
    DHT12 dht12;
    Adafruit_BMP280 bme;
    /* data */
public:
    envClass(/* args */);
    ~envClass();
    void begin();
    float getTemp();
    float getHumi();
    float getPressure();
};

envClass::envClass(/* args */)
{
}

envClass::~envClass()
{
}
void envClass::begin()
{
    Wire.begin();
    while (!this->bme.begin(0x76))
    {
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    }
}
float envClass::getTemp()
{
    return dht12.readTemperature();
}
float envClass::getHumi()
{
    return dht12.readHumidity();
}
float envClass::getPressure()
{
    return bme.readPressure();
}

#endif