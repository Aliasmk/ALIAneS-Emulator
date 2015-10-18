/* PPU Header :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#ifndef NESPPU
#define	NESPPU

#include <iostream>

class PPU {
	private:
	typedef std::uint8_t byte;
	
	//MEMORY//
	
	//PPU's internal memory contains pattern data, nametables and other information.
	byte ppuMemory[0x3FFF];
	//OAM memory contains information about how sprites are rendered.
	byte oamMemory[0xFF];
	
	
	//STATE TRACKING//
	
	//Cycles range from 0-340 (x), then increments scanline up to 261 (y), then frame increments
	int cycles; 	//0 (idle), 1-256 (fetches nametable byte, attribute table byte, tile bitmap low, tile bitmap high [both colors])
				//Sprite evalutation is taking place for the next scan line at the same time.
				//257-320 (fetches 'garbage' nt and at byte, low high btmap value)
				//321-336 (fetches first two tiles for the next scanline
				//337-340 (unknown)
				//Note that palettes can only be changed every 8 bytes. 
				//
				//On each cycle the PPU renders one pixel, taken from the shift registers, this is why
				//two tiles worth are loaded in the dummy (pre render) scanline
	int scanLine; //Scanline 0-239 (visible), 240 (post, idle), 241-260 VBLANK (on cycle 1), 261 (pre-render, on tick 1 VBLANK clears)
	int frame;
	
	
	//RENDERING - http://wiki.nesdev.com/w/index.php/PPU_rendering//
	
	//Two nameTable addresses are held in memory and are read and written from.
	byte nameTable1;
	byte nameTable2;
	
	//Two '16 bit shift registers' that hold the data about one line of the next nametable value
	//Example: = {0x11, 0x10, 0x01, 0x00, 0x00, 0x01, 0x10, 0x11} 
	//these would be preassembled from the high bit and low bit of the bitmaps in the 
	//pattern table in the same place they are fetched. That way the renderer only sees the
	//concatenated values
	byte bitmapStorage1[8];
	byte bitmapStorage2[8];
	
	//Two '8 bit shift registers' that hold palette data. for now just two 8 bit vars, values can be grabbed with booleans.
	byte palette1;
	byte palette2;
	
	//REGISTERS - BROKEN DOWN from MSB to LSB//
	
	enum PPUregisters {
		PPUCTRL = 0x2000,
		PPUMASK = 0x2001,
		PPUSTATUS = 0x2002,
		OAMADDR = 0x2003,
		OAMDATA = 0x2004,
		PPUSCROLL = 0x2005,
		PPUADDR = 0x2006,
		PPUDATA = 0x2007,
		OAMDMA = 0x4014
	};
	
	//2000 - PPUCTRL - WRITE
	byte nmiEnable;
	byte ppuMode;
	byte spriteHeight;
	byte backgroundPatternTable;
	byte spritePatternTable;
	byte vramIncrementMode;
	
	
	//2001 - PPUMASK - WRITE
	byte emphasizeBlue;
	byte emphasizeGreen;
	byte emphasizeRed;
	byte showSprites;
	byte showBackground;
	byte showLeftSprites;
	byte showLeftBackground;
	byte greyscale;
	
	//2002 - PPUSTATUS - READ
	byte vblankStatus;
	byte sprite0hit;
	byte spriteOverflow;
	
	//2003 - OAMADDR - WRITE
	byte oamAddress;
	
	//2004 - OAMDATA - WRITE
	byte oamValue;
	
	//2005 - PPUSCROLL - WRITE 2x
	byte ppuscroll_scrollPosX;
	byte ppuscroll_scrollPosY;
	
	//2006 - PPUADDR - WRITE 2x
	byte ppuaddr_upperByte;
	byte ppuaddr_lowerByte;
	
	//2007 - PPUDATA - READ/WRITE
	byte ppudata_value;
	
	//4014 - OAMDMA - WRITE
	
	//Internal interface functions
	void writePPUCTRL();
	void writePPUMASK();
	byte readPPUSTATUS();
	void writeOAMADDR();
	void writeOAMDATA();
	byte readOAMDATA();
	void writePPUSCROLL();
	void writePPUADDR();
	void writePPUDATA();
	byte readPPUDATA();
	void writeOAMDMA();
	
	
	public:
	PPU();
	void start();
	void cycle();
	void writeMem(int address, byte value);
	byte readMem(int address);
	byte readRegister(int accessReg, int readWrite);


};
#endif
