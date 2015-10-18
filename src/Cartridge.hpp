/* Cartridge Header :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#ifndef NES_CARTRIDGE
#define NES_CARTRIDGE

#include <fstream>
#include <string>
#include <iostream>
#include "CPU.hpp"
#include "PPU.hpp"

class Cartridge {
	typedef std::uint8_t byte;
	
	private:
		void parseHeader();
		std::ifstream currentCart;	
		void openCartridge(std::string path, CPU& nesCPU, PPU& nesPPU);
		byte header[0xf];
		
		bool validCart;
		
		bool verified;
		short PRGsize16x;
		short CHRsize8x;
		struct flag6;
		struct flag7;
	
	public:
		Cartridge(std::string cartPath, CPU& nesCPU, PPU& nesPPU);
		void unloadCartridge();	
		
		
		bool isValid();
};
#endif