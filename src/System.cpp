/* System Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "System.hpp"
#include "CPU.hpp"
#include "PPU.hpp"
#include "Cartridge.hpp"
#include "SDLrender.hpp"


using namespace std;

CPU nesCPU;
PPU nesPPU;
//APU nesAPU;
Cartridge* cart;
SDLrender* renderer;

string game;

System::System(){
	if(init()){
		run();
	}
}

/*System::System(string customCart){
	cart = new Cartridge(customCart, nesCPU);
	init();
	run();
}*/

/*
int main2(){ //Was Main before the Shell was implemented
	System nes; //Creates an instance of the system, starting the program.
	//nes.run();
	return 0;
}	*/

void System::loadConfig(){
	//System config file lists (each on its on line): Cartridge Path, Start Address, Number of Cycles
	string configPath = "../res/alianes.cfg";

	ifstream config;
	string cartPath;
	int startAddress,cycles;
	config.open(configPath);
	if(!config){
		cout << "No config file" << endl;
		config.close();
		ofstream configCreate;
		configCreate.open(configPath);
		configCreate << "../res/dk.nes" << endl << "0" << endl << "20000";
		configCreate.close();
		config.open(configPath);
	}
		config >> cartPath >> startAddress >> cycles;
		config.close();

	game = cartPath;
	getCPU().setConfig(startAddress,cycles);
}


bool System::init(){
	bool status = true;

	loadConfig();

	cart = new Cartridge(game, nesCPU, nesPPU);
	if(!cart->isValid())
		status = false;

	tickCount=0;
	
  	renderer = new SDLrender(); //TODO destroy
	renderer->initSDL();
	nesPPU.start(getRenderer());
	nesCPU.start();
	setPowerState(true);

	return status;
}
void System::run(){

	while(nesCPU.running && getPowerState()){
		tick();
	}
	renderer->closeSDL();
}
void System::tick(){
	//Run CPU then run PPU 3 times.
	nesCPU.cycle();
	nesPPU.cycle();
	nesPPU.cycle();
	nesPPU.cycle();
	tickCount++;
}

CPU System::getCPU(){
	return nesCPU;
}

PPU System::getPPU(){
	return nesPPU;
}

SDLrender* System::getRenderer(){
	return renderer;
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
