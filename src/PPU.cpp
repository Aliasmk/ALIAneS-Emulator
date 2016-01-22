/* PPU Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "PPU.hpp"
#include <math.h>
#include <time.h>
using namespace std;

typedef uint8_t byte;


SDLrender* SDLrenderer;

PPU::PPU(){
	
}

void PPU::start(SDLrender* r){
	SDLrenderer = r;
	startTime = time(0);
	cout << "PPU Initialization: Complete" << endl;
}

void PPU::stop(){
	endTime = time(0);
	int delta = endTime-startTime;
	double FPS = frame/(delta);
	cout << dec << "Average FPS: " << FPS << " (" << frame << " frames in " << delta << " seconds)" << endl;
}

void PPU::cycle(){
	//cout << "PPU blip" << endl;
	if(cycles < 340)
		cycles++;
	else {
		cycles = 0;
		if(scanLine < 261)
			scanLine++;
		else{
			scanLine = 0;
			frame++;
			SDLrenderer->onFrameEnd();
			//cout << "PPU render: " << frame << endl;
		}
	}
	
	//cout << "PPU cycle " << cycles << " in scanline " << scanLine << " of frame " << frame << endl;
	
	
	ppuR = frame%255;
	ppuG = (scanLine%255);//+(frame%255)/5;
	ppuB = (cycles%255) - (frame%255)*2;
	
	SDLrenderer->setNextColor(ppuR,ppuG,ppuB);
	SDLrenderer->setDrawLoc(cycles, scanLine);
	SDLrenderer->renderPixelCallback();
}


void PPU::writeMem(int address, byte value){
	ppuMemory[address] = value;
	//cout << "writing value " << hex << (int)value << " to PPU address " << (int)address << endl;
}

byte PPU::readMem(int address){
	return ppuMemory[address];
}