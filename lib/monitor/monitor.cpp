#include <Arduino.h>
#include "monitor.h"
#include "ADS1X15.h"
ADS1115 ADS(0x48);
monitor::monitor() {
  iot_data2* d = iot_data2::getInstance();
  this->sda = d->get_sda_pin();
  this->scl = d->get_scl_pin();
  this->buffer_index = 0;
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
  attachInterrupt(digitalPinToInterrupt(13), this->read_adc, RISING);
  ADS.setMode(0);
  ADS.readADC(this->buffer_index);
  while(first == 1){ // Wait for the first reading used for calibration
    delay(100); 
  }
}

void monitor::read_adc() {
  //  save the value
  buffer[buffer_index] = ADS.getValue();
  //  request next channel
  if (++buffer_index >= 4) {
    buffer_index = 0;
    send_to_collector(buffer);
  }
  ADS.readADC(buffer_index);
}

void monitor::send_to_collector(int16_t* buffer) {
  collector c = collector();
  c.get_data_from_adc(buffer);
}

collector::collector() {
  if (this->multiplier == 0.0) this->multiplier = ADS.toVoltage(1);
}

void collector::get_data_from_adc(int16_t* buffer) {
  auto tmp = new int16_t[4];
  for (int i = 0; i < 4; i++) {
    tmp[i] = buffer[i];
  }
  if (data.pv_voltage == 0) {
    data.pv_voltage = (tmp[2] * multiplier) * 1000 * pv_voltage_divider_ratio;
  } else {
    data.pv_voltage = (data.pv_voltage + tmp[2] * multiplier) / 2;
  }
  if (data.pv_current == 0) {
    data.pv_current = (tmp[1] - (tmp[0] / 2)) * 1000 * multiplier;
  } else {
    data.pv_current = (data.pv_current + tmp[1] * multiplier) / 2;
  }
  if (data.battery_voltage == 0) {
    data.battery_voltage = tmp[3] * multiplier;
  } else {
    data.battery_voltage = (data.battery_voltage + tmp[3] * multiplier) / 2;
  }
}

send_data_t* collector::get_data() { return &data; }
