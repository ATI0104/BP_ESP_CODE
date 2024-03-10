#include "data.h"

#include <ArduinoJson.h>
#include <SPIFFS.h>
void Data::save_data() {
  SPIFFS.begin();
  File file = SPIFFS.open("/data.txt", "w");

  JsonDocument doc;
  doc["devEUI"] = this->devEui;
  doc["joinEUI"] = this->joinEui;
  doc["appkey"] = this->appKey;
  doc["ssid"] = this->ssid;
  doc["pass"] = this->password;
  doc["sda"] = this->sda;
  doc["scl"] = this->scl;
  doc["mosfet"] = this->mosfet;
  doc["configured"] = this->configured;
  doc.shrinkToFit();
  serializeJson(doc, file);
  file.close();
  SPIFFS.end();
}

void Data::load_data() {
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
  this->devEui = str_to_byte_array(doc["devEui"]);
  this->joinEui = str_to_byte_array(doc["joinEui"]);
  this->appKey = str_to_byte_array(doc["appKey"]);
  strcpy(this->ssid, doc["ssid"]);
  strcpy(this->password, doc["password"]);
  this->sda = doc["sda"];
  this->scl = doc["scl"];
  this->mosfet = doc["mosfet"];
  this->configured = doc["configured"];
  file.close();
  SPIFFS.end();
  if (this->devEui == nullptr) {
    this->devEui = new uint8_t[8];
    uint8_t* p = this->devEui;
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
      p = this->devEui;
      for (size_t i = 0; i < 8; i++) {
        *p++ = dmac[i];
      }
    }
  }
}

uint8_t* Data::str_to_byte_array(const char* str) {
  size_t strLen = strlen(str);
  size_t numBytes = strLen / 2;
  uint8_t* byteArray = new uint8_t[numBytes];
  for (size_t i = 0; i < numBytes; ++i) {
    sscanf(&str[i * 2], "%2hhx", &byteArray[i]);
  }
  return byteArray;
}

Data::Data() {
  if (this->devEui == nullptr) this->load_data();
}

uint8_t* Data::get_devEui() { return this->devEui; }

uint8_t* Data::get_joinEui() { return this->joinEui; }

uint8_t* Data::get_appkey() { return this->appKey; }

char* Data::get_ssid() { return this->ssid; }

char* Data::get_password() { return this->ssid; }

uint8_t Data::get_configured() { return this->configured; }

uint8_t Data::get_sda_pin() { return this->sda; }

uint8_t Data::get_scl_pin() { return this->scl; }

uint8_t Data::get_mosfet_pin() { return this->mosfet; }

void Data::set_devEui(char* devEui) {
  if (devEui == nullptr) return;
  if (this->devEui != nullptr) delete[] this->devEui;
  this->devEui = str_to_byte_array(devEui);
}

void Data::set_joinEui(char* joinEui) {
  if (joinEui == nullptr) return;
  if (this->joinEui != nullptr) delete[] this->joinEui;
  this->joinEui = str_to_byte_array(joinEui);
}

void Data::set_appkey(char* appKey) {
  if (appKey == nullptr) return;
  if (this->appKey != nullptr) delete[] this->appKey;
  this->appKey = str_to_byte_array(appKey);
}

void Data::set_configured(uint8_t configured) {}
