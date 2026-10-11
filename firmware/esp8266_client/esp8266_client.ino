#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// Wi-Fi credentials (replace before flashing)
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// API URL
const char* jsonUrl = "http://YOUR_COMPUTER_IP:5000/data";

WiFiClient wifiClient;  // Create a WiFiClient object

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);  // Initialize Serial communication

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected!");
  Serial.println("ESP8266 IP Address: " + WiFi.localIP().toString());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, jsonUrl);  // Use the new API

    int httpCode = http.GET();

    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println("JSON Data:");
        Serial.println(payload);

        // Parse JSON response
        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          for (JsonObject obj : doc.as<JsonArray>()) {
            const char* label = obj["name"];
            float confidence = obj["confidence"];

            Serial.print("Detected Object: ");
            Serial.println(label);
            Serial.print("Confidence: ");
            Serial.println(confidence);

            // Take action based on detected object
            if (String(label) == "person" && confidence > 0.7) {
              Serial.println("Action: person detected! Turning on LED.");
              digitalWrite(LED_BUILTIN, HIGH);  // Turn on LED
              delay(500);
              digitalWrite(LED_BUILTIN, LOW);  // Turn off LED
            }
          }
        } else {
          Serial.print("JSON Parse Error: ");
          Serial.println(error.c_str());
        }
      }
    } else {
      Serial.print("HTTP request failed: ");
      Serial.println(httpCode);
    }
    http.end();
  }
  delay(10000);  // Fetch data every 10 seconds
}
