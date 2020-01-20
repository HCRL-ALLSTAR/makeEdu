#ifndef MOTION_CLASS_H
#define MOTION_CLASS_H
#include <Arduino.h>
#include "System/SystemDefaults.hpp"
#include "System/SystemMacros.hpp"

class MotionClass
{
private:
    /* data */
    TaskHandle_t UpdateHandle;

    int value;
    static void UpdateCode(void *);

public:
    MotionClass(/* args */);
    ~MotionClass();
    void begin();
    int getValue();
};

MotionClass::MotionClass(/* args */)
{
}

MotionClass::~MotionClass()
{
}

/*
    begin Motion Sensor
*/
void MotionClass::begin()
{
    pinMode(36, INPUT);
    xTaskCreate(UpdateCode, "Motion Update Task", Default_Task_Stack, this, 1, &UpdateHandle);
}
/*
    Get Current Value ( 0 || 1 )
*/
int MotionClass::getValue()
{
    return this->value;
}

void MotionClass::UpdateCode(void *_this)
{
    MotionClass *task = (MotionClass *)(_this);
    for (;;)
    {
        task->value = digitalRead(36);
        TaskDelay(delay_Time);
    }
}

#endif