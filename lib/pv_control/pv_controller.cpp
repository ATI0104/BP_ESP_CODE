#include "pv_controller.h"

#include <Arduino.h>
#include <iot_data2.h>
pv_controller *pv_controller::instance = nullptr;
void pv_controller::init() {
  iot_data2 *data = iot_data2::getInstance();
  this->bypass_pin = data->get_mosfet_pin();
  this->bypassed = data->get_bypass_pv();
  pinMode(this->bypass_pin, OUTPUT);
  digitalWrite(this->bypass_pin, this->bypassed);
}

uint8_t pv_controller::toggle_bypass() {
  if (this->bypassed) {
    this->bypassed = 0;
  } else {
    this->bypassed = 1;
  }
  digitalWrite(this->bypass_pin, this->bypassed);
  return this->bypassed;
}

uint8_t pv_controller::get_bypass_status() { return this->bypassed; }
