#ifndef LED_RGB_CLASS_H
#define LED_RGB_CLASS_H
#include <Arduino.h>
//#include <Adafruit_NeoPixel.h>
#include "Adafruit_NeoPixel/Adafruit_NeoPixel.h"

class LedRGBClass
{
private:
    /* data */
    int LedPin = 26;
    int NumPixels = 3;
    Adafruit_NeoPixel pixels;

    TaskHandle_t UpdateHandle;
    static void UpdateCode(void *pv);

public:
    LedRGBClass(/* args */);
    ~LedRGBClass();
    void begin();
    void setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(int Value);
};

LedRGBClass::LedRGBClass(/* args */)
{
    this->pixels = Adafruit_NeoPixel(this->NumPixels, this->LedPin, NEO_GRB + NEO_KHZ800);
}

LedRGBClass::~LedRGBClass()
{
}

void LedRGBClass::UpdateCode(void *pv)
{
    LedRGBClass *task = (LedRGBClass *)(pv);
    for (;;)
    {

        task->pixels.show();
        TaskDelay(delay_Time);
    }
}

/*
    begin Led RGB
*/
void LedRGBClass::begin()
{
    pixels.begin();
    xTaskCreate(UpdateCode, "Pixels Update Task", Default_Task_Stack, this, 1, &UpdateHandle);
}

/*
    Set Color of each Led 
    n -> order of number (start 0)
    r -> Red (max 255)
    g -> Green (max 255)
    b -> Blue (max 255)
*/
void LedRGBClass::setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    this->pixels.setPixelColor(n, r, g, b);
}

/*
    Set Brightness of all ledB
    value -> Brightness value (max 255)
*/
void LedRGBClass::setBrightness(int Value)
{
    this->pixels.setBrightness(Value);
}

#endif