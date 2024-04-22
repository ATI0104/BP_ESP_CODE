#include "controller.h"
#include "pv_controller.h"
// Because of the way my specific ADC board is wired, the channels are in the
// wrong order, by using macros I can easily change the order of the channels
#define a0 1
#define a1 2
#define a2 3
#define a3 0
// Constant taken from the ACS773 datasheet used for converting the ADC readings
// to current
#define mv_to_a 26.4
controller* controller::instance = nullptr;
/**
 * @brief Returns a pointer to the singleton instance of the controller class.
 *
 * @return controller*
 */
controller* controller::get_instance() {
  if (!instance) instance = new controller();
  return instance;
}
/**
 * @brief Function called by monitor when it has received data from the ADC.
 *
 * @param buffer The data from the ADC
 */
void controller::get_data_from_adc(int16_t* buffer) {
  auto tmp = new int16_t[4];
  // Copy the buffer to prevent it from being modified
  for (int i = 0; i < 4; i++) {
    tmp[i] = buffer[i];
  }
  // Calibrate the current sensor
  if (calibration_steps) {
    Serial.println("Calibrating...");
    if (old_bypass >
        1) {  // The Mosfet was disengaded during the calibration we need to
              // burn some data to prevent the calibration from being off
      old_bypass--;
      delete[] tmp;
      return;
    }
    // Checks if the panel is bypassed, with a working panel the calibration is
    // usesless
    if (pv_controller::get_instance()->get_bypass_status() == 0) {
      Serial.println("Panel is not bypassed. Bypassing panel for calibration");
      old_bypass = 3;
      pv_controller::get_instance()->toggle_bypass();
      delete[] tmp;
      return;
    }
    calibration_steps--;
    number_of_measurements++;
    // Offset: Cout - 3.3v/2 = 0A
    average(&offset, &number_of_measurements,
            (double)(tmp[a1] - (tmp[a0] / 2)));
    if (calibration_steps == 0) {
      number_of_measurements = 0;
      Serial.println("Calibration done");
      Serial.println("Offset: " + String(offset));
      if (old_bypass == 1) {
        Serial.println("Disabling bypass");
        pv_controller::get_instance()->toggle_bypass();
      }
    }
    return;
  }
  ++number_of_measurements;
  // Voltage: ADC * sensitivity * divider ratio = mV
  average(&data->pv_voltage, &number_of_measurements,
          (double)tmp[a3] * multiplier * pv_voltage_divider_ratio);
  // Current: (Cout - 3.3v/2- cal_offset) * bit_to_mV * mv_to_A * 1000 = mA
  average(&data->pv_current, &number_of_measurements,
          ((double)(tmp[a1] - (tmp[a0] / 2) - offset) * multiplier / mv_to_a) *
              1000.0);
  // Battery voltage: ADC * sensitivity * divider ratio = mV
  average(&data->battery_voltage, &number_of_measurements,
          (double)tmp[a2] * multiplier * bat_voltage_divider_ratio);
  Serial.println("PV Voltage: " + String(data->pv_voltage));
  delete[] tmp;
}
/**
 * @brief Function called by the LoRa module to obtain the data to be sent.
 * During the sending the controller is already accumulating new data for the
 * next send
 *
 * @return send_data_t*  The data to be sent
 */
send_data_t* controller::get_data() {
  if (d == nullptr) d = iot_data2::getInstance();
  data->report_interval = d->get_report_interval();
  data->bypassed = d->get_bypass_pv();
  auto tmp = data;
  Serial.println("Number of measurements: " + String(number_of_measurements));
  data = new send_data_t{0};
  number_of_measurements = 0;
  return tmp;
}
/**
 * @brief Function called by the LoRa module when data is received from the
 * server.
 *
 * @param data  The received data
 */
void controller::receive_data(recv_data_t* data) { this->recv = data; }
/**
 * @brief Calculates the average of incoming numbers. Source:
 * https://math.stackexchange.com/questions/106313/regular-average-calculated-accumulatively
 *
 * @param avg
 * @param n
 * @param x
 */
void controller::average(double* avg, size_t* count, double x) {
  if (!avg || !count) {
    Serial.println("avg or count is null");
    return;
  }
  if (*count == 0) {
    Serial.println("division by zero!!!");
    return;
  }
  if (*count == 1) {
    *avg = 0.0;
  }
  *avg += (x - *avg) / *count;
  return;
}
/**
 * @brief Because receive_data is called from an interrupt context, we need to
 * process the data elsewhere. This function is called from the loop running on
 * core 0 and processes the received data.
 *
 */
void controller::x_recv_data() {
  if (recv == nullptr) return;
  auto pv = pv_controller::get_instance();
  if (recv->bypass) {
    d->set_bypass_pv(1);
    if (!pv->get_bypass_status()) {
      pv->toggle_bypass();
    }
  } else {
    d->set_bypass_pv(0);
    if (pv->get_bypass_status()) {
      pv->toggle_bypass();
    }
  }
  if (recv->report_interval) {
    d->set_report_interval(recv->report_interval);
  }
  if (recv->reset) {
    ESP.restart();
  }
  delete recv;
  recv = nullptr;
}