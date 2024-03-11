#include <Arduino.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <control.h>
#include <iot_data2.h>
#include <monitor.h>

#include "ESPAsyncWebServer.h"
iot_data2 *data;
DNSServer dnsServer;
AsyncWebServer server(80);
String www;
class CaptiveRequestHandler : public AsyncWebHandler {
 public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request) {
    // request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print(www);
    request->send(response);
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  data = iot_data2::getInstance();
  if (data->get_appkey() == NULL) {
    SPIFFS.begin();
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
      Serial.println("Failed to open file for reading");
      return;
    }
    www += file.readString();
    file.close();
    SPIFFS.end();
    Serial.println("Starting AP");
    Serial.println(data->get_ssid());
    Serial.println(data->get_password());
    delay(200000);
    WiFi.softAP(data->get_ssid(), data->get_password());
    dnsServer.start(53, "*", WiFi.softAPIP());
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server.begin();
  }
}

void loop() { dnsServer.processNextRequest(); }