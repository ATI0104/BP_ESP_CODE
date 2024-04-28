import base64
import struct


def decode_payload(payload):
    binary_data = base64.b64decode(payload)
    # Define the struct format:
    # 'dddBI' corresponds to three doubles, one uint8_t, and one uint32_t
    # '=' ensures no padding is added for alignment
    data_format = "=dddBI"
    # Unpack the binary data according to the specified format
    decoded_payload = struct.unpack(data_format, binary_data)
    return {
        "pv_voltage": decoded_payload[0],
        "pv_current": decoded_payload[1],
        "battery_voltage": decoded_payload[2],
        "bypass_state": decoded_payload[3],
        "report_interval": decoded_payload[4],
    }


def encode_data(report_interval, bypass, reset):
    data_format = "=IBB"
    bypass = int(bypass)
    reset = int(reset)
    report_interval = int(report_interval)
    encoded_data = struct.pack(data_format, report_interval, bypass, reset)
    encoded_string = base64.b64encode(encoded_data).decode("utf-8")
    return encoded_string
