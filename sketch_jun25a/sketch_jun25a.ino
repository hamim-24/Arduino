#include "esp_system.h"

void setup() {
  Serial.begin(115200);
  if(psramFound()){
    Serial.println("✅ PSRAM is working!");
  } else {
    Serial.println("❌ PSRAM NOT detected!");
  }
}

void loop() {}