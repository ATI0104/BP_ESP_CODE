/**
 * @file lora.h
 * @brief Header file for the Lora class.
 */

#ifndef __lora_h__
#define __lora_h__

#include <Arduino.h>
#include <lmic.h>
#include <SPI.h>
#include <hal/hal.h>
#include <queue>
#include "iot_data2.h"
#include "controller.h"

extern SemaphoreHandle_t ongoingLoraCommunication;

/**
 * @class Lora
 * @brief Class for handling LoRaWAN communication.
 */
class Lora {
 private:
  std::queue<uint8_t *> data_queue;  // Queue for storing data to be sent.
  static Lora *instance;             // Singleton instance of the Lora class.
  static osjob_t sendjob;            // Job for sending data.
  u1_t *APPEUI;                      // Join EUI for LoRaWAN.
  u1_t *DEVEUI;                      // Device EUI for LoRaWAN.
  u1_t *APPKEY;                      // Application Key for LoRaWAN.

  /**
   @brief Convert bytes from MSB to LSB format.
   * @param bytes The bytes to be converted.
   * @param length The length of the bytes.
   * @return The converted bytes.
   */
  u1_t *MSB_to_LSB(const u1_t *bytes, size_t length);

  uint32_t report_interval;  // Interval for reporting data.

  Lora() {}  // Private constructor for the Lora class.

  /**
  @brief Function for sending data.
   * @param j The job for sending data.
   */
  void doSend(osjob_t *j);

 public:
  // Deleted copy constructor and assignment operator
  Lora(const Lora &) = delete;
  Lora &operator=(const Lora &) = delete;

  /**
   *@brief Get the singleton instance of the Lora class.
   * @return The singleton instance.
   */
  static Lora *getInstance() {
    if (instance == nullptr) {
      instance = new Lora();
    }
    return instance;
  }

  /**
   *@brief Get the Join EUI for LoRaWAN.
   * @return The Join EUI.
   */
  const u1_t *get_joinEUI() { return APPEUI; }

  /**
   *@brief Get the Device EUI for LoRaWAN.
   * @return The Device EUI.
   */
  const u1_t *get_devEUI() { return DEVEUI; }

  /**
   *@brief Get the Application Key for LoRaWAN.
   * @return The Application Key.
   */
  const u1_t *get_appKey() { return APPKEY; }

  /**
   * @brief Initialize the LoRaWAN module. Set its pinout, communication mode...
   */
  void setup();

  /**
   * @brief Send data to LoRaWAN except from the first call this function is
   * called internally.
   *
   */
  void send_data(const send_data_t *data);

  /**
   * @brief Get the Join EUI for LoRaWAN.
   * @param buf The buffer to store the Join EUI.
   */
  static void os_getjoinEui(u1_t *buf);

  /**
   *@brief Get the Device EUI for LoRaWAN.
   * @param buf The buffer to store the Device EUI.
   */
  static void os_getdevEui(u1_t *buf);

  /**
   *@brief Get the Application Key for LoRaWAN.
   * @param buf The buffer to store the Application Key.
   */
  static void os_getappKey(u1_t *buf);

  /**
   *@brief Callback function for LoRa events.
   * @param pUserData User data.
   * @param ev The event.
   */
  static void onEvent(void *pUserData, ev_t ev);

  /**
   *@brief Callback function for sending data.
   * @param j The job for sending data.
   */
  static void send_data_clb(osjob_t *j);
  /**
   * @brief Sends received data to the controller.
   * 
   * @param data The received data
   */
  void recv_data(recv_data_t *data) {
    controller *c = controller::get_instance();
    c->receive_data(data);
  }
};

#endif