#!/usr/bin/env python3
import base64
import threading
from flask import Flask, request, jsonify
import db_adapter as db
import chirpstack as cs
from mqtt_adapter import mqtt

threading.Thread(target=mqtt.start).start()
app = Flask(__name__)


@app.route("/GetPanels", methods=["GET"])
def get_panels():
    panels = db.get_panels()
    return jsonify(panels)


@app.route("/AddPanel", methods=["POST"])
def add_panel():
    name = request.json["name"]
    deveui = bytes.fromhex(request.json["deveui"])
    joineui =bytes.fromhex(request.json["joineui"])
    appkey = bytes.fromhex(request.json["appkey"])
    panel = db.Panels(
        name=name,
        id=deveui,
    )
    db.add_panel(panel)
    cs.add_device(deveui, joineui, appkey, name)
    return "OK"


@app.route("/SendData", methods=["POST"])
def send_data():
    panel_dev_eui = request.json["dev_eui"]
    report_interval = request.json["report_interval"]
    bypass = request.json["bypass"]
    reset = request.json["reset"]
    mqtt.send_downlink(panel_dev_eui, report_interval, bypass, reset)
    return "OK"


if __name__ == "__main__":
    app.run(debug=False, host="0.0.0.0", port=8888)
