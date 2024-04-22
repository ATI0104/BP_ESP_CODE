#include "lora.h"
#include <arduino_lmic_hal_boards.h>
Lora *Lora::instance = nullptr;
osjob_t Lora::sendjob = {};
// Save the singleton object in this application. This is needed because the
// LMIC library uses a C-style callback function to handle events.
Lora *lora = nullptr;
u1_t *Lora::MSB_to_LSB(const u1_t *bytes, size_t length) {
  u1_t *result = new u1_t[length];
  for (size_t i = 0; i < length; i++) {
    result[i] = bytes[length - i - 1];
  }
  return result;
}
void Lora::setup() {
  iot_data2 *data = iot_data2::getInstance();
  lora = this;
  this->APPEUI = MSB_to_LSB(data->get_joinEui(), 8);
  this->DEVEUI = MSB_to_LSB(data->get_devEui(), 8);
  this->APPKEY = data->get_appKey();
  this->report_interval = data->get_report_interval();
  const lmic_pinmap *pPinMap = Arduino_LMIC::GetPinmap_ThisBoard();
  os_init_ex(pPinMap);
  LMIC_reset();
  LMIC_registerEventCb(onEvent, nullptr);
  xSemaphoreGive(ongoingLoraCommunication);
}

void Lora::send_data(const send_data_t *data) {
  uint8_t *send_data = new uint8_t[sizeof(send_data_t)];
  memcpy(send_data, data, sizeof(send_data_t));
  delete data;
  data_queue.push(send_data);
  Serial.println(String((float)data->pv_voltage) + " " +
                 String((float)data->pv_current) + " " +
                 String((float)data->battery_voltage));
  // Seize semaphore
  xSemaphoreTake(ongoingLoraCommunication, portMAX_DELAY);
  this->doSend(&sendjob);
}
void Lora::doSend(osjob_t *j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {
    Serial.println(F("OP_TXRXPEND, not sending"));
  } else {
    // Prepare upstream data transmission at the next possible time.
    LMIC_setTxData2(1, this->data_queue.front(), sizeof(send_data_t), 1);
    Serial.println(F("Packet queued"));
  }
}

void Lora::os_getjoinEui(u1_t *buf) { memcpy(buf, lora->get_joinEUI(), 8); }
void Lora::os_getdevEui(u1_t *buf) { memcpy(buf, lora->get_devEUI(), 8); }
void Lora::os_getappKey(u1_t *buf) { memcpy(buf, lora->get_appKey(), 16); }
void Lora::onEvent(void *pUserData, ev_t ev) {
  switch (ev) {
    case EV_TXCOMPLETE:
      if (LMIC.txrxFlags & TXRX_ACK) {
        // Remove the used data from the queue
        uint8_t *d = instance->data_queue.front();
        delete[] d;
        instance->data_queue.pop();
        // Release the semaphore
        xSemaphoreGiveFromISR(ongoingLoraCommunication, nullptr);
        // Schedule next transmission
        os_setTimedCallback(
            &sendjob, os_getTime() + sec2osticks(instance->report_interval),
            send_data_clb);
      }
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
        recv_data_t *recv = new recv_data_t;
        memcpy(recv, LMIC.frame + LMIC.dataBeg, sizeof(recv_data_t));
        instance->recv_data(recv);
      }
      break;
    default:
      break;
  }
}

void Lora::send_data_clb(osjob_t *j) {
  controller *c = controller::get_instance();
  instance->send_data(c->get_data());
}
