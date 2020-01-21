#ifndef ANGLE_CLASS_H
#define ANGLE_CLASS_H

#include <Arduino.h>

class angleClass
{
private:
    /* data */

    int sensorPin = 36;
    int lastSensorValue = 100;
    int curSensorValue = 0;

public:
    angleClass(/* args */);
    ~angleClass();
    void begin();
    int getValue();
};

angleClass::angleClass(/* args */)
{
}

angleClass::~angleClass()
{
}

void angleClass::begin()
{
    pinMode(sensorPin, INPUT);
    dacWrite(25, 0);
}
int angleClass::getValue()
{
    this->curSensorValue = analogRead(this->sensorPin);
    if (abs(this->curSensorValue - this->lastSensorValue) > 10)
    {
        this->lastSensorValue = this->curSensorValue;
    }
    return this->lastSensorValue;
}
#endif