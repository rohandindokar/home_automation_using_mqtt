/*******************************************************
 * AetherHome ESP8266 MQTT Controller - ON/OFF Motor
 * Using L293D Motor Driver Module (M1 M2 IN1 IN2 EN1)
 *******************************************************/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Servo.h>

/* =========================
   == Configuration ==
   ========================= */
// WiFi Configuration
const char* WIFI_SSID = "your_ssid";
const char* WIFI_PASS = "your_password";

// MQTT Configuration (TLS)
const char* MQTT_HOST = "mqtt.example.com";
const int   MQTT_PORT = 8883;
const char* MQTT_USER = "mqtt_user";
const char* MQTT_PASS = "mqtt_password";
const char* CLIENT_ID  = "AetherHome_ESP8266_001";

// Topics
const char* TOPIC_DOOR      = "home/livingroom/door/set";
const char* TOPIC_LIGHT     = "home/livingroom/light/set";
const char* TOPIC_FAN_POWER = "home/livingroom/fan/set";
const char* TOPIC_STATUS    = "home/livingroom/status";

/* =========================
   == Pin Definitions ==
   ========================= */
// Note: D# names allowed on ESP8266 (NodeMCU/WeMos)
#define SERVO_PIN   D4
#define LIGHT_PIN   D6

// L293D Motor Pins (Revised)
#define MOTOR_EN1   D7     // Enable pin (EN1)
#define MOTOR_IN1   D5     // Input 1 (IN1)
#define MOTOR_IN2   D8     // Input 2 (IN2)

// Servo positions
#define DOOR_OPEN     180
#define DOOR_CLOSED   0

/* =========================
   == Globals ==
   ========================= */
WiFiClientSecure espClient;
PubSubClient mqtt(espClient);
Servo doorServo;

bool lightState = false;
bool fanState   = false;

// Non-blocking reconnect timing
unsigned long lastMqttReconnectAttempt = 0;
const unsigned long MQTT_RECONNECT_INTERVAL = 5000; // ms

/* =========================
   == Function prototypes ==
   ========================= */
void setupWiFi();
void connectMQTT_ifNeeded();
bool connectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleDoor(const String &cmd);
void handleLight(const String &cmd);
void handleFan(const String &cmd);
void publishStatus(const char* status);

/* =========================
   == Setup ==
   ========================= */
void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println("AetherHome ESP8266 Starting...");

  // Servo
  doorServo.attach(SERVO_PIN);
  doorServo.write(DOOR_CLOSED);

  // Light pin
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);

  // Motor (L293D) pins
  pinMode(MOTOR_EN1, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  // Ensure motor is OFF initially
  digitalWrite(MOTOR_EN1, LOW);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  // WiFi + MQTT setup
  setupWiFi();
  espClient.setInsecure(); // NOTE: Insecure TLS; replace with proper cert handling for production
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(mqttCallback);

  // Try initial MQTT connect (non-blocking will manage retries)
  connectMQTT_ifNeeded();
}

/* =========================
   == Main Loop ==
   ========================= */
void loop() {
  // Keep MQTT connection alive and process incoming messages
  connectMQTT_ifNeeded();
  if (mqtt.connected()) {
    mqtt.loop();
  }

  // ... add other periodic tasks here (e.g., status heartbeat)
}

/* =========================
   == WiFi & MQTT Helpers ==
   ========================= */
void setupWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  const unsigned long WIFI_TIMEOUT = 15000; // 15s timeout
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    if (millis() - start > WIFI_TIMEOUT) {
      Serial.println("\nWiFi connection timeout - retrying...");
      start = millis();
    }
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT_ifNeeded() {
  if (mqtt.connected()) return;

  if (millis() - lastMqttReconnectAttempt < MQTT_RECONNECT_INTERVAL) return;
  lastMqttReconnectAttempt = millis();

  Serial.println("Attempting MQTT connection...");
  if (connectMQTT()) {
    Serial.println("MQTT connected.");
  } else {
    Serial.println("MQTT connect failed - will retry.");
  }
}

bool connectMQTT() {
  if (mqtt.connect(CLIENT_ID, MQTT_USER, MQTT_PASS)) {
    // Subscribe to topics
    mqtt.subscribe(TOPIC_DOOR);
    mqtt.subscribe(TOPIC_LIGHT);
    mqtt.subscribe(TOPIC_FAN_POWER);

    // Publish online status
    mqtt.publish(TOPIC_STATUS, "online", true);
    return true;
  }
  return false;
}

/* =========================
   == MQTT Callback ==
   ========================= */
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  message.reserve(length + 1);
  for (unsigned int i = 0; i < length; ++i) message += (char)payload[i];
  message.trim();

  Serial.print("MQTT msg: [");
  Serial.print(topic);
  Serial.print("] -> ");
  Serial.println(message);

  String t = String(topic);
  if (t == TOPIC_DOOR)      handleDoor(message);
  else if (t == TOPIC_LIGHT) handleLight(message);
  else if (t == TOPIC_FAN_POWER) handleFan(message);
}

/* =========================
   == Device Handlers ==
   ========================= */
void handleDoor(const String &cmdRaw) {
  String cmd = cmdRaw;
  cmd.toUpperCase();
  if (cmd == "OPEN") {
    doorServo.write(DOOR_OPEN);
    publishStatus("door_open");
    Serial.println("Door: OPEN");
  } else if (cmd == "CLOSE") {
    doorServo.write(DOOR_CLOSED);
    publishStatus("door_closed");
    Serial.println("Door: CLOSE");
  } else {
    Serial.println("Door: Unknown command");
  }
}

void handleLight(const String &cmdRaw) {
  String cmd = cmdRaw;
  cmd.toUpperCase();
  if (cmd == "ON") {
    digitalWrite(LIGHT_PIN, HIGH);
    lightState = true;
    publishStatus("light_on");
    Serial.println("Light: ON");
  } else if (cmd == "OFF") {
    digitalWrite(LIGHT_PIN, LOW);
    lightState = false;
    publishStatus("light_off");
    Serial.println("Light: OFF");
  } else {
    Serial.println("Light: Unknown command");
  }
}

void handleFan(const String &cmdRaw) {
  String cmd = cmdRaw;
  cmd.toUpperCase();
  if (cmd == "ON") {
    // L293D: set direction IN1 HIGH, IN2 LOW, enable HIGH
    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
    digitalWrite(MOTOR_EN1, HIGH);
    fanState = true;
    publishStatus("fan_on");
    Serial.println("Fan: ON");
  } else if (cmd == "OFF") {
    // stop motor
    digitalWrite(MOTOR_EN1, LOW);
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    fanState = false;
    publishStatus("fan_off");
    Serial.println("Fan: OFF");
  } else {
    Serial.println("Fan: Unknown command");
  }
}

/* =========================
   == Utility ==
   ========================= */
void publishStatus(const char* status) {
  if (mqtt.connected()) {
    mqtt.publish(TOPIC_STATUS, status, true);
  }
}
