#include <Arduino.h>
#include "monitor.h"
#include "controller.h"
#include "ADS1X15.h"
ADS1115 ADS(0x48);
int16_t monitor::buffer[4] = {0};
uint8_t monitor::buffer_index = 0;
controller* monitor::c = nullptr;
monitor* monitor::instance = nullptr;
monitor::monitor() {
  iot_data2* d = iot_data2::getInstance();
  this->sda = d->get_sda_pin();
  this->scl = d->get_scl_pin();
  this->first = 1;
  this->IOffset = 1;
}

void monitor::init() {
  Wire.begin(this->sda, this->scl);
  ADS.begin();
  ADS.setGain(0);
  ADS.setDataRate(7);
  ADS.setComparatorThresholdHigh(0x8000);
  ADS.setComparatorThresholdLow(0x0000);
  ADS.setComparatorQueConvert(0);
  pinMode(13, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(13), read_adc, RISING);
  ADS.setMode(0);
  c = controller::get_instance();
  ADS.readADC(this->buffer_index);
}

void monitor::read_adc() {
  //  save the value
  buffer[buffer_index] = ADS.getValue();
  //  request next channel
  if (++buffer_index >= 4) {
    buffer_index = 0;
    c->get_data_from_adc(buffer);
  }
  ADS.readADC(buffer_index);
}