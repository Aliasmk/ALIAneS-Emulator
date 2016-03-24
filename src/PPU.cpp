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

bool nDebugging = false;

SDLrender* SDLrenderer;
//CPU* ptr_nesCPU;


PPU::PPU(){
	cout << "NES PPU is now on." <<endl;
	for(int i = 0; i<0x3FFF; i++){
		ppuWriteMem(i, 0);
	}
	//ptr_nesCPU = nCPU; //address of?
}

void PPU::start(SDLrender* r){
	SDLrenderer = r;
	startTime = time(0);
	addrFirstWrite = true;
	vblank = false;
	spriteIndex=0;
	cout << "PPU Initialization: Complete" << endl;
	
}

void PPU::stop(){
	endTime = time(0);
	int delta = endTime-startTime;
	if(delta > 0){
		double FPS = frame/(delta);
		cout << dec << "Average FPS: " << FPS << " (" << frame << " frames in " << delta << " seconds)" << endl;
	}
	else 
	{
		cout << "Frame rate count error (too fast) " << frame << " frames"<< endl;
	}
}

void PPU::ppuWriteMem(int address, byte value){
	ppuMemory[address] = value;
	//cout << "writing value " << hex << (int)value << " to PPU address " << (int)address << endl;
}

void PPU::ppuWriteOAM(int address, byte value){
	oamMemory[address] = value;
	//cout << "writing value " << hex << (int)value << " to PPU OAM address " << (int)address << endl;
}

void PPU::ppuWriteSecOAM(int address, byte value){
	oamMemorySec[address] = value;
	//cout << "writing value " << hex << (int)value << " to PPU Sec OAM address " << (int)address << endl;
}

byte PPU::ppuReadMem(int address){
	return ppuMemory[address];
}

byte PPU::ppuReadOAM(int address){
	return oamMemory[address];
}

byte PPU::ppuReadSecOAM(int address){
	return oamMemorySec[address];
}


void PPU::cycle(){
	//cout << "PPU" << endl;
	//cout << "vram address: " << vramAddr << endl;
	
	//if(scanLine == 241 && cycles==1 && nmiEnable){
	if(scanLine == 241 && cycles==1){
		vblank = true;
		vblankStatus = true;
		//ptr_nesCPU->triggerInterrupt(1);
		//cout << endl<< "vBlank begins" <<endl;
	}
	if(scanLine == 261 && cycles==1){
		vblank = false;
		vblankStatus = false;
		vblankSeen = false;
		sprite0hit = false;
		spriteOverflow = false;
		//cout << endl<< "vBlank ends" <<endl;
	}
	
	if(cycles < 340)
		cycles++;
	else {
		cycles = 0;
		if(scanLine < 261){
			
			clearSecOAM();
			
			for(int i = 0; i<64;i++){
				if(spriteIndex > 7) break;
				
				if(scanLine+1-(ppuReadOAM(i*4))<8 && scanLine+1-(ppuReadOAM(i*4))>=0 && scanLine < 240){
					//cout << dec << (int)ppuReadOAM(i*4) << " - " << scanLine << " = " << ppuReadOAM(i*4)-scanLine << endl;
					//cout << "Sprite " << spriteIndex << " data: ";
					for(int n = 0; n<4; n++){
						ppuWriteSecOAM((spriteIndex*4)+n, ppuReadOAM((i*4)+n));
						//cout << hex << (int)ppuReadSecOAM(spriteIndex*4+n) << " ";
					}
					//cout << endl;
					
					spriteIndex++;
				}
			}
			
			
			scanLine++;
			
		}else{
			scanLine = 0;
			frame++;
			/*cout << endl << endl;
			
			for(int y = 0; y<=29;y++){
				for(int x = 0; x<31; x++)
				{
					cout << << " ";				
				}
				cout << endl;
			}
			
			
			cout << endl << endl;*/
			
			
			
			
			//cout << "Sprites on screen: " << endl;
			/*for(int i = 0; i<256; i+=4){
				if(oamMemory[i] != 0xf0 && oamMemory[i+1] != 0xf0 && oamMemory[i+2] != 0xf0 && oamMemory[i+3] != 0xf0)
					cout << "(" << floor(i/4) << ") tile " << hex << (int)oamMemory[i+1] << " at SL/C " << hex << (int)oamMemory[i] << "/" << (int)oamMemory[i+3] << " w attribute value " <<  (int)oamMemory[i+2] << endl;
			}*/
			
			
			frameEnd = true;
			SDLrenderer->onFrameEnd();
			
			
		
			
			//cout << "PPU render: " << frame << endl;
		}
	}
	
	
	
	
	//cout << "PPU cycle " << cycles << " in scanline " << scanLine << " of frame " << frame << endl;
	//int PPU::fetchTilePixel(int tileID, int scanL, int cyc, bool ptHalf){
	int color;
	//color = ppuReadMem(0x2000+0x20*floor(scanLine/8)+floor(cycles/8));
	int tileID = ppuReadMem(0x2000+0x20*floor(scanLine/8)+floor(cycles/8));
	color = fetchTilePixel(tileID, scanLine, cycles, backgroundPatternTable);
	int tempcolor;
	for(int i = 0; i<= spriteIndex; i++){
		if(cycles-ppuReadSecOAM(i*4+3)<8 && cycles-ppuReadSecOAM(i*4+3)>=0){
			tempcolor = fetchSpritePixel(ppuReadSecOAM(i*4+1),scanLine-ppuReadSecOAM(i*4), cycles-ppuReadSecOAM(i*4+3), spritePatternTable, ppuReadSecOAM(i*4+2));
			
			if(tempcolor != 0){
				color = tempcolor;
			}
			
			//if(tempcolor != 0) 
			//	color = tempcolor;	
			
			
		}	
	}
	
	/*int tempColor = 0;
	for(int i = 0; i<64;i++){
		if(ppuReadOAM(i*4) < 0xF0 || (scanLine-ppuReadOAM(i*4)<8 && cycles-ppuReadOAM((i*4)+3)<8)){
			tempColor = fetchTilePixel(ppuReadOAM((i*4)+1), scanLine-ppuReadOAM((i*4))-1, cycles-ppuReadOAM((i*4)+3), spritePatternTable);		
			if(tempColor != 0){
				color = tempColor;
			}	
		}
	}*/
	
	color = color*85;
	
	/*if(color == 0x24){
		color = 0;
	}else{*/
		//color *= 1;
		//if(color > 0xFF)
		//	color = 0xFF;
	//}
	
	//color += (cycles%255) - (frame%255)*2;
	ppuR = color;
	ppuG = color;
	ppuB = color; 
	
	//ppuR += frame%255;
	//ppuG += (scanLine%255);//+(frame%255)/5;
	
	
	
	SDLrenderer->setNextColor(ppuR,ppuG,ppuB);
	SDLrenderer->setDrawLoc(cycles, scanLine);
	SDLrenderer->renderPixelCallback();
}


//Reading and writing of the PPU registers. 
//These are public methods that will usually be called by the CPU

//condition ? expression1 : expression2

void PPU::writePPUCTRL(byte in){
	if(nDebugging)
		cout << endl << "PPUCTRL Write: " << hex << (int)in << endl;
	
	
	(in&0x80)==0x80 ? nmiEnable = true : nmiEnable = false;
	(in&0x40)==0x40 ? ppuMode = true : ppuMode = false;
	(in&0x20)==0x20 ? spriteHeight = true : spriteHeight = false;
	(in&0x10)==0x10 ? backgroundPatternTable = true : backgroundPatternTable = false;
	(in&0x8)==0x8 ? spritePatternTable = true : spritePatternTable = false;
	(in&0x4)==0x4 ? vramIncrementMode = true : vramIncrementMode = false;

	if(nDebugging)
		cout << "nmiEnable( " << nmiEnable << ") ppuMode(" <<ppuMode<< ") spriteHeight(" << spriteHeight << ") backgroundPatternTable (" << backgroundPatternTable << ") spritePatternTable (" << spritePatternTable << ") vramIncrementMode (" << vramIncrementMode << ")" << endl;
	
	
	/*nmiEnable = in&0x80; 				//0=false, 1=true
	ppuMode = in&0x40;					//0=read from ext, 1=output to ext
	spriteHeight = in&0x20;				//0=8x8, 1=8x16
	backgroundPatternTable = in&0x10;	//0=0000h, 1=1000h
	spritePatternTable = in&0x8;		//0=0000h, 1=1000h
	vramIncrementMode = in&0x4; 		//0= add 1, going across, 1= add 32, going down*/
	baseNametableAddress = in&0x3; 		//(0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
	
	
}

void PPU::writePPUMASK(byte in){
	if(nDebugging)
		cout << endl << "PPUMASK Write: " << hex << (int)in << endl;
	
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
	vblankStatus = false;
	byte result = (int)vblank*0x80 + (int)sprite0hit*0x40 + (int)spriteOverflow*0x20;
	//cout << "PPUStatus Read: " << hex << (int)result;
	return result;
	
}

void PPU::writeOAMADDR(byte in){
	//cout << "OAM address written. was " << hex << (int)oamAddress;
	oamAddress = in;
	//cout << ", now " << hex << (int)oamAddress << endl;
	
}

void PPU::writeOAMDATA(byte in){
	oamValue = in;
	oamMemory[oamAddress] = oamValue;
	//cout << "Writing " << hex << (int)in << " to OAM address " << hex << oamAddress << endl;
}
	
byte PPU::readOAMDATA(){
	cout << "Reading out value " << hex << (int)oamMemory[oamAddress] << " from oam:" << oamAddress;
	return oamMemory[oamAddress];
}

void PPU::writePPUSCROLL(byte in){
	if(nDebugging)
		cout << endl << "PPUSCROLL write: " << hex << (int)in << endl;
	if(scrollFirstWrite)
		ppuscroll_scrollPosX = in;
	else
		ppuscroll_scrollPosY = in;
		
	scrollFirstWrite = !scrollFirstWrite;
}
	
void PPU::writePPUADDR(byte in){
	if(nDebugging)
		cout << endl << "PPUADDR write: " << hex << (int)in << endl;
	if(addrFirstWrite)
		ppuaddr_upperByte = in;
	else{
		ppuaddr_lowerByte = in;
		if(nDebugging){
			cout << "VRAM address changed from " << vramAddr;
			vramAddr =  ppuaddr_lowerByte| (ppuaddr_upperByte << 8);	
			cout << " to " << vramAddr <<endl;
		} else
			vramAddr =  ppuaddr_lowerByte| (ppuaddr_upperByte << 8);
	}
		
	addrFirstWrite = !addrFirstWrite;
	
	
}

void PPU::writePPUDATA(byte in){
	//TODO PPUDATA WRITE
	if(nDebugging){
	
		cout << endl << "PPUDATA WRITE: " <<  hex << (int)in << " to vram addr " << hex << vramAddr << endl;
	
	}
	
	ppuWriteMem(vramAddr,in);
	if(vramIncrementMode)
		vramAddr+=32;
	else
		vramAddr+=1;
}

byte PPU::readPPUDATA(){
	//TODO PPUDATA READ
	
	byte out = ppuReadMem(vramAddr);
	if(nDebugging)
		cout << endl << "PPUDATA READ: " <<  hex << (int)out << " from vram addr " << hex << vramAddr << endl;
	if(vramIncrementMode)
		vramAddr+=32;
	else
		vramAddr+=1;
	return out;
}

void PPU::writeOAMDMA(){
	//TODO OAM DATA WRITE
	//cout << "Write OAMDMA has been called" << endl;
}

void PPU::clearSecOAM(){
	for(int i = 0; i<0x20; i++){
		//cout << "Clearing "
		oamMemorySec[i] = 0;
	}
	spriteIndex=0;
}

//returns 0 for none, 1 for left, 2 for right, 3 for both.
int PPU::fetchTilePixel(int tileID, int scanL, int cyc, bool ptHalf){
	int pixelValue = 0;
	uint16_t ppuTileLineAddress;
	for(int half = 0; half<=1; half++){
	//Possibly spritePatternTable
		ppuTileLineAddress = ptHalf*0x1000 + floor(tileID/16)*0x100 + (tileID%16)*0x10 + (half*0x8) + (scanL%8);
		
		byte bitmask = 0x80>>(cyc%8);
			
		if((ppuReadMem(ppuTileLineAddress) & bitmask) != 0)
			pixelValue += half+1;
		
	}
	return pixelValue;
}

int PPU::fetchSpritePixel(int tileID, int scanL, int cyc, bool ptHalf, byte attributes){
	int pixelValue = 0;
	uint16_t ppuTileLineAddress;
	for(int half = 0; half<=1; half++){
	//Possibly spritePatternTable
		ppuTileLineAddress = ptHalf*0x1000 + floor(tileID/16)*0x100 + (tileID%16)*0x10 + (half*0x8) + (scanL%8);
		byte bitmask;
		if((attributes&0x40) > 0)
			bitmask = 0x80>>(7-cyc%8);
		else
			bitmask = 0x80>>(cyc%8);
			
		if((ppuReadMem(ppuTileLineAddress) & bitmask) != 0)
			pixelValue += half+1;
		
	}
	return pixelValue;
}

bool PPU::getNMI(){
	return nmiEnable;
}