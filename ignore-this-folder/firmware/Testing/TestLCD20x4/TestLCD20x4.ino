#define ENABLE_MODULE_LCD_MENU

#include "Kinematrix.h"

LcdMenu menu(0x27, 20, 4);

void setup() {
  Serial.begin(115200);
  menu.initialize(true);
  menu.setLen(20, 4);
}

void loop() {
  MenuCursor cursor{
    .up = false,
    .down = false,
    .select = false,
    .back = false,
    .show = true
  };
  menu.onListen(&cursor, []() {
    static auto menuMain = menu.createMenu(menu.begin(4), "Testing LCD", "I2C 20 x 4", "LCD I2C", "OK");
    menu.showMenu(menuMain);
  });
}
