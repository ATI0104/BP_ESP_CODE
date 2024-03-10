#ifndef __data__
#define __data__
#include <Arduino.h>
class Data {
 private:
  static uint8_t* devEui;
  static uint8_t* joinEui;
  static uint8_t* appKey;
  static char ssid[32];
  static char password[64];
  static uint8_t sda;
  static uint8_t scl;
  static uint8_t mosfet;
  static uint8_t configured;
  static uint16_t report_interval;
  static uint8_t bypass_pv;
  void load_data();
  uint8_t* str_to_byte_array(const char* str);

 public:
  // constructor
  Data();
  // save object to file
  void save_data();
  // getters and setters
  uint8_t* get_devEui();
  uint8_t* get_joinEui();
  uint8_t* get_appkey();
  char* get_ssid();
  char* get_password();
  uint8_t get_configured();
  uint8_t get_sda_pin();
  uint8_t get_scl_pin();
  uint8_t get_mosfet_pin();
  uint8_t get_configured();
  uint16_t get_report_interval();
  uint8_t get_bypass_pv();
  void set_report_interval(uint16_t report_interval);
  void set_bypass_pv(uint8_t bypass_pv);
  void set_devEui(char* devEui);
  void set_joinEui(char* joinEui);
  void set_appkey(char* appKey);
  void set_configured(uint8_t configured);
};

#endif