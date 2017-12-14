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
bool togglePressed;

//frame status (0-59)
unsigned int frameCount = 0;

//game status
enum {SPLASH, PLAY} game_state;


//keykat status
int kx, ky;  //position
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
                    {computer_off, 16, 27, 0, OFF},
                    {computer_off, 28, 27, 0, OFF},
                    {computer_off, 40, 27, 0, OFF},
                    {computer_off, 80, 27, 0, OFF},
                    {computer_off, 92, 27, 0, OFF},
                    {computer_off, 104, 27, 0, OFF}};
const unsigned int STARTMAXTTF = 3000;
const unsigned int MINMAXTTF=600;
const unsigned int SPEEDUP=20;                   
unsigned int ncomps = 12;
unsigned int maxttf = STARTMAXTTF;
unsigned int minttf = 60;


//scoring information
unsigned int nworking=0; // number of working computers
unsigned int score=0;

//Prototypes
void splashScreen();
void playGame();
void gameOver();
void renderSprite(int x, int y, sprite &s);
void updateButtons();
void drawKeyKat();
void drawComputers();
void drawScore();
void turnComputerOn(computer &comp);
void turnComputerOff(computer &comp);
void breakComputer(computer &comp);
bool collision(int x, int y);
void keyKatToggle();

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
  togglePressed = btnup = btndown = btnleft = btnright = btna = btnb = false;

  //turn all the computers on
  for(int i=0; i<ncomps; i++) {
    turnComputerOn(comps[i]);
  }

  game_state = SPLASH;
}


void loop() {
  //wait until the next frame
  if(not arduboy.nextFrame()) return;

  //update the frame counter
  frameCount = (frameCount+1) % 60;

  //update the buttons
  updateButtons();
    
  arduboy.clear();
  switch(game_state) {
    case SPLASH:
      splashScreen();
      break;
    case PLAY:
      playGame();
      break;
  }
  arduboy.display();
}


/* Splash Screen Frame Loop */
void splashScreen() 
{
  arduboy.drawBitmap(0, 0, splash_screen_bitmap, 128, 64, WHITE);  

  //see if we are ready to move into gamepaly
  if(btna or btnb or btnup or btndown or btnleft or btnright) {
    game_state = PLAY;
  }
}


/* play the game */
void playGame()
{
  drawComputers(); 
  drawKeyKat();  
  drawScore();  
}


/* 
 *  The game is over!  Alert the user.
 */
void gameOver()
{
  
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
  bool a, b;
  
  //get the state of each button
  btnup = arduboy.pressed(UP_BUTTON);
  btndown = arduboy.pressed(DOWN_BUTTON);
  btnleft = arduboy.pressed(LEFT_BUTTON);
  btnright = arduboy.pressed(RIGHT_BUTTON);
  a = arduboy.pressed(A_BUTTON);
  b = arduboy.pressed(B_BUTTON);

  //detect toggle event
  if((not a and btna) or (not b and btnb)) {
    togglePressed = true;
  }

  //record btna and btnb
  btna = a;
  btnb = b;
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

  //update coordinates (if no collision occur)
  if(btnup) {
    if(not collision(kx, ky-1)){
      ky--;
    }
  } else if(btndown) {
    if(not collision(kx, ky+1)) {
      ky++;
    }
  } 
  
  if(btnleft) {
    if(not collision(kx-1, ky)) {
      kx--;
    }
  } else if(btnright) {
    if(not collision(kx+1, ky)) {
      kx++;
    }
  }

  //handle action buttons
  if(togglePressed) {
    keyKatToggle();
    togglePressed = false;
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
 * Draw the score and the number of working computers
 */
void drawScore()
{
  arduboy.setCursor(0, 56);
  arduboy.print("Score: ");
  arduboy.print(score);
  arduboy.setCursor(80, 56);
  arduboy.print("Up: ");
  arduboy.print(100*nworking/ncomps);
  arduboy.print("%");
}


/*
 * Turn the computer on (which lasts for a temporary number of frames)
 */
void turnComputerOn(computer &comp)
{
  //set the machine to working
  comp.state = ON;
  comp.s = computer_working;
  nworking++;

  //determine how long the computer will work
  comp.ttf = random(minttf, maxttf);
  arduboy.tunes.playScore(snd_on);
}


/*
 * Turn the computer off
 */
void turnComputerOff(computer &comp)
{
  //was it working before?
  if(comp.state == ON) {
    nworking--;
  } else {
    score += 50 + (5000*(STARTMAXTTF-maxttf)) / MINMAXTTF;
  }
  
  //turn the machine off
  comp.state = OFF;
  comp.s = computer_off;
  arduboy.tunes.playScore(snd_off);
}


/*
 * Do what user's do best!
 */
void breakComputer(computer &comp)
{
  //break the machine
  comp.state = BROKEN;
  comp.s = computer_broken;
  nworking--;
  arduboy.tunes.playScore(snd_fail);
}


/* 
 * Returns true if KeyKat would be in a collision with something at
 * the specified point.
 */
bool collision(int x, int y)
{
  int kxmax, kymax;
  int cxmax, cymax;
  
  //edge collision
  if(x < 0 or x > 120 or y < 0 or y > 48) {
    return true;
  } 

  //check the computers
  kxmax = x + 7;
  kymax = y + 15;
  for(int i=0; i<ncomps; i++) {
    //computer bounding boxes (allowing for keyboard traversal)
    cxmax = comps[i].x + 7;
    cymax = comps[i].y + 5;

    //just simple axis aligned bounding box
    if(x <= cxmax && comps[i].x <= kxmax && y <= cymax && comps[i].y <= kymax) {
      return true;
    }
  }
  
  //all passed! no collision
  return false;
}


/*
 * This function has KeyKat toggle all computers within her reach.
 * (She has to be touching the keyboards)
 */
void keyKatToggle()
{
  int kxmax, kymax;
  int cxmax, cymax;

  //check the computers
  kxmax = kx + 7;
  kymax = ky + 15;
  for(int i=0; i<ncomps; i++) {
    //computer bounding boxes (for keyboard touching)
    cxmax = comps[i].x + 7;
    cymax = comps[i].y + 6;

    //just simple axis aligned bounding box
    if(kx <= cxmax && comps[i].x <= kxmax && ky <= cymax && comps[i].y <= kymax) {
      //toggle the machine
      if(comps[i].state != OFF) {
        turnComputerOff(comps[i]);
        if(maxttf > MINMAXTTF) {
          maxttf -= SPEEDUP; //mwahaha
        }
      } else {
        turnComputerOn(comps[i]);
      }
    }
  }
}

