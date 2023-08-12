import os
import cv2
import numpy as np
import torch
import torchvision.transforms as transforms
from torchvision.models import resnet50
from PIL import Image

# Khởi tạo mô hình ArcFace
model = resnet50(pretrained=True)
model.fc = torch.nn.Linear(2048, 512)
model.eval()

# Đường dẫn tới thư mục dữ liệu huấn luyện
data_dir = r'D:\document\IOT\nhandienkhuonmat-20230626T162738Z-001\nhandienkhuonmat\Dataset\FaceData\processed'
# Biến để lưu trữ nhúng khuôn mặt và nhãn
embeddings = []
labels = []

# Chuyển đổi hình ảnh thành dữ liệu phù hợp cho mô hình
transform = transforms.Compose([
    transforms.Resize((112, 112)),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])
])

# Duyệt qua các thư mục và hình ảnh trong dữ liệu huấn luyện
for label in os.listdir(data_dir):
    label_dir = os.path.join(data_dir, label)
    
    if not os.path.isdir(label_dir):
        continue
    
    # Duyệt qua các tệp hình ảnh trong thư mục nhãn
    for image_file in os.listdir(label_dir):
        image_path = os.path.join(label_dir, image_file)
        
        # Tải và tiền xử lý hình ảnh
        image = Image.open(image_path).convert('RGB')
        image = transform(image).unsqueeze(0)
        
        # Tính toán nhúng khuôn mặt
        with torch.no_grad():
            embedding = model(image)
            embedding = torch.nn.functional.normalize(embedding, p=2, dim=1).squeeze().numpy()
        
        # Thêm nhúng và nhãn vào mảng
        embeddings.append(embedding)
        labels.append(label)

# Chuyển đổi mảng sang NumPy array
embeddings = np.array(embeddings)
labels = np.array(labels)

# Lưu trữ nhúng và nhãn vào tệp
np.save('face_embeddings.npy', embeddings)
np.save('face_labels.npy', labels)
