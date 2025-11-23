# 🏠 AetherHome - IoT Smart Home Control System

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![ESP8266](https://img.shields.io/badge/Platform-ESP8266-green.svg)](https://github.com/esp8266/Arduino)
[![MQTT](https://img.shields.io/badge/Protocol-MQTT-orange.svg)](https://mqtt.org/)

A modern, web-based IoT smart home control system built with ESP8266 and MQTT protocol. Control your home devices remotely through an elegant dashboard interface with real-time feedback.

![AetherHome Dashboard](https://img.shields.io/badge/Status-Production%20Ready-brightgreen)

## 📋 Table of Contents

- [Features](#-features)
- [Demo](#-demo)
- [System Architecture](#-system-architecture)
- [Hardware Requirements](#-hardware-requirements)
- [Software Requirements](#-software-requirements)
- [Circuit Diagram](#-circuit-diagram)
- [Installation](#-installation)
- [Configuration](#-configuration)
- [Usage](#-usage)
- [MQTT Topics](#-mqtt-topics)
- [Troubleshooting](#-troubleshooting)
- [License](#-license)
- [Acknowledgments](#-acknowledgments)

## ✨ Features

### Dashboard
- 🎨 Modern, responsive web interface with AetherHome branding
- 🔒 Secure WebSocket connection (WSS) to MQTT broker
- 📊 Real-time device status updates
- 🌙 Dark mode design with glassmorphism effects
- 📱 Mobile-friendly responsive layout
- 🔔 Activity logging and console output
- 🎯 One-click device control

### Device Control
- **🚪 Door Control** - Servo motor with open/close functionality
- **💡 Light Control** - LED with simple ON/OFF toggle
- **🌀 Fan Control** - DC motor with PWM speed control (0-100%)

### Technical Features
- 🔐 TLS/SSL encrypted communication
- 🔄 Auto-reconnect on connection loss
- 📡 MQTT protocol for reliable messaging
- ⚡ Low latency control (<100ms)
- 🛡️ Protected motor circuit with flyback diode
- 📈 PWM-based speed control for smooth operation

## 🎬 Demo

![Dashboard Preview](https://github.com/rohandindokar/home_automation_using_mqtt/blob/main/Screenshot%202025-11-23%20144427.png)

### Dashboard Controls
- **Door**: Open/Close buttons with visual feedback
- **Light**: Toggle switch with glowing effect when ON
- **Fan**: Toggle with speed slider (PWM control)

## 🏗️ System Architecture

```
┌─────────────┐         ┌─────────────┐         ┌─────────────┐
│   Browser   │  WSS    │   HiveMQ    │   TLS   │  ESP8266    │
│  Dashboard  │◄───────►│   Cloud     │◄───────►│ Controller  │
│             │  8884   │   Broker    │  8883   │             │
└─────────────┘         └─────────────┘         └──────┬──────┘
                                                        │
                                         ┌──────────────┼──────────────┐
                                         │              │              │
                                    ┌────▼───┐    ┌────▼───┐    ┌────▼───┐
                                    │ Servo  │    │  LED   │    │ DC     │
                                    │ Motor  │    │        │    │ Motor  │
                                    └────────┘    └────────┘    └────────┘
                                      Door          Light         Fan
```

### Communication Flow
1. User interacts with dashboard
2. Dashboard publishes MQTT message to HiveMQ Cloud
3. ESP8266 receives message via subscription
4. ESP8266 controls hardware based on command
5. Feedback displayed in dashboard console

## 🔧 Hardware Requirements

### Core Components
| Component | Specification | Quantity | Purpose |
|-----------|--------------|----------|---------|
| ESP8266 NodeMCU | ESP-12E | 1 | Main controller |
| SG90 Servo Motor | 0-90° rotation | 1 | Door simulation |
| LED (5mm) | Any color | 1 | Light simulation |
| DC Motor | 3-6V, <100mA | 1 | Fan simulation |
| BC547 Transistor | NPN, TO-92 | 1 | Motor driver |
| 1N4007 Diode | 1A, 1000V | 1 | Flyback protection |
| 220Ω Resistor | 1/4W | 1 | LED current limiting |
| 1kΩ Resistor | 1/4W | 1 | Transistor base |

### Additional Items
- Breadboard (830 points)
- Jumper wires (M-M, ~15 pieces)
- 5V External Power Supply (for servo and motor)
- USB cable (Micro USB for programming)

### Total Estimated Cost
- **~$15-20 USD** for all components

## 💻 Software Requirements

### Development Tools
- [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.19 or later)
- ESP8266 Board Package (v3.0.0 or later)
- Web browser (Chrome, Firefox, Safari, or Edge)

### Arduino Libraries
Install via Arduino Library Manager:
```
1. ESP8266WiFi (included with ESP8266 board package)
2. PubSubClient by Nick O'Leary (v2.8.0)
3. WiFiClientSecure (included with ESP8266 board package)
4. Servo (ESP8266 version)
```

### MQTT Broker
- [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/) (Free tier)
  - Create account and cluster
  - Note down cluster URL, port, username, and password

## 📐 Circuit Diagram

### Wiring Table

| Device | Component Pin | ESP8266 Pin | Connection Details |
|--------|--------------|-------------|-------------------|
| **Servo** | Orange (Signal) | D4 (GPIO2) | Direct connection |
| | Red (VCC) | External 5V | Power supply |
| | Brown (GND) | GND | Common ground |
| **LED** | Anode (+) | D6 (GPIO12) | Via ESP8266 |
| | Cathode (-) | GND | Via 220Ω resistor |
| **BC547** | Base (B) | D7 (GPIO13) | Via 1kΩ resistor |
| | Collector (C) | Motor (-) | Motor control |
| | Emitter (E) | GND | Common ground |
| **DC Motor** | Positive (+) | External 5V | Power supply |
| | Negative (-) | BC547 Collector | Via transistor |
| **1N4007** | Cathode | External 5V | Silver band to +5V |
| | Anode | BC547 Collector | Flyback protection |

### Circuit Schematic

```
                    +5V External
                         │
        ┌────────────────┼────────────────┐
        │                │                │
        │           1N4007 Cathode        │
        │           (Silver Band)         │
   Servo Red            │            Motor (+)
        │           ┌────┴────┐          │
        │           │  ━━│►━━ │          │
        │           └────┬────┘          │
        │           1N4007 Anode         │
        │                │                │
   Servo Orange    Motor (-)──────────┬──┘
        │                              │
       D4                         BC547 (C)
        │                         ┌────┴────┐
        │                         │  BC547  │
   Servo Brown                    │   NPN   │
        │                         └─┬───┬───┘
       GND                          E   B
        │                           │   │
        │                          GND  │
   LED Cathode                          │
        │                              1kΩ
      220Ω                               │
        │                               D7
   LED Anode
        │
       D6
```

### BC547 Pinout (Important!)
```
    Flat side facing you:
    ┌─────────┐
    │  BC547  │
    │  ─────  │
    └┬───┬───┬┘
     E   B   C
```

### Pin Summary
```
ESP8266 Pin | GPIO  | Device
------------|-------|--------
D4          | GPIO2 | Servo Motor (Door)
D6          | GPIO12| LED (Light)
D7          | GPIO13| DC Motor via BC547 (Fan)
GND         | -     | Common Ground
```

## 🚀 Installation

### Step 1: Hardware Setup

1. **Connect Servo Motor**
   ```
   Orange wire → D4
   Red wire → 5V external
   Brown wire → GND
   ```

2. **Connect LED**
   ```
   Anode (+) → D6
   Cathode (-) → 220Ω → GND
   ```

3. **Connect DC Motor Circuit**
   ```
   ESP8266 D7 → 1kΩ → BC547 Base (B)
   BC547 Emitter (E) → GND
   BC547 Collector (C) → Motor (-)
   Motor (+) → 5V external
   1N4007 Cathode (silver band) → 5V
   1N4007 Anode → BC547 Collector
   ```

4. **Power Connections**
   - Connect all GND points together
   - Power ESP8266 via USB
   - Power servo and motor from external 5V supply

### Step 2: Software Setup

#### A. Install Arduino IDE & ESP8266 Support

1. Download and install [Arduino IDE](https://www.arduino.cc/en/software)

2. Add ESP8266 board support:
   - Open Arduino IDE
   - Go to `File → Preferences`
   - Add this URL to "Additional Board Manager URLs":
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Go to `Tools → Board → Boards Manager`
   - Search "ESP8266" and install

3. Install required libraries:
   - Go to `Sketch → Include Library → Manage Libraries`
   - Install **PubSubClient** by Nick O'Leary

#### B. Configure HiveMQ Cloud

1. Create free account at [HiveMQ Cloud](https://www.hivemq.com/mqtt-cloud-broker/)
2. Create a new cluster
3. Go to "Access Management" tab
4. Create credentials (username and password)
5. Note down:
   - Cluster URL (e.g., `xxxxx.s1.eu.hivemq.cloud`)
   - Port: `8883` (for ESP8266) and `8884` (for dashboard)
   - Username
   - Password

#### C. Upload ESP8266 Code

1. Open `aetherhome_esp8266.ino` in Arduino IDE

2. Update credentials (lines 31-45):
   ```cpp
   const char* WIFI_SSID = "YOUR_WIFI_SSID";
   const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
   
   const char* MQTT_HOST = "your-cluster.s1.eu.hivemq.cloud";
   const char* MQTT_USER = "your_username";
   const char* MQTT_PASS = "your_password";
   ```

3. Select board and port:
   - `Tools → Board → ESP8266 Boards → NodeMCU 1.0 (ESP-12E Module)`
   - `Tools → Port → [Select your COM port]`

4. Upload:
   - Click `Upload` button
   - Wait for "Done uploading"
   - Open Serial Monitor (`Tools → Serial Monitor`)
   - Set baud rate to `115200`
   - You should see connection logs

#### D. Setup Dashboard

1. Open `dashboard.html` in a text editor

2. The HiveMQ credentials are entered via the web interface (no code changes needed)

3. Open `dashboard.html` in your web browser

4. Click the ⚙️ settings icon

5. Enter your HiveMQ credentials:
   - Broker Host: `your-cluster.s1.eu.hivemq.cloud`
   - Port: `8884`
   - Username: `your_username`
   - Password: `your_password`

6. Click **Connect**

## ⚙️ Configuration

### WiFi Configuration
Edit in ESP8266 code:
```cpp
const char* WIFI_SSID = "YourWiFiName";
const char* WIFI_PASS = "YourWiFiPassword";
```

### MQTT Broker Configuration
Edit in ESP8266 code:
```cpp
const char* MQTT_HOST = "your-cluster.s1.eu.hivemq.cloud";
const int   MQTT_PORT = 8883;
const char* MQTT_USER = "your_username";
const char* MQTT_PASS = "your_password";
```

### Device Pin Configuration
To change pins, edit in ESP8266 code:
```cpp
#define SERVO_PIN  D4   // Change to desired pin
#define LIGHT_PIN  D6   // Change to desired pin
#define FAN_PIN    D7   // Change to desired pin
```

### Servo Position Calibration
Adjust door positions:
```cpp
#define DOOR_OPEN    90   // Angle when door is open
#define DOOR_CLOSED  0    // Angle when door is closed
```

## 📖 Usage

### Starting the System

1. **Power on ESP8266**
   - Connect USB cable
   - ESP8266 will connect to WiFi automatically
   - Check Serial Monitor for connection status

2. **Open Dashboard**
   - Open `dashboard.html` in browser
   - Click settings icon (⚙️)
   - Enter HiveMQ credentials
   - Click **Connect**
   - Status indicator should turn green

### Controlling Devices

#### Door Control
- Click **Open** button to open door (servo rotates to 90°)
- Click **Close** button to close door (servo rotates to 0°)
- Status text updates in real-time

#### Light Control
- Toggle switch to turn light ON/OFF
- LED glows when ON
- Label shows current state

#### Fan Control
1. Turn ON fan with toggle switch
2. Adjust speed slider (0-100%)
3. Fan icon spins at corresponding speed
4. Motor speed changes in real-time
5. Setting speed to 0 turns fan OFF automatically

### Monitoring

- **Console Log**: Bottom panel shows all MQTT messages
- **Serial Monitor**: Shows ESP8266 debug information
- **Status Indicator**: Shows connection status (Green = Connected)

## 📡 MQTT Topics

| Topic | Direction | Payload | Description |
|-------|-----------|---------|-------------|
| `home/livingroom/door/set` | Dashboard → ESP8266 | `OPEN` or `CLOSE` | Door control |
| `home/livingroom/light/set` | Dashboard → ESP8266 | `ON` or `OFF` | Light control |
| `home/livingroom/fan/set` | Dashboard → ESP8266 | `ON` or `OFF` | Fan power |
| `home/livingroom/fan/speed/set` | Dashboard → ESP8266 | `0-255` | Fan speed (PWM) |
| `home/livingroom/status` | ESP8266 → Dashboard | JSON | Device status |

### Example MQTT Messages

**Open Door:**
```
Topic: home/livingroom/door/set
Payload: OPEN
```

**Turn On Light:**
```
Topic: home/livingroom/light/set
Payload: ON
```

**Set Fan Speed to 75%:**
```
Topic: home/livingroom/fan/speed/set
Payload: 191
```

**Status Update:**
```json
Topic: home/livingroom/status
Payload: {
  "light": true,
  "fan": true,
  "fanSpeed": 191,
  "fanSpeedPercent": 75
}
```

## 🔍 Troubleshooting

### ESP8266 Issues

#### Cannot Connect to WiFi
- ✅ Check SSID and password spelling
- ✅ Ensure 2.4GHz WiFi (ESP8266 doesn't support 5GHz)
- ✅ Check if router MAC filtering is enabled
- ✅ Move ESP8266 closer to router

#### MQTT Connection Failed
- ✅ Verify HiveMQ credentials (username/password)
- ✅ Check cluster URL is correct
- ✅ Ensure port is `8883` (not 8884)
- ✅ Check if firewall is blocking connection

**Error Codes:**
- `-4`: Connection timeout (check internet)
- `-2`: Connection failed (check credentials)
- `4`: Bad credentials (wrong username/password)
- `5`: Unauthorized (check HiveMQ access management)

#### Device Not Responding
- ✅ Check wiring connections
- ✅ Verify pin definitions match physical connections
- ✅ Check Serial Monitor for error messages
- ✅ Ensure external 5V power is connected

### Dashboard Issues

#### Cannot Connect to Broker
- ✅ Verify port is `8884` (WebSocket port)
- ✅ Check username and password
- ✅ Try refreshing page (Ctrl+F5)
- ✅ Check browser console for errors (F12)

#### Commands Not Working
- ✅ Ensure green "Connected" status
- ✅ Check console log for published messages
- ✅ Verify ESP8266 is online (check Serial Monitor)
- ✅ Check topic names match between code and dashboard

### Hardware Issues

#### Motor Not Spinning
- ✅ Check BC547 is inserted correctly (E-B-C from left)
- ✅ Verify 1kΩ resistor is in base circuit
- ✅ Check external 5V power supply
- ✅ Try different motor (current motor may be >100mA)
- ✅ Consider upgrading to TIP120 transistor

#### Servo Jittering
- ✅ Add 100µF capacitor across servo power pins
- ✅ Use separate 5V supply (not USB power)
- ✅ Check ground connections

#### LED Not Lighting
- ✅ Check LED polarity (long leg = anode = +)
- ✅ Verify 220Ω resistor is connected
- ✅ Test LED with multimeter

#### ESP8266 Resets Randomly
- ✅ Insufficient power supply (use good quality USB cable)
- ✅ Add 100µF capacitor across ESP8266 power pins
- ✅ Ensure external 5V supply doesn't backfeed into ESP8266

### Flyback Diode Issues

#### Motor Runs But Transistor Gets Hot
- ✅ Check 1N4007 diode orientation (silver band to +5V)
- ✅ Verify diode is 1N4007 (not lower rating)
- ✅ Motor may be drawing too much current

## 🤝 Contributing

Contributions are welcome! Here's how you can help:


### Suggesting Features
- Open an issue with `[Feature Request]` tag
- Describe the feature and use case
- Explain why it would be useful


## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 AetherHome

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 Acknowledgments

### Libraries & Frameworks
- [PubSubClient](https://github.com/knolleary/pubsubclient) - MQTT client library
- [ESP8266 Arduino Core](https://github.com/esp8266/Arduino) - ESP8266 support for Arduino
- [Tailwind CSS](https://tailwindcss.com/) - Utility-first CSS framework
- [Font Awesome](https://fontawesome.com/) - Icon library
- [Paho MQTT](https://www.eclipse.org/paho/) - MQTT JavaScript client

### Services
- [HiveMQ Cloud](https://www.hivemq.com/) - MQTT broker hosting
- [Arduino](https://www.arduino.cc/) - Development platform

### Inspiration
- Modern smart home systems
- IoT community projects
- Material Design principles

## 📞 Support

Need help? Here are your options:

- 📧 Email: rohandindokar25@gmail.com

## 📊 Project Status

- ✅ Core functionality complete
- ✅ Documentation complete
- ✅ Tested on multiple devices
- 🔄 Active maintenance
- 🎯 Production ready

## 📝 Changelog

### v1.0.0 (Current)
- Initial release
- Door, Light, and Fan control
- PWM speed control for fan
- Web-based dashboard
- MQTT over TLS/SSL
- Comprehensive documentation

---

**Made with ❤️ by the Rohan& Team**

⭐ Star this repo if you find it helpful!

📢 Share with the IoT community!

🔔 Watch for updates!
