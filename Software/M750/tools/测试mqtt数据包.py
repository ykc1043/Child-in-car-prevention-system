import socket

# 目标 MQTT 服务器地址和端口（请修改为你的 MQTT 服务器信息）
MQTT_SERVER = "pve.afrain.top"  # 替换为你的 MQTT 服务器 IP
MQTT_PORT = 1883  # MQTT 默认端口

# MQTT CONNECT 数据包
mqtt_connect_packet = bytes([
    0x10, 0x1E, 0x00, 0x04, 0x4D, 0x51, 0x54, 0x54, 0x04, 0xC0, 0x01, 0x00, 0x00,
    0x04, 0x4D, 0x37, 0x35, 0x30, 0x00, 0x04, 0x4D, 0x37, 0x35, 0x30, 0x00,
    0x06, 0x70, 0x61, 0x73, 0x73, 0x77, 0x64
])

# 创建 TCP 连接
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.settimeout(5)  # 设置超时时间 5 秒

try:
    print(f"Connecting to {MQTT_SERVER}:{MQTT_PORT} ...")
    sock.connect((MQTT_SERVER, MQTT_PORT))  # 连接 MQTT 服务器
    print("Connected!")

    # 发送 MQTT CONNECT 数据包
    sock.sendall(mqtt_connect_packet)
    print("MQTT CONNECT packet sent.")

    # 接收服务器返回的数据（MQTT CONNACK 响应）
    response = sock.recv(1024)
    print("Received response:", response.hex())  # 打印 16 进制格式
except socket.timeout:
    print("Connection timed out!")
except Exception as e:
    print(f"Error: {e}")
finally:
    sock.close()
    print("Socket closed.")
