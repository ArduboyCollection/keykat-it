#include <Arduboy.h>
#include "assets.h"
Arduboy arduboy;



//support functions
void renderSprite(int x, int y, sprite &s);
void updateButtons();
void drawKeyKat();

//button state global
bool btnup;
bool btndown;
bool btnleft;
bool btnright;
bool btna;
bool btnb;

//frame status (0-59)
unsigned int frameCount = 0;

//keykat status
unsigned int kx, ky;  //position
sprite keykat;
bool kmoving;


void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.setFrameRate(60);

  //initialize keykat
  keykat = keykat_standing;
  kx = 0;
  ky = 0;
  kmoving = false;

  //initialize button state
  btnup = btndown = btnleft = btnright = btna = btnb = false;
}


void loop() {
  //wait until the next frame
  if(not arduboy.nextFrame()) return;

  //update the frame counter
  frameCount = (frameCount+1) % 60;

  //update the buttons
  updateButtons();
    
  arduboy.clear(); 
  drawKeyKat();  
  arduboy.display();
}


/*
 * Render the sprite frame, updating its index as needed.
 */
void renderSprite(int x, int y, sprite &s)
{
  if(frameCount % s.fdelay == 0) {
    s.fi = (s.fi+1) % s.frames;
  }
  
  //display the bitmap
  arduboy.drawBitmap(x, y, s.bitmap + s.fi*s.height, s.width, s.height, WHITE); 
}



/*
 * Update which button(s) is/are pressed
 */
void updateButtons()
{
  //get the state of each button
  btnup = arduboy.pressed(UP_BUTTON);
  btndown = arduboy.pressed(DOWN_BUTTON);
  btnleft = arduboy.pressed(LEFT_BUTTON);
  btnright = arduboy.pressed(RIGHT_BUTTON);
  btna = arduboy.pressed(A_BUTTON);
  btnb = arduboy.pressed(B_BUTTON);
}


/*
 * Update and draw KeyKat
 */
void drawKeyKat()
{
  bool nowmoving = btnup or btndown or btnleft or btnright or btna or btnb;
  
  //handle start moving
  if((not kmoving) and nowmoving) {
    keykat = keykat_walking;
  }

  //remember walking state
  kmoving = nowmoving;

  //handle standing
  if(not kmoving) {
    keykat = keykat_standing;
  }

  //update coordinates
  if(btnup) {
    ky--;
  } else if(btndown) {
    ky++;
  } else if(btnleft) {
    kx--;
  } else if(btnright) {
    kx++;
  }

  //draw keykat
  renderSprite(kx, ky, keykat);
}

