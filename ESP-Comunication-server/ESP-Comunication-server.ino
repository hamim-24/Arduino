#include <esp_now.h>
#include <WiFi.h>

// Receiver MAC Address
uint8_t receiverMac[] = {0x24, 0x6F, 0x28, 0xAE, 0x4C, 0x70}; // Replace with actual MAC

typedef struct struct_message {
  char text[32];
} struct_message;

struct_message msg;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_send_cb([](const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Delivery Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  });

  strcpy(msg.text, "Hey bro 👋 from sender!");
  esp_now_send(receiverMac, (uint8_t *)&msg, sizeof(msg));
}

void loop() {}