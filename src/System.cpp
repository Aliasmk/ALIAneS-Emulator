#include "System.hpp"
#include "CPU.cpp"


CPU nesCPU;
//PPU nesPPU;
//APU newAPU;

System::System(){
	init();
	openCartridge("../res/mario.nes");
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
	
	std::ifstream in(cartPath, std::ifstream::ate | std::ifstream::binary);
    int length = in.tellg(); 
    in.close();
	
	if(currentCart){
		if(length < 0xBFE0){
			std::cout << "ROM Import Begins:" <<std::endl;
			std::uint8_t tempBit;
			int offset = 0;
			while(!currentCart.eof()){
				nesCPU.writeMem(0x4020+offset,(byte)currentCart.get());
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
	
	if(nesCPU.readMem(0x4020) == 'N' && nesCPU.readMem(0x4021) == 'E' && nesCPU.readMem(0x4022) == 'S')
		std::cout << "Verified!" << std::endl;
	else
		std::cout << "Unverified!"<< std::endl;
}
void System::unloadCartridge(std::ifstream& cartFile){
	currentCart.close();
	//currentCart = null;
}


		
	