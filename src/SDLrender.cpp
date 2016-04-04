/* SDL Renderer Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include <SDL2/SDL.h>
#include "SDLrender.hpp"
#include <cstdlib>
#include <csignal>

using namespace std;

SDL_Window* window = NULL;
SDL_Renderer* render = NULL;

//DEBUG
int pixSkipped = 0;
int pixDrawn = 0;

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
 	clearScreen();
  cout << "Completed" << endl;
  return status;
}

void SDLrender::drawPixel(int x, int y, int red, int green, int blue){
	//if((oddFrame && (frameBuffer[x][y].red != red || frameBuffer[x][y].blue != blue || frameBuffer[x][y].green != green)) || (!oddFrame && (frameBufferOdd[x][y].red != red || frameBufferOdd[x][y].blue != blue || frameBufferOdd[x][y].green != green))){
	//if((oddFrame && (frameBuffer[x][y].red != red)) || (!oddFrame && (frameBufferOdd[x][y].red != red ))){
	
		SDL_SetRenderDrawColor(render, red, green, blue, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawPoint(render, x, y);
	//}
}


void SDLrender::closeSDL(){
  cout << "SDL Renderer is now off." <<endl;
  cout << "Drawn: " << pixDrawn << ", Skipped: " << pixSkipped << endl;
  
  //raise(SIGINT);
  
  
  SDL_DestroyRenderer(render);
  render = NULL;

  SDL_DestroyWindow(window);
  window = NULL;
  SDL_VideoQuit();
  SDL_Quit();
}

void SDLrender::renderPixelCallback(){
	//TODO temporary - draws pixel to next avaliable location
	
	if(gx < SCREEN_WIDTH && gy < SCREEN_HEIGHT){
		if(oddFrame){
			frameBufferOdd[gx][gy].red = r;
			frameBufferOdd[gx][gy].green = g;
			frameBufferOdd[gx][gy].blue = b;
		} else {
			frameBuffer[gx][gy].red = r;
			frameBuffer[gx][gy].green = g;
			frameBuffer[gx][gy].blue = b;
		}
	}
	//drawPixel(gx, gy, r, g, b);
	  
}



void SDLrender::onFrameEnd(){
	
	for(int y = 0; y < SCREEN_HEIGHT; y++){
  		for(int x = 0; x < SCREEN_WIDTH; x++){
  				
  				//cout << "xy: " << x << ", " << y;
  				
  				if(oddFrame){
  					//cout << " ODD " << " --- ";
  					//cout << "comparing previous value " << hex << (int)frameBuffer[x][y].red << " to this frames value " << hex << (int)frameBufferOdd[x][y].red << endl;
  					if((int)frameBufferOdd[x][y].red != (int)frameBuffer[x][y].red){
  						drawPixel(x,y,frameBufferOdd[x][y].red ,255, frameBufferOdd[x][y].blue);
  						pixDrawn++;
  					} else {
  						//drawPixel(x,y,frameBuffer[x][y].red ,frameBuffer[x][y].green, frameBuffer[x][y].blue);
  						pixSkipped++;
  					}
  					
  				
  				} else {
  					//cout << " EVEN " << " --- ";
  					//cout << "comparing previous value " << hex << (int)frameBufferOdd[x][y].red << " to this frames value " << hex << (int)frameBuffer[x][y].red << endl;
  					if((int)frameBuffer[x][y].red != (int)frameBufferOdd[x][y].red){
  						drawPixel(x,y,frameBuffer[x][y].red ,255, frameBuffer[x][y].blue);
  						pixDrawn++;
  					} else {
  						//drawPixel(x,y,frameBuffer[x][y].red ,frameBuffer[x][y].green, frameBuffer[x][y].blue);
  						pixSkipped++;
  					}
  				}
  				
  				
  				
		}
	}
	oddFrame = !oddFrame;
	SDL_RenderPresent(render);
	
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
  		
  		frameBufferOdd[x][y].red = 0;
  		frameBufferOdd[x][y].green = 0;
  		frameBufferOdd[x][y].blue = 0;
  		frameBuffer[x][y].red = 0;
  		frameBuffer[x][y].green = 0;
  		frameBuffer[x][y].blue = 0;
  		drawPixel(x,y,0,0,0);

  	}
  }
  SDL_RenderPresent(render);
}
