# 🏠 Smart Home Automation System (ESP32 + Flask Dashboard)

A complete **IoT-based Smart Home System** built using **ESP32**, multiple sensors, **RFID-based access control**, and a **Flask web dashboard**.
The system supports **Over-The-Air (OTA)** operation and displays **real-time sensor data** on a web dashboard.

---

## 📌 Project Overview

This project demonstrates a smart home automation system where different sensors monitor environmental conditions and control appliances automatically.
An **ESP32** collects sensor data and sends it to a **Flask-based dashboard** over Wi-Fi using HTTP and JSON.

The system also implements **RFID-based authentication** using tokenization for secure access control.

---

## ✨ Features

* 🔐 RFID-based access control (Token stored in array)
* 🌡️ Temperature monitoring using DHT sensor
* 🔥 Gas leakage detection with buzzer alert
* 💡 Automatic lighting using LDR
* 🌧️ Rain detection using HW-028 rain sensor
* 📏 Distance measurement using ultrasonic sensor
* 🌀 Fan control using BO motor + L298N motor driver
* 🌐 Real-time web dashboard using Flask
* 📡 Wireless data transfer (Wi-Fi)
* 🔄 OTA-ready architecture (no wired monitoring required)

---

## 🧰 Hardware Components Used

* ESP32 Development Board
* MFRC522 RFID Reader + RFID Tags
* DHT11 / DHT22 Temperature Sensor
* Gas Sensor (MQ series)
* LDR (Light Dependent Resistor)
* Rain Sensor Module (HW-028)
* Ultrasonic Sensor (HC-SR04)
* BO Motor (Fan simulation)
* L298N Motor Driver
* Buzzer
* LEDs
* Resistors, jumper wires, breadboard

---

## 💻 Software & Technologies

* **Arduino IDE** (ESP32 programming)
* **Flask (Python)** – Web dashboard
* **HTML + CSS** – Dashboard UI
* **HTTP + JSON** – Data communication
* **GitHub** – Version control

---

## 🧠 System Working Logic

* RFID tag is scanned → UID checked against **authorized token array**
* If authorized → access granted
* Gas detected → buzzer turns ON
* Temperature > 30°C → fan turns ON
* Low light → LED turns ON automatically
* Rain sensor detects water → rain status updated
* Ultrasonic sensor measures distance
* All sensor data is sent to Flask server
* Dashboard displays real-time status

---

## 📂 Project Structure

```
Smart-Home-System/
│
├── Arduino_Code/
│   └── smart_home_esp32.ino
│
├── Dashboard/
│   ├── app.py
│   ├── data.json
│   ├── templates/
│   │   └── index.html
│   └── static/
│       └── style.css
│
├── assets/
│   └── dashboard.png
│
└── README.md
```

---

## 🚀 How to Run the Project

### 1️⃣ ESP32 Setup

* Open Arduino IDE
* Select **ESP32 Dev Module**
* Install required libraries:

  * MFRC522
  * DHT sensor library (Adafruit)
* Upload the Arduino code to ESP32

### 2️⃣ Flask Dashboard Setup

```bash
pip install flask
python app.py
```

Open browser:

```
http://<YOUR_PC_IP>:5000
```

---

## 📊 Dashboard Screenshot

> Add your dashboard screenshot inside the `assets/` folder and name it `dashboard.png`

```md
![Smart Home Dashboard Screenshot 1](assets/dashboard(1).jpeg)
![Smart Home Dashboard Sceeenshot 2](assets/dashboard(2).jpeg)
![System image](assets/System.jpeg.png)
```

📌 This section visually shows:

* RFID access status
* Gas status
* Temperature
* Fan status
* LDR & LED status
* Ultrasonic distance
* Rain status

---

## 🔐 RFID Tokenization

* RFID UIDs are stored in an **array**
* Multiple authorized users are supported
* UID is compared with stored tokens for authentication

Example:

```cpp
String authorizedTokens[] = { "32ADCF55", "A1B2C3D4" };
```

---

## 🎓 Use Cases

* Smart homes
* Home security systems
* IoT learning projects
* College mini / major projects
* Automation demonstrations

---

## 🧪 Results

* Real-time sensor monitoring achieved
* Secure RFID-based access implemented
* Dashboard updates successfully over Wi-Fi
* System runs wirelessly without physical monitoring

---

## 🗣️ Viva / Presentation Line

> “The ESP32 collects sensor data and securely transmits it to a Flask-based web dashboard using HTTP and JSON, enabling real-time monitoring and smart home automation.”

---


## 📜 License

This project is for **educational purposes** only.


