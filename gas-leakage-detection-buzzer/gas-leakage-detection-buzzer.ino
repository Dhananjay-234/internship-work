#define GAS_PIN 34
#define LED_PIN 26
#define BUZZER_PIN 27

int threshold = 600;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  int gasValue = analogRead(GAS_PIN);

  Serial.print("Gas Value: ");
  Serial.print(gasValue);

  if (gasValue > threshold) {
    Serial.println("  | Status: DANGER");
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(1000);
  } else {
    Serial.println("  | Status: SAFE");
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
  }
}
