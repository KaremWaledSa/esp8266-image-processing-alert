from flask import Flask, jsonify, request
import json
from pathlib import Path

app = Flask(__name__)
DATA_FILE = Path("server_data.json")

@app.route("/upload", methods=["POST"])
def upload_json():
    data = request.json or {}
    DATA_FILE.write_text(json.dumps(data, indent=2), encoding="utf-8")
    return jsonify({"message": "Data received and saved on server."})

@app.route("/data", methods=["GET"])
def get_json():
    if not DATA_FILE.exists():
        return jsonify([])
    return jsonify(json.loads(DATA_FILE.read_text(encoding="utf-8")))

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
