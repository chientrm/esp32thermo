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
    int pos = (int)((g_lastTemp - minTemp) / (maxTemp - minTemp) * (LED_COUNT - 1));
    if (pos < 0)
        pos = 0;
    if (pos >= LED_COUNT)
        pos = LED_COUNT - 1;
    pos = (LED_COUNT - 1) - pos; // invert strip for upside-down mounting

    // Color mapping: blue (cool), white (mid), orange/yellow (hot)

    float tempNorm = (g_lastTemp - minTemp) / (maxTemp - minTemp);
    if (tempNorm < 0.0)
        tempNorm = 0.0;
    if (tempNorm > 1.0)
        tempNorm = 1.0;

    uint8_t r, g, b;
    if (tempNorm < 0.5)
    {
        // Blue to white
        float t = tempNorm / 0.5;
        r = (uint8_t)(255 * t);
        g = (uint8_t)(255 * t);
        b = 255;
    }
    else
    {
        // White to orange/yellow
        float t = (tempNorm - 0.5) / 0.5;
        r = 255;
        g = (uint8_t)(255 * (1.0 - 0.5 * t)); // fade to yellow
        b = (uint8_t)(255 * (1.0 - t));       // fade to zero
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
