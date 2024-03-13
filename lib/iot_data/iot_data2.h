#ifndef IOT_DATA2_H
#define IOT_DATA2_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#pragma pack(push, 1)  // Disables struct padding
struct send_data_t {
  double pv_voltage;                  // 8B
  double pv_current;                  // 8B
  double battery_voltage;             // 8B
  uint64_t report_interval_bypassed;  // 8B - MSB- bypass, <[MSB-1],LSB> report
                                      // interval
};                                    // 32B total
struct recv_data_t {
  uint32_t report_interval;  // 4B
  uint8_t bypass;            // 1B
  uint8_t reset;             // 1B
  uint16_t reserved;         // 2B
};
#pragma pack(pop)
class iot_data2 {
 private:
  static iot_data2* instance;
  uint8_t* devEui;
  uint8_t* joinEui;
  uint8_t* appKey;
  char* ssid;
  char* password;
  uint8_t sda;
  uint8_t scl;
  uint8_t mosfet;
  uint8_t adc_rdy;

  uint32_t report_interval;
  uint8_t bypass_pv;
  void load_data();
  iot_data2() { this->load_data(); };
  void save_data();

 public:
  // Deleted copy constructor and assignment operator
  iot_data2(const iot_data2&) = delete;
  iot_data2& operator=(const iot_data2&) = delete;

  // Static method to access class instance
  static iot_data2* getInstance() {
    if (instance == nullptr) {
      instance = new iot_data2();
    }
    return instance;
  }
  // getters and setters
  uint8_t* get_devEui();
  uint8_t* get_joinEui();
  uint8_t* get_appKey();
  char* get_ssid();
  char* get_password();
  uint8_t get_sda_pin();
  uint8_t get_scl_pin();
  uint8_t get_mosfet_pin();
  uint16_t get_report_interval();
  uint8_t get_bypass_pv();
  void set_report_interval(uint32_t report_interval);
  void set_bypass_pv(uint8_t bypass_pv);
  void set_devEui(const char* devEui);
  void set_joinEui(const char* joinEui);
  void set_appkey(const char* appKey);
  uint8_t* str_to_byte_array(const char* str);
  String* to_hex_str(uint8_t* data, size_t len);
};

#endif