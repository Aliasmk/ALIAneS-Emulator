/* SDL Renderer Header :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
*/
#ifndef RENDERER
#define	RENDERER

#include <sys/types.h>
#include <iostream>
#include <cstdint>

#include <SDL2/SDL.h>


class SDLrender {
	private:
		typedef std::uint8_t byte;
		static const int SCREEN_WIDTH = 256;
		static const int SCREEN_HEIGHT = 240;
		bool oddFrame;
		
		struct Pixel {
			byte red;
			byte green;
			byte blue;
		};
		Pixel frameBuffer[SCREEN_WIDTH][SCREEN_HEIGHT];
		
		int gy;
		int gx;
		
		int r;
		int g;
		int b;
		
		
		
		//may not need
		//SDL_Point framebuffer[SCREEN_WIDTH][SCREEN_HEIGHT];
		//RGB framebufferCOLOR[SCREEN_WIDTH][SCREEN_HEIGHT];
	public:
		SDLrender();
    	bool initSDL();
    	void closeSDL();
    	void renderPixelCallback();
    	void onFrameEnd();
    	void drawPixel(int x, int y, int red, int green, int blue);
    	void setNextColor(int r, int g, int b);
    	void setDrawLoc(int x, int y);
    	void writeFrameBuffer(int x, int y, int red, int green, int blue);
    	void renderFrame();
    	void clearScreen();
};
#endif
