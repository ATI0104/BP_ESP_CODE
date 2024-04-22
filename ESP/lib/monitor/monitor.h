#ifndef __monitor_h__
#define __monitor_h__
#include <Arduino.h>
#include "iot_data2.h"
#include "controller.h"
/**
 * @class monitor
 * @brief Represents a monitor object that handles reading the ADC values.
 */
class monitor {
 private:
  static monitor *instance;     // The singleton instance of the monitor class.
  static controller *c;         // The controller object used by the monitor.
  uint8_t sda;                  // The SDA pin number.
  uint8_t scl;                  // The SCL pin number.
  static int16_t buffer[4];     // The buffer to store ADC values.
  static uint8_t buffer_index;  // The index of currently read channel.
  uint8_t IOffset;              // The IOffset value.
  uint8_t first;                // The first value.
  static uint8_t rdy;           // The rdy value.

  /*/**
   @brief Private constructor to prevent instantiation of the monitor class.
   */
  monitor();

 public:
  monitor(const monitor &) = delete;
  monitor &operator=(const monitor &) = delete;

  /**
   * @brief Gets the singleton instance of the monitor class.
   * @return The monitor instance.
   */
  static monitor *getInstance() {
    if (instance == nullptr) {
      instance = new monitor();
    }
    return instance;
  }

  /*/**
   @brief Sets up the ADC for continous measurements and to notify when a
   measurement is readt by using the ALR/RDY pin.
   */
  void init();

  /**
   * @brief Used as an interrupt to notify when a measurement is ready.
   */
  static void read_adc();

  /**
   * @brief Reads the ADC measurements when all 4 measurements are done it sends
   * the result to the controller.
   */
  void run_once();
};

#endif