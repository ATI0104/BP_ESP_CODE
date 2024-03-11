#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <control.h>
#include <iot_data2.h>
#include <monitor.h>
#define OLED_SCL 15  // GPIO15 /
#define OLED_SDA 4   // GPIO4 /
#define OLED_RST 16  // Reset pin (Some OLED displays do not have it)
#include "ESPAsyncWebServer.h"
iot_data2 *data = nullptr;
DNSServer *dnsServer = nullptr;
AsyncWebServer *server = nullptr;
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
    auto url = request->url();
    if (url.equals("/")) {
      AsyncResponseStream *response = request->beginResponseStream("text/html");
      response->print(www);
      request->send(response);
      return;
    } else if (url == "/config") {
      AsyncResponseStream *response =
          request->beginResponseStream("application/json");
      JsonDocument doc;
      doc["deveui"] = data->get_devEui();
      doc["appeui"] = data->get_joinEui();
      doc["appkey"] = data->get_appkey();
      doc.shrinkToFit();
      String json;
      serializeJson(doc, json);
      response->print(json);
      request->send(response);
      return;
    } else if (url == "/update") {
      AsyncResponseStream *response =
          request->beginResponseStream("text/plain");
      response->print("OK");
      request->send(response);
      // Get json from request
      AsyncWebParameter *p = request->getParam("json");
      if (p) {
        auto json = p->value();
        JsonDocument doc;
        deserializeJson(doc, json);
        data->set_devEui(doc["deveui"]);
        data->set_joinEui(doc["appeui"]);
        data->set_appkey(doc["appkey"]);
      }
      return;
    } else {
      request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    }
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
    dnsServer = new DNSServer();
    server = new AsyncWebServer(80);
    WiFi.softAP(data->get_ssid(), data->get_password());
    dnsServer->start(53, "*", WiFi.softAPIP());
    server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server->begin();
  }
}

void loop() { dnsServer->processNextRequest(); }