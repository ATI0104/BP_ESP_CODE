#ifndef __lora_h__
#define __lora_h__
#include <Arduino.h>
#include <lmic.h>
#include <SPI.h>
#include <hal/hal.h>
#include <queue>
#include "iot_data2.h"

class Lora {
 private:
  std::queue<send_data_t *> data_queue;
  static Lora *instance;
  u1_t *APPEUI;
  u1_t *DEVEUI;
  u1_t *APPKEY;
  u1_t *MSB_to_LSB(const u1_t *bytes, size_t length);
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
  const u1_t *get_joinEUI() { return APPEUI; }
  const u1_t *get_devEUI() { return DEVEUI; }
  const u1_t *get_appKey() { return APPKEY; }
  void setup();
  void send_data(const send_data_t *data);
  void loop();
};
void os_getArtEui(u1_t *buf);
void os_getDevEui(u1_t *buf);
void os_getDevKey(u1_t *buf);
void onEvent(ev_t ev);
static osjob_t sendjob;
#endif