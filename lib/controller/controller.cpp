#include "controller.h"

controller* controller::get_instance() {
  if (!instance) instance = new controller();
  return instance;
}
void controller::get_data_from_adc(int16_t* buffer) {
  if (multiplier == 0.0) multiplier = ADS.toVoltage(1);
  auto tmp = new int16_t[4];
  for (int i = 0; i < 4; i++) {
    tmp[i] = buffer[i];
  }
  if (first) {
    offset = tmp[1] - tmp[0];  // Setting the offset for the current sensor
    first = 0;
  }
  if (data->pv_voltage == 0.0) {
    data->pv_voltage = (tmp[2] * multiplier) * 1000 * pv_voltage_divider_ratio;
  } else {
    auto current_voltage =
        (tmp[2] * multiplier) * 1000 * pv_voltage_divider_ratio;
    data->pv_voltage = (data->pv_voltage + current_voltage) / 2;
  }
  if (data->pv_current == 0.0) {
    data->pv_current = (tmp[1] - (tmp[0] / 2)) * 1000 * multiplier;
  } else {
    data->pv_current = (data->pv_current + tmp[1] * multiplier) / 2;
  }
  if (data->battery_voltage == 0.0) {
    data->battery_voltage = tmp[3] * multiplier;
  } else {
    data->battery_voltage = (data->battery_voltage + tmp[3] * multiplier) / 2;
  }
  delete[] tmp;
}

send_data_t* controller::get_data() {
  if (d == nullptr) d = iot_data2::getInstance();
  if (d->get_bypass_pv()) {
    data->report_interval_bypassed =
        static_cast<uint64_t>(1) << 63 | d->get_report_interval();
  } else {
    data->report_interval_bypassed = d->get_report_interval();
  }
  auto tmp = data;
  data = new send_data_t{0};
  return tmp;
}

void controller::receive_data(recv_data_t* data) { this->recv = data; }
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
  recv = nullptr;
}