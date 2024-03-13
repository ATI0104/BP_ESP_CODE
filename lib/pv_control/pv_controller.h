#ifndef __control_h
#define __control_h
#include <Arduino.h>
class pv_controller {
 private:
  static pv_controller *instance;
  uint8_t bypassed;
  uint8_t bypass_pin;
  pv_controller();
 public:
  pv_controller(const pv_controller &) = delete;
  pv_controller &operator=(const pv_controller &) = delete;
  void init();
  uint8_t toggle_bypass();
  uint8_t get_bypass_status();
  static pv_controller *get_instance() {
    if (instance == nullptr) {
      instance = new pv_controller();
    }
    return instance;
  }
};
#endif