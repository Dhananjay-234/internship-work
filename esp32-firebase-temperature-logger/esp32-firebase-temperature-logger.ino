#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <time.h>

#define WIFI_SSID "xxxxxx"
#define WIFI_PASSWORD "xxxxx"

#define API_KEY "xxxx"
#define DATABASE_URL "xxxxx"

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 5000;

String getDateTimeKey() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "time_error";

  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d_%H-%M-%S", &timeinfo);
  return String(buffer);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    yield();
  }
  Serial.println("\nWiFi connected");

  configTime(19800, 0, "pool.ntp.org");   // IST

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (!Firebase.signUp(&config, &auth, "", "")) {
    Serial.print("Auth failed: ");
    Serial.println(config.signer.signupError.message.c_str());
  } else {
    Serial.println("Anonymous auth OK");
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase ready");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastSendTime >= sendInterval) {
    lastSendTime = currentMillis;

    float temperature = dht.readTemperature();
    if (isnan(temperature)) {
      Serial.println("Temperature read failed");
      return;
    }

    String dateTimeKey = getDateTimeKey();
    String path = "/temp_log/" + dateTimeKey + "/temperature";

    if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), temperature)) {
      Serial.print("Stored ");
      Serial.print(temperature);
      Serial.print(" at ");
      Serial.println(dateTimeKey);
    } else {
      Serial.print("FAILED: ");
      Serial.println(fbdo.errorReason());
    }
  }

  delay(10);
  yield();
}
