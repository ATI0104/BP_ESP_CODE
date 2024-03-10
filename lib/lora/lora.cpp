#include "lora.h"

void Lora::do_send(osjob_t* j) {}

void Lora::on_event(ev_t ev) {}

void Lora::print_hex_2(unsigned v) {
  v &= 0xff;
  if (v < 16) Serial.print('0');
  Serial.print(v, HEX);
}

u1_t* Lora::MSB_to_LSB(const u1_t* bytes) {
  size_t len = sizeof(bytes);
  static u1_t* lsb = new u1_t[len];
  for (size_t i = 0; i < len; i++) {
    lsb[i] = bytes[len - i - 1];
  }
  return lsb;
}

Lora::Lora() {}

void Lora::setup() {}

void Lora::send_data(const u1_t* data) {}

void Lora::loop() {}
