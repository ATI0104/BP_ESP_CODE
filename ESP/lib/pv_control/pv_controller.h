#ifndef __control_h
#define __control_h
#include <Arduino.h>
/**
 * @brief . This class provides functionality to control the output of a PV
 * panel. It allows initializing the controller, toggling the bypass status, and
 * retrieving the current bypass status.
 */
class pv_controller {
 private:
  static pv_controller *instance;
  uint8_t bypassed;
  uint8_t bypass_pin;
  pv_controller() {}

 public:
  // Delete constructor and assignment operator
  pv_controller(const pv_controller &) = delete;
  pv_controller &operator=(const pv_controller &) = delete;

  /**
   * @brief Initializes the PV controller.
   *
   * This function initializes the PV controller and sets up any necessary
   * configurations or resources.
   */
  void init();

  /**
   * @brief Toggles the bypass status of the PV controller.
   *
   * This function toggles the bypass status of the PV controller. If the
   * controller is currently bypassed, it will be activated. If it is currently
   * activated, it will be bypassed.
   *
   * @return The new bypass status after toggling (0 for bypassed, 1 for
   * activated).
   */
  uint8_t toggle_bypass();

  /**
   * @brief Retrieves the current bypass status of the PV controller.
   *
   * This function returns the current bypass status of the PV controller.
   *
   * @return The current bypass status (0 for bypassed, 1 for activated).
   */
  uint8_t get_bypass_status();

  /**
   * @brief Retrieves the singleton instance of the PV controller.
   *
   * This function returns the singleton instance of the PV controller. If the
   * instance does not exist, it will be created.
   *
   * @return A pointer to the PV controller instance.
   */
  static pv_controller *get_instance() {
    if (instance == nullptr) {
      instance = new pv_controller();
    }
    return instance;
  }
};
#endif