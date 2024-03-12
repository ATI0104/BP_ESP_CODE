#include "control.h"

#include <Arduino.h>
#include <iot_data2.h>
// #include <preferences.h>
pv_controller::pv_controller() {
  iot_data2 *data = iot_data2::getInstance();
  this->bypass_pin = data->get_mosfet_pin();
}

void pv_controller::init() {
  pinMode(this->bypass_pin, OUTPUT);
  digitalWrite(this->bypass_pin, LOW);
  this->bypassed = 0;
}

uint8_t pv_controller::toggle_bypass() {
  if (this->bypassed) {
    digitalWrite(this->bypass_pin, LOW);
    this->bypassed = 0;
  } else {
    digitalWrite(this->bypass_pin, HIGH);
    this->bypassed = 1;
  }
  return this->bypassed;
}

uint8_t pv_controller::get_bypass_status() { return this->bypassed; }
