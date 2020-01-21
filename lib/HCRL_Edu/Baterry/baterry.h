#ifndef BATERRY_H
#define BATERRY_H
#include <Arduino.h>
#include <Wire.h>
#include <System/SystemMacros.hpp>

class BaterryClass
{
private:
    /* data */
    int8_t level;

    TaskHandle_t updateHandle;
    static void updateCode(void *);
    int8_t getBatteryLevel();

public:
    BaterryClass(/* args */);
    ~BaterryClass();
    /*
        begin Battery Level Check
    */
    void begin();
    /*
        Get Current Baterry level
    */
    int getLevel();
};

BaterryClass::BaterryClass(/* args */)
{
}

BaterryClass::~BaterryClass()
{
}
void BaterryClass::updateCode(void *pv)
{
    BaterryClass *task = (BaterryClass *)(pv);
    for (;;)
    {
        task->level = task->getBatteryLevel();
        TaskDelay(delay_Time);
    }
}
void BaterryClass::begin()
{
    Wire.begin();
    xTaskCreate(updateCode, "bat update", 2048, this, 1, &updateHandle);
}
int BaterryClass::getLevel()
{
    return this->level;
}
int8_t BaterryClass::getBatteryLevel()
{
    Wire.beginTransmission(0x75);
    Wire.write(0x78);
    if (Wire.endTransmission(false) == 0 && Wire.requestFrom(0x75, 1))
    {
        switch (Wire.read() & 0xF0)
        {
        case 0xE0:
            return 25;
        case 0xC0:
            return 50;
        case 0x80:
            return 75;
        case 0x00:
            return 100;
        default:
            return 0;
        }
    }
    return -1;
}
#endif