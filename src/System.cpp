/* System Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "System.hpp"
#include "CPU.hpp"
#include "Cartridge.hpp"

CPU nesCPU;
//PPU nesPPU;
//APU nesAPU;
Cartridge* cart;

System::System(){
	cart = new Cartridge("../res/dk.nes", nesCPU);
	init();	
	run();
}

int main(){
	System nes; //Creates an instance of the system, starting the program.
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