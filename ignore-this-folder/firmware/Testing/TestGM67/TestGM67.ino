#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_GM67

#include "Kinematrix.h"

SensorModule sensor;

void setup() {
  Serial.begin(115200);
  sensor.addModule("code", new GM67Sens(&Serial1, 9600, SERIAL_8N1, 26, 25));
  sensor.init();
}

void loop() {
  sensor.update([]() {
    if (!sensor["code"].as<String>().isEmpty()) {
      sensor.debug(10, false);
    }
  });
}
