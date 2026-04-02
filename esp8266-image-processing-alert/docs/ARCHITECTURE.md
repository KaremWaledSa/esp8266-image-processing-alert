# Architecture

## Component Roles

| File | Role |
|---|---|
| `image_processing.py` | Opens webcam, captures on keypress, runs YOLOv5 inference, saves `result.json` |
| `api_server.py` | Flask server with two routes: `POST /upload` (receive JSON) and `GET /data` (serve JSON). Intended as the primary server when the detection script pushes results over HTTP |
| `result_server.py` | Minimal read-only Flask server: `GET /data` reads and serves `result.json` directly. Use this when detection runs locally on the same machine as the server |
| `push_to_device.py` | One-shot helper that POSTs a detection result to an ESP8266 endpoint |
| `esp8266_client.ino` | Arduino sketch that polls `GET /data` every 10 seconds, parses the JSON array, and reacts to target detections |

## Data Flow

```
image_processing.py
  │  cv2.VideoCapture → frame
  │  model(img_path)  → detections
  │  json.dump        → result.json
  │
  ▼  (Option A: local server)
result_server.py
  │  GET /data → reads result.json → returns JSON
  │
  ▼  (Option B: upload server)
api_server.py
  │  POST /upload ← push_to_device.py (or image_processing.py)
  │  GET  /data   → returns last uploaded JSON
  │
  ▼
esp8266_client.ino
    HTTP GET /data every 10s
    → parse JSON array
    → for each detection: check label + confidence
    → if match: blink LED_BUILTIN
```

## Why Two Server Files?

`result_server.py` is the simpler approach — detection and serving run on the same machine, the file is the shared state. `api_server.py` supports an upload workflow where detection runs on a separate machine and pushes results to the server. Both expose the same `GET /data` endpoint so the ESP8266 firmware works with either.

## JSON Schema

```json
[
  {
    "xmin": float,
    "ymin": float,
    "xmax": float,
    "ymax": float,
    "confidence": float,   // 0.0 – 1.0
    "class": int,          // COCO class index
    "name": string         // COCO class name, e.g. "person"
  }
]
```

## ESP8266 Poll Cycle

```
loop() runs every ~10 seconds:
  1. Check WiFi.status() == WL_CONNECTED
  2. HTTPClient GET /data
  3. If HTTP 200: deserializeJson(doc, payload)
  4. Iterate JsonArray:
       - read obj["name"] and obj["confidence"]
       - if label == "person" && confidence > 0.7: pulse LED
  5. http.end()
  6. delay(10000)
```
