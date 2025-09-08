// --- Includes ---
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "webserver.h"
#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include "../include/wifi_secrets.h"
#include "ledstrip.h"
#include "wifi_status_led.h"
#include "thermistor.h"

// --- Globals ---
WebServer server(80);
float g_smoothedTemp = 25;

void setup()
{
  // --- Serial ---
  Serial.begin(115200);
  delay(1000);

  // --- SPIFFS ---
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS Mount Failed!");
  }

  // --- WiFi ---
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  unsigned long wifiStart = millis();
  bool wifiConnected = false;
  while (millis() - wifiStart < 5000)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      wifiConnected = true;
      break;
    }
    delay(500);
    Serial.print(".");
  }
  if (wifiConnected)
  {
    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    // --- OTA ---
    ArduinoOTA.setHostname("esp32thermo");
    ArduinoOTA.begin();
    Serial.println("OTA Ready");
    // --- mDNS ---
    if (MDNS.begin("esp32thermo"))
    {
      Serial.println("mDNS responder started");
    }
    else
    {
      Serial.println("Error setting up mDNS responder!");
    }
    // --- Web Server ---
    setupWebServer(server);
    server.begin();
    Serial.println("Web server started.");
    setWebServerTempPtr(&g_smoothedTemp);
  }
  else
  {
    Serial.println();
    Serial.println("WiFi not connected. Running in offline mode.");
  }

  // --- WiFi Status LED ---
  setupWifiStatusLed();

  // --- LED Strip ---
  setup_ledstrip();
}

void loop()
{
  // --- Update WiFi Status LED ---
  setWifiStatusLed(WiFi.status() == WL_CONNECTED);

  // --- Network Handlers ---
  if (WiFi.status() == WL_CONNECTED)
  {
    ArduinoOTA.handle();
    server.handleClient();
  }
  else
  {
    // Offline mode: only run LED and temperature logic
    // Optionally print status
    // Serial.println("Offline mode: WiFi not connected.");
  }

  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate >= 1000)
  {
    lastUpdate = now;
    float tempC = readThermistorC();
    // Simple exponential smoothing
    g_smoothedTemp = 0.8 * g_smoothedTemp + 0.2 * tempC;
    set_ledstrip_temp(g_smoothedTemp);
    animate_ledstrip();

    // Log temperature to SPIFFS
    File logFile = SPIFFS.open(LOG_FILE, "a");
    if (logFile)
    {
      logFile.printf("%lu,%.2f\n", now, g_smoothedTemp);
      logFile.close();
      g_logError = false;
    }
    else
    {
      g_logError = true;
    }
  }
}