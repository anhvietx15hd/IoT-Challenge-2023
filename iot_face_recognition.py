import os 
import cv2
import numpy as np
import torch
import torchvision.transforms as transforms
from insightface.app import FaceAnalysis
import paho.mqtt.client as mqtt
import time
import shutil


add_user_path = r'D:\Document\IOT\Face_bank'
face_embedding_path = r'D:\Document\IOT\face_embeddings.npy'
face_label_path = r'D:\Document\IOT\face_labels.npy'
face_bank_path = r'D:\Document\IOT\face_embeddings.npy'
model = FaceAnalysis(providers=['CPUExecutionProvider'])
model.prepare(ctx_id = -1)

class Embedding:
    def __init__(self, model, add_user_path ,face_embedding_path, face_label_path, name):
        self.name = name
        self.model = model
        self.add_user_path = add_user_path
        self.face_embedding_path = face_embedding_path
        self.face_label_path = face_label_path

    def embedding_face(self, name , add_user_path, face_embedding_path, face_label_path, model):
        global images_path
        embeddings = []
        names = []
        images_path = os.path.join(add_user_path, name)
        print(images_path)
        for path in os.listdir(images_path):
            global image_path
            image_path = os.path.join(images_path, path)
            img = cv2.imread(image_path)
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            print(img.shape)
            face = model.get(img)[0]
            embedding = face.embedding
            embeddings.append(embedding)
            names.append(name)
        return np.array(embeddings), np.array(names)
    
    def update_embedding(self, face_embedding_path, face_label_path, embedding, name):
        embeddings = np.load(face_embedding_path)
        labels = np.load(face_label_path)
        embeddings = np.concatenate([embeddings, embedding], axis = 0)
        labels = np.concatenate([labels, name])
        print(labels)
        np.save(face_embedding_path, embeddings)
        np.save(face_label_path, labels)
        print(f'Updated face embedding and label in {face_embedding_path} and {face_label_path}')

class Options:
    def __init__(self, add_user_path, face_bank_path, face_embedding_path, face_label_path, model, name):
        self.name = name
        self.model = model
        self.face_embedding_path = face_embedding_path
        self.face_label_path = face_label_path
        self.add_user_path = add_user_path
        self.face_bank_path = face_bank_path
    def is_q_pressed():
        return cv2.waitKey(1) & 0xFF == ord('q')
    
    def is_a_pressed():
        return cv2.waitKey(1) & 0xFF == ord('a')

    def update_user(self, name, add_user_path):
        cap = cv2.VideoCapture(0)
        count = 0
        while True:
            ret, frame = cap.read()
            if ret == False:
                print('Cannot capture frame device')
                break
            if not os.path.exists(os.path.join(add_user_path, name)):
                os.mkdir(os.path.join(add_user_path, name))
                cv2.imshow('frame', frame)
                print(f'Press "a" to take picture {count+1} or "q" to quit')
            if cv2.waitKey(1) or 0xFF == ord('a'):
                if count > 3:
                    cv2.imwrite(os.path.join(self.add_user_path, name, f'{count}.jpg'), frame)   
                count += 1
            if count == 12 or (cv2.waitKey(1) & 0xFF == ord('q')):
                cap.release()
                break
    
    def delete_user(self, name,add_user_path ,face_embedding_path, face_label_path ):
        embeddings = np.load(face_embedding_path)
        labels = np.load(face_label_path)
        index = np.where(labels == name)
        print(index)
        embeddings = np.delete(embeddings, index, axis = 0)
        labels = np.delete(labels, index, axis = 0)
        print(np.load(face_label_path))
        # remove folder images
        # check if none exist folder
        if os.path.exists(os.path.join(add_user_path, name)):
            shutil.rmtree(os.path.join(add_user_path, name))
        else: print('User does not exist')
        np.save(face_embedding_path, embeddings)
        np.save(face_label_path, labels)
        print(f'Deleted {name} in {face_embedding_path} and {face_label_path}')
        print(f'label: {np.load(face_label_path)}')

def recognize_frame(frame, embeddings, labels, model):
    faces = model.get(frame)
    if faces is not None and len(faces) > 0:
            face_embeddings = [face.embedding for face in faces]
            dot_product = face_embeddings@(embeddings.T)
            norm_face = np.linalg.norm(face_embeddings, axis=1, keepdims=True)
            norm_db = np.linalg.norm(embeddings, axis=1, keepdims=True)
            similarity = dot_product / (norm_face * norm_db.T)
            # print(similarity)
            cosine_distance = 1 - similarity
            print(cosine_distance)
            distances = np.squeeze(cosine_distance)
            min_distance_idx = np.argmin(distances)
            print(min_distance_idx)
            min_distance = np.min(distances)
            print(min_distance)
            label = labels[min_distance_idx]
            if min_distance > 0.5:
                label = 'unknow'
            else:
                label = labels[min_distance_idx]
            return label

def recognize_face_from_webcam(embeddings, labels, model):
    cap = cv2.VideoCapture(0)
    while True:
        ret, frame = cap.read()
        if ret == False:
            print('Cannot capture frame device')
            break
        cv2.imshow('frame', frame)
        label= recognize_frame(frame, embeddings, labels, model)
        return label

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Khi kết nối thành công, đăng ký để subscribe vào topic_sub
    client.subscribe(topic_sub)

if __name__ == '__main__':
    broker_address = "hungviet.hopto.org"  
    broker_port = 1883  
    topic_pub = "kookdump"
    topic_sub = "kook"
    username = "vikings"  
    password = "19013005" 
    
    # print(labels)
    # message =  recognize_face_from_webcam(embeddings, labels, model)
    # print(message)
    def on_message(client, userdata, msg):
        options =  str(msg.payload.decode("utf-8"))
        print(options)
        option, user_name = options.split(":")
        selections = Options(add_user_path, face_bank_path, face_embedding_path, face_label_path, model, user_name)
        embedd = Embedding(model, add_user_path ,face_embedding_path, face_label_path, user_name)
        if option == 'ADD':
            selections.update_user(user_name, add_user_path)
            emb, name_user = embedd.embedding_face(user_name, add_user_path, face_embedding_path, face_label_path, model)
            embedd.update_embedding(face_embedding_path, face_label_path, emb, name_user)

        elif option == 'DELETE':
            selections.delete_user(user_name, add_user_path, face_embedding_path, face_label_path)
        
    client = mqtt.Client()
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.on_message = on_message
    # Kết nối đến broker MQTT
    client.connect(broker_address, broker_port, 60)
    client.loop_start()
    # # client.publish(topic_pub, message)
    # # i = 0
# recognize 
embeddings = np.load(face_embedding_path)
labels = np.load(face_label_path)
try:
        while True:
            options = input('Receive requirement from server\n ==========================================\nPress "d" to detect face\n')
            
            if options =='d':
                print('detecting ...')
                message = recognize_face_from_webcam(embeddings, labels, model)
                print(message)
                client.publish(topic_pub, message)
                options = None
            else: time.sleep(1)
except KeyboardInterrupt:
        client.loop_stop()
        client.disconnect()
        print("Disconnected from broker.")


