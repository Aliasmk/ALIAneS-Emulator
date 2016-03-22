/* SDL Renderer Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include <SDL2/SDL.h>
#include "SDLrender.hpp"
#include <cstdlib>

using namespace std;

SDL_Window* window = NULL;
SDL_Renderer* render = NULL;

SDLrender::SDLrender(){
	cout << "SDL Renderer is spinning up." << endl;
}

bool SDLrender::initSDL(){
  bool status = true;
  cout << "Renderer Initialization: Start" << endl;
  cout << "Renderer Initialization: ";
  if(SDL_Init(SDL_INIT_VIDEO)<0){
    status = false;
   cout << "Failed: Could not initialize SDL!" << endl;
  } else {
    window = SDL_CreateWindow("ALIAneS Emulation Project",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL){
      status = false;
      cout << "Failed: Unable to initialize window" << endl;
    } else {
      //surface = SDL_GetWindowSurface(window);
      render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
 }
  cout << "Completed" << endl;
  return status;
}

void SDLrender::drawPixel(int x, int y, int red, int green, int blue){
	SDL_SetRenderDrawColor(render, red, green, blue, SDL_ALPHA_OPAQUE);
	SDL_RenderDrawPoint(render, x, y);
}


void SDLrender::closeSDL(){
  cout << "SDL Renderer is now off." <<endl;
  
  
  
  SDL_DestroyRenderer(render);
 // render = NULL;

  SDL_DestroyWindow(window);
  //window = NULL;
SDL_VideoQuit();
  SDL_Quit();
}

void SDLrender::renderPixelCallback(){
	//TODO temporary - draws pixel to next avaliable location
	
	if(gx < SCREEN_WIDTH && gy < SCREEN_HEIGHT){
		
			frameBuffer[gx][gy].red = r;
			frameBuffer[gx][gy].green = g;
			frameBuffer[gx][gy].blue = b;
		
	}
	//drawPixel(gx, gy, r, g, b);
	  
}

void SDLrender::onFrameEnd(){
	for(int y = 0; y < SCREEN_HEIGHT; y++){
  		for(int x = 0; x < SCREEN_WIDTH; x++){
			
  					drawPixel(x,y,frameBuffer[x][y].red,frameBuffer[x][y].green,frameBuffer[x][y].blue);

		}
	}
	SDL_RenderPresent(render);
	oddFrame = !oddFrame;
}

void SDLrender::setNextColor(int nxtr, int nxtg, int nxtb){
	r = nxtr;
	g = nxtg;
	b = nxtb;
}

void SDLrender::setDrawLoc(int x, int y){
	gx = x;
	gy = y;
}
    

void SDLrender::clearScreen(){
  for(int y = 0; y < SCREEN_HEIGHT; y++){
  	for(int x = 0; x < SCREEN_WIDTH; x++){
  		
  		drawPixel(x,y,0,0,0);

  	}
  }
}
