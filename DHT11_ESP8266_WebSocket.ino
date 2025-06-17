#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <DHT.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

// DHT11 sensor setup
#define DHT_PIN 2        // Pin D4 pada NodeMCU
#define DHT_TYPE DHT11   // DHT11 sensor type
DHT dht(DHT_PIN, DHT_TYPE);

// WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Web server on port 80
ESP8266WebServer server(80);

// Variables for sensor data
float temperature = 0.0;
float humidity = 0.0;
float heatIndex = 0.0;

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastWebSocketSend = 0;
const unsigned long sensorInterval = 2000;  // Read sensor every 2 seconds
const unsigned long wsInterval = 2000;      // Send WebSocket data every 2 seconds

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== DHT11 ESP8266 WebSocket Server ===");
  
  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT11 sensor initialized");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("WebSocket Server: ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":81/");
  
  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket server started on port 81");
  
  // Start web server
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("HTTP server started on port 80");
  
  Serial.println("Setup complete!");
  Serial.println("=====================================");
}

void loop() {
  // Handle WebSocket events
  webSocket.loop();
  
  // Handle HTTP requests
  server.handleClient();
  
  // Read sensor data
  if (millis() - lastSensorRead >= sensorInterval) {
    readSensorData();
    lastSensorRead = millis();
  }
  
  // Send WebSocket data
  if (millis() - lastWebSocketSend >= wsInterval) {
    sendWebSocketData();
    lastWebSocketSend = millis();
  }
  
  delay(10); // Small delay for stability
}

void readSensorData() {
  // Read humidity and temperature
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Calculate heat index
  float hi = dht.computeHeatIndex(t, h, false); // false = Celsius
  
  // Update global variables
  temperature = t;
  humidity = h;
  heatIndex = hi;
  
  // Print to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C, Humidity: ");
  Serial.print(humidity);
  Serial.print("%, Heat Index: ");
  Serial.print(heatIndex);
  Serial.println("°C");
}

void sendWebSocketData() {
  // Create JSON object
  StaticJsonDocument<200> doc;
  doc["temperature"] = String(temperature, 1);
  doc["humidity"] = String(humidity, 1);
  doc["heatIndex"] = String(heatIndex, 1);
  doc["timestamp"] = getTimeString();
  
  // Serialize JSON to string
  String jsonString;
  serializeJson(doc, jsonString);
  
  // Send to all connected WebSocket clients
  webSocket.broadcastTXT(jsonString);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("WebSocket [%u] Disconnected!\n", num);
      break;
      
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("WebSocket [%u] Connected from %d.%d.%d.%d\n", 
                    num, ip[0], ip[1], ip[2], ip[3]);
      
      // Send current data to newly connected client
      sendWebSocketData();
      break;
    }
    
    case WStype_TEXT:
      Serial.printf("WebSocket [%u] Received: %s\n", num, payload);
      
      // Handle commands from client
      String message = String((char*)payload);
      if (message == "reset") {
        Serial.println("Reset command received");
        // You can add reset functionality here
      } else if (message == "status") {
        sendWebSocketData();
      }
      break;
      
    default:
      break;
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html>";
  html += "<html><head><title>DHT11 ESP8266 Server</title></head>";
  html += "<body style='font-family: Arial, sans-serif; margin: 40px;'>";
  html += "<h1>🌡️ DHT11 ESP8266 WebSocket Server</h1>";
  html += "<h2>Current Readings:</h2>";
  html += "<div style='background: #f0f0f0; padding: 20px; border-radius: 10px;'>";
  html += "<p><strong>Temperature:</strong> " + String(temperature, 1) + " °C</p>";
  html += "<p><strong>Humidity:</strong> " + String(humidity, 1) + " %</p>";
  html += "<p><strong>Heat Index:</strong> " + String(heatIndex, 1) + " °C</p>";
  html += "<p><strong>WebSocket URL:</strong> ws://" + WiFi.localIP().toString() + ":81/</p>";
  html += "</div>";
  html += "<h2>Instructions:</h2>";
  html += "<ol>";
  html += "<li>Open the HTML dashboard</li>";
  html += "<li>Update the WebSocket URL in the JavaScript code</li>";
  html += "<li>Replace '192.168.1.100' with: " + WiFi.localIP().toString() + "</li>";
  html += "</ol>";
  html += "<p><a href='/status'>View JSON Status</a></p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleStatus() {
  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;
  doc["heatIndex"] = heatIndex;
  doc["timestamp"] = getTimeString();
  doc["freeHeap"] = ESP.getFreeHeap();
  doc["uptime"] = millis();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  server.send(200, "application/json", jsonString);
}

String getTimeString() {
  unsigned long currentTime = millis();
  unsigned long seconds = currentTime / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  
  seconds = seconds % 60;
  minutes = minutes % 60;
  hours = hours % 24;
  
  String timeStr = "";
  if (hours < 10) timeStr += "0";
  timeStr += String(hours) + ":";
  if (minutes < 10) timeStr += "0";
  timeStr += String(minutes) + ":";
  if (seconds < 10) timeStr += "0";
  timeStr += String(seconds);
  
  return timeStr;
}

// Additional utility functions
void printWiFiStatus() {
  Serial.println("=== WiFi Status ===");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.println("==================");
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("WiFi reconnected!");
      printWiFiStatus();
    } else {
      Serial.println();
      Serial.println("Failed to reconnect to WiFi");
    }
  }
}
