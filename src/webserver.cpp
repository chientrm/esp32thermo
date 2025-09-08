#include "webserver.h"
#include <SPIFFS.h>

bool g_logError = false;
const char *LOG_FILE = "/temp_log.txt";

static float *g_smoothedTempPtr = nullptr;

void handleRoot(WebServer &server)
{
    float tempC = g_smoothedTempPtr ? *g_smoothedTempPtr : 0;
    size_t totalBytes = SPIFFS.totalBytes();
    size_t usedBytes = SPIFFS.usedBytes();
    size_t freeBytes = totalBytes > usedBytes ? totalBytes - usedBytes : 0;
    float totalMB = totalBytes / 1048576.0;
    float usedMB = usedBytes / 1048576.0;
    float freeMB = freeBytes / 1048576.0;
    char html[2048];
    snprintf(html, sizeof(html),
             "<!DOCTYPE html>\n"
             "<html lang='en'>\n"
             "<head>\n"
             "  <meta charset='UTF-8'>\n"
             "  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
             "  <title>ESP32 Thermistor</title>\n"
             "  <style>\n"
             "    body { font-family: Arial, sans-serif; background: #f4f4f4; color: #222; text-align: center; margin: 0; padding: 0; }\n"
             "    .container { background: #fff; margin: 40px auto; padding: 30px 20px; border-radius: 10px; box-shadow: 0 2px 8px #aaa; max-width: 350px; }\n"
             "    h1 { color: #0077cc; }\n"
             "    .temp { font-size: 2.5em; margin: 20px 0; }\n"
             "    .error { color: #c00; font-weight: bold; margin: 10px 0; }\n"
             "    .spiffs { font-size: 0.95em; color: #555; margin: 10px 0; }\n"
             "    button, .wipe-btn { background: #0077cc; color: #fff; border: none; padding: 10px 20px; border-radius: 5px; font-size: 1em; cursor: pointer; margin: 5px; }\n"
             "    button:hover, .wipe-btn:hover { background: #005fa3; }\n"
             "  </style>\n"
             "  <script>\n"
             "    function wipeLog() {\n"
             "      fetch('/wipe', {method: 'POST'})\n"
             "        .then(() => location.reload());\n"
             "    }\n"
             "  </script>\n"
             "</head>\n"
             "<body>\n"
             "  <div class='container'>\n"
             "    <h1>Temperature Monitor</h1>\n"
             "    <div class='temp'>Temperature: <b>%.2f &deg;C</b></div>\n"
             "    <div class='spiffs'>SPIFFS: %.2f MB used / %.2f MB total (<b>%.2f MB free</b>)</div>\n"
             "    <button onclick='location.reload()'>Reload</button>\n"
             "    <button class='wipe-btn' onclick='wipeLog()'>Wipe Temp Log</button>\n"
             "    <br><br>\n"
             "    <a href='/log'>Download Temp Log</a>\n",
             tempC, usedMB, totalMB, freeMB);
    String page = html;
    if (g_logError)
    {
        page += "<div class='error'>Log error: SPIFFS full or write failed!</div>\n";
    }
    page += "  </div>\n";
    page += "</body>\n";
    page += "</html>\n";
    server.send(200, "text/html", page);
}

void handleLogDownload(WebServer &server)
{
    File logFile = SPIFFS.open(LOG_FILE, "r");
    if (!logFile)
    {
        server.send(404, "text/plain", "Log file not found");
        return;
    }
    String logData;
    while (logFile.available())
    {
        logData += logFile.readStringUntil('\n') + "\n";
    }
    logFile.close();
    server.send(200, "text/plain", logData);
}

void handleWipeLog(WebServer &server)
{
    if (SPIFFS.remove(LOG_FILE))
    {
        g_logError = false;
        server.send(200, "text/plain", "Log file wiped");
    }
    else
    {
        server.send(500, "text/plain", "Failed to wipe log file");
    }
}

void setupWebServer(WebServer &server)
{
    server.on("/", [&server]()
              { handleRoot(server); });
    server.on("/log", [&server]()
              { handleLogDownload(server); });
    server.on("/wipe", HTTP_POST, [&server]()
              { handleWipeLog(server); });
}

void setWebServerTempPtr(float *tempPtr)
{
    g_smoothedTempPtr = tempPtr;
}
