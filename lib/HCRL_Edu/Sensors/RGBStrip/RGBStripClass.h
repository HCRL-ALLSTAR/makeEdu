#ifndef LED_STRIP_CLASS_H
#define LED_STRIP_CLASS_H
#include <Arduino.h>
#include "System/SystemMacros.hpp"
#include "System/SystemDefaults.hpp"
//#include <Adafruit_NeoPixel.h>
#include "Adafruit_NeoPixel/Adafruit_NeoPixel.h"

class RGBStrip
{
private:
    /* data */
    int LedPin = 15;
    int NumPixels = 10;
    Adafruit_NeoPixel pixels;

    TaskHandle_t UpdateHandle;
    static void UpdateCode(void *pv);

public:
    RGBStrip(/* args */);
    ~RGBStrip();
    void begin();
    void setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(int Value);
};

RGBStrip::RGBStrip(/* args */)
{
    this->pixels = Adafruit_NeoPixel(this->NumPixels, this->LedPin, NEO_GRB + NEO_KHZ800);
}

RGBStrip::~RGBStrip()
{
}

void RGBStrip::UpdateCode(void *pv)
{
    RGBStrip *task = (RGBStrip *)(pv);
    for (;;)
    {

        task->pixels.show();
        TaskDelay(delay_Time);
    }
}

/*
    begin Led rgb strip
*/
void RGBStrip::begin()
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
void RGBStrip::setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    this->pixels.setPixelColor(n, r, g, b);
}

/*
    Set Brightness of all led
    value -> Brightness value (max 255)
*/
void RGBStrip::setBrightness(int Value)
{
    this->pixels.setBrightness(Value);
}
#endif