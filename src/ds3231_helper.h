#pragma once
#include <Wire.h>
#include <RTClib.h>

void initDS3231();
void updateDS3231FromNTP();
String getDS3231DateTime();
