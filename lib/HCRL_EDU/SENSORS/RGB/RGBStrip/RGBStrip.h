#ifndef RGBStrip_h
#define RGBStrip_h

#include <Arduino.h>
#include "SENSORS/RGB/Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#define STRIP_PIN 15
#define STRIP_NUM 10

class RGBStrip
{
public:
    RGBStrip();
    void begin();
    void update();
    void set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
    void set_status(uint8_t index, bool enabled);
    void setBrightness(uint8_t value);
    uint8_t getRed(uint8_t index);
    uint8_t getGreen(uint8_t index);
    uint8_t getBlue(uint8_t index);
    bool getStatus(uint8_t index);

private:
    Adafruit_NeoPixel pixel_STRIP;
    uint8_t r[10], g[10], b[10];
    bool st[10];
    bool began = false;
};

RGBStrip::RGBStrip()
{
    this->pixel_STRIP = Adafruit_NeoPixel(STRIP_NUM, STRIP_PIN, NEO_GRB + NEO_KHZ800);
}

void RGBStrip::begin()
{
    this->began = true;
    pixel_STRIP.begin();
    for (uint8_t i = 0; i < STRIP_NUM; i++)
    {
        r[i] = 0;
        g[i] = 0;
        b[i] = 0;
        st[i] = 0;
        pixel_STRIP.setPixelColor(i, pixel_STRIP.Color(0, 0, 0));
    }
}

void RGBStrip::update()
{
    if (began)
    {
        for (uint8_t i = 0; i < STRIP_NUM; i++)
        {
            if (st[i] == 0)
            {
                pixel_STRIP.setPixelColor(i, pixel_STRIP.Color(0, 0, 0));
            }
            else
            {
                pixel_STRIP.setPixelColor(i, pixel_STRIP.Color(r[i], g[i], b[i]));
            }
            delay(10);
        }
        pixel_STRIP.show();
    }
}

void RGBStrip::set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    this->r[index] = r;
    this->g[index] = g;
    this->b[index] = b;
}

void RGBStrip::set_status(uint8_t index, bool enabled)
{
    this->st[index] = enabled;
}

void RGBStrip::setBrightness(uint8_t value)
{
    if (began)
        pixel_STRIP.setBrightness(value);
}

uint8_t RGBStrip::getRed(uint8_t index)
{
    return r[index];
}
uint8_t RGBStrip::getGreen(uint8_t index)
{
    return g[index];
}
uint8_t RGBStrip::getBlue(uint8_t index)
{
    return b[index];
}
bool RGBStrip::getStatus(uint8_t index)
{
    return st[index];
}

#endif