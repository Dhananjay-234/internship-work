import cv2
import tkinter as tk
from PIL import Image, ImageTk
from ultralytics import YOLO

model = YOLO("yolov8n.pt")

cap = None
running = False

def start_camera():
    global cap, running
    if not running:
        cap = cv2.VideoCapture(0)
        running = True
        update_frame()

def stop_camera():
    global cap, running
    running = False
    if cap:
        cap.release()

def update_frame():
    if not running:
        return

    ret, frame = cap.read()
    if not ret:
        return

    results = model(frame, stream=True)

    for result in results:
        for box in result.boxes:
            cls = int(box.cls[0])
            if cls == 0:
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                conf = float(box.conf[0]) * 100

                if conf < 60:
                    continue

                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                cv2.putText(
                    frame,
                    f"Human {conf:.1f}%",
                    (x1, y1 - 10),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.6,
                    (0, 255, 0),
                    2
                )

    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    img = Image.fromarray(frame)
    imgtk = ImageTk.PhotoImage(image=img)

    video_label.imgtk = imgtk
    video_label.configure(image=imgtk)

    video_label.after(10, update_frame)

root = tk.Tk()
root.title("Live Human Detection System")
root.geometry("900x600")

title = tk.Label(root, text="Live Human Detection", font=("Arial", 20))
title.pack(pady=10)

video_label = tk.Label(root)
video_label.pack()

btn_frame = tk.Frame(root)
btn_frame.pack(pady=10)

start_btn = tk.Button(btn_frame, text="Start Camera", width=15, command=start_camera)
start_btn.pack(side=tk.LEFT, padx=10)

stop_btn = tk.Button(btn_frame, text="Stop Camera", width=15, command=stop_camera)
stop_btn.pack(side=tk.LEFT, padx=10)

root.mainloop()
