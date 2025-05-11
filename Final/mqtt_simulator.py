import paho.mqtt.client as mqtt
import ssl
import json
import time
import random

# MQTT 접속 정보
MQTT_BROKER = "192.168.124.130"        # 또는 브로커 IP (예: "192.168.124.130")
MQTT_PORT = 8883
MQTT_TOPIC = "sensor/data"
MQTT_USERNAME = "esp32client"
MQTT_PASSWORD = "password"

# 인증서 경로
CA_CERT = "/etc/mosquitto/certs/ca.crt"
CLIENT_CERT = "/etc/mosquitto/certs/esp32client.crt"  # 클라이언트 인증서 필요 시 경로 입력
CLIENT_KEY = "/etc/mosquitto/certs/esp32client.key"   # 클라이언트 키 필요 시 경로 입력

# MQTT 클라이언트 생성
client = mqtt.Client()

# 인증 설정
client.tls_set(ca_certs=CA_CERT,
               certfile=CLIENT_CERT,
               keyfile=CLIENT_KEY,
               tls_version=ssl.PROTOCOL_TLSv1_2)

client.tls_insecure_set(False)  # CN 불일치 무시 (테스트용)
client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

# 연결 콜백
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✅ Connected to broker")
    else:
        print(f"❌ Connection failed: {rc}")

client.on_connect = on_connect

# 브로커 연결
client.connect(MQTT_BROKER, MQTT_PORT, 60)
client.loop_start()

# 가짜 온습도 퍼블리시 반복
try:
    while True:
        data = {
            "temperature": round(random.uniform(20.0, 30.0), 2),
            "humidity": round(random.uniform(40.0, 60.0), 2)
        }
        payload = json.dumps(data)
        client.publish(MQTT_TOPIC, payload)
        print(f"📤 Published: {payload}")
        time.sleep(5)
except KeyboardInterrupt:
    print("🛑 Stopped by user")
    client.loop_stop()
