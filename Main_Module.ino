#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// === DHT22 ===
#define DHTPIN D5         
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// === MQ-2 ===
#define MQ2_PIN A0        

// === LCD ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === ESP-NOW Struct ===
struct struct_message {
  int ammoniaLevel;
};
struct_message incomingData;
int latestAmmonia = 0;

// === ESP-NOW Receive Callback ===
void OnDataRecv(uint8_t *mac, uint8_t *incomingDataBytes, uint8_t len) {
  memcpy(&incomingData, incomingDataBytes, sizeof(incomingData));
  latestAmmonia = incomingData.ammoniaLevel;
  Serial.print("Received Ammonia: ");
  Serial.println(latestAmmonia);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  WiFi.mode(WIFI_STA);

  // === LCD init ===
  Wire.begin(D2, D1);        // SDA, SCL
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("System Ready");

  // === ESP-NOW Init ===
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed!");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int smoke = analogRead(MQ2_PIN);

  Serial.println("====== Sensor Readings ======");
  Serial.printf("Temp: %.2f °C\n", temp);
  Serial.printf("Humidity: %.2f %%\n", hum);
  Serial.printf("Smoke (MQ-2): %d\n", smoke);
  Serial.printf("Ammonia (MQ-135): %d\n", latestAmmonia);
  Serial.println();

  // === LCD Display ===
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(((temp*(9/5))+32), 0);
  lcd.print("F H:");
  lcd.print(hum, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(smoke);
  lcd.print(" CO2:");
  lcd.print(latestAmmonia);

  delay(1000);
}