#include <Arduino.h>
#include "thermistor.h"

#define THERMISTOR_PIN 34        // GPIO34 (ADC1_CH6)
#define SERIES_RESISTOR 10000    // 10k resistor
#define NOMINAL_RESISTANCE 10000 // 10k thermistor
#define NOMINAL_TEMPERATURE 25   // 25°C
#define B_COEFFICIENT 3950       // Beta coefficient

float readThermistorC()
{
    int adc = analogRead(THERMISTOR_PIN);
    float resistance = SERIES_RESISTOR / ((4095.0 / adc) - 1.0);
    float steinhart;
    steinhart = resistance / NOMINAL_RESISTANCE;       // (R/Ro)
    steinhart = log(steinhart);                        // ln(R/Ro)
    steinhart /= B_COEFFICIENT;                        // 1/B * ln(R/Ro)
    steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                       // Invert
    steinhart -= 273.15;                               // convert to °C
    return steinhart;
}
