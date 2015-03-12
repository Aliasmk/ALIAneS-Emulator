#include "System.hpp"
#include "CPU.cpp"


CPU nesCPU;
//PPU nesPPU;
//APU newAPU;

System::System(){
	init();
	openCartridge("../res/dk.nes");
	run();
}

int main(){
	System nes;
	return 0;
}	

void System::run(){
	while(nesCPU.running && getPowerState()){
		tick();
	}	
}

void System::init(){
	tickCount=0;
	nesCPU.start();
	setPowerState(true);
}

void System::tick(){
	nesCPU.cycle();
	tickCount++;
}

void System::setPowerState(bool state){
	powerState=state;
}
bool System::getPowerState(){
	return powerState;
}
void System::sendReset(){
	unloadCartridge(currentCart);
	setPowerState(false);
	init();
}

void System::openCartridge(std::string cartPath){
	currentCart.open(cartPath, std::ios::binary);
	//std::string rom;
	//currentCart >> rom
	
	//currentCart >> rom;
	//std::cout << rom;
	
	
	//MAIN TODO: Implement a function to parse the header of the NES file, then sort out 
	//what memory on the cartridge actually gets written to the CPU. As it stands right now, 
	//I'm trying to write CHARACTER DATA to the CPU where the PROGRAM should go.
	//Mappers are a Gold goal, so for now game must have a maximum of 32k prg mem. Thus,
	//all that needs to be done is to read from the header is how many blocks of 16k memory
	//it has. This will be be either 1 or 2. If its 1, it gets read into 8000 and MIRRORED
	//at C000. If its two, it starts at 8000 and ends at FFFF. 
	//TODO Implement functions based on header, which actually does NOT get read into memory apparently.
	//Info: http://wiki.nesdev.com/w/index.php/NES_2.0
	std::ifstream in(cartPath, std::ifstream::ate | std::ifstream::binary);
    int length = in.tellg(); 
    in.close();
	
	if(currentCart){
		if(length < 0xBFE0){
			std::cout << "ROM Import Begins:" <<std::endl;
			std::uint8_t tempBit;
			int offset = 0;
			while(!currentCart.eof()){
				nesCPU.writeMem(0x8000+offset,(byte)currentCart.get());
				offset++;
			}
			std::cout << "ROM Import Complete.";
			
		}else{
			std::cout << "ROM File too big. Wrappers NOT supported." << std::endl;
		}
	}
	else{
		std::cout << "ROM Import Failed: File unreadable"<< std::endl;
	}
	
	
	
	if(nesCPU.readMem(0x8000) == 'N' && nesCPU.readMem(0x8001) == 'E' && nesCPU.readMem(0x8002) == 'S')
		std::cout << "Verified!" << std::endl;
	else
		std::cout << "Unverified!"<< std::endl;
}
void System::unloadCartridge(std::ifstream& cartFile){
	currentCart.close();
	//currentCart = null;
}


		
	