# Setup Guide

## Python Requirements

| Package | Purpose |
|---|---|
| flask | HTTP API server |
| torch + torchvision | YOLOv5 inference engine |
| opencv-python | Webcam capture |
| pandas | YOLOv5 result parsing |
| requests | Used by push_to_device.py |

```bash
cd backend/python
python -m venv venv
source venv/bin/activate        # Windows: venv\Scripts\activate
pip install -r requirements.txt
```

> YOLOv5 weights (~14 MB) download automatically on first run via `torch.hub.load`.

## Run the Backend

```bash
# Terminal 1: start the API server
python api_server.py
# → Running on http://0.0.0.0:5000

# Terminal 2: start detection
python image_processing.py
# → Camera window opens
# → Press 'c' to capture and detect
# → Press 'q' to quit
```

Find your PC's local IP:
```bash
# Windows
ipconfig | findstr IPv4

# Linux / macOS
ip addr show | grep "inet "
```

## ESP8266 Setup

### Arduino Library Manager — Install:

- ArduinoJson (Benoit Blanchon) ≥ 6.x
- ESP8266HTTPClient (bundled with ESP8266 board package)

### Edit the sketch

```cpp
// firmware/esp8266_client/esp8266_client.ino
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* jsonUrl  = "http://192.168.x.x:5000/data";  // your PC IP
```

### Upload and verify

Open Serial Monitor at **115200 baud**. Expected output:
```
WiFi connected!
ESP8266 IP Address: 192.168.x.x
JSON Data:
[{"name":"person","confidence":0.726,...}]
Detected Object: person
Confidence: 0.73
Action: person detected! Turning on LED.
```

## Troubleshooting

| Issue | Fix |
|---|---|
| `Connection refused` on ESP8266 | Confirm PC IP in jsonUrl matches `ipconfig` output |
| No camera feed | Change `cv2.VideoCapture(1)` to `(0)` in image_processing.py |
| YOLOv5 download fails | Check internet connection on first run |
| ESP8266 not connecting | Confirm 2.4 GHz network (ESP8266 does not support 5 GHz) |
