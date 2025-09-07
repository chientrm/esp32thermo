#include "ledstrip.h"

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

float g_lastTemp = 0;
void set_ledstrip_temp(float tempC)
{
    g_lastTemp = tempC;
}

void setup_ledstrip()
{
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
}

void animate_ledstrip()
{
    // Display temperature as a bar graph on the LED strip
    // Example: 0°C = 0 LEDs, 60°C = all LEDs
    float minTemp = 10.0;
    float maxTemp = 40.0;
    float tempNorm = (g_lastTemp - minTemp) / (maxTemp - minTemp);
    if (tempNorm < 0.0)
        tempNorm = 0.0;
    if (tempNorm > 1.0)
        tempNorm = 1.0;
    int pos = (int)(tempNorm * (LED_COUNT - 1));
    pos = (LED_COUNT - 1) - pos; // invert for upside-down

    // Classic temperature bands
    float tempC = g_lastTemp;
    uint8_t r, g, b;
    if (tempC <= 22)
    {
        r = 30;
        g = 144;
        b = 255; // Deep Blue
    }
    else if (tempC <= 25)
    {
        r = 0;
        g = 255;
        b = 247; // Cyan
    }
    else if (tempC <= 28)
    {
        r = 173;
        g = 255;
        b = 47; // Green-Yellow
    }
    else if (tempC <= 31)
    {
        r = 255;
        g = 255;
        b = 0; // Yellow
    }
    else if (tempC <= 34)
    {
        r = 255;
        g = 140;
        b = 0; // Orange
    }
    else
    {
        r = 255;
        g = 48;
        b = 48; // Red
    }
    for (int i = 0; i < LED_COUNT; i++)
    {
        if (i == pos)
        {
            strip.setPixelColor(i, strip.Color(r, g, b));
        }
        else
        {
            strip.setPixelColor(i, 0);
        }
    }
    strip.show();
}
