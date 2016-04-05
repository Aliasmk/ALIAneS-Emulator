/* Cartridge Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "Cartridge.hpp"



//Bitsets for flags 6 and 7 of the header line
struct Cartridge::flag6 {
	byte lowermap : 4; 			//Lower 4 bits of mapper number
	bool fourscreenmode : 1;	//Four screenmode
	bool trainer : 1;			//Trainer located in RAM?
	bool batterySRAM : 1;		//Is there a battery backed SRAM?
	bool vertMirror: 1;			//Horizontal (0) / Vertical (1) mirroring
};
struct Cartridge::flag7 {
	byte uppermap : 4;			//Upper 4 bits of mapper number
	bool : 2;					//Unused
	bool PC10: 1;				//PC10
	bool VS: 1;					//VS
};

Cartridge::Cartridge(std::string cartPath, CPU::CPU *nesCPU, PPU::PPU *nesPPU){
	openCartridge(cartPath, nesCPU, nesPPU);
	std::cout << "Cartridge is now loaded." << std::endl;
}

//Open the file stream, parse the header and based on that information, allocate CPU memory for program rom.
void Cartridge::openCartridge(std::string cartPath, CPU::CPU *nesCPU, PPU::PPU *nesPPU){
	validCart = false;
	//Open file as binary and check that it opened
	currentCart.open(cartPath, std::ios::binary);
	if(currentCart){
			parseHeader();
			std::cout << "ROM Import: Start " <<std::endl;
			std::cout << "ROM Import: ";
			//This branch determines the size of the rom PRG based on the header info
			if(PRGsize16x == 0x02){
				std::cout << "32K PRG loaded." << std::endl;
				for(int offset = 0x0; offset<0x8000; offset++){
					nesCPU->writeMem(0x8000+offset,(byte)currentCart.get());
				}
				validCart = true;
			} else if(PRGsize16x == 0x01) {
				std::cout << "16K PRG loaded. Mirroring" << std::endl;
				//Memory is written to both banks. 
				//TODO: Add an implicit function in the 
				//write mem function to mirror data automatically based on conditions,
				//such as writing to ram (4 times mirrored) or PRG with a 16k rom.
				for(int offset = 0x0; offset<0x4000; offset++){
					nesCPU->writeMem(0x8000+offset,(byte)currentCart.get());
				}
				for(int offset = 0x0; offset<0x4000; offset++){
					nesCPU->writeMem(0xc000+offset,nesCPU->readMem(0x8000+offset));
				}
				validCart = true;
			}
			else {
				std::cout << "Error - PRG-ROM too large (>32k) or too small (<16k)." << std::endl;
			}
			
			//Determines the size of the character memory 
			if(CHRsize8x == 0x01){
				std::cout << "8K CHR memory loaded." << std::endl;
				for(int offset = 0x0; offset<0x1FFF; offset++){
					nesPPU->ppuWriteMem(0x0+offset,(byte)currentCart.get());
				}
			} else {
				std::cout << "Error - CHR-ROM too large (>8k) or too small (<8k)." << std::endl;
			}
			
			std::cout << "ROM Import: Complete ("<< cartPath <<")"<<std::endl;
			if(getMirrorMode() == 1){
				std::cout << "Vertical Mirroring" << std::endl;
			} else if(getMirrorMode() == 0){
				std::cout << "Horizontal Mirroring" << std::endl;
			}
	}
	else{
		std::cout << "ROM Import: Failed - File unreadable ("<< cartPath << ")" <<std::endl;
	}
	currentCart.close();
}

//Read the 16 byte header at the top of the .NES file. 
void Cartridge::parseHeader(){
	//Import all header information into an array for easy access later
	for(int i = 0; i <= 0xf; i++){
		header[i]=currentCart.get();
	}	
	//Checks the first 4 bits for NES<eof> as rom verification, then continue
	if(header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1A){
		verified = true; 			//if NES<eof> begins the header
		PRGsize16x=header[4];		//Number of 16k blocks of program code
		CHRsize8x=header[5];		//Number of 8k blocks of character code
		flag6 f6 = {header[6]};		//bitfield for flag6. see struct above
		flag7 f7 = {header[7]};		//bitfield for flag7. see struct above
	}
	else{	
		//TODO implement errors not just quitting.
		verified = false;
		std::cout << "Not a valid NES rom file." << std::endl;
		std::exit(1);
	}	
}	

//close file streams, TODO:unload memory
void Cartridge::unloadCartridge(){
	currentCart.close();
	//currentCart = null;
}

//1 = vert mirror 0 = horiz mirror
int Cartridge::getMirrorMode(){
	return header[6]&0x1;
	//TODO 4 screen mode
}

bool Cartridge::isValid(){
	return validCart;
}
