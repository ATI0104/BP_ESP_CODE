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
  struct send_data_t *data;
  long double multiplier;
  uint16_t offset;  // Calibration offset for the current sensor
  static const uint16_t pv_voltage_divider_ratio =
      10;  // TODO: Change this to the actual value
  controller();
  recv_data_t *recv;
  void x_recv_data();

 public:
  controller(const controller &) = delete;
  controller &operator=(const controller &) = delete;
  static controller *get_instance();
  void get_data_from_adc(int16_t *buffer);
  send_data_t *get_data();
  void receive_data(recv_data_t *data);
};
#endif