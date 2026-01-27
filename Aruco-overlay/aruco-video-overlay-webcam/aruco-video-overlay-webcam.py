import cv2
import numpy as np
import tkinter as tk
from threading import Thread

video = cv2.VideoCapture("overlay.mp4")
ret, overlay_img = video.read()

def start_camera():
    cap = cv2.VideoCapture(0)
    video = cv2.VideoCapture("overlay.mp4")

    aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)
    parameters = cv2.aruco.DetectorParameters()
    detector = cv2.aruco.ArucoDetector(aruco_dict, parameters)

    while True:
        ret, frame = cap.read()
        if not ret:
            break

        ret_v, overlay = video.read()
        if not ret_v:
            video.set(cv2.CAP_PROP_POS_FRAMES, 0)
            ret_v, overlay = video.read()

        corners, ids, _ = detector.detectMarkers(frame)

        if ids is not None:
            for i in range(len(ids)):
                marker_id = ids[i][0]

                if 0 <= marker_id <= 49:
                    pts = corners[i][0]

                    h, w = overlay.shape[:2]

                    src_pts = np.array([
                        [0, 0],
                        [w, 0],
                        [w, h],
                        [0, h]
                    ], dtype=np.float32)

                    dst_pts = pts.astype(np.float32)

                    matrix = cv2.getPerspectiveTransform(src_pts, dst_pts)

                    warped = cv2.warpPerspective(
                        overlay,
                        matrix,
                        (frame.shape[1], frame.shape[0])
                    )

                    mask = np.zeros(frame.shape, dtype=np.uint8)
                    cv2.fillConvexPoly(mask, dst_pts.astype(int), (255, 255, 255))

                    frame = cv2.bitwise_and(frame, cv2.bitwise_not(mask))
                    frame = cv2.bitwise_or(frame, warped)

                    cv2.polylines(frame, [pts.astype(int)], True, (0, 255, 0), 2)
                    cv2.putText(
                        frame,
                        f"ID: {marker_id}",
                        tuple(pts[0].astype(int)),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        0.6,
                        (0, 255, 0),
                        2
                    )

        cv2.imshow("ArUco Video AR", frame)

        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    video.release()
    cv2.destroyAllWindows()

def start_thread():
    Thread(target=start_camera).start()

root = tk.Tk()
root.title("ArUco Video AR System")
root.geometry("300x200")

label = tk.Label(root, text="ArUco Video Overlay", font=("Arial", 14))
label.pack(pady=20)

btn = tk.Button(
    root,
    text="Start Camera",
    font=("Arial", 12),
    width=15,
    height=2,
    command=start_thread
)
btn.pack()

root.mainloop()
