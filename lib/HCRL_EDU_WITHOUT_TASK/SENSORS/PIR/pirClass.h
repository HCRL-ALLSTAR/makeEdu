#ifndef PIR_CLASS_H
#define PIR_CLASS_H

#include <Arduino.h>

class pirClass
{
private:
    int value;
    /* data */
public:
    pirClass(/* args */);
    ~pirClass();
    void begin();
    int getValue();
};

pirClass::pirClass(/* args */)
{
}

pirClass::~pirClass()
{
}

void pirClass::begin()
{
    pinMode(36, INPUT);
}
int pirClass::getValue()
{
    return digitalRead(36);
}

#endif