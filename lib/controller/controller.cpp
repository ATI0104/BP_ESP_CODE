#include "controller.h"
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
  if (multiplier == 0.0) multiplier = 0.12500381;
  auto tmp = new int16_t[4];
  for (int i = 0; i < 4; i++) {
    tmp[i] = buffer[i];
  }
  if (calibration_steps) {
    calibration_steps--;
    number_of_measurements++;
    average(&offset, &number_of_measurements,
            (double)(tmp[a1] - (tmp[a0] / 2)) * multiplier * mv_to_a);
    if (calibration_steps == 0) {
      number_of_measurements = 0;
    }
    return;
  }
  ++number_of_measurements;
  average(&data->pv_voltage, &number_of_measurements,
          (double)tmp[a2] * multiplier * pv_voltage_divider_ratio);
  average(&data->pv_current, &number_of_measurements,
          ((double)(tmp[a1] - (tmp[a0] / 2)) * multiplier * mv_to_a) - offset);
  average(&data->battery_voltage, &number_of_measurements,
          (double)tmp[a3] * multiplier);
  // data->pv_voltage += (tmp[a2] * multiplier) *
  // (double)pv_voltage_divider_ratio; data->pv_current += (tmp[a1] - (tmp[a0] /
  // 2)) * multiplier * mv_to_a; data->battery_voltage += tmp[a3] * multiplier;

  delete[] tmp;
}

send_data_t* controller::get_data() {
  if (d == nullptr) d = iot_data2::getInstance();
  data->report_interval = d->get_report_interval();
  data->bypassed = d->get_bypass_pv();

  auto tmp = data;
  data = new send_data_t{0};
  Serial.println(data->bypassed + String(data->pv_current));
  number_of_measurements = 0;
  data->battery_voltage = 0.0;
  data->pv_current = 0.0;
  data->pv_voltage = 0.0;
  data->report_interval = 0;
  data->bypassed = 0;
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
  if (recv->bypass) {
    d->set_bypass_pv(1);
  } else {
    d->set_bypass_pv(0);
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