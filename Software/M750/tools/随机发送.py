import paho.mqtt.client as mqtt
import json
import random
import time
from datetime import datetime

# MQTT 服务器配置
BROKER = "pve.afrain.top"
PORT = 1883
TOPIC = "RX"

# 连接回调函数
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
    else:
        print(f"Failed to connect, return code {rc}")

# 创建 MQTT 客户端
client = mqtt.Client()
client.on_connect = on_connect

# 连接到 MQTT 服务器
client.connect(BROKER, PORT, 60)
client.loop_start()

try:
    while True:
        data = {
            "t": round(random.uniform(20.0, 30.0), 1),  # 温度
            "h": round(random.uniform(30.0, 60.0), 1),  # 湿度
            "c": random.randint(600, 1000),  # CO2 浓度
            "v": random.randint(200, 400),  # VOC 浓度
            "lng": 118.481931,#round(116.397128 + random.uniform(-0.01, 0.01), 6),  # 小范围变化的经度
            "lat": 24.03845,#round(39.916527 + random.uniform(-0.01, 0.01), 6),  # 小范围变化的纬度
            "ts": datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%S")  # 时间戳
        }
        
        payload = json.dumps(data)
        client.publish(TOPIC, payload)
        print(f"Published: {payload}")
        
        time.sleep(5)  # 每 5 秒发送一次
except KeyboardInterrupt:
    print("Exiting...")
    client.loop_stop()
    client.disconnect()
