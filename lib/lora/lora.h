#ifndef __lora_h__
#define __lora_h__
#include <Arduino.h>
#include <lmic.h>
#include <SPI.h>
#include <hal/hal.h>
#include <queue>
class Lora {
 private:
  std::queue<send_data_t *> data_queue;
  static Lora *instance;
  static u1_t *APPEUI;
  static u1_t *DEVEUI;
  static u1_t *APPKEY;
  static u1_t *MSB_to_LSB(const u1_t *bytes, size_t length);
  Lora() {}

 public:
  // Deleted copy constructor and assignment operator
  Lora(const Lora &) = delete;
  Lora &operator=(const Lora &) = delete;
  static Lora *getInstance() {
    if (instance == nullptr) {
      instance = new Lora();
    }
    return instance;
  }
  void setup();
  void send_data(const u1_t *data);
  void loop();
};
#endif