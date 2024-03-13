#ifndef __monitor_h__
#define __monitor_h__
#include <Arduino.h>
#include "iot_data2.h"
#include "controller.h"
class monitor {
 private:
  static monitor *instance;
  static controller *c;
  uint8_t sda;
  uint8_t scl;
  static int16_t buffer[4];
  static uint8_t buffer_index;
  uint8_t IOffset;
  uint8_t first;
  monitor();

 public:
  monitor(const monitor &) = delete;
  monitor &operator=(const monitor &) = delete;
  static monitor *getInstance() {
    if (instance == nullptr) {
      instance = new monitor();
    }
    return instance;
  }
  void init();
  static void read_adc();
};

#endif