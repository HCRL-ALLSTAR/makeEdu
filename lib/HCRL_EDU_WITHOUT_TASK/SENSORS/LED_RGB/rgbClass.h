#ifndef RGB_CLASS_H
#define RGB_CLASS_H

#include <Arduino.h>
#include "SYSTEM/macros.hpp"
#include "Adafruit_NeoPixel/Adafruit_NeoPixel.h"
class rgbLedClass
{
private:
    /* data */
    int LedPin = RGB_LED_PIN;
    int NumPixels = RGB_LED_PIXELS;
    boolean isStart = false;
    Adafruit_NeoPixel pixels;

public:
    rgbLedClass();
    ~rgbLedClass();
    void begin();
    void setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(int Value);
    void update();
};
rgbLedClass::rgbLedClass()
{
    this->pixels = Adafruit_NeoPixel(this->NumPixels, this->LedPin, NEO_GRB + NEO_KHZ800);
}

rgbLedClass::~rgbLedClass()
{
}

void rgbLedClass::begin()
{
    pixels.begin();
    isStart = true;
}
void rgbLedClass::setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    this->pixels.setPixelColor(n, r, g, b);
}
void rgbLedClass::setBrightness(int Value)
{
    this->pixels.setBrightness(Value);
}
void rgbLedClass::update()
{
    if (isStart)
    {
        this->pixels.show();
    }
}

#endif