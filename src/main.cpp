// --- Includes ---
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include "../include/wifi_secrets.h"
#include "ledstrip.h"
#include "wifi_status_led.h"
#include "thermistor.h"

// --- Globals ---
WebServer server(80);
float g_smoothedTemp = 25;

// --- Web Handler ---
void handleRoot()
{
  float tempC = g_smoothedTemp;
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang='en'>
    <head>
      <meta charset='UTF-8'>
      <meta name='viewport' content='width=device-width, initial-scale=1.0'>
      <title>ESP32 Thermistor</title>
      <style>
        body { font-family: Arial, sans-serif; background: #f4f4f4; color: #222; text-align: center; margin: 0; padding: 0; }
        .container { background: #fff; margin: 40px auto; padding: 30px 20px; border-radius: 10px; box-shadow: 0 2px 8px #aaa; max-width: 350px; }
        h1 { color: #0077cc; }
        .temp { font-size: 2.5em; margin: 20px 0; }
        button { background: #0077cc; color: #fff; border: none; padding: 10px 20px; border-radius: 5px; font-size: 1em; cursor: pointer; }
        button:hover { background: #005fa3; }
      </style>
      <script>
        setTimeout(function(){ location.reload(); }, 1000);
      </script>
    </head>
    <body>
      <div class='container'>
        <h1>Temperature Monitor</h1>
        <div class='temp'>Temperature: <b>)rawliteral";
  html += String(tempC, 2);
  html += R"rawliteral( &deg;C</b></div>
        <button onclick='location.reload()'>Reload</button>
      </div>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void setup()
{
  // --- Serial ---
  Serial.begin(115200);
  delay(1000);

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
    server.on("/", handleRoot);
    server.begin();
    Serial.println("Web server started.");
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
  }
}