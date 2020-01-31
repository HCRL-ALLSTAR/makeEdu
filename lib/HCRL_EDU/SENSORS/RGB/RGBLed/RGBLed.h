#ifndef RGBLed_h
#define RGBLed_h

#include <Arduino.h>
#include "SENSORS/RGB/Adafruit_NeoPixel/Adafruit_NeoPixel.h"
#define LED_PIN 26
#define LED_NUM 3

class RGBLed
{
public:
    RGBLed();
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
    Adafruit_NeoPixel pixel_LED;
    uint8_t r[10], g[10], b[10];
    bool st[10];
    bool began = false;
};
RGBLed::RGBLed()
{
    this->pixel_LED = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);
}

void RGBLed::begin()
{
    this->began = true;
    pixel_LED.begin();
    for (uint8_t i = 0; i < LED_NUM; i++)
    {
        r[i] = 0;
        g[i] = 0;
        b[i] = 0;
        st[i] = 0;
        pixel_LED.setPixelColor(i, pixel_LED.Color(0, 0, 0));
    }
    pixel_LED.show();
}

void RGBLed::update()
{
    if (began)
    {
        for (uint8_t i = 0; i < LED_NUM; i++)
        {
            if (st[i] == 0)
            {
                pixel_LED.setPixelColor(i, pixel_LED.Color(0, 0, 0));
            }
            else
            {
                pixel_LED.setPixelColor(i, pixel_LED.Color(r[i], g[i], b[i]));
            }
            delay(10);
        }
        pixel_LED.show();
    }
}

void RGBLed::set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b)
{
    this->r[index] = r;
    this->g[index] = g;
    this->b[index] = b;
}

void RGBLed::set_status(uint8_t index, bool enabled)
{
    this->st[index] = enabled;
}

void RGBLed::setBrightness(uint8_t value)
{
    if (began)
        pixel_LED.setBrightness(value);
}

uint8_t RGBLed::getRed(uint8_t index)
{
    return r[index];
}
uint8_t RGBLed::getGreen(uint8_t index)
{
    return g[index];
}
uint8_t RGBLed::getBlue(uint8_t index)
{
    return b[index];
}
bool RGBLed::getStatus(uint8_t index)
{
    return st[index];
}

#endif