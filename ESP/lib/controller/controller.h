#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>
#include "iot_data2.h"
#include "ADS1X15.h"

extern ADS1115 ADS;

/**
 * @brief The controller class handles the control logic and data management for
 * the system.
 */
class controller {
 private:
  int old_bypass;  // The previous bypass state before calibration took place
  static controller
      *instance;              // The singleton instance of the controller class
  iot_data2 *d;               // Pointer to the iot_data2 instance
  uint8_t calibration_steps;  // The number of calibration steps remaining
  send_data_t *data;          // Pointer to the send_data_t structure
  double multiplier;  // The multiplier for converting ADC readings to physical
                      // values
  double offset;      // The calibration offset for the current sensor
  const double pv_voltage_divider_ratio =
      16.518275539;  // The voltage divider ratio for PV voltage
  const double bat_voltage_divider_ratio = 1.3377483443708609;
  recv_data_t *recv;              // Pointer to the received data structure
  size_t number_of_measurements;  // The number of measurements taken

  /*/**
   @brief Private constructor for the controller class.
   */
  controller() {
    data = new send_data_t;
    data->battery_voltage = 0.0;
    data->pv_current = 0.0;
    data->pv_voltage = 0.0;
    data->report_interval = 0;
    data->bypassed = 0;
    offset = 0.0;
    recv = nullptr;
    number_of_measurements = 0;
    calibration_steps = 5;
    multiplier = 0.12500381;
    old_bypass = 0;
    d = iot_data2::getInstance();
  }
  void average(double *avg, size_t *count, double x);

 public:
  void x_recv_data();
  controller(const controller &) = delete;
  static controller *get_instance();
  void get_data_from_adc(int16_t *buffer);
  send_data_t *get_data();
  void receive_data(recv_data_t *data);
  /**
   * @brief Returns 0 when the calibration is complete.
   * 
   * @return uint8_t 
   */
  uint8_t ready() { return this->calibration_steps > 0 ? 1 : 0; }
};
#endif