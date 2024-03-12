#include "lora.h"
#include "iot_data2.h"
Lora *Lora::instance = nullptr;
u1_t *Lora::APPEUI = nullptr;
u1_t *Lora::DEVEUI = nullptr;
u1_t *Lora::APPKEY = nullptr;
u1_t *Lora::MSB_to_LSB(const u1_t *bytes, size_t length) { return nullptr; }
void Lora::setup() {
  iot_data2 *data = iot_data2::getInstance();
  this->APPEUI = data->get_joinEui();
  this->DEVEUI = data->get_devEui();
  this->APPKEY = data->get_appKey();
  os_init();
  LMIC_reset();
}

void Lora::send_data(const u1_t *data) {}

void Lora::loop() {}
