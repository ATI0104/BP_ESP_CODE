#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <Arduino.h>
#include "iot_data2.h"
#include "ADS1X15.h"
extern ADS1115 ADS;
class controller {
 private:
  static controller *instance;
  iot_data2 *d;
  uint8_t first;
  send_data_t *data;
  float multiplier;
  uint16_t offset;  // Calibration offset for the current sensor
  static const uint16_t pv_voltage_divider_ratio =
      10;  // TODO: Change this to the actual value
  recv_data_t *recv;
  int32_t number_of_measurements;
  controller() {
    data = new send_data_t;
    data->battery_voltage = 0.0;
    data->pv_current = 0.0;
    data->pv_voltage = 0.0;
    data->report_interval = 0;
    data->bypassed = 0;
    recv = nullptr;
    number_of_measurements = 0;
    d = iot_data2::getInstance();
  }

 public:
  void x_recv_data();
  controller(const controller &) = delete;
  controller &operator=(const controller &) = delete;
  static controller *get_instance();
  void get_data_from_adc(int16_t *buffer);
  send_data_t *get_data();
  void receive_data(recv_data_t *data);
  uint8_t ready() { return this->first; }
};
#endif