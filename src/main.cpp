#include <Arduino.h>
// Needed for function callbacks from arduino-lmic to work using a class
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <iot_data2.h>
#include <monitor.h>
#include <Wire.h>
#include <ESPAsyncWebServer.h>
#include <SSD1306Wire.h>
// import Lora class
#include <lora.h>
#include <controller.h>
#include <pv_controller.h>
#define OLED_SCL 15
#define OLED_SDA 4
#define OLED_RST 16
#define LED_PIN 25
SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL, GEOMETRY_128_64, I2C_TWO);
iot_data2 *data = nullptr;
DNSServer *dnsServer = nullptr;
AsyncWebServer *server = nullptr;
String www;
uint8_t configured = 0;
SemaphoreHandle_t ongoingLoraCommunication = xSemaphoreCreateBinary();

void os_getDevKey(u1_t *buf) { Lora::os_getappKey(buf); }
void os_getDevEui(u1_t *buf) { Lora::os_getdevEui(buf); }
void os_getArtEui(u1_t *buf) { Lora::os_getjoinEui(buf); }
void setup0(void *parameter);
void loop0(void *parameter);
void dnsloop(void *parameter);
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
void reset_config(void *parameter) {
  SPIFFS.begin();
  File orginal = SPIFFS.open("/config_org.json", "r");
  File config = SPIFFS.open("/config.json", "w");
  if (!orginal || !config) {
    Serial.println("Error opening files");
    return;
  }
  String org = orginal.readString();
  config.print(org);
  config.close();
  orginal.close();
  SPIFFS.end();
  ESP.restart();
}
void button_hold() {
  static time_t last_time = 0;
  if (last_time == 0) {
    last_time = millis();
    return;
  }
  if (millis() - last_time > 3000) {
    // Call reset_config on core 0
    Serial.println("Resetting config");
    xTaskCreatePinnedToCore(reset_config, "reset_config", 5000, NULL, 1, NULL,
                            0);
  } else {
    last_time = 0;
  }
}
monitor *m = monitor::getInstance();
controller *c = controller::get_instance();
pv_controller *p = pv_controller::get_instance();
void setup() {
  Serial.begin(115200);
  data = iot_data2::getInstance();
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_PIN, 0);
  ledcWrite(0, 25);
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(0), button_hold, CHANGE);
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  display.init();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  if (data->get_appKey() == NULL) {
    // Enabling display
    String ssid = data->get_ssid() + String("_") +
                  String(data->to_hex_str(data->get_devEui(), 8)->c_str());
    display.drawStringMaxWidth(0, 0, 128, "Device not configured");
    display.drawStringMaxWidth(0, 22, 128, String("Connect to: ") + ssid);
    DNSServer *dnsServer = new DNSServer();
    AsyncWebServer *server = new AsyncWebServer(80);
    WiFi.softAP(ssid, data->get_password());
    server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    server->begin();
    display.display();
    dnsServer->start(53, "*", WiFi.softAPIP());
    xTaskCreatePinnedToCore(dnsloop, "dnsloop", 20, NULL, 2, NULL, 0);
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
    WiFi.mode(WIFI_OFF);
    digitalWrite(OLED_RST, LOW);
    ESP.restart();
  }
  // INIT monitoring and control on core 0
  xTaskCreatePinnedToCore(setup0, "setup0", 10 * configMINIMAL_STACK_SIZE, NULL,
                          2, NULL, 0);
  display.drawStringMaxWidth(0, 0, 128, "Device configured");
  display.drawStringMaxWidth(
      0, 11, 128,
      String("DevEUI: ") + data->to_hex_str(data->get_devEui(), 8)->c_str());
  display.drawStringMaxWidth(
      0, 33, 128,
      String("JoinEUI: ") + data->to_hex_str(data->get_joinEui(), 8)->c_str());
  display.display();
  delay(30000);
  display.clear();
  display.displayOff();
  display.end();

  // Init LoraWAN
  Lora *lora = Lora::getInstance();
  lora->setup();
  lora->send_data(c->get_data());
}
// Core 1 loop (LoraWAN communication)
void loop() { os_runloop_once(); }

// Core 0 setup and loop control and monitoring
void setup0(void *parameter) {
  m->init();
  while (!c->ready()) {  // calibrating current sensor
    delay(1000);
  }
  p->init();  // Enabling PV output
  for (;;) {
    c->x_recv_data();
  }
}
void dnsloop(void *parameter) {
  for (;;)
    if (dnsServer)
      dnsServer->processNextRequest();
    else
      break;
  vTaskDelete(NULL);
}