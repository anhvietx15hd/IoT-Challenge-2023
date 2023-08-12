import cv2
import numpy as np
import insightface
from insightface.app import FaceAnalysis

# Khởi tạo mô hình InsightFace
model = FaceAnalysis()

# Tải pre-trained model (ví dụ: model R100)
model.prepare(ctx_id=-1)

# Load dữ liệu huấn luyện
# Bạn có thể sử dụng các phương pháp như tạo tệp face embeddings và face labels từ dữ liệu huấn luyện
# và sử dụng chúng trong việc nhận diện khuôn mặt
embeddings = np.load('face_embeddings.npy')
labels = np.load('face_labels.npy')

# Hàm nhận diện khuôn mặt
def recognize_face(image):
    # Phát hiện khuôn mặt trong hình ảnh đầu vào
    faces = model.get(image)

    if faces is not None and len(faces) > 0:
        # Tính toán nhúng khuôn mặt cho các khuôn mặt phát hiện được
        face_embeddings = model.get_embedding(image, faces)
        
        # Tính toán khoảng cách Euclidean giữa các nhúng khuôn mặt và nhúng từ dữ liệu huấn luyện
        distances = np.linalg.norm(embeddings - face_embeddings, axis=1)
        
        # Tìm vị trí của nhúng khuôn mặt gần nhất
        min_distance_idx = np.argmin(distances)
        min_distance = distances[min_distance_idx]
        
        # Xác định nhãn tương ứng
        if min_distance < 0.5:
            label = labels[min_distance_idx]
            return label, min_distance
        else:
            return "Unknown", min_distance
    else:
        return "No face detected", None

# Load hình ảnh để nhận diện
image_path = r'D:\document\IOT\quoc.jpg'
image = cv2.imread(image_path)

# Chuyển đổi màu từ BGR sang RGB
image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

# Nhận diện khuôn mặt và xác định nhãn
label, distance = recognize_face(image_rgb)

# Hiển thị kết quả
cv2.putText(image, f"Label: {label}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
cv2.putText(image, f"Distance: {distance}", (10, 70), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
cv2.imshow("Face Recognition", image)
cv2.waitKey(0)
cv2.destroyAllWindows()
