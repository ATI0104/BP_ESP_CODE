import json
import paho.mqtt.client as mqtt
import threading
import base64
import random
import time
import db_adapter as db
import decoder
import chirpstack as cs
from config import mqtt_broker_address

application_id = cs.get_application_id()
topic_up = f"application/{application_id}/device/+/event/up"
client_id = f"python-mqtt-{random.randint(0, 100)}"


class MQTT:
    def __init__(self):
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_message = self.on_message
        self.token = None

    def on_message(self, _client, _userdata, message):
        payload = json.loads(message.payload)
        dev_eui = payload["deviceInfo"]["devEui"]
        if payload['fPort']>0: 
            d = decoder.decode_payload(payload["data"])
            if db.add_event(dev_eui, d,payload["time"]):
                print(f"Added event to database for device {dev_eui}")
            else:
                print(f"Failed to add to database for device {dev_eui}")

    def start(self):
        connected = self.client.connect(mqtt_broker_address)
        if connected == 0:
            self.client.subscribe(topic_up)
            self.client.loop_start()

        else:
            print("Failed to connect to MQTT broker.")

    def stop(self):
        self.client.loop_stop()
        self.client.disconnect()

    def send_downlink(self, dev_eui: str, report_interval, bypass, reset):
        topic = f"application/{application_id}/device/{dev_eui}/command/down"
        payload = decoder.encode_data(report_interval, bypass, reset)
        data = {
            "devEui": dev_eui,
            "confirmed": True,
            "fPort": 1,
            "data": payload,
        }
        self.client.publish(topic, json.dumps(data))


mqtt = MQTT()
if __name__ == "__main__":
    mqtt.start()
    while True:
        time.sleep(5)
