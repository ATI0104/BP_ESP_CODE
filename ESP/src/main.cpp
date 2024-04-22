/**
 * @file main.cpp
 * @brief Main source file for the ESP project.
 *
 * This file contains the main setup and loop functions for the ESP project.
 * It includes various libraries and defines necessary constants and variables.
 * The setup function initializes the device and checks if it is already
 * configured. If not, it sets up a configuration site for the user to configure
 * the device. The loop function handles LoraWAN communication on core 1. The
 * setup_and_loop_0 function handles control and monitoring on core 0. Other
 * helper functions and classes are also defined in this file.
 */

#include <Arduino.h>
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
/**
 * @brief Function which copies the appKey to the buf variable. Needed because
 * the LMIC library is not compatible with C++ classes.
 *
 * @param buf
 */
void os_getDevKey(u1_t *buf) { Lora::os_getappKey(buf); }
/**
 * @brief Function which copies the devEui to the buf variable. Needed because
 * the LMIC library is not compatible with C++ classes.
 *
 * @param buf
 */
void os_getDevEui(u1_t *buf) { Lora::os_getdevEui(buf); }
/**
 * @brief Function which copies the joinEui to the buf variable. Needed because
 * the LMIC library is not compatible with C++ classes.
 *
 * @param buf
 */
void os_getArtEui(u1_t *buf) { Lora::os_getjoinEui(buf); }

void setup_and_loop_0(void *parameter);
void dnsloop(void *parameter);
/**
 * @brief HTTP request handler for the configuration page
 *
 * This class is responsible for handling HTTP requests related to the
 * configuration page. It inherits from the AsyncWebHandler class and implements
 * the necessary methods to handle requests.
 */
class CaptiveRequestHandler : public AsyncWebHandler {
 public:
  /**
   * @brief Default constructor for CaptiveRequestHandler
   */
  CaptiveRequestHandler() {}

  /*/**
   @brief Destructor for CaptiveRequestHandler
   */
  virtual ~CaptiveRequestHandler() {}

  /**
  @brief Determines if the request can be handled by this handler
   *
   * This method is called to determine if the incoming request can be handled
  by this request handler.
   * In this implementation, it always returns true.
   *
   * @param request The incoming request
   * @return true if the request can be handled, false otherwise
   */
  bool canHandle(AsyncWebServerRequest *request) { return true; }

  /**
  @brief Handles the incoming request
   *
   * This method is called to handle the incoming request. It checks the URL of
  the request and performs
   * the necessary actions based on the URL.
   *
   * @param request The incoming request
   */
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

  /**
  @brief Handles the body of the request
   *
   * This method is called to handle the body of the request. It is specifically
  used for handling
   * the body of the "/update" request. If the request URL is "/update" and the
  index is 0, it
   * deserializes the JSON data and updates the device configuration.
   *
   * @param request The incoming request
   * @param d The data buffer
   * @param len The length of the data buffer
   * @param index The index of the current chunk of data
   * @param total The total length of the data
   */
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
/**
 * @brief Resets the configuration to the original configuration
 *
 * @param parameter Not used, needed for xTaskCreatePinnedToCore
 */
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
/**
 * @brief Checks if a buton is held down for more than 5s if it is, it calls
 * reset_config
 *
 */
void button_hold() {
  static time_t last_time = 0;
  if (last_time == 0) {
    last_time = millis();
    return;
  }
  if (millis() - last_time > 5000) {
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
/**
 * @brief Default Arduino setup function. If the device is not configured it
 * initiates the configuration site. If the configuration is already
 * done it displays the configuration on the OLED for 30s and initializes the
 * monitoring and controlling section on core 0 and the LoRaWan communication
 * module on core 1.
 *
 */
void setup() {
  // Initialize the serial port for 115200 baud
  Serial.begin(115200);
  // Load the data from the configuration file
  data = iot_data2::getInstance();
  // Turn on the built in LED at 25% duty cycle, the LED was too bright
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_PIN, 0);
  ledcWrite(0, 25);
  // Attach an interupt to the PROG button, which is used for resetting the
  // configuration file
  pinMode(0, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(0), button_hold, CHANGE);
  // Initialize the OLED display
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  display.init();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  // The app key can only be set after a succesful configuration, by checking
  // the value inside appKey we can determine if the device is configured
  if (data->get_appKey() == NULL) {
    // Assemble ssid from ssid and devEui(by default it's generated from the
    // ESP32's mac address)
    String ssid = data->get_ssid() + String("_") +
                  String(data->to_hex_str(data->get_devEui(), 8)->c_str());
    // Print information about the device on the display
    display.drawStringMaxWidth(0, 0, 128, "Device not configured");
    display.drawStringMaxWidth(0, 22, 128, String("Connect to: ") + ssid);
    // Start the DNS server and the http server
    DNSServer *dnsServer = new DNSServer();
    AsyncWebServer *server = new AsyncWebServer(80);
    // Turn on Wi-Fi in AP mode
    WiFi.softAP(ssid, data->get_password());
    // Add the captive portal handler to the server
    server->addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
    // Start the webserver
    server->begin();
    // Display the information on the OLED
    display.display();
    // Start the DNS server
    dnsServer->start(53, "*", WiFi.softAPIP());
    // Manage DNS requests on core 0
    xTaskCreatePinnedToCore(dnsloop, "dnsloop", 10 * configMINIMAL_STACK_SIZE,
                            NULL, 2, NULL, 0);
    // Wait for a client to connect
    while (WiFi.softAPgetStationNum() == 0) {
      delay(500);
    }
    // Display the next steps on the display
    display.clear();
    display.drawStringMaxWidth(0, 0, 128, "Connection established!");
    display.drawStringMaxWidth(0, 11, 128,
                               String("To configure device go to: http://") +
                                   WiFi.softAPIP().toString() + "/");
    display.display();
    // Wait for the configuration to be done
    while (!configured) {
      delay(500);
    }
    // After 5 minutes turn off the display and Wi-Fi and restart the ESP32
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
  xTaskCreatePinnedToCore(setup_and_loop_0, "setup_and_loop_0",
                          10 * configMINIMAL_STACK_SIZE, NULL, 2, NULL, 0);
  // Display the configuration on the OLED for 10 seconds
  display.drawStringMaxWidth(0, 0, 128, "Device configured");
  display.drawStringMaxWidth(
      0, 11, 128,
      String("DevEUI: ") + data->to_hex_str(data->get_devEui(), 8)->c_str());
  display.drawStringMaxWidth(
      0, 33, 128,
      String("JoinEUI: ") + data->to_hex_str(data->get_joinEui(), 8)->c_str());
  display.display();
  delay(10 * 1000);
  display.clear();
  display.displayOff();
  display.end();
  // Init LoraWAN
  Lora *lora = Lora::getInstance();
  lora->setup();
  // Wait for the hall effect sensor calibration to be done
  while (c->ready()) {
    sleep(1);
  }
  // Wait for the first report interval
  sleep(data->get_report_interval());
  // Send the first report
  lora->send_data(c->get_data());
}
// Core 1 loop (LoraWAN communication)
void loop() { os_runloop_once(); }

// Core 0 setup and loop of control and monitoring
void setup_and_loop_0(void *parameter) {
  m->init();
  while (!c->ready()) {
    m->run_once();
    delay(1000);
  }
  p->init();  // Enabling PV output
  for (;;) {
    c->x_recv_data();
    m->run_once();
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