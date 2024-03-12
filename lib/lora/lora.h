#ifndef __lora_h__
#define __lora_h__
#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
class Lora {
 private:
  static Lora *instance;
  static u1_t PROGMEM APPEUI[8];
  static u1_t PROGMEM DEVEUI[8];
  static u1_t PROGMEM APPKEY[16];
  static void do_send(osjob_t *j);
  static void on_event(ev_t ev);
  static void print_hex_2(unsigned v);
  static u1_t *MSB_to_LSB(const u1_t *bytes);

 public:
  static Lora *getInstance(){
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