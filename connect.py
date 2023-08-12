import paho.mqtt.client as mqtt
import time
# topic = "kook"
# def subscribe(client: mqtt):
#     def on_message(client, userdata, msg):
#         print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
#     client.subscribe(topic)
#     client.on_message = on_message
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("kook")

def callback(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
    

broker_add = "hungviet.hopto.org"
port = 1883
client_ID = 'device_1'
username = 'vikings'
password = '19013005'
client = mqtt.Client(client_ID)
client.username_pw_set(username, password)
client.connect(broker_add, port=port)
client.on_message = callback
while True:
    client.publish("kookdump", "Hello World")
    time.sleep(3)

# client.loop_forever()
import paho.mqtt.client as mqtt

# Hàm xử lý khi kết nối thành công
def on_connect(client, userdata, flags, rc):
    print("Kết nối thành công!")
    client.subscribe("kook")  # Đăng ký để nhận dữ liệu từ chủ đề (topic) cần theo dõi

# Hàm xử lý khi nhận dữ liệu
def on_message(client, userdata, msg):
    print("Nhận dữ liệu từ chủ đề {}: {}".format(msg.topic, msg.payload.decode()))

# Tạo đối tượng MQTT client
client = mqtt.Client()

# Cấu hình thông tin đăng nhập
client.username_pw_set("vikings", "19013005")

# Thiết lập các hàm xử lý sự kiện
client.on_connect = on_connect
client.on_message = on_message

# Kết nối đến server MQTT
client.connect("hungviet.hopto.org", 1883, 60)

# Bắt đầu vòng lặp để duy trì kết nối và xử lý sự kiện
client.loop_forever()``