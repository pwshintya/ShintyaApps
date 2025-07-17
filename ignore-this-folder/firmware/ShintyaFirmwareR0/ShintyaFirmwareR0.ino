#include "Header.h"

void setup() {
  usbSerial.begin(&Serial, 115200);
  task.initialize(wifiTask);
  sensor.addModule("sensor1", new AnalogSens(34, 3.3, 4096));
  sensor.addModule("sensor2", new AnalogSens(35, 3.3, 4096));
  sensor.init();
  buzzer.toggleInit(100, 5);
}

void loop() {
  sensor.update([]() {
    // sensor.debug();
  });
  usbSerial.receive(usbCommunicationTask);

  DigitalIn::updateAll(&buttonDown, &buttonOk, DigitalIn::stop());
  DigitalOut::updateAll(&buzzer, DigitalOut::stop());
}
