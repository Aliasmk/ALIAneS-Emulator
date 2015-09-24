/* System Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "System.hpp"
#include "CPU.hpp"
#include "Cartridge.hpp"

using namespace std;

CPU nesCPU;
//PPU nesPPU;
//APU nesAPU;
Cartridge* cart;

string game = "../res/nestest.nes";

System::System(){
	cart = new Cartridge(game, nesCPU);
	init();	
	run();	
}

System::System(string customCart){
	cart = new Cartridge(customCart, nesCPU);
	init();	
	run();	
}

int main2(){ //Was Main before the Shell was implemented
	System nes; //Creates an instance of the system, starting the program.
	//nes.run();
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

CPU System::getCPU(){
	return nesCPU;
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