#include "Kinematrix.h"
#include "ESP32Servo.h"

Servo servo;

void setup() {
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);
  servo.attach(16, 500, 2500);
  servo.write(90);
}

void loop() {
  if (Serial.available()) {
    int degree = Serial.readStringUntil('\n').toInt();
    degree = constrain(degree, 0, 180);
    Serial.print("| degree: ");
    Serial.print(degree);
    Serial.println();
    servo.write(degree);
  }
}
