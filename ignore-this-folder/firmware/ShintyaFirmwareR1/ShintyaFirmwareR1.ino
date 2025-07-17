#include "Header.h"

void setup() {
  usbSerial.begin(&Serial, 115200);
  task.initialize(wifiTask);

  menu.initialize(true);
  menu.setLen(20, 4);

#if JUST_TESTING
  if (!mp3Player.begin(Serial2, true, true)) {
    while (true) { Serial.println("| mp3Player Error !!"); }
  }

  if (!pcfModuleA.begin()) {
    while (true) { Serial.println("| pcfModuleA Error !!"); }
  }
  if (!pcfModuleB.begin()) {
    while (true) { Serial.println("| pcfModuleB Error !!"); }
  }

  limitSwitch1.init(&pcfModuleA);
  limitSwitch2.init(&pcfModuleA);
  limitSwitch3.init(&pcfModuleA);
  limitSwitch4.init(&pcfModuleA);
  limitSwitch5.init(&pcfModuleA);
  limitSwitch6.init(&pcfModuleA);
  limitSwitch7.init(&pcfModuleA);
  limitSwitch8.init(&pcfModuleA);
  limitSwitch9.init(&pcfModuleB);
  limitSwitch10.init(&pcfModuleB);
  limitSwitch11.init(&pcfModuleB);
  limitSwitch12.init(&pcfModuleB);

  servoDriver.Init(SERVO_MODE);
  servoDriver.Sleep(false);
#endif

  sensor.addModule("sonar", new UltrasonicSens(14, 27));
  sensor.addModule("code", new GM67Sens(&Serial1, 9600, SERIAL_8N1, 26, 25));
#if JUST_TESTING
  sensor.addModule("keypad", []() -> BaseSens* {
    const uint8_t NUM_ROW = 4;
    char customKeys[NUM_ROW][NUM_ROW] = {
      { 'D', '#', '0', '.' },
      { 'C', '9', '8', '7' },
      { 'B', '6', '5', '4' },
      { 'A', '3', '2', '1' }
    };
    uint8_t rowPins[NUM_ROW] = { 3, 2, 1, 0 };
    uint8_t colPins[NUM_ROW] = { 7, 6, 5, 4 };
    return new KeypadI2CSens(makeKeymap(customKeys), rowPins, colPins, 4, 4, 0x20);
  });
#endif
  sensor.init();
  buzzer.toggleInit(100, 5);
}

void loop() {
  sensor.update([]() {
    // sensor.debug();
  });
  usbSerial.receive(usbCommunicationTask);

  MenuCursor cursor{
    .up = false,
    .down = !buttonDownStr.isEmpty(),
    .select = !buttonOkStr.isEmpty(),
    .back = false,
    .show = true
  };
  menu.onListen(&cursor, onLcdMenu);

  buttonDownStr = "";
  buttonOkStr = "";

  DigitalIn::updateAll(&buttonDown, &buttonOk, DigitalIn::stop());
  DigitalOut::updateAll(&buzzer, &ledRed, &ledGreen, &ledYellow, &relayA, &relayB, DigitalOut::stop());
#if JUST_TESTING
  PCF8574DigitalIn::updateAll(&limitSwitch1, &limitSwitch2, &limitSwitch3, &limitSwitch4, &limitSwitch5, &limitSwitch6, &limitSwitch7, &limitSwitch8, &limitSwitch9, &limitSwitch10, &limitSwitch11, &limitSwitch12, PCF8574DigitalIn::stop());
#endif
}
