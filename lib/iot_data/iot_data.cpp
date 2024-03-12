#include "iot_data.h"

#include <ArduinoJson.h>
#include <SPIFFS.h>
uint8_t* iot_data::devEui = nullptr;
uint8_t* iot_data::joinEui = nullptr;
uint8_t* iot_data::appKey = nullptr;
char* iot_data::ssid = nullptr;
char* iot_data::password = nullptr;
uint8_t iot_data::sda = 0xff;
uint8_t iot_data::scl = 0xff;
uint8_t iot_data::mosfet = 0xff;
uint8_t iot_data::configured = 0;
uint32_t iot_data::report_interval = 0;
uint8_t iot_data::bypass_pv = 0;

void iot_data::save_data() {
  SPIFFS.begin();
  File file = SPIFFS.open("/data.txt", "w");

  JsonDocument doc;
  doc["devEUI"] = devEui;
  doc["joinEUI"] = joinEui;
  doc["appkey"] = appKey;
  doc["ssid"] = ssid;
  doc["pass"] = password;
  doc["sda"] = sda;
  doc["scl"] = scl;
  doc["mosfet"] = mosfet;
  doc["configured"] = configured;
  doc["report_interval"] = report_interval;
  doc["bypass_pv"] = bypass_pv;
  doc.shrinkToFit();
  serializeJson(doc, file);
  file.close();
  SPIFFS.end();
}

void iot_data::load_data() {
  SPIFFS.begin();
  File file = SPIFFS.open("/data.txt", "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, file);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  devEui = str_to_byte_array(doc["devEui"]);
  joinEui = str_to_byte_array(doc["joinEui"]);
  appKey = str_to_byte_array(doc["appKey"]);
  this->ssid = new char[strlen(doc["ssid"]) + 1];
  strcpy(ssid, doc["ssid"]);
  password = new char[strlen(doc["pass"]) + 1];
  strcpy(password, doc["pass"]);
  sda = doc["sda"];
  scl = doc["scl"];
  mosfet = doc["mosfet"];
  configured = doc["configured"];
  report_interval = doc["report_interval"];
  bypass_pv = doc["bypass_pv"];
  file.close();
  SPIFFS.end();
  if (devEui == nullptr) {
    devEui = new uint8_t[8];
    uint8_t* p = devEui;
    uint8_t dmac[8] = {0};
    ESP_ERROR_CHECK(esp_efuse_mac_get_default(dmac));
    *p++ = dmac[0];
    *p++ = dmac[1];
    *p++ = dmac[2];
    *p++ = 0xff;
    *p++ = 0xfe;
    *p++ = dmac[3];
    *p++ = dmac[4];
    *p++ = dmac[5];
    if (dmac[6] != 0) {  // Device has a EUI-64
      p = devEui;
      for (size_t i = 0; i < 8; i++) {
        *p++ = dmac[i];
      }
    }
  }
}

uint8_t* iot_data::str_to_byte_array(const char* str) {
  size_t strLen = strlen(str);
  if (strLen % 2 != 0) return nullptr;
  if (strLen == 0) return nullptr;
  size_t numBytes = strLen / 2;
  uint8_t* byteArray = new uint8_t[numBytes];
  for (size_t i = 0; i < numBytes; ++i) {
    sscanf(&str[i * 2], "%2hhx", &byteArray[i]);
  }
  return byteArray;
}

iot_data::iot_data() {
  if (devEui == nullptr) load_data();
}

uint8_t* iot_data::get_devEui() { return devEui; }

uint8_t* iot_data::get_joinEui() { return joinEui; }

uint8_t* iot_data::get_appkey() { return appKey; }

char* iot_data::get_ssid() { return ssid; }

char* iot_data::get_password() { return password; }

uint8_t iot_data::get_configured() { return configured; }

uint8_t iot_data::get_sda_pin() { return sda; }

uint8_t iot_data::get_scl_pin() { return scl; }

uint8_t iot_data::get_mosfet_pin() { return mosfet; }

void iot_data::set_devEui(char* de) {
  if (devEui == nullptr) return;
  if (devEui != nullptr) delete[] devEui;
  devEui = str_to_byte_array(de);
}

void iot_data::set_joinEui(char* je) {
  if (joinEui == nullptr) return;
  if (joinEui != nullptr) delete[] joinEui;
  joinEui = str_to_byte_array(je);
}

void iot_data::set_appkey(char* ak) {
  if (appKey == nullptr) return;
  if (appKey != nullptr) delete[] appKey;
  appKey = str_to_byte_array(ak);
}

uint16_t iot_data::get_report_interval() { return report_interval; }

void iot_data::set_configured(uint8_t configured) { configured = configured; }

uint8_t iot_data::get_bypass_pv() { return bypass_pv > 0 ? 1 : 0; }

void iot_data::set_report_interval(uint32_t report_interval) {
  if (report_interval < 60) report_interval = 60;
  if (report_interval > 86400) report_interval = 86400;
  report_interval = report_interval;
}

void iot_data::set_bypass_pv(uint8_t bp) { bypass_pv = bp > 0 ? 1 : 0; }
