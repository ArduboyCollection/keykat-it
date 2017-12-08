#include <Arduboy.h>
Arduboy arduboy;

const unsigned char keykat[] PROGMEM = {
  0x80, 0x8c, 0x9f, 0xfe, 0xfe, 0x9f, 0x8c, 0x80, 0x1, 0xf1, 0xf9, 0x1f, 0x1f, 0xf9, 0xf1, 0x1
};

void setup() {
  arduboy.begin();
  arduboy.clear();
}

void loop() {
 arduboy.clear(); 
 arduboy.drawBitmap(5, 10, keykat, 8, 16, WHITE); 
 arduboy.display();
}
