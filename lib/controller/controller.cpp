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
  if (multiplier == 0.0) multiplier = 0.12500381;  // Converts it to mV
  Serial.println(String(buffer[a0]) + " " + String(buffer[a1]) + " " +
                 String(buffer[a2]) + " " + String(buffer[a3]));
  Serial.println(String(ADS.toVoltage(buffer[a0])) + " " +
                 String(ADS.toVoltage(buffer[a1])) + " " +
                 String(ADS.toVoltage(buffer[a2])) + " " +
                 String(ADS.toVoltage(buffer[a3])));
  Serial.println(String((float)((float)buffer[a0] * multiplier)) + " " +
                 String((float)((float)buffer[a1] * multiplier)) + " " +
                 String((float)((float)buffer[a2] * multiplier)) + " " +
                 String((float)((float)buffer[a3] * multiplier)));
  auto tmp = new int16_t[4];
  for (int i = 0; i < 4; i++) {
    tmp[i] = buffer[i];
  }
  if (data->pv_voltage == 0.0) {
    data->pv_voltage =
        (tmp[a2] * multiplier) * (double)pv_voltage_divider_ratio;
  } else {
    auto current_voltage =
        (tmp[a2] * multiplier) * (double)pv_voltage_divider_ratio;
    data->pv_voltage = (data->pv_voltage + current_voltage) / 2.0;
  }
  if (data->pv_current == 0.0) {
    data->pv_current = (tmp[a1] - (tmp[a0] / 2)) * multiplier * mv_to_a;
  } else {
    data->pv_current = (data->pv_current +
                        ((tmp[a1] - (tmp[a0] / 2)) * multiplier * mv_to_a)) /
                       2.0;
  }
  if (data->battery_voltage == 0.0) {
    data->battery_voltage = tmp[a3] * multiplier;
  } else {
    data->battery_voltage = (data->battery_voltage + tmp[a3] * multiplier) / 2.0;
  }
  if(first){
    first=0;
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
  data->battery_voltage = 0.0;
  data->pv_current = 0.0;
  data->pv_voltage = 0.0;
  data->report_interval_bypassed = 0;
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