#include <hd44780.h>


#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL3xxxxx"
#define BLYNK_TEMPLATE_NAME "RobotCar"
#define BLYNK_AUTH_TOKEN "authxxxx"


#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>


#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>


hd44780_I2Cexp lcd;

// WiFi
char ssid[] = "wifixxxx";
char pass[] = "pass@123";


#define LM_L1 26
#define LM_L2 27
#define RM_L1 14
#define RM_L2 12

#define LDR_L 34
#define LDR_R 35
#define TEMP_PIN 32
#define TRIG 25
#define ECHO 33

int mode = 0;   


void forward() {
  digitalWrite(LM_L1, HIGH);
  digitalWrite(LM_L2, LOW);
  digitalWrite(RM_L1, HIGH);
  digitalWrite(RM_L2, LOW);
}

void stopMotor() {
  digitalWrite(LM_L1, LOW);
  digitalWrite(LM_L2, LOW);
  digitalWrite(RM_L1, LOW);
  digitalWrite(RM_L2, LOW);
}

void left() {
  digitalWrite(LM_L1, LOW);
  digitalWrite(LM_L2, LOW);
  digitalWrite(RM_L1, HIGH);
  digitalWrite(RM_L2, LOW);
}

void right() {
  digitalWrite(LM_L1, HIGH);
  digitalWrite(LM_L2, LOW);
  digitalWrite(RM_L1, LOW);
  digitalWrite(RM_L2, LOW);
}

long distanceCM() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long d = pulseIn(ECHO, HIGH, 30000);
  return d * 0.034 / 2;
}

float readTemp() {
  int val = analogRead(TEMP_PIN);
  float voltage = val * (3.3 / 4095.0);
  return voltage * 100;
}

BLYNK_WRITE(V0) { if (!mode && param.asInt()) forward(); }
BLYNK_WRITE(V1) { if (!mode && param.asInt()) stopMotor(); }
BLYNK_WRITE(V2) { if (!mode && param.asInt()) left(); }
BLYNK_WRITE(V3) { if (!mode && param.asInt()) right(); }
BLYNK_WRITE(V4) { if (!mode && param.asInt()) stopMotor(); }
BLYNK_WRITE(V5) { mode = param.asInt(); }


void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("Booting...");

  pinMode(LM_L1, OUTPUT);
  pinMode(LM_L2, OUTPUT);
  pinMode(RM_L1, OUTPUT);
  pinMode(RM_L2, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Wire.begin(21, 22);

  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("Robot Starting");

  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run();

  float temp = readTemp();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" C   ");

  lcd.setCursor(0, 1);
  lcd.print(mode ? "Mode: AUTO   " : "Mode: MANUAL ");

  if (mode) {
    long dist = distanceCM();
    int ldrL = analogRead(LDR_L);
    int ldrR = analogRead(LDR_R);

    if (dist < 20) {
      stopMotor();
      delay(300);
      right();
      delay(400);
    } else {
      if (ldrL > ldrR + 200) left();
      else if (ldrR > ldrL + 200) right();
      else forward();
    }
  }
}
