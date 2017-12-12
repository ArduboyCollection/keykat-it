#include <Arduboy.h>
#include "assets.h"
Arduboy arduboy;

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

//computer types and globals
enum compstate {ON, OFF, BROKEN};
struct computer {
  sprite s;          //the computer's sprite
  unsigned int x;    // position x
  unsigned int y;    // position y
  unsigned int ttf;  //time to failure (in frames)
  compstate state;   //the current computer state
};
computer comps[] = {{computer_off, 16, 0, 0, OFF},
                    {computer_off, 28, 0, 0, OFF},
                    {computer_off, 40, 0, 0, OFF},
                    {computer_off, 80, 0, 0, OFF},
                    {computer_off, 92, 0, 0, OFF},
                    {computer_off, 104, 0, 0, OFF},
                    {computer_off, 16, 32, 0, OFF},
                    {computer_off, 28, 32, 0, OFF},
                    {computer_off, 40, 32, 0, OFF},
                    {computer_off, 80, 32, 0, OFF},
                    {computer_off, 92, 32, 0, OFF},
                    {computer_off, 104, 32, 0, OFF}};
                    
unsigned int ncomps = 12;
unsigned int maxttf = 600;


//Prototypes
void renderSprite(int x, int y, sprite &s);
void updateButtons();
void drawKeyKat();
void drawComputers();
void turnComputerOn(computer &comp);
void turnComputerOff(computer &comp);
void breakComputer(computer &comp);


void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();

  //initialize keykat
  keykat = keykat_standing;
  kx = 60;
  ky = 15;
  kmoving = false;

  //initialize button state
  btnup = btndown = btnleft = btnright = btna = btnb = false;

  //turn all the computers on
  for(int i=0; i<ncomps; i++) {
    turnComputerOn(comps[i]);
  }
}


void loop() {
  //wait until the next frame
  if(not arduboy.nextFrame()) return;

  //update the frame counter
  frameCount = (frameCount+1) % 60;

  //update the buttons
  updateButtons();
    
  arduboy.clear();
  drawComputers(); 
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


/*
 * Update and draw all the computers.  (One frame worth)
 */
void drawComputers()
{
  for(int i=0; i<ncomps; i++) {
    //count down to failure
    if(comps[i].ttf > 0) {
      comps[i].ttf--;

      //BOOOM
      if(comps[i].ttf == 0) {
        breakComputer(comps[i]);
      }
    }

    //draw the machine
    renderSprite(comps[i].x, comps[i].y, comps[i].s);
  }
}


/*
 * Turn the computer on (which lasts for a temporary number of frames)
 */
void turnComputerOn(computer &comp)
{
  //set the machine to working
  comp.state = ON;
  comp.s = computer_working;

  //determine how long the computer will work
  comp.ttf = random(30, maxttf);
}


/*
 * Turn the computer off
 */
void turnComputerOff(computer &comp)
{
  //turn the machine off
  comp.state = OFF;
  comp.s = computer_off;
}


/*
 * Do what user's do best!
 */
void breakComputer(computer &comp)
{
  //break the machine
  comp.state = BROKEN;
  comp.s = computer_broken;
}

