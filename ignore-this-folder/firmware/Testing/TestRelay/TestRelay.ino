#define ENABLE_MODULE_DIGITAL_OUTPUT

#include "Kinematrix.h"

DigitalOut relay(33, true);

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    while (Serial.available()) {
      Serial.read();
    }
    relay.toggle();
    int state = relay.getState();
    Serial.println(state ? "ON" : "OFF");
  }
  relay.update();
}
