import cv2
import sys
# print(sys.path)
import numpy as np
import insightface
from insightface.app import FaceAnalysis
from insightface.data import get_image as ins_get_image

app = FaceAnalysis(providers=['CUDAExecutionProvider', 'CPUExecutionProvider'])
app.prepare(ctx_id=0, det_size=(640, 640))
img = ins_get_image(r'D:\document\IOT\viet')
faces = app.get(img)
print(faces)

rimg = app.draw_on(img, faces)
# write text on image
font = cv2.FONT_HERSHEY_SIMPLEX
cv2.putText(rimg, 'viet', (10, 500), font, 4, (255, 255, 255), 2, cv2.LINE_AA)
cv2.imwrite("./t1_output.jpg", rimg)