/* PPU Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "PPU.hpp"
using namespace std;

typedef uint8_t byte;


SDLrender* SDLrenderer;

PPU::PPU(){
	
}

void PPU::start(SDLrender* r){
	SDLrenderer = r;
	cout << "PPU Initialization: Complete" << endl;
}

void PPU::cycle(){
	
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
		}
	}
	
	//cout << "PPU cycle " << cycles << " in scanline " << scanLine << " of frame " << frame << endl;
	
	
	ppuR = cycles%255;
	ppuG = scanLine%255;
	ppuB = frame%255;
	
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