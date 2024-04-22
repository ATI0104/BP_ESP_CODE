#!/usr/bin/env python3
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

@app.route("/GetPanel", methods=["GET"])
def get_panel():
    panel_dev_eui = request.args.get("panel_dev_eui")
    panel = db.get_panel_by_deveui(panel_dev_eui)
    return jsonify(panel.to_dict())

@app.route("/AddPanel", methods=["POST"])
def add_panel():
    name=request.json["name"]
    deveui=request.json["deveui"]
    joineui=request.json["joineui"]
    appkey=request.json["appkey"]
    panel = db.Panels(
        name=name,
        deveui=deveui,
    )
    db.add_panel(panel)
    cs.add_device(deveui, joineui, appkey, name)
    return "OK"
@app.route("/SendData",methods=["POST"])
def send_data():
    panel_dev_eui = request.json["panel_dev_eui"]
    data = request.json["data"]
    panel = db.get_panel_by_deveui(panel_dev_eui)
    mqtt.send_downlink(panel_dev_eui, data)
    return "OK"

if __name__ == "__main__":
    app.run(debug=False, host="0.0.0.0", port=8888)
