/*
 * AetherHome ESP8266 MQTT Controller
 * ===================================
 * PROTOTYPE VERSION with PWM Speed Control
 * 
 * Controls: 
 *   - Servo Motor (D4/GPIO2) → Door simulation
 *   - LED (D6/GPIO12) → Light simulation
 *   - DC Motor (D7/GPIO13) → Fan with PWM speed control
 * 
 * Broker: HiveMQ Cloud (TLS/SSL)
 * 
 * WIRING SUMMARY:
 * ---------------
 * SERVO MOTOR (SG90):
 *   Orange → D4 (GPIO2) Signal
 *   Red    → 5V external
 *   Brown  → GND (common)
 * 
 * LED (Light):
 *   LED(+) → D6 (GPIO12)
 *   LED(-) → 220Ω resistor → GND
 * 
 * DC MOTOR (Fan) via BC547 Transistor:
 *   D7 (GPIO13) → 1kΩ resistor → BC547 Base
 *   BC547 Emitter → GND
 *   BC547 Collector → DC Motor (-)
 *   DC Motor (+) → 5V external
 *   1N4007 Diode: Cathode to +5V, Anode to Collector
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Servo.h>

// ============== CONFIGURATION ==============

// WiFi Credentials
const char* WIFI_SSID = "IQOO Neo 10R";
const char* WIFI_PASS = "Ron@1234";

// HiveMQ Cloud Credentials
const char* MQTT_HOST = "af3ea6f8e14447d8a93bae9f7b761b26.s1.eu.hivemq.cloud";
const int   MQTT_PORT = 8883;
const char* MQTT_USER = "ronik25";
const char* MQTT_PASS = "#Ronik25hive";
const char* CLIENT_ID = "aether_esp8266";

// MQTT Topics
const char* TOPIC_DOOR      = "home/livingroom/door/set";
const char* TOPIC_LIGHT     = "home/livingroom/light/set";
const char* TOPIC_FAN_POWER = "home/livingroom/fan/set";
const char* TOPIC_FAN_SPEED = "home/livingroom/fan/speed/set";
const char* TOPIC_STATUS    = "home/livingroom/status";

// ============== PIN CONFIGURATION ==============

#define SERVO_PIN  D4   // GPIO2  - Servo Motor (Door)
#define LIGHT_PIN  D6   // GPIO12 - LED (Light)
#define FAN_PIN    D7   // GPIO13 - DC Motor via BC547 (Fan) - PWM Capable

// Servo Positions
#define DOOR_OPEN    180
#define DOOR_CLOSED  0

// ============== OBJECTS ==============

WiFiClientSecure espClient;
PubSubClient mqtt(espClient);
Servo doorServo;

// State variables
bool lightState = false;
bool fanState = false;
int fanSpeed = 0;  // 0-255 PWM value

// ============== SETUP ==============

void setup() {
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n");
  Serial.println("====================================");
  Serial.println("    AetherHome Controller");
  Serial.println("    PWM SPEED CONTROL ENABLED");
  Serial.println("====================================");

  // Initialize Servo (Door)
  doorServo.attach(SERVO_PIN);
  doorServo.write(DOOR_CLOSED);
  Serial.println("[INIT] Servo initialized (Door CLOSED)");

  // Initialize LED (Light)
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  Serial.println("[INIT] LED initialized (Light OFF)");

  // Initialize DC Motor pin (Fan) - PWM capable
  pinMode(FAN_PIN, OUTPUT);
  analogWrite(FAN_PIN, 0);
  Serial.println("[INIT] DC Motor PWM initialized (Fan OFF)");

  // Connect WiFi
  setupWiFi();

  // Configure MQTT
  espClient.setInsecure();
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512);

  // Initial connection
  connectMQTT();
}

// ============== MAIN LOOP ==============

void loop() {
  if (!mqtt.connected()) {
    connectMQTT();
  }
  mqtt.loop();
}

// ============== WIFI SETUP ==============

void setupWiFi() {
  Serial.println();
  Serial.print("[WIFI] Connecting to: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("[WIFI] Connected!");
    Serial.print("[WIFI] IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WIFI] Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println();
    Serial.println("[WIFI] Failed! Restarting...");
    ESP.restart();
  }
}

// ============== MQTT CONNECTION ==============

void connectMQTT() {
  int retries = 0;
  
  while (!mqtt.connected() && retries < 5) {
    Serial.print("[MQTT] Connecting...");

    if (mqtt.connect(CLIENT_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println(" Connected!");

      mqtt.subscribe(TOPIC_DOOR);
      mqtt.subscribe(TOPIC_LIGHT);
      mqtt.subscribe(TOPIC_FAN_POWER);
      mqtt.subscribe(TOPIC_FAN_SPEED);

      Serial.println("[MQTT] Subscribed:");
      Serial.println("  -> " + String(TOPIC_DOOR));
      Serial.println("  -> " + String(TOPIC_LIGHT));
      Serial.println("  -> " + String(TOPIC_FAN_POWER));
      Serial.println("  -> " + String(TOPIC_FAN_SPEED));

      mqtt.publish(TOPIC_STATUS, "online");

    } else {
      Serial.print(" Failed: ");
      Serial.println(mqtt.state());
      retries++;
      delay(5000);
    }
  }
  
  if (!mqtt.connected()) {
    Serial.println("[MQTT] Restarting...");
    ESP.restart();
  }
}

// ============== MQTT CALLBACK ==============

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  Serial.print("[RECV] ");
  Serial.print(topic);
  Serial.print(" -> ");
  Serial.println(message);

  String topicStr = String(topic);
  
  if (topicStr == TOPIC_DOOR) {
    handleDoor(message);
  }
  else if (topicStr == TOPIC_LIGHT) {
    handleLight(message);
  }
  else if (topicStr == TOPIC_FAN_POWER) {
    handleFanPower(message);
  }
  else if (topicStr == TOPIC_FAN_SPEED) {
    handleFanSpeed(message);
  }
}

// ============== DEVICE HANDLERS ==============

void handleDoor(String cmd) {
  cmd.toUpperCase();
  
  if (cmd == "OPEN") {
    Serial.print("[DOOR] Opening... ");
    for (int pos = doorServo.read(); pos <= DOOR_OPEN; pos += 2) {
      doorServo.write(pos);
      delay(15);
    }
    doorServo.write(DOOR_OPEN);
    Serial.println("OPENED");
  } 
  else if (cmd == "CLOSE") {
    Serial.print("[DOOR] Closing... ");
    for (int pos = doorServo.read(); pos >= DOOR_CLOSED; pos -= 2) {
      doorServo.write(pos);
      delay(15);
    }
    doorServo.write(DOOR_CLOSED);
    Serial.println("CLOSED");
  }
}

void handleLight(String cmd) {
  cmd.toUpperCase();
  
  if (cmd == "ON") {
    digitalWrite(LIGHT_PIN, HIGH);
    lightState = true;
    Serial.println("[LIGHT] ON");
  } 
  else if (cmd == "OFF") {
    digitalWrite(LIGHT_PIN, LOW);
    lightState = false;
    Serial.println("[LIGHT] OFF");
  }
}

void handleFanPower(String cmd) {
  cmd.toUpperCase();
  
  if (cmd == "ON") {
    fanState = true;
    // Apply current speed or default to medium speed if 0
    if (fanSpeed == 0) {
      fanSpeed = 128; // 50% speed
    }
    analogWrite(FAN_PIN, fanSpeed);
    Serial.print("[FAN] Motor ON - Speed: ");
    Serial.print(map(fanSpeed, 0, 255, 0, 100));
    Serial.println("%");
  } 
  else if (cmd == "OFF") {
    fanState = false;
    analogWrite(FAN_PIN, 0);
    Serial.println("[FAN] Motor OFF");
  }
}

void handleFanSpeed(String cmd) {
  int speed = cmd.toInt();
  
  // Constrain to valid PWM range
  speed = constrain(speed, 0, 255);
  fanSpeed = speed;
  
  // Only apply if fan is ON
  if (fanState) {
    analogWrite(FAN_PIN, fanSpeed);
    Serial.print("[FAN] Speed changed: ");
    Serial.print(map(fanSpeed, 0, 255, 0, 100));
    Serial.println("%");
  } else {
    Serial.print("[FAN] Speed set to ");
    Serial.print(map(fanSpeed, 0, 255, 0, 100));
    Serial.println("% (will apply when turned ON)");
  }
  
  // Auto-off if speed set to 0
  if (speed == 0 && fanState) {
    fanState = false;
    analogWrite(FAN_PIN, 0);
    Serial.println("[FAN] Auto-OFF (speed = 0)");
  }
}

// ============== UTILITY ==============

void publishStatus() {
  if (mqtt.connected()) {
    String status = "{\"light\":";
    status += lightState ? "true" : "false";
    status += ",\"fan\":";
    status += fanState ? "true" : "false";
    status += ",\"fanSpeed\":";
    status += fanSpeed;
    status += ",\"fanSpeedPercent\":";
    status += map(fanSpeed, 0, 255, 0, 100);
    status += "}";
    mqtt.publish(TOPIC_STATUS, status.c_str());
    Serial.println("[STATUS] Published");
  }
}