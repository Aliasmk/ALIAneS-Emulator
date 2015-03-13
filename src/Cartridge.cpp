#include "Cartridge.hpp"
#include "CPU.hpp"

struct Cartridge::flag6 {
	byte lowermap : 4;
	bool fourscreenmode : 1;
	bool trainer : 1;
	bool batterySRAM : 1;
	bool vertMirror: 1;
};
struct Cartridge::flag7 {
	byte uppermap : 4;
	bool : 2;
	bool PC10: 1;
	bool VS: 1;
};

Cartridge::Cartridge(std::string cartPath, CPU::CPU& nesCPU){
	openCartridge(cartPath, nesCPU);
}

void Cartridge::openCartridge(std::string cartPath, CPU::CPU& nesCPU){
	currentCart.open(cartPath, std::ios::binary);
	
	if(currentCart){
			parseHeader();
			std::cout << "ROM Import: Start " <<std::endl;
			std::cout << "ROM Import: ";
			if(PRGsize16x == 0x02){
				std::cout << "32K rom loaded." << std::endl;
				for(int offset = 0x0; offset<0x8000; offset++){
					nesCPU.writeMem(0x8000+offset,(byte)currentCart.get());
				}
			} else if(PRGsize16x == 0x01) {
				std::cout << "16K rom loaded. Mirroring" << std::endl;
				for(int offset = 0x0; offset<0x4000; offset++){
					nesCPU.writeMem(0x8000+offset,(byte)currentCart.get());
				}
				for(int offset = 0x0; offset<0x4000; offset++){
					nesCPU.writeMem(0xc000+offset,nesCPU.readMem(0x8000+offset));
				}
			}
			else {
				std::cout << "Error - ROM too large (>32k) or too small (8k)." << std::endl;
			}
			std::cout << "ROM Import: Complete."<<std::endl;
	}
	else{
		std::cout << "ROM Import: Failed - File unreadable"<< std::endl;
	}
}

//Read the 16 byte header at the top of the .NES file. 
void Cartridge::parseHeader(){
	for(int i = 0; i <= 0xf; i++){
		header[i]=currentCart.get();
	}	
	if(header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1A){
		verified = true;
		PRGsize16x=header[4];
		CHRsize8x=header[5];
		flag6 f6 = {header[6]};
		flag7 f7 = {header[7]};
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
