#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>

const char* ssid = "WIFI";
const char* password = "Pass@123";
const char* serverURL = "http://(YOUR_IP):5000/update";


#define SS_PIN 5
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);


#define DHTPIN 27
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define GAS_PIN 34
#define LDR_PIN 32
#define RAIN_PIN 33

#define LED_PIN 2
#define BUZZER_PIN 4

#define IN1 16
#define IN2 17
#define ENA 25

#define TRIG_PIN 12
#define ECHO_PIN 14

String authorizedTokens[] = {
  "32ADCF55",
  "A1B2C3D4"
};
int tokenCount = 2;

float temperature = 0;
unsigned long lastDHTRead = 0;

bool isAuthorized(String uid) {
  for (int i = 0; i < tokenCount; i++) {
    if (uid == authorizedTokens[i]) {
      return true;
    }
  }
  return false;
}

long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long d = pulseIn(ECHO_PIN, HIGH, 25000);
  if (d == 0) return 0;
  return d * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("SYSTEM STARTED");

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(ENA, HIGH);

  SPI.begin(18, 19, 23, SS_PIN);
  rfid.PCD_Init();
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(300);

  Serial.println("WiFi Connected");
}

void loop() {

  // ---------- RFID ----------
  String rfidStatus = "Waiting";

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] < 0x10) uid += "0";
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    Serial.print("RFID UID: ");
    Serial.println(uid);

    if (isAuthorized(uid)) {
      rfidStatus = "Granted";
      Serial.println("ACCESS GRANTED");
    } else {
      rfidStatus = "Denied";
      Serial.println("ACCESS DENIED");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  if (millis() - lastDHTRead > 2000) {
    float t = dht.readTemperature();
    if (!isnan(t)) temperature = t;
    lastDHTRead = millis();
  }

  int gasValue = analogRead(GAS_PIN);
  String gasStatus = gasValue > 2000 ? "Gas Detected" : "Safe";
  digitalWrite(BUZZER_PIN, gasValue > 2000);

  String fanStatus = "OFF";
  if (temperature > 30) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    fanStatus = "ON";
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  
  int ldrValue = analogRead(LDR_PIN);
  String ledStatus = ldrValue > 1500 ? "ON" : "OFF";
  digitalWrite(LED_PIN, ldrValue < 1500);

  long distance = readDistance();

  int rainValue = analogRead(RAIN_PIN);
  String rainStatus = rainValue < 2500 ? "No Rain" : "No Rain";

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    String json =
      "{"
      "\"rfid\":\"" + rfidStatus + "\","
      "\"gas\":\"" + gasStatus + "\","
      "\"temperature\":" + String(temperature) + ","
      "\"fan\":\"" + fanStatus + "\","
      "\"ldr\":" + String(ldrValue) + ","
      "\"led\":\"" + ledStatus + "\","
      "\"ultrasonic\":" + String(distance) + ","
      "\"rain_value\":" + String(rainValue) + ","
      "\"rain_status\":\"" + rainStatus + "\""
      "}";

    http.POST(json);
    http.end();
  }

  delay(1500);
}
