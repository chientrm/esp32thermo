#pragma once
#include <WebServer.h>

extern bool g_logError;
extern const char *LOG_FILE;

void setupWebServer(WebServer &server);
void setWebServerTempPtr(float *tempPtr);
