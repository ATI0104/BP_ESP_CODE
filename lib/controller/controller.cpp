#include "controller.h"
#include "pv_controller.h"
#define a0 1
#define a1 2
#define a2 3
#define a3 0
#define mv_to_a 26.4
controller* controller::instance = nullptr;
controller* controller::get_instance() {
  if (!instance) instance = new controller();
  return instance;
}
void controller::get_data_from_adc(int16_t* buffer) {
  auto tmp = new int16_t[4];
  for (int i = 0; i < 4; i++) {
    tmp[i] = buffer[i];
  }
  if (calibration_steps) {
    Serial.println("Calibrating...");
    if (old_bypass >
        1) {  // The Mosfet was disengaded during the calibration we need to
              // burn some data to prevent the calibration from being off
      old_bypass--;
      delete tmp;
      return;
    }
    if (pv_controller::get_instance()->get_bypass_status() == 0) {
      Serial.println("Panel is not bypassed. Bypassing panel for calibration");
      old_bypass = 3;
      pv_controller::get_instance()->toggle_bypass();
      delete tmp;
      return;
    }
    calibration_steps--;
    number_of_measurements++;
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
  average(&data->pv_voltage, &number_of_measurements,
          (double)tmp[a2] * multiplier * pv_voltage_divider_ratio);
  // Current: (Cout - 3.3v/2- cal_offset) * bit_to_mV * mv_to_A * 1000 = mA
  average(&data->pv_current, &number_of_measurements,
          ((double)(tmp[a1] - (tmp[a0] / 2) - offset) * multiplier / mv_to_a) *
              1000.0);
  Serial.println(((double)(tmp[a1] - (tmp[a0] / 2)) * multiplier / mv_to_a),
                 16);
  average(&data->battery_voltage, &number_of_measurements,
          (double)tmp[a3] * multiplier);
  delete[] tmp;
}

send_data_t* controller::get_data() {
  if (d == nullptr) d = iot_data2::getInstance();
  data->report_interval = d->get_report_interval();
  data->bypassed = d->get_bypass_pv();

  auto tmp = data;
  data = new send_data_t{0};
  return tmp;
}

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