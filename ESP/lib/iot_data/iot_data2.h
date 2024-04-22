/**
 * @file iot_data2.h
 * @brief Header file for the iot_data2 class.
 *
 * This file contains the declaration of the iot_data2 class, which is
 * responsible for managing internal data. It includes necessary libraries and
 * defines the data structures used for sending and receiving data. The class
 * provides methods for accessing and modifying the IoT data, as well as
 * converting data types.
 */

#ifndef IOT_DATA2_H
#define IOT_DATA2_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#pragma pack(push, 1)  // Disables struct padding

/** @struct send_data_t
 * @brief Structure for sending data.
 *
 * This structure defines the data format for sending data.
 * It includes fields for PV voltage, PV current, battery voltage, bypass
 * status, and report interval.
 */
struct send_data_t {
  double pv_voltage;         // 8B
  double pv_current;         // 8B
  double battery_voltage;    // 8B
  uint8_t bypassed;          // 1B
  uint32_t report_interval;  // 4B
};                           // 29B total

/**
 * @struct recv_data_t
 * @brief Structure for receiving data.
 *
 * This structure defines the data format for receiving data.
 * It includes fields for report interval, bypass status, and reset flag.
 */
struct recv_data_t {
  uint32_t report_interval;  // 4B
  uint8_t bypass;            // 1B
  uint8_t reset;             // 1B
};

#pragma pack(pop)  // Reenables struct padding

/**
 * @class iot_data2
 * @brief Class for managing IoT data.
 *
 * The iot_data2 class is a singleton class that provides methods for accessing
 * and modifying IoT data. It also includes methods for converting data types.
 */
class iot_data2 {
 private:
  static iot_data2* instance;
  uint8_t* devEui;
  uint8_t* joinEui;
  uint8_t* appKey;
  char* ssid;
  char* password;
  uint8_t sda;
  uint8_t scl;
  uint8_t mosfet;
  uint8_t adc_rdy;
  uint32_t report_interval;
  uint8_t bypass_pv;

  /**
   * @brief Loads data from storage.
   *
   * This private method loads the configuration data from storage.
   */
  void load_data();

  /**
   * @brief Saves data to storage.
   *
   * This private method saves the IoT data to storage.
   */
  void save_data();

  /**
   * @brief Private constructor.
   *
   * This private constructor initializes the iot_data2 class and loads the data
   * from storage.
   */
  iot_data2() { this->load_data(); };

 public:
  // Deleted copy constructor and assignment operator
  iot_data2(const iot_data2&) = delete;
  iot_data2& operator=(const iot_data2&) = delete;

  /**
   * @brief Static method to access class instance.
   *
   * This static method returns the instance of the iot_data2 class.
   * If the instance does not exist, it creates a new instance.
   *
   * @return Pointer to the iot_data2 instance.
   */
  static iot_data2* getInstance() {
    if (instance == nullptr) {
      instance = new iot_data2();
    }
    return instance;
  }

  // getters and setters

  /**
   * @brief Get the device EUI.
   *
   * This method returns a pointer to the device EUI.
   *
   * @return Pointer to the device EUI.
   */
  uint8_t* get_devEui();

  /**
   * @brief Get the join EUI.
   *
   * This method returns a pointer to the join EUI.
   *
   * @return Pointer to the join EUI.
   */
  uint8_t* get_joinEui();

  /**
   * @brief Get the application key.
   *
   * This method returns a pointer to the application key.
   *
   * @return Pointer to the application key.
   */
  uint8_t* get_appKey();

  /**
   * @brief Get the SSID.
   *
   * This method returns a pointer to the SSID.
   *
   * @return Pointer to the SSID.
   */
  char* get_ssid();

  /**
   * @brief Get the password.
   *
   * This method returns a pointer to the password.
   *
   * @return Pointer to the password.
   */
  char* get_password();

  /**
   * @brief Get the SDA pin.
   *
   * This method returns the SDA pin number.
   *
   * @return The SDA pin number.
   */
  uint8_t get_sda_pin();

  /**
   * @brief Get the SCL pin.
   *
   * This method returns the SCL pin number.
   *
   * @return The SCL pin number.
   */
  uint8_t get_scl_pin();

  /**
   * @brief Get the MOSFET pin.
   *
   * This method returns the MOSFET pin number.
   *
   * @return The MOSFET pin number.
   */
  uint8_t get_mosfet_pin();

  /**
   * @brief Get the report interval.
   *
   * This method returns the report interval.
   *
   * @return The report interval.
   */
  uint32_t get_report_interval();

  /**
   * @brief Get the bypass PV status.
   *
   * This method returns the bypass PV status.
   *
   * @return The bypass PV status.
   */
  uint8_t get_bypass_pv();

  /**
   * @brief Set the report interval.
   *
   * This method sets the report interval.
   *
   * @param report_interval The report interval to set.
   */
  void set_report_interval(uint32_t report_interval);

  /**
   * @brief Set the bypass PV status.
   *
   * This method sets the bypass PV status.
   *
   * @param bypass_pv The bypass PV status to set.
   */
  void set_bypass_pv(uint8_t bypass_pv);

  /**
   * @brief Set the device EUI.
   *
   * This method sets the device EUI.
   *
   * @param devEui The device EUI to set.
   */
  void set_devEui(const char* devEui);

  /**
   * @brief Set the join EUI.
   *
   * This method sets the join EUI.
   *
   * @param joinEui The join EUI to set.
   */
  void set_joinEui(const char* joinEui);

  /**
   * @brief Set the application key.
   *
   * This method sets the application key.
   *
   * @param appKey The application key to set.
   */
  void set_appkey(const char* appKey);

  /**
   * @brief Convert string to byte array.
   *
   * This method converts a string to a byte array.
   *
   * @param str The string to convert.
   * @return Pointer to the byte array.
   */
  uint8_t* str_to_byte_array(const char* str);

  /**
   * @brief Convert byte array to hexadecimal string.
   *
   * This method converts a byte array to a hexadecimal string.
   *
   * @param data The byte array to convert.
   * @param len The length of the byte array.
   * @return Pointer to the hexadecimal string.
   */

  String* to_hex_str(uint8_t* data, size_t len);
};

#endif