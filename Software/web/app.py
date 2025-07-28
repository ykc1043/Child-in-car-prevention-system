#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from flask import Flask, request, jsonify, render_template
import paho.mqtt.client as mqtt
from collections import deque
from threading import Lock
import datetime
import json
import uuid

app = Flask(__name__)

# ------------------------------
# 全局变量定义
# ------------------------------

# 调试页面使用的消息缓存（最多 50 条）
messages_cache = deque(maxlen=50)
cache_lock = Lock()

# MQTT 连接状态
mqtt_connected = False
connection_lock = Lock()

# Dashboard 数据存储（历史数据各最多 50 条）
dashboard_temphum_history = deque(maxlen=500)   # 存储温度(t)和湿度(h)
dashboard_co2tvoc_history = deque(maxlen=500)     # 存储 CO2(c)和 TVOC(v)
dashboard_geo = None                             # 最新经纬度数据（lng, lat）

# ------------------------------
# MQTT 客户端回调函数
# ------------------------------

def on_connect(client, userdata, flags, rc):
    global mqtt_connected
    with connection_lock:
        mqtt_connected = True
    # 连接成功后订阅 RX 主题
    client.subscribe("RX")
    print("MQTT connected, subscribed to RX")

def on_disconnect(client, userdata, rc):
    global mqtt_connected
    with connection_lock:
        mqtt_connected = False
    print("MQTT disconnected")

def on_message(client, userdata, msg):
    # 将收到的消息加入调试缓存
    with cache_lock:
        messages_cache.append({
            'payload': msg.payload,
            'timestamp': datetime.datetime.now().strftime("%H:%M:%S")
        })
    print("Received message on {}: {}".format(msg.topic, msg.payload))
    
    # 尝试将消息内容解码为字符串
    try:
        payload_str = msg.payload.decode("utf8")
    except Exception:
        payload_str = ""

    # 如果在 RX 主题中收到 "get/ts" 消息，则返回 Unix 时间戳到 TX 主题
    if msg.topic == "RX" and payload_str.strip() == "get/ts":
        ts = int(datetime.datetime.now().timestamp())
        response = {"ts": ts}
        response_str = json.dumps(response)
        client.publish("TX", response_str)
        print("Published Unix timestamp response to TX:", response_str)
        # 返回后不再执行后续逻辑
        return

    # 以下部分为原来的 JSON 解析逻辑，用于 Dashboard 数据处理
    try:
        data = json.loads(payload_str)
        # 要求 JSON 中必须包含 t, h, c, v, lng, lat 字段
        if all(k in data for k in ["t", "h", "c", "v", "lng", "lat"]):
            try:
                raw_ts = data.get("ts", None)
                if raw_ts:
                    try:
                        dt = datetime.datetime.fromisoformat(raw_ts)
                        ts = dt.strftime("%H:%M:%S")
                    except Exception:
                        ts = datetime.datetime.now().strftime("%H:%M:%S")
                else:
                    ts = datetime.datetime.now().strftime("%H:%M:%S")
            except Exception:
                ts = datetime.datetime.now().strftime("%H:%M:%S")
            dashboard_temphum_history.append({
                "ts": ts,
                "t": data["t"],
                "h": data["h"]
            })
            dashboard_co2tvoc_history.append({
                "ts": ts,
                "c": data["c"],
                "v": data["v"]
            })
            global dashboard_geo
            dashboard_geo = {
                "ts": ts,
                "lng": data["lng"],
                "lat": data["lat"]
            }
    except Exception as e:
        # 如果解析 JSON 数据出错，则忽略
        pass

# ------------------------------
# 初始化 MQTT 客户端
# ------------------------------

mqtt_client = mqtt.Client(client_id=f"Web-{uuid.uuid4().hex[:8]}")
mqtt_client.on_connect = on_connect
mqtt_client.on_disconnect = on_disconnect
mqtt_client.on_message = on_message

try:
    mqtt_client.connect("127.0.0.1", 1883, 60)
except Exception as e:
    print("MQTT connection error:", e)

# 在后台线程中运行 MQTT 网络循环
mqtt_client.loop_start()

# ------------------------------
# Flask 路由
# ------------------------------

# Dashboard 页面（首页）
@app.route("/")
def dashboard():
    return render_template("dashboard.html")

# 调试页面
@app.route("/debug")
def debug():
    return render_template("debug.html")

# 地图页面（用于显示经纬度位置）
@app.route("/map")
def map_page():
    return render_template("map.html")

# 获取调试页面消息（调试页面使用）
@app.route("/get_messages")
def get_messages():
    enc = request.args.get("encoding", "utf8")
    mode = request.args.get("mode", "string")
    with cache_lock:
        messages = list(messages_cache)
    converted_messages = []
    for msg in messages:
        raw = msg["payload"]
        if mode == "string":
            try:
                text = raw.decode(enc, errors="replace")
            except Exception as e:
                text = "解码错误：" + str(e)
        elif mode == "hex":
            text = " ".join("{:02X}".format(b) for b in raw)
        else:
            text = raw.decode(enc, errors="replace")
        converted_messages.append({
            "timestamp": msg["timestamp"],
            "payload": text
        })
    return jsonify(converted_messages)

# 获取 MQTT 连接状态（调试页面使用）
@app.route("/connection_status")
def connection_status():
    with connection_lock:
        status = "Connected" if mqtt_connected else "Disconnected"
    return jsonify({"status": status})

# 发送消息（调试页面使用），发布到 TX 主题
@app.route("/send_message", methods=["POST"])
def send_message():
    data = request.get_json()
    if not data:
        return jsonify({"success": False, "error": "没有收到数据"})
    message = data.get("message", "")
    enc = data.get("encoding", "utf8")
    mode = data.get("mode", "string")
    if mode == "string":
        try:
            payload = message.encode(enc)
        except Exception as e:
            return jsonify({"success": False, "error": str(e)})
    elif mode == "hex":
        hex_str = message.replace(" ", "")
        if len(hex_str) % 2 != 0:
            return jsonify({"success": False, "error": "Hex 字符串长度必须为偶数"})
        try:
            payload = bytes.fromhex(hex_str)
        except Exception as e:
            return jsonify({"success": False, "error": str(e)})
    else:
        return jsonify({"success": False, "error": "无效的发送模式"})
    mqtt_client.publish("TX", payload)
    print("Published message to TX:", payload)
    return jsonify({"success": True})

# 清除调试页面消息缓存
@app.route("/clear_messages", methods=["POST"])
def clear_messages():
    with cache_lock:
        messages_cache.clear()
    return jsonify({"success": True})

# 获取 Dashboard 数据接口，返回最新数据及历史数据
@app.route("/get_dashboard_data")
def get_dashboard_data():
    default_temphum = {"t": "--", "h": "--"}
    default_co2tvoc = {"c": "--", "v": "--"}
    default_geo = {"lng": "--", "lat": "--"}
    latest_temphum = dashboard_temphum_history[-1] if dashboard_temphum_history else default_temphum
    latest_co2tvoc = dashboard_co2tvoc_history[-1] if dashboard_co2tvoc_history else default_co2tvoc
    geo = dashboard_geo if dashboard_geo else default_geo
    return jsonify({
        "t": latest_temphum.get("t", "--"),
        "h": latest_temphum.get("h", "--"),
        "c": latest_co2tvoc.get("c", "--"),
        "v": latest_co2tvoc.get("v", "--"),
        "lng": geo.get("lng", "--"),
        "lat": geo.get("lat", "--"),
        "temphum_history": list(dashboard_temphum_history),
        "co2tvoc_history": list(dashboard_co2tvoc_history)
    })

# ------------------------------
# 主函数
# ------------------------------

if __name__ == "__main__":
    # app.run(host="0.0.0.0", port=5000, debug=True)
    # app.run(host="0.0.0.0", port=5000, debug=True, ssl_context=("/root/py/afrain.pem", "/root/py/afrain.key"))
    app.run(host="0.0.0.0", port=5000, debug=True, threaded=True, ssl_context=("/root/py/afrain.pem", "/root/py/afrain.key"))

