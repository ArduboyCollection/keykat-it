#include <Arduboy.h>
#include "assets.h"
Arduboy arduboy;

unsigned int frameCount = 0;

void renderSprite(int x, int y, sprite &s);

sprite alist[5];

void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.setFrameRate(60);
  alist[0] = keykat_standing;
  alist[1] = keykat_walking;
  alist[2] = computer_off;
  alist[3] = computer_working;
  alist[4] = computer_broken;
}


void loop() {
  //wait until the next frame
  if(not arduboy.nextFrame()) return;

  //update the frame counter
  ++frameCount;
  if(frameCount > 60) {
    frameCount -= 60;
  }

  arduboy.clear(); 
  int x = 8;

  for(int i=0; i<5; i++) {
    renderSprite(x, 5, alist[i]);
    x+=16;
  }
  
  arduboy.display();
}


void renderSprite(int x, int y, sprite &s)
{
  if(frameCount % s.fdelay == 0) {
    s.fi = (s.fi+1) % s.frames;
  }
  
  //display the bitmap
  arduboy.drawBitmap(x, y, sprite_data + s.offset + s.fi*s.height, s.width, s.height, WHITE); 
}

