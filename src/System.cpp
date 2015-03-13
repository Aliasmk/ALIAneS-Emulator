#include "System.hpp"
#include "CPU.hpp"
#include "Cartridge.hpp"


CPU nesCPU;
//PPU nesPPU;
//APU newAPU;
Cartridge* cart;


System::System(){
	init();
	cart = new Cartridge("../res/mario.nes", nesCPU);
	run();
}

int main(){
	System nes;
	return 0;
}	
void System::init(){
	tickCount=0;
	nesCPU.start();
	
	setPowerState(true);
}
void System::run(){
	while(nesCPU.running && getPowerState()){
		tick();
	}	
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
	cart->unloadCartridge();
	delete cart;
	//cart=null;
	setPowerState(false);
	init();
}




		
	