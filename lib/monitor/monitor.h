#ifndef __monitor_h__
#define __monitor_h__
#include <Arduino.h>
#pragma pack(push, 1)
struct send_data_t {
  uint16_t pv_voltage;
  uint16_t pv_current;
  uint16_t battery_voltage;
  uint16_t report_interval;
  uint8_t bypassed;
};
#pragma pack(pop)
class monitor{
    private:
};
#endif