#ifndef NES_CARTRIDGE
#define NES_CARTRIDGE

#include <fstream>
#include <string>
#include <iostream>
#include "CPU.hpp"

class Cartridge {
	typedef std::uint8_t byte;
	
	private:
		void parseHeader();
		std::ifstream currentCart;	
		void openCartridge(std::string path, CPU& nesCPU);
		byte header[0xf];
		
		bool verified;
		short PRGsize16x;
		short CHRsize8x;
		struct flag6;
		struct flag7;
	
	public:
		Cartridge(std::string cartPath, CPU& nesCPU);
		void unloadCartridge();	
};
#endif