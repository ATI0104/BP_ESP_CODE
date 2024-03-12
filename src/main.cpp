#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <control.h>
#include <iot_data2.h>
#include <monitor.h>
#define OLED_SCL 15
#define OLED_SDA 4
#define OLED_RST 16
#include <Wire.h>

#include "ESPAsyncWebServer.h"
#include "SSD1306Wire.h"
#include "lora.h"
SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_64, I2C_TWO);
iot_data2 *data = nullptr;
DNSServer *dnsServer = nullptr;
AsyncWebServer *server = nullptr;
String www;
uint8_t configured = 0;
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
      SPIFFS.begin();
      request->send(SPIFFS, "/index.html", "text/html");
      SPIFFS.end();
      return;

    } else if (url == "/config") {
      AsyncResponseStream *response =
          request->beginResponseStream("application/json");
      JsonDocument doc;
      doc["deveui"] = data->to_hex_str(data->get_devEui(), 8)->c_str();
      doc["appeui"] = data->to_hex_str(data->get_joinEui(), 8)->c_str();
      doc["appkey"] = data->to_hex_str(data->get_appKey(), 16)->c_str();
      serializeJson(doc, *response);
      request->send(response);
      return;
    } else if (url == "/update") {
      AsyncResponseStream *response =
          request->beginResponseStream("text/plain");
      response->print("OK");
      request->send(response);
      return;
    } else {
      Serial.println(url);
      request->redirect("http://" + WiFi.softAPIP().toString() + "/");
    }
  }

  void handleBody(AsyncWebServerRequest *request, uint8_t *d, size_t len,
                  size_t index, size_t total) {
    if (request->url() == "/update") {
      if (!index) {
        JsonDocument doc;
        deserializeJson(doc, d);
        if (doc["deveui"] != nullptr) {
          data->set_devEui(doc["deveui"]);
          data->set_joinEui(doc["appeui"]);
          data->set_appkey(doc["appkey"]);
          Serial.println(data->get_joinEui()[0]);
        }
        request->send(200);
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.drawString(0, 0, "Device configured");
        display.drawStringMaxWidth(
            0, 11, 128,
            "Display and Wi-Fi will be turned off in 5 minutes. You can "
            "download the Devices Configuration.");
        display.display();
        configured = 1;
        return;
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  data = iot_data2::getInstance();
  if (data->get_appKey() == NULL) {
    // Enabling display
    pinMode(OLED_RST, OUTPUT);
    digitalWrite(OLED_RST, HIGH);
    String ssid = data->get_ssid() + String("_") +
                  String(data->to_hex_str(data->get_devEui(), 8)->c_str());
    display.init();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawStringMaxWidth(0, 0, 128, "Device not configured");
    display.drawStringMaxWidth(0, 22, 128, String("Connect to: ") + ssid);
    DNSServer *dnsServer = new DNSServer();
    AsyncWebServer *server = new AsyncWebServer(80);
    WiFi.softAP(ssid, data->get_password());
    server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server->begin();
    display.display();
    while (WiFi.softAPgetStationNum() == 0) {
      delay(500);
    }
    display.clear();
    display.drawStringMaxWidth(0, 0, 128, "Connection established!");
    display.drawStringMaxWidth(0, 11, 128,
                               String("To configure device go to: http://") +
                                   WiFi.softAPIP().toString() + "/");
    display.display();
    while (!configured) {
      delay(500);
    }
    delay(5 * 60 * 1000);
    display.clear();
    display.displayOff();
    display.end();
    WiFi.disconnect(true);
    digitalWrite(OLED_RST, LOW);
  }
  configured = 1;
}

void loop() {}