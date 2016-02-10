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

void PPU::writeMem(int address, byte value){
	ppuMemory[address] = value;
	//cout << "writing value " << hex << (int)value << " to PPU address " << (int)address << endl;
}

byte PPU::readMem(int address){
	return ppuMemory[address];
}

void PPU::cycle(){
	//cout << "PPU" << endl;
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


//Reading and writing of the PPU registers. 
//These are public methods that will usually be called by the CPU

//condition ? expression1 : expression2

void PPU::writePPUCTRL(byte in){
	cout << "PPUCTRL Write: " << in << endl;
	
	
	(in&0x80)==0x80 ? nmiEnable = true : nmiEnable = false;
	(in&0x40)==0x40 ? ppuMode = true : ppuMode = false;
	(in&0x20)==0x20 ? spriteHeight = true : spriteHeight = false;
	(in&0x10)==0x10 ? backgroundPatternTable = true : backgroundPatternTable = false;
	(in&0x8)==0x8 ? spritePatternTable = true : spritePatternTable = false;
	(in&0x4)==0x4 ? vramIncrementMode = true : vramIncrementMode = false;

	
	
	/*nmiEnable = in&0x80; 				//0=false, 1=true
	ppuMode = in&0x40;					//0=read from ext, 1=output to ext
	spriteHeight = in&0x20;				//0=8x8, 1=8x16
	backgroundPatternTable = in&0x10;	//0=0000h, 1=1000h
	spritePatternTable = in&0x8;		//0=0000h, 1=1000h
	vramIncrementMode = in&0x4; 		//0= add 1, going across, 1= add 32, going down*/
	baseNametableAddress = in&0x3; 		//(0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
}

void PPU::writePPUMASK(byte in){
	cout << "PPUCTRL Write: " << in << endl;
	
	(in&0x80)==0x80 ? emphasizeBlue = true : emphasizeBlue = false;
	(in&0x40)==0x40 ? emphasizeGreen = true : emphasizeGreen = false;
	(in&0x20)==0x20 ? emphasizeRed = true : emphasizeRed = false;
	(in&0x10)==0x10 ? showSprites = true : showSprites = false;
	(in&0x8)==0x8 ? showBackground = true : showBackground = false;
	(in&0x4)==0x4 ? showLeftSprites = true : showLeftSprites = false;
	(in&0x2)==0x2 ? showLeftBackground = true : showLeftBackground = false;
	(in&0x1)==0x1 ? greyscale = true : greyscale = false;
	
	/*
	emphasizeBlue = in&0x80;
	emphasizeGreen = in&0x40;
	emphasizeRed = in&0x20;
	showSprites = in&0x10;
	showBackground = in&0x8;
	showLeftSprites = in&0x4;
	showLeftBackground = in&0x2;
	greyscale = in&0x1;*/
}

byte PPU::readPPUSTATUS(){
	return (int)vblankStatus*0x80 + (int)sprite0hit*0x40 + (int)spriteOverflow*0x20;
}
	
void PPU::writeOAMADDR(byte in){
	oamAddress = in;
}

void PPU::writeOAMDATA(byte in){
	oamValue = in;
}
	
byte PPU::readOAMDATA(){
	return oamValue;
}

void PPU::writePPUSCROLL(byte in){
	if(scrollFirstWrite)
		in = ppuscroll_scrollPosX;
	else
		in = ppuscroll_scrollPosY;
		
	scrollFirstWrite = !scrollFirstWrite;
}
	
void PPU::writePPUADDR(byte in){
	if(addrFirstWrite)
		in = ppuaddr_upperByte;
	else
		in = ppuaddr_lowerByte;
		
	addrFirstWrite = !addrFirstWrite;
}

void PPU::writePPUDATA(byte in){
	//TODO PPUDATA WRITE
	
	
	if(vramIncrementMode)
		vramAddr+=1;
	else
		vramAddr+=32;
}

byte PPU::readPPUDATA(){
	//TODO PPUDATA READ
	
	return 0;
}

void PPU::writeOAMDMA(){
	//TODO OAM DATA WRITE
}