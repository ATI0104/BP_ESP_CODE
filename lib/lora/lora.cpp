#include "lora.h"
#include <arduino_lmic_hal_boards.h>
Lora *Lora::instance = nullptr;
u1_t *Lora::MSB_to_LSB(const u1_t *bytes, size_t length) { return nullptr; }
void Lora::setup() {
  iot_data2 *data = iot_data2::getInstance();
  this->APPEUI = MSB_to_LSB(data->get_joinEui(), 8);
  this->DEVEUI = MSB_to_LSB(data->get_devEui(), 8);
  this->APPKEY = data->get_appKey();
  const lmic_pinmap *pPinMap = Arduino_LMIC::GetPinmap_ThisBoard();
  os_init_ex(pPinMap);
  LMIC_reset();
}

void Lora::send_data(const send_data_t *data) {
  send_data_t *send_data = new send_data_t;
  memcpy(send_data, data, sizeof(send_data_t));
  data_queue.push(send_data);
}

void Lora::loop() {}

Lora *lora = Lora::getInstance();

void os_getArtEui(u1_t *buf) { memcpy_P(buf, lora->get_joinEUI(), 8); }
void os_getDevEui(u1_t *buf) { memcpy_P(buf, lora->get_devEUI(), 8); }
void os_getDevKey(u1_t *buf) { memcpy_P(buf, lora->get_appKey(), 8); }

void onEvent(ev_t ev) {}
