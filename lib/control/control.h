#ifndef __control_h
#define __control_h
#include <Arduino.h>
class pv_controller {
 private:
  static pv_controller *instance;
  uint8_t bypassed;
  uint8_t bypass_pin;

 public:
  pv_controller();
  void init();
  uint8_t toggle_bypass();
  uint8_t get_bypass_status();
};
#endif