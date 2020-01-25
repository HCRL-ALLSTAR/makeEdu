#ifndef RGB_STRIP_CLASS_H
#define RGB_STRIP_CLASS_H

#include <Arduino.h>
#include "SYSTEM/macros.hpp"
#include "Adafruit_NeoPixel/Adafruit_NeoPixel.h"
class rgbStripClass
{
private:
    /* data */
    uint8_t LedPin = RGB_STRIP_PIN;
    uint8_t NumPixels = RGB_STRIP_PIXELS;
    boolean isStart = false;
    Adafruit_NeoPixel pixels;

public:
    rgbStripClass();
    ~rgbStripClass();
    void begin();
    void setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
    void setBrightness(int Value);
    void update();
};
rgbStripClass::rgbStripClass()
{
    this->pixels = Adafruit_NeoPixel(this->NumPixels, this->LedPin, NEO_GRB + NEO_KHZ800);
}

rgbStripClass::~rgbStripClass()
{
}

void rgbStripClass::begin()
{
    pixels.begin();
    isStart = true;
}
void rgbStripClass::setPixelsColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b)
{
    this->pixels.setPixelColor(n, r, g, b);
}
void rgbStripClass::setBrightness(int Value)
{
    this->pixels.setBrightness(Value);
}
void rgbStripClass::update()
{
    if (isStart)
    {
        this->pixels.show();
    }
}

#endif