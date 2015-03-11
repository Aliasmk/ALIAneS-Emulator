#include "System.hpp"
#include "CPU.cpp"


CPU nesCPU;
//PPU nesPPU;
//APU newAPU;

System::System(){
	init();
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
	//unloadCartridge(currentCart);
	setPowerState(false);
	init();
}

/*void System::openCartridge(File& cartFile){
}
void System::unloadCartridge(File& cartFile)
{
	cartFile.close();
	currentCart = null;
}*/


		
	