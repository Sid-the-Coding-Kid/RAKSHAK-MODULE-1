#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t receiverMAC[] = { 0x48, 0x55, 0x19, 0xE0, 0x15, 0xBE };

struct struct_message {
  int ammoniaLevel;
};
struct_message msg;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
  int mq135Value = analogRead(A0);
  msg.ammoniaLevel = mq135Value;

  esp_now_send(receiverMAC, (uint8_t *)&msg, sizeof(msg));
  Serial.printf("Sent carbon dioxide: %d\n", mq135Value);

  delay(1000); // Send every 5 seconds
}


