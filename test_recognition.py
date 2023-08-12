import cv2
import numpy as np
import insightface
from insightface.app import FaceAnalysis
from insightface.data import get_image as ins_get_image

handler = insightface.model_zoo.get_model('D:/document/IOT/weights/model.onnx')