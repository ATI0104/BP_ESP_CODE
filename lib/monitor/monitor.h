#ifndef __monitor_h__
#define __monitor_h__
#include <Arduino.h>
#include "iot_data2.h"
class monitor {
 private:
  static uint8_t sda;
  static uint8_t scl;
  static int16_t buffer[4];
  static uint8_t buffer_index;
  uint8_t first;
  uint8_t IOffset;
 public:
  monitor();
  void init();
  static void read_adc();
  static void get_data();
  static void send_to_collector(int16_t *buffer);
};
class collector {
 private:
  static struct send_data_t data;
  static long double multiplier;
  static const uint16_t pv_voltage_divider_ratio =
      10;  // TODO: Change this to the actual value or get from config

 public:
  collector();
  void init();
  static void get_data_from_adc(int16_t *buffer);
  static send_data_t *get_data();
};
#endif