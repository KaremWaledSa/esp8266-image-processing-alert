# ESP8266 Image Processing Alert

> A computer-vision IoT bridge: a Python backend runs YOLOv5 object detection on a webcam stream and exposes results over a local HTTP API — the ESP8266 polls the API and triggers a physical alert when a target object is detected.

![Platform](https://img.shields.io/badge/Platform-ESP8266%20%2F%20Arduino-blue)
![Backend](https://img.shields.io/badge/Backend-Python%20%7C%20Flask%20%7C%20YOLOv5-green)
![Model](https://img.shields.io/badge/Model-YOLOv5s%20%28COCO%29-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## Overview

This project connects a Python computer-vision backend to a physical microcontroller reaction. The system detects objects in a webcam frame using YOLOv5, serves the latest detection results through a REST API, and lets an ESP8266 react to specific detections — turning on an LED, triggering a buzzer, or activating a relay.

No cloud service is used. Everything runs locally on the same Wi-Fi network.

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    PC (Python Backend)                       │
│                                                              │
│  Webcam ──► image_processing.py                             │
│                    │                                         │
│                    │  YOLOv5s inference (PyTorch)            │
│                    │                                         │
│                    ▼                                         │
│             result.json  ◄──────────────────────────        │
│                    │                                         │
│                    ▼                                         │
│             api_server.py   (:5000)                         │
│              ├── POST /upload  ← receive detection results   │
│              └── GET  /data    → serve latest results        │
└──────────────────────┬──────────────────────────────────────┘
                       │
                  Local Wi-Fi
                       │
┌──────────────────────▼──────────────────────────────────────┐
│                  ESP8266 Client                               │
│                                                              │
│   loop():                                                    │
│     GET http://<PC_IP>:5000/data  every 10 seconds          │
│     parse JSON → if "person" detected (confidence > 0.7)    │
│     → blink LED_BUILTIN                                      │
└─────────────────────────────────────────────────────────────┘
```

---

## Features

| Feature | Implementation |
|---|---|
| Object detection | YOLOv5s pretrained on COCO (80 classes) |
| Webcam capture | OpenCV, press `c` to capture |
| Detection API | Flask REST — GET `/data`, POST `/upload` |
| ESP8266 polling | HTTP GET every 10 seconds |
| Configurable trigger | Object label + confidence threshold in `.ino` |
| Example output | `examples/sample_result.json` |

---

## Repository Structure

```
esp8266-image-processing-alert/
├── backend/
│   └── python/
│       ├── image_processing.py   # Webcam capture + YOLOv5 inference
│       ├── api_server.py         # Flask API: POST /upload + GET /data
│       ├── result_server.py      # Minimal read-only server: GET /data
│       ├── push_to_device.py     # Helper: push result to ESP8266 endpoint
│       └── requirements.txt
├── firmware/
│   └── esp8266_client/
│       ├── esp8266_client.ino    # Wi-Fi + HTTP poll + LED reaction
│       └── secrets.example.h    # Credential template
├── examples/
│   └── sample_result.json       # Example YOLOv5 output format
├── media/
│   └── screenshots/             # Demo screenshots
├── docs/
│   ├── ARCHITECTURE.md
│   └── SETUP.md
├── .gitignore
├── LICENSE
└── README.md
```

---

## Detection Output Format

YOLOv5 returns a list of detection objects. Example (`examples/sample_result.json`):

```json
[
  {
    "name": "person",
    "confidence": 0.726,
    "xmin": 141.3, "ymin": 89.4,
    "xmax": 631.9, "ymax": 480.0,
    "class": 0
  }
]
```

The ESP8266 reads the `"name"` and `"confidence"` fields from each object in the array.

---

## Trigger Configuration

Edit these two values in `esp8266_client.ino` to change what object triggers the alert:

```cpp
if (String(label) == "person" && confidence > 0.7) {
    // trigger alert
}
```

Any of the [80 COCO class names](https://github.com/ultralytics/yolov5/blob/master/data/coco.yaml) can be used (e.g. `"cat"`, `"car"`, `"cell phone"`).

---

## Getting Started

### Python Backend

```bash
cd backend/python
python -m venv venv && source venv/bin/activate   # Windows: venv\Scripts\activate
pip install -r requirements.txt

# Terminal 1: run the API server
python api_server.py

# Terminal 2: run detection (press 'c' to capture, 'q' to quit)
python image_processing.py
```

### ESP8266 Firmware

```
1. Open: firmware/esp8266_client/esp8266_client.ino
2. Edit:
      const char* ssid     = "YOUR_WIFI_SSID";
      const char* password = "YOUR_WIFI_PASSWORD";
      const char* jsonUrl  = "http://YOUR_PC_IP:5000/data";
3. Install libraries (Arduino Library Manager):
      - ArduinoJson (by Benoit Blanchon) ≥ 6.x
      - ESP8266HTTPClient (bundled with ESP8266 board package)
4. Upload to board
5. Open Serial Monitor at 115200 baud
```

---

## Potential Improvements

- Replace polling with MQTT or WebSocket push for lower latency
- Add `.env` file support for configurable IP and thresholds
- Add a proper HTTP endpoint on the ESP8266 to receive push notifications
- Add continuous frame capture mode instead of manual key press

---

## Security

Never commit Wi-Fi credentials or local IP addresses. Copy `secrets.example.h` → `secrets.h` and add `secrets.h` to `.gitignore`. Runtime outputs (`result.json`, `captured.jpg`) are already excluded via `.gitignore`.

---

## License

MIT — see [LICENSE](LICENSE).
