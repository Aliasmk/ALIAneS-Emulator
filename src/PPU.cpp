/* PPU Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "PPU.hpp"
#include <math.h>
#include <time.h>
#include <assert.h>
using namespace std;

typedef uint8_t byte;

bool nDebugging = false;

SDLrender* SDLrenderer;

PPU::PPU(){
	cout << "NES PPU is now on." <<endl;
	for(int i = 0; i<0x3FFF; i++){
		ppuWriteMem(i, 0);
	}
}

void PPU::start(SDLrender* r){
	SDLrenderer = r;
	startTime = time(0);
	spriteIndex=0;
	frame=0;
	vramAddr = 0;
	vramAddrTemp = 0;
	writeToggle = false;
	vramIncrementMode = false;
	
	addrFirstWrite = true;
	vblank = false;
	sprite0hit=false;
	showSprites=true;
	showBackground=true;
	showLeftSprites=true;
	showLeftBackground=true;
	ppuscroll_scrollPosX = 0;
	ppuscroll_scrollPosY = 0;
	scrollFirstWrite=true;
	readBuffer = 0;
	
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

//MEMORY FUNCTIONS

//WRITING
void PPU::ppuWriteMem(int address, byte value){
	address = address%0x4000;
	assert(address <= 0x3FFF);
	//1 = vert mirror 
	//0 = horiz mirror
	int offset;
	//cout << "Writing " << hex << (int)value << " to PPU vram " << address << endl;
	//Nametable Mirroring
	if(address >= 0x2000 && address < 0x2400){
		offset = address-0x2000;
		ppuMemory[address] = value;
		if(mirrorMode == 0){ //Horiz Mirror
			ppuMemory[0x2400+offset] = value;
		} else if(mirrorMode == 1){ //Vert Mirror
			ppuMemory[0x2800+offset] = value;
		}
		
	} else if(address >= 0x2400 && address < 0x2800){
		offset = address-0x2400;
		ppuMemory[address] = value;
		if(mirrorMode == 0){ //Horiz Mirror
			ppuMemory[0x2000+offset] = value;
		} else if(mirrorMode == 1){ //Vert Mirror
			ppuMemory[0x2C00+offset] = value;
		}
		
	} else if ( address >= 0x2800 && address < 0x2C00 ){
		offset = address-0x2800;
		ppuMemory[address] = value;
		if(mirrorMode == 0){ //Horiz Mirror
			ppuMemory[0x2C00+offset] = value;
		} else if(mirrorMode == 1){ //Vert Mirror
			ppuMemory[0x2000+offset] = value;
		}
		
	} else if ( address >= 0x2C00 && address < 0x3000){
		offset = address-0x2C00;
		ppuMemory[address] = value;
		if(mirrorMode == 0){ //Horiz Mirror
			ppuMemory[0x2800+offset] = value;
		} else if(mirrorMode == 1){ //Vert Mirror
			ppuMemory[0x2400+offset] = value;
		}
	} else {
		ppuMemory[address] = value;
	}
	
	//Memory Mirroring 
	//TODO What is this
	if(address >= 0x2000 && address <= 0x2EFF){
		offset = address - 0x2000;
		ppuMemory[0x3000+offset] = value;
	}
	
	if(address >= 0x3F00 && address <= 0x3F1F){
		offset = address - 0x3F00;
		ppuMemory[0x3F20+offset]=value;
	}
	
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

//READING
//TODO memory access takes 2 cycles? http://wiki.nesdev.com/w/index.php/PPU_rendering
byte PPU::ppuReadMem(int address){
	//address = address % 0x4000;
	assert(address <= 0x3FFF);
	
	//System Memory Mirroring
	//if(address >= 0x0 && address < 0x2000){
	//	address &= 0x07FF;
	//}
	
	return ppuMemory[address];	
}
byte PPU::ppuReadOAM(int address){
	return oamMemory[address];
}
byte PPU::ppuReadSecOAM(int address){
	return oamMemorySec[address];
}




//READING AND WRITING OF PPU REGISTERS

//These are public methods that will usually be called by the CPU
//condition ? expression1 : expression2
void PPU::writePPUCTRL(byte in){
	if(nDebugging)
		cout << endl << "PPUCTRL Write: " << hex << (int)in << endl;
	
	//DEBUG
	bool prevVram = vramIncrementMode;
	
	(in&0x80)==0x80 ? nmiEnable = true : nmiEnable = false;
	(in&0x40)==0x40 ? ppuMode = true : ppuMode = false;
	(in&0x20)==0x20 ? spriteHeight = true : spriteHeight = false;
	(in&0x10)==0x10 ? backgroundPatternTable = true : backgroundPatternTable = false;
	(in&0x8)==0x8 ? spritePatternTable = true : spritePatternTable = false;
	(in&0x4)==0x4 ? vramIncrementMode = true : vramIncrementMode = false;

	if(nDebugging)
		cout << "nmiEnable( " << nmiEnable << ") ppuMode(" <<ppuMode<< ") spriteHeight(" << spriteHeight << ") backgroundPatternTable (" << backgroundPatternTable << ") spritePatternTable (" << spritePatternTable << ") vramIncrementMode (" << vramIncrementMode << ")" << endl;
	
	/*if(vramIncrementMode != prevVram){
		cout << "***** VRAM INCREMENT MODE IS NOW " << vramIncrementMode << endl;
	}*/
	
	/*nmiEnable = in&0x80; 				//0=false, 1=true
	ppuMode = in&0x40;					//0=read from ext, 1=output to ext
	spriteHeight = in&0x20;				//0=8x8, 1=8x16
	backgroundPatternTable = in&0x10;	//0=0000h, 1=1000h
	spritePatternTable = in&0x8;		//0=0000h, 1=1000h
	vramIncrementMode = in&0x4; 		//0= add 1, going across, 1= add 32, going down*/
	baseNametableAddress = in&0x3; 		//(0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
	
	//t: ...BA.......... = d: ......BA
	vramAddrTemp &= ~(0xC00);
	vramAddrTemp |= (in&0x3)<<10;
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
	
	byte result = (int)vblank*0x80 + (int)sprite0hit*0x40 + (int)spriteOverflow*0x20;
	vblank = false;
	writeToggle=false;
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
	//cout << "Reading out value " << hex << (int)oamMemory[oamAddress] << " from oam:" << oamAddress;
	return oamMemory[oamAddress];
}

void PPU::writePPUSCROLL(byte in){
		if(writeToggle==false){ //First write
			/*
			t: ....... ...HGFED = d: HGFED...
			x:              CBA = d: .....CBA
			w:                  = 1
			*/
			
			vramAddrTemp &= ~(0x1F);
			vramAddrTemp |= (in&0xF8)>>3;
			fineXScroll = in&7;
			writeToggle=true;
			
			//FIXME remove
			//ppuscroll_scrollPosX = in;
			//cout << endl << "PPUSCROLL X write: " << hex << floor((int)ppuscroll_scrollPosX/8) ;
		}else{
			/*
				t: CBA..HGFED..... = d: HGFEDCBA
				w:                  = 0
			
			*/
			vramAddrTemp &= ~(0x73E0);
			vramAddrTemp |= ((in&7)<<12) ;
			vramAddrTemp |= ((in&0xF8)<<2);

			writeToggle=false;
			
			
			//FIXME remove
			//ppuscroll_scrollPosY = in;
			//cout << endl << "PPUSCROLL Y write: " << hex << floor((int)ppuscroll_scrollPosY/8) ;
		}	
		//cout << " at cycle " << dec << (int)cycles << ", scanline " << (int)scanLine << " frame " << (int)frame << endl;
		//FIXME remove
		//scrollFirstWrite = !scrollFirstWrite;
}
	
//METHOD WORKS - SAME VALUES
void PPU::writePPUADDR(byte in){
	if(nDebugging)
		cout << endl << "PPUADDR write: " << hex << (int)in << endl;
	if(writeToggle==false){
		/*
			t: .FEDCBA ........ = d: ..FEDCBA
			t: X...... ........ = 0
			w:                  = 1
		*/	
		vramAddrTemp &= ~(0x3F00);
		vramAddrTemp |= ((in&0x3f)<<8);
		//vramAddrTemp &= ~(1<<14);
		writeToggle=true;
		
		//FIXME remove
		//ppuaddr_upperByte = in;
	}else{
		/*
			t: ....... HGFEDCBA = d: HGFEDCBA
			v                   = t
			w:                  = 0
		*/
		vramAddrTemp &= ~(0xFF);
		vramAddrTemp |= in;vramAddr=vramAddrTemp;
		
		vramAddr = vramAddr%0x4000;
		//FIXME vram set assertion
		assert(vramAddr <= 0x3FFF);
		if(nDebugging){
			cout << "VRAM address changed to " << vramAddr;
		}
		writeToggle=false;
		
		//FIXME remove
		/*ppuaddr_lowerByte = in;
		if(nDebugging){
			cout << "VRAM address changed from " << vramAddr;
			vramAddr =  ppuaddr_lowerByte| (ppuaddr_upperByte << 8);	
			cout << " to " << vramAddr <<endl;
		} else {
			vramAddr =  ppuaddr_lowerByte| (ppuaddr_upperByte << 8);
		}*/
	}
	//FIXME remove
	//addrFirstWrite = !addrFirstWrite;	
}

void PPU::writePPUDATA(byte in){
	
	int writeLocation = vramAddr % 0x4000;
	assert(writeLocation <= 0x3FFF);
	
	//TODO PPUDATA WRITE
	if(nDebugging){
	
		//cout << endl << "PPUDATA WRITE: " <<  hex << (int)in << " to vram addr " << hex << writeLocation << endl;
	
	}
	
	if(in == 0x4B || in == 0x4F)
			cout << endl << "PPUDATA WRITE: " <<  hex << (int)in << " to vram addr " << hex << writeLocation << endl;
	
	//FIXME vram write assertion
	
	ppuWriteMem(writeLocation,in);
	
	if(vramIncrementMode)
		vramAddr+=32;
	else
		vramAddr+=1;
}

byte PPU::readPPUDATA(){
	//TODO PPUDATA READ
	
	int readLocation = vramAddr % 0x4000;
	assert(readLocation <= 0x3FFF);
	
	byte temp = ppuReadMem(readLocation);
	byte out;
	if(vramAddr < 0x3F00){
		out = readBuffer;
		readBuffer = temp;
	
	} else {
		out = temp;
		readBuffer = temp;
	}
	
	if(nDebugging)
		cout << endl << "PPUDATA READ: " <<  hex << (int)out << " from vram addr " << hex << readLocation << endl;
	if(vramIncrementMode)
		vramAddr+=32;
	else
		vramAddr+=1;
	return out;
}

void PPU::writeOAMDMA(){
	//Done on CPU
	//cout << "Write OAMDMA has been called" << endl;
}

void PPU::clearSecOAM(){
	for(int i = 0; i<0x20; i++){
		//cout << "Clearing "
		oamMemorySec[i] = 0;
	}
	spriteIndex=0;
}



//MAIN PPU CYCLE FUNCTION

void PPU::cycle(){
	
	//VBLANK scanlines
	//Overrides @ 241,1 and 261,1
	if(scanLine == 241 && cycles==1){
		vblank = true;
		vblankStatus = true;
	}
	
	
	
	//SCROLLING RELATED
	
	//At Dot 256 the PPU increments the vertical position in v
	if(cycles == 256 && (showSprites || showBackground)){
		//Y incrementing
		//cout << showSprites << " - " << showBackground << endl;
		if((vramAddr & 0x7000) != 0x7000){ //fine y < 7
			//cout << (vramAddr & 0x7000) << endl;
			vramAddr += 0x1000; //Add one to fine y
		} else {
			vramAddr &= ~0x7000; //fine y = 0;
			int coarseY = (vramAddr & 0x03E0) >> 5; //y is coarse y
			//cout << "sL " << scanLine << " - cY " << coarseY << endl;
			if(coarseY == 29){
				coarseY = 0;
				vramAddr ^= 0x0800; //switch vertical nametable
			} else if( coarseY == 31 ){
				coarseY = 0;
			} else {
				coarseY++;
			}
			vramAddr = (vramAddr & ~0x03E0) | (coarseY << 5 );	
		}
	}
	
	//At dot 257 PPU copies all bits related to horizontal position from t to v:
	if(cycles == 257 && (showSprites || showBackground)){
		//Copy horizontal position bits
		//v: .....F.. ...EDCBA = t: ....F.. ...EDCBA
		
		vramAddr &= ~(0x041F);
		vramAddr |= (vramAddrTemp&0x041F);
		
		//vramAddr = vramAddrTemp;	
		
	}
	
	//During dots 280 to 304 of the pre-render scanline at the end of vblank, shortly after the horizontal bits
	//are copied from t to v at dot 257, the PPU will repeatedly copy the vertical bits from t to v from dots 280 to 304
	if(scanLine == 261 && cycles >= 280 && cycles <= 304 && (showSprites || showBackground)){
		//copy vertical bits
		//v: .IHGF.ED CBA..... = t: .IHGF.ED CBA.....
		vramAddr &= ~(0x7BE0);
		vramAddr |= (vramAddrTemp&0x7BE0);
	}
	
	
	if(scanLine < 240 && cycles <= 248 && (showSprites || showBackground)){
		if(cycles%8 == 0){
			if((vramAddr & 0x001F) == 31){
				vramAddr &= ~0x001F; //Coarse X = 0
				vramAddr ^= 0x0400;	 //Switch nametables
			} else {
				vramAddr += 1;	//increment coarse x
			}	
		}
	}
	
	
	if(scanLine == 261 && cycles==1){
		vblank = false;
		vblankStatus = false;
		vblankSeen = false;
		sprite0hit = false;
		spriteOverflow = false;
	}
	
	
	
	if(cycles <= 340)
		cycles++;
	else {
		//End of scanline
		cycles = 0;
		//Increment scanline or increment frame
		if(scanLine <= 261){
			//Spite evaluation on each scanline beginning 
			clearSecOAM();
			for(int i = 0; i<64;i++){
				if(spriteIndex > 7) break;
				if(scanLine+1-(ppuReadOAM(i*4))<8 && scanLine+1-(ppuReadOAM(i*4))>=0 && scanLine < 240){
					for(int n = 0; n<4; n++){
						ppuWriteSecOAM((spriteIndex*4)+n, ppuReadOAM((i*4)+n));
					}
					spriteIndex++;
				}
			}
			//Increment Scanline
			scanLine++;
		}else{
			//End of frame
			scanLine = 0;
			frame++;
			frameEnd = true;
			SDLrenderer->onFrameEnd();
		}
	}
	
	int tileAddr = (0x2000 | (vramAddr& 0x0FFF));

	//int tileAddr = 0x2000+(0x20*floor(scanLine/8))+floor(cycles/8);
	//if(frame > 400)
	// 	cout << "finex: " << hex << (int)fineXScroll << endl;
		
	int tileID = ppuReadMem(tileAddr);
	int nextTileID = ppuReadMem(tileAddr+1);
	
	//cout << hex << "vram @ " << cycles << ", " << scanLine << ", " << frame <<" -> " << vramAddr <<" -> tile addr -> " << tileAddr << " -> tile -> " << tileID << endl;
	
	//TODO implement palette
	int color;
	
	if(showBackground){
		//cout << dec <<" frame " << frame << " sl " << scanLine << " dot " << cycles << hex << "tileAddr " << (0x2000 | (vramAddr& 0x0FFF)) << endl;
		color = fetchTilePixel(tileID, nextTileID, scanLine, cycles, backgroundPatternTable);
		
		//color = tileID;
	} else {
		color = 0; //do not draw background
	}
	
	if(!showSprites || (cycles<8 && showLeftSprites)){
		; //do not draw sprites
	} else {
		int spriteColor;
		for(int i = 0; i<= spriteIndex; i++){
			if(cycles-ppuReadSecOAM(i*4+3)<8 && cycles-ppuReadSecOAM(i*4+3)>=0){
				spriteColor = fetchSpritePixel(ppuReadSecOAM(i*4+1),scanLine-ppuReadSecOAM(i*4), cycles-ppuReadSecOAM(i*4+3), spritePatternTable, ppuReadSecOAM(i*4+2));
				//Sprite zero hit detection: uses the background color value just assigned to 
				//check if the sprite has drawn an opaque pixel over a opaque background pixel
				if(i==0 && color!=0 && spriteColor!=0){
					//Sprite 0 hit detected
					//cout << "Sprite 0 hit" << endl;
					sprite0hit=true;
				}
			
				if(spriteColor != 0){
					color = spriteColor;
				}
			}	
		
		
		}
	}
	color = color*85;
	
	ppuR = color;
	ppuG = color;
	ppuB = color; 
	
	/*if((0x2000 | (vramAddr& 0x0FFF)) > 0x2400){
		if(cycles%8 == 0){
			ppuR = 0xFF;
		}
	}
	
	if((0x2000 | (vramAddr& 0x0FFF)) > 0x2800){
		if(cycles%8 == 2){
			ppuG = 0xFF;
		}
	}
	
	if((0x2000 | (vramAddr& 0x0FFF)) > 0x2C00){
		if(cycles%8 == 4){
			ppuB = 0xFF;
		}
	}*/
	
	SDLrenderer->setNextColor(ppuR,ppuG,ppuB);
	SDLrenderer->setDrawLoc(cycles, scanLine);
	SDLrenderer->renderPixelCallback();
	
}


//returns pixel value - 0 for none, 1 for left, 2 for right, 3 for both.
int PPU::fetchTilePixel(int tileID, int nextTileID, int scanL, int cyc, bool ptHalf){
	int pixelValue = 0;
	uint16_t ppuTileLineAddress;
	int tempfineXScroll = fineXScroll;
	
	//SCROLLING WORKS HELL YEAH
	
	//This checks if the current pixel has passed the end of the first tile. If it does, it loads the next tile so that the offset begins at this boundary.
	if(((cyc-1)%8)>(7-tempfineXScroll)){
			tileID = nextTileID;
			cyc = ((cyc-1)%8)-(7-tempfineXScroll);
			tempfineXScroll = 0;
			
	} 

	for(int half = 0; half<=1; half++){
		int fineY = (vramAddr&0x7000)>>12;
		ppuTileLineAddress = ptHalf*0x1000 + floor(tileID/16)*0x100 + (tileID%16)*0x10 + (half*0x8) + fineY;// + (scanL%8);
		
		
		
		byte bitmask = 0x80>>(((cyc-1)%8)+tempfineXScroll);
			
		if((ppuReadMem(ppuTileLineAddress) & bitmask) != 0)
			pixelValue += half+1;
		
	}
	return pixelValue;
}

int PPU::fetchSpritePixel(int tileID, int scanL, int cyc, bool ptHalf, byte attributes){
	int pixelValue = 0;
	uint16_t ppuTileLineAddress;
	for(int half = 0; half<=1; half++){
		
		ppuTileLineAddress = ptHalf*0x1000 + floor(tileID/16)*0x100 + (tileID%16)*0x10 + (half*0x8);
		
		if((attributes&0x80)>0)
		 	ppuTileLineAddress += 7-(scanL%8);
		else
			ppuTileLineAddress += (scanL%8);
		
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

bool PPU::getVBlank(){
	return vblank;
}

void PPU::setMirrorMode(int mode){
	mirrorMode = mode;
}