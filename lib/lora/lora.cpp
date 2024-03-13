#include "lora.h"
#include <arduino_lmic_hal_boards.h>
Lora *Lora::instance = nullptr;
osjob_t Lora::sendjob = {};
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
  const lmic_pinmap *pPinMap = Arduino_LMIC::GetPinmap_ThisBoard();
  os_init_ex(pPinMap);
  LMIC_reset();
  LMIC_registerEventCb(onEvent, nullptr);
  xSemaphoreGive(ongoingLoraCommunication);
}

void Lora::send_data(const send_data_t *data) {
  send_data_t *send_data = new send_data_t;
  memcpy(send_data, data, sizeof(send_data_t));
  data_queue.push(send_data);
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
    LMIC_setTxData2(1, (uint8_t *)this->data_queue.front(),
                    sizeof(this->data_queue.front()), 1);
    Serial.println(F("Packet queued"));
  }
  // Next TX is scheduled after TX_COMPLETE event.
}

void Lora::os_getjoinEui(u1_t *buf) { memcpy_P(buf, lora->get_joinEUI(), 8); }
void Lora::os_getdevEui(u1_t *buf) { memcpy_P(buf, lora->get_devEUI(), 8); }
void Lora::os_getappKey(u1_t *buf) { memcpy_P(buf, lora->get_appKey(), 16); }
void Lora::onEvent(void *pUserData, ev_t ev) {
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      break;
      break;
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
      if (LMIC.txrxFlags & TXRX_ACK) Serial.println(F("Received ack"));
      if (LMIC.dataLen) {
        Serial.println(F("Received "));
        Serial.println(LMIC.dataLen);
        Serial.println(F(" bytes of payload"));
        char buf[LMIC.dataLen + 1];
        memcpy(buf, LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
        // TODO process result
      }
      // Release the semaphore
      xSemaphoreGiveFromISR(ongoingLoraCommunication, nullptr);
      break;
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      // data received in ping slot
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
    case EV_TXSTART:
      Serial.println(F("EV_TXSTART"));
      break;
    case EV_TXCANCELED:
      Serial.println(F("EV_TXCANCELED"));
      break;
    case EV_RXSTART:
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
      break;
    default:
      Serial.print(F("Unknown event: "));
      Serial.println((unsigned)ev);
      break;
  }
}
