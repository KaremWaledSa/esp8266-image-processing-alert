import cv2
import torch
import json

# Load YOLOv5 model (pre-trained on COCO dataset)
model = torch.hub.load('ultralytics/yolov5', 'yolov5s')  # downloads weights automatically on first run


def capture_and_detect():
    # Open the webcam
    cap = cv2.VideoCapture(1)
    print("Press 'c' to capture an image or 'q' to quit.")

    while True:
        ret, frame = cap.read()
        cv2.imshow("Camera", frame)

        key = cv2.waitKey(1) & 0xFF
        if key == ord('c'):  # Capture the image
            img_path = "captured.jpg"
            cv2.imwrite(img_path, frame)
            print(f"Image saved: {img_path}")

            # Perform object detection
            results = model(img_path)
            detections = results.pandas().xyxy[0].to_dict(orient="records")
            print("Detections:", detections)

            # Save results to JSON
            with open("result.json", "w") as json_file:
                json.dump(detections, json_file, indent=4)
            print("Results saved to result.json")

        elif key == ord('q'):  # Quit
            break

    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    capture_and_detect()
