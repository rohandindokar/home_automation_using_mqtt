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
- **🌀 Fan Control** - DC motor with ON/OFF control via L293D motor driver

### Technical Features
- 🔐 TLS/SSL encrypted communication
- 🔄 Auto-reconnect on connection loss
- 📡 MQTT protocol for reliable messaging
- ⚡ Low latency control (<100ms)
- 🛡️ L293D motor driver with built-in protection
- 🎛️ Simple ON/OFF motor control

## 🎬 Demo

![Dashboard Preview](https://github.com/rohandindokar/home_automation_using_mqtt/blob/main/Screenshot%202025-11-23%20144427.png)

### Dashboard Controls
- **Door**: Open/Close buttons with visual feedback
- **Light**: Toggle switch with glowing effect when ON
- **Fan**: Toggle switch for ON/OFF control

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
                                                               (via L293D)
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
| SG90 Servo Motor | 0-180° rotation | 1 | Door simulation |
| LED (5mm) | Any color | 1 | Light simulation |
| DC Motor | 3-6V, <600mA | 1 | Fan simulation |
| L293D Motor Driver | Module/IC | 1 | Motor control |
| 220Ω Resistor | 1/4W | 1 | LED current limiting |

### Additional Items
- Breadboard (830 points)
- Jumper wires (M-M, ~12 pieces)
- 5V External Power Supply (for servo and motor)
- USB cable (Micro USB for programming)

### Total Estimated Cost
- **~$12-15 USD** for all components

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
| **L293D** | ENS (Enable) | D7 (GPIO13) | Motor enable |
| | MC5 (IN1) | D5 (GPIO14) | Direction control 1 |
| | MC6 (IN2) | D8 (GPIO15) | Direction control 2 |
| | Motor1 | Motor Wire A | Motor terminal |
| | Motor2 | Motor Wire B | Motor terminal |
| | Motor Power | External 5V | Motor supply |
| | Logic Power | NodeMCU 5V | Logic supply |
| | GND | Common GND | Ground |

### Circuit Schematic

```
                    +5V External
                         │
        ┌────────────────┼────────────────┐
        │                │                │
   Servo Red        L293D Motor Power     │
        │                │                │
   Servo Orange    ┌─────┴─────┐          │
        │          │   L293D   │          │
       D4          │   MOTOR   │     DC Motor
        │          │  DRIVER   │          │
        │          │           │     ┌────┴────┐
   Servo Brown     │ Motor1────┼─────┤ Wire A  │
        │          │ Motor2────┼─────┤ Wire B  │
       GND         │           │     └─────────┘
        │          │  ENS ─────┤──── D7 (GPIO13)
        │          │  MC5 ─────┤──── D5 (GPIO14)
   LED Anode       │  MC6 ─────┤──── D8 (GPIO15)
        │          │           │
       D6          │  GND ─────┤──── GND (Common)
        │          │           │
   LED Cathode     │ Logic Power──── NodeMCU 5V
        │          └───────────┘
      220Ω
        │
       GND ────────────┴──────────────────────────┘
```

### L293D Module Pinout
```
L293D Pin Layout:
─────────────────
ENS   → D7 (Enable/Speed)
MC5   → D5 (Direction bit 1)
MC6   → D8 (Direction bit 2)
Motor1 → DC Motor Terminal 1
Motor2 → DC Motor Terminal 2
Vin   → 5V External
GND   → Common Ground
```

### Pin Summary
```
ESP8266 Pin | GPIO  | Device
------------|-------|------------------------
D4          | GPIO2 | Servo Motor (Door)
D5          | GPIO14| L293D MC5 (Motor Control)
D6          | GPIO12| LED (Light)
D7          | GPIO13| L293D ENS (Motor Enable)
D8          | GPIO15| L293D MC6 (Motor Control)
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

3. **Connect L293D Motor Driver**
   ```
   D7 (GPIO13) → L293D ENS
   D5 (GPIO14) → L293D MC5
   D8 (GPIO15) → L293D MC6
   Motor Wire A → L293D Motor1
   Motor Wire B → L293D Motor2
   5V External → L293D Motor Power
   NodeMCU 5V → L293D Logic Power
   GND → Common Ground (all connected together)
   ```

4. **Power Connections**
   - Connect all GND points together (CRITICAL!)
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

1. Open `home_automation.ino` in Arduino IDE

2. Update credentials in the code:
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

1. Open `index.html` in your web browser

2. Click the ⚙️ settings icon

3. Enter your HiveMQ credentials:
   - Broker Host: `your-cluster.s1.eu.hivemq.cloud`
   - Port: `8884`
   - Username: `your_username`
   - Password: `your_password`

4. Click **Connect**

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
#define SERVO_PIN   D4   // Servo motor
#define LIGHT_PIN   D6   // LED
#define MOTOR_ENS   D7   // L293D Enable
#define MOTOR_IN1   D5   // L293D IN1
#define MOTOR_IN2   D8   // L293D IN2
```

### Servo Position Calibration
Adjust door positions:
```cpp
#define DOOR_OPEN    180  // Angle when door is open
#define DOOR_CLOSED  0    // Angle when door is closed
```

## 📖 Usage

### Starting the System

1. **Power on ESP8266**
   - Connect USB cable
   - ESP8266 will connect to WiFi automatically
   - Check Serial Monitor for connection status

2. **Open Dashboard**
   - Open `index.html` in browser
   - Click settings icon (⚙️)
   - Enter HiveMQ credentials
   - Click **Connect**
   - Status indicator should turn green

### Controlling Devices

#### Door Control
- Click **Open** button to open door (servo rotates to 180°)
- Click **Close** button to close door (servo rotates to 0°)
- Status text updates in real-time

#### Light Control
- Toggle switch to turn light ON/OFF
- LED glows when ON
- Label shows current state

#### Fan Control
- Toggle switch to turn motor ON/OFF
- Motor runs at full speed when ON
- Motor stops completely when OFF

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
| `home/livingroom/status` | ESP8266 → Dashboard | Status string | Device status |

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

**Turn On Fan:**
```
Topic: home/livingroom/fan/set
Payload: ON
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

#### Motor Not Running
- ✅ Check all L293D connections (ENS, MC5, MC6)
- ✅ Verify common ground connection
- ✅ Check external 5V power supply
- ✅ Try swapping Motor1 and Motor2 if runs backward
- ✅ Ensure motor current is <600mA

#### Motor Runs Backward
- **Solution**: Physically swap Motor1 and Motor2 wires
- No code changes needed

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

### L293D Motor Driver Issues

#### Motor Doesn't Turn Off
- ✅ Check ENS is connected to D7
- ✅ Verify MC5 and MC6 connections
- ✅ Check common ground connection

#### Motor Runs Continuously
- ✅ Ensure MC5 and MC6 are NOT tied to power/ground
- ✅ They must be controlled by ESP8266 pins D5 and D8

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2025 Ronik

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

Need help? Contact:

- 📧 Email: rohandindokar25@gmail.com
- 💬 GitHub Issues: [Create an issue](https://github.com/rohandindokar/home_automation_using_mqtt/issues)

## 📊 Project Status

- ✅ Core functionality complete
- ✅ Documentation complete
- ✅ Tested with L293D motor driver
- 🔄 Active maintenance
- 🎯 Production ready

## 📝 Changelog

### v1.0.0 (Current)
- Initial release
- Door, Light, and Fan control
- Simple ON/OFF motor control via L293D
- Web-based dashboard
- MQTT over TLS/SSL
- Comprehensive documentation

---

**Made with ❤️ by Rohan & Team**

⭐ Star this repo if you find it helpful!

📢 Share with the IoT community!

🔔 Watch for updates!
