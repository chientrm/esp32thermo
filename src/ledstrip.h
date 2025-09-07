#pragma once
#include <Adafruit_NeoPixel.h>

#define LED_PIN 23
#define LED_COUNT 60

extern Adafruit_NeoPixel strip;
void setup_ledstrip();
void animate_ledstrip();
void set_ledstrip_temp(float tempC);
