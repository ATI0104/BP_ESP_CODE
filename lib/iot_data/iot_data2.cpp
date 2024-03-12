#include <iot_data2.h>
iot_data2* iot_data2::instance = nullptr;
void iot_data2::load_data() {
  SPIFFS.begin();
  File file = SPIFFS.open("/config.json", "r");
  if (file < 0) {
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
  this->devEui = str_to_byte_array(doc["devEUI"]);
  this->joinEui = str_to_byte_array(doc["joinEUI"]);
  this->appKey = str_to_byte_array(doc["appkey"]);
  this->ssid = new char[strlen(doc["ssid"]) + 1];
  strcpy(this->ssid, doc["ssid"]);
  this->password = new char[strlen(doc["pass"]) + 1];
  strcpy(this->password, doc["pass"]);
  this->sda = doc["sda"];
  this->scl = doc["scl"];
  this->mosfet = doc["mosfet"];
  this->adc_rdy = doc["adc_rdy"];
  this->report_interval = doc["report_interval"];
  this->bypass_pv = doc["bypass_pv"];
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

void iot_data2::save_data() {
  SPIFFS.begin();
  File file = SPIFFS.open("/config.json", "w");

  JsonDocument doc;
  Serial.println("Saving data:");
  Serial.println(to_hex_str(this->devEui, 8)->c_str());

  doc["devEUI"] = to_hex_str(this->devEui, 8)->c_str();
  doc["joinEUI"] = to_hex_str(this->joinEui, 8)->c_str();
  doc["appkey"] = to_hex_str(this->appKey, 16)->c_str();
  doc["ssid"] = this->ssid;
  doc["pass"] = this->password;
  doc["sda"] = this->sda;
  doc["scl"] = this->scl;
  doc["mosfet"] = this->mosfet;
  doc["report_interval"] = this->report_interval;
  doc["bypass_pv"] = this->bypass_pv;
  doc["adc_rdy"] = this->adc_rdy;
  doc.shrinkToFit();
  serializeJson(doc, file);
  file.close();
  SPIFFS.end();
}

uint8_t* iot_data2::get_devEui() { return this->devEui; }

uint8_t* iot_data2::get_joinEui() { return this->joinEui; }

uint8_t* iot_data2::get_appKey() { return this->appKey; }

char* iot_data2::get_ssid() { return this->ssid; }

char* iot_data2::get_password() { return this->password; }

uint8_t iot_data2::get_sda_pin() { return this->sda; }

uint8_t iot_data2::get_scl_pin() { return this->scl; }

uint8_t iot_data2::get_mosfet_pin() { return this->mosfet; }

uint16_t iot_data2::get_report_interval() { return this->report_interval; }

uint8_t iot_data2::get_bypass_pv() { return this->bypass_pv; }

void iot_data2::set_report_interval(uint32_t report_interval) {
  if (report_interval > 0) {
    this->report_interval = report_interval;
  }
  if (this->report_interval > 86400) {
    this->report_interval = 86400;
  }
  this->save_data();
}

void iot_data2::set_bypass_pv(uint8_t bypass_pv) {
  if (bypass_pv > 0) {
    this->bypass_pv = 1;
  } else {
    this->bypass_pv = 0;
  }
  this->save_data();
}

void iot_data2::set_devEui(const char* devEui) {
  if (devEui == nullptr) return;
  if (this->devEui != nullptr) delete[] this->devEui;
  this->devEui = str_to_byte_array(devEui);
  this->save_data();
}

void iot_data2::set_joinEui(const char* joinEui) {
  if (joinEui == nullptr) return;
  if (this->joinEui != nullptr) delete[] this->joinEui;
  this->joinEui = str_to_byte_array(joinEui);
  this->save_data();
}

void iot_data2::set_appkey(const char* appKey) {
  if (appKey == nullptr) return;
  if (this->appKey != nullptr) delete[] this->appKey;
  this->appKey = str_to_byte_array(appKey);
  this->save_data();
}

uint8_t* iot_data2::str_to_byte_array(const char* str) {
  if (str == nullptr) return nullptr;
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
String* iot_data2::to_hex_str(uint8_t* data, size_t len) {
  String* str = new String();
  if (data == nullptr) return str;
  for (size_t i = 0; i < len; i++) {
    str->concat(String(data[i] < 16 ? "0" : ""));
    str->concat(String(data[i], HEX));
  }
  return str;
}