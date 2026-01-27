#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "wifi";
const char* WIFI_PASS = "12345678";

#define PCA_ADDR 0x40
#define SERVOMIN 150
#define SERVOMAX 600
#define SERVO_FREQ 50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(PCA_ADDR);
WebServer server(80);

bool paradeMode = false;

int servoAngles[16] = {
  65, 60, 90, 20, 90, 20, 90, 140,
  90, 20, 90, 90, 90, 90, 90, 90
};

const char* servoNames[16] = {
  "Head", "Neck", "R_Shoulder", "R_ARM1", "R_ARM2", "R_Hand",
  "L_Shoulder", "L_ARM1", "L_ARM2", "L_Hand",
  "R_Leg", "R_Knee", "L_Leg", "L_Knee", "Servo14", "Servo15"
};

void setServoAngle(uint8_t ch, int angle) {
  angle = constrain(angle, 0, 180);
  servoAngles[ch] = angle;
  pwm.setPWM(ch, 0, map(angle, 0, 180, SERVOMIN, SERVOMAX));
}

void moveServoSmooth(uint8_t ch, int target, int delayMs) {
  int current = servoAngles[ch];
  if (target > current) {
    for (int p = current; p <= target; p++) {
      setServoAngle(ch, p);
      delay(delayMs);
    }
  } else {
    for (int p = current; p >= target; p--) {
      setServoAngle(ch, p);
      delay(delayMs);
    }
  }
}

void resetPose() {
  paradeMode = false;
  moveServoSmooth(1, 90, 8);
  moveServoSmooth(0, 90, 8);
  moveServoSmooth(2, 90, 8);
  moveServoSmooth(3, 15, 8);
  moveServoSmooth(4, 90, 8);
  moveServoSmooth(5, 20, 8);
  moveServoSmooth(6, 90, 8);
  moveServoSmooth(7, 15, 8);
  moveServoSmooth(8, 90, 8);
  moveServoSmooth(9, 20, 8);
  moveServoSmooth(10, 90, 8);
  moveServoSmooth(12, 90, 8);
}

void performSalute() {
  moveServoSmooth(2, 80, 8);
  moveServoSmooth(3, 120, 8);
  moveServoSmooth(4, 150, 8);
  moveServoSmooth(5, 135, 8);
}

void flexBiceps() {
  moveServoSmooth(2, 70, 8);
  moveServoSmooth(6, 110, 8);

  moveServoSmooth(3, 140, 8);
  moveServoSmooth(7, 140, 8);

  moveServoSmooth(4, 150, 8);
  moveServoSmooth(8, 30, 8);
}

void waveRightHand() {
  moveServoSmooth(2, 70, 8);
  moveServoSmooth(3, 120, 8);

  for (int i = 0; i < 4; i++) {
    moveServoSmooth(5, 150, 5);
    moveServoSmooth(5, 90, 5);
  }

  resetPose();
}

void paradeStep() {

  int speed = 10;
  int swing = 35;   

  for (int i = 0; i <= swing; i++) {
    setServoAngle(2, 90 + i);   
    setServoAngle(12, 90 - i);  
    delay(speed);
  }

  delay(200);

  for (int i = swing; i >= 0; i--) {
    setServoAngle(2, 90 + i);
    setServoAngle(12, 90 - i);
    delay(speed);
  }

  delay(200);

  for (int i = 0; i <= swing; i++) {
    setServoAngle(6, 90 + i);   
    setServoAngle(10, 90 - i);  
    delay(speed);
  }

  delay(200);

  for (int i = swing; i >= 0; i--) {
    setServoAngle(6, 90 + i);
    setServoAngle(10, 90 - i);
    delay(speed);
  }

  delay(300);
}


String htmlPage() {
  String page = "<!DOCTYPE html><html><head>";
  page += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += "<style>";
  page += "body{font-family:Arial;text-align:center;}";
  page += ".servoBox{border:1px solid #ccc;padding:10px;margin:10px;display:inline-block;width:220px;}";
  page += "button{margin:3px;padding:5px 10px;}";
  page += "input[type=range]{width:180px;}";
  page += "</style></head><body>";

  page += "<h2>ESP32 PCA9685 Humanoid Controller</h2>";

  for (int i = 0; i < 16; i++) {
    page += "<div class='servoBox'><h4>";
    page += servoNames[i];
    page += " (" + String(i) + ")</h4>";
    page += "<input type='range' min='0' max='180' value='" + String(servoAngles[i]) +
            "' id='s" + String(i) +
            "' oninput='u(" + String(i) +
            ",this.value)' onchange='s(" + String(i) +
            ",this.value)'><br>";
    page += "<button onclick='c(" + String(i) + ",-1)'>-</button>";
    page += "<span id='l" + String(i) + "'>" + String(servoAngles[i]) + "</span>";
    page += "<button onclick='c(" + String(i) + ",1)'>+</button>";
    page += "</div>";
  }

  page += "<hr><h3>Actions</h3>";
  page += "<button onclick=\"fetch('/salute')\">Salute</button>";
  page += "<button onclick=\"fetch('/flex')\">Flex Biceps</button>";
  page += "<button onclick=\"fetch('/wave')\">Wave Right</button>";
  page += "<button onclick=\"fetch('/parade')\">Parade</button>";
  page += "<button onclick=\"fetch('/reset')\">Reset</button>";

  page += "<script>";
  page += "function u(c,v){document.getElementById('l'+c).innerText=v;}";
  page += "function s(c,v){fetch('/set?ch='+c+'&val='+v);}";
  page += "function c(c,d){let x=document.getElementById('s'+c);";
  page += "let v=parseInt(x.value)+d;if(v<0)v=0;if(v>180)v=180;";
  page += "x.value=v;u(c,v);s(c,v);}";
  page += "</script></body></html>";

  return page;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);

  for (int i = 0; i < 16; i++) setServoAngle(i, servoAngles[i]);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) delay(300);

  server.on("/", [](){ server.send(200, "text/html", htmlPage()); });
  server.on("/set", [](){
    setServoAngle(server.arg("ch").toInt(), server.arg("val").toInt());
    server.send(200, "text/plain", "OK");
  });
  server.on("/salute", [](){ performSalute(); server.send(200, "text/plain", "OK"); });
  server.on("/flex", [](){ flexBiceps(); server.send(200, "text/plain", "OK"); });
  server.on("/wave", [](){ waveRightHand(); server.send(200, "text/plain", "OK"); });
  server.on("/parade", [](){ paradeMode = true; server.send(200, "text/plain", "OK"); });
  server.on("/reset", [](){ resetPose(); server.send(200, "text/plain", "OK"); });

  server.begin();
}

void loop() {
  server.handleClient();
  if (paradeMode) paradeStep();
}
