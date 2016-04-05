/* System Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "System.hpp"
#include "Cartridge.hpp"
#include <SDL2/SDL.h>


using namespace std;


//Keybindings
const int NES_START	= SDLK_SPACE;
const int NES_SELECT = SDLK_RETURN;
const int NES_UP = SDLK_w;
const int NES_DOWN = SDLK_s;
const int NES_LEFT = SDLK_a;
const int NES_RIGHT = SDLK_d;
const int NES_A = SDLK_j;
const int NES_B = SDLK_k;

CPU* nesCPU;
PPU* nesPPU;
//APU nesAPU;
Cartridge* cart;
SDLrender* renderer;

string game;

System::System(){
	cout << "NES System is now running." <<endl;
	for(int i = 0; i < 8; i++){
		keysPressed[i] = 0;
	}
	if(init()){
		run();
	}
	
	
	delete nesCPU;
	nesCPU = NULL;
	nesPPU->stop();
	delete nesPPU;
	nesPPU = NULL;
	renderer->closeSDL();
	delete renderer;
	renderer = NULL;
	delete cart;
	cart = NULL;
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
	getCPU()->setConfig(startAddress,cycles);
}


bool System::init(){
	bool status = true;
	nesPPU = new PPU();
	nesCPU = new CPU(nesPPU);
	
	
	renderer = new SDLrender();
	renderer->initSDL();
	
	loadConfig();
	cart = new Cartridge(game, nesCPU, nesPPU);
	if(!cart->isValid())
		status = false;
  	
	nesPPU->start(getRenderer());
	nesPPU->setMirrorMode(cart->getMirrorMode());
	nesCPU->start();
	
	setPowerState(true);
	tickCount=0;
	
	
	return status;
}
void System::run(){
	while(nesCPU->running && getPowerState()){
		if(nesPPU->frameEnd == true){
			getInput();
			nesPPU->frameEnd = false;
		}
		
		tick();
		
	}
}

void System::getInput(){
	SDL_Event event;
	//A, B, Select, Start, Up, Down, Left, Right is the update order.
        while ( SDL_PollEvent(&event) ) { 
       		if ( event.type == SDL_KEYDOWN && event.key.repeat == 0){
            	if ( event.key.keysym.sym == NES_START ){
            			cout << "START DOWN" <<endl; 
            			keysPressed[3]=1;
            	}
            	if ( event.key.keysym.sym == NES_SELECT ){
            			cout << "SELECT DOWN" <<endl; 
            			keysPressed[2]=1;
            	}
            	if ( event.key.keysym.sym == NES_UP ){
            			cout << "UP DOWN" <<endl; 
            			keysPressed[4]=1;
            	}
            	if ( event.key.keysym.sym == NES_DOWN ){
            			cout << "DOWN DOWN" <<endl; 
            			keysPressed[5]=1;
            	}
            	if ( event.key.keysym.sym == NES_LEFT ){
            			cout << "LEFT DOWN" <<endl;
            			keysPressed[6]=1; 
            	}
            	if ( event.key.keysym.sym == NES_RIGHT ){
            			cout << "RIGHT DOWN" <<endl; 
            			keysPressed[7]=1;
            	}
            	if ( event.key.keysym.sym == NES_A ){
            			cout << "A DOWN" <<endl; 
            			keysPressed[0]=1;
            	}
            	if ( event.key.keysym.sym == NES_B ){
            			cout << "B DOWN" <<endl; 
            			keysPressed[1]=1;
            	}
        	}
        	
        	if ( event.type == SDL_KEYUP && event.key.repeat == 0){
            	if ( event.key.keysym.sym == NES_START ){
            			cout << "START UP" <<endl; 
            			keysPressed[3]=0;
            	}
            	if ( event.key.keysym.sym == NES_SELECT ){
            			cout << "SELECT UP" <<endl; 
            			keysPressed[2]=0;
            	}
            	if ( event.key.keysym.sym == NES_UP ){
            			cout << "UP UP" <<endl; 
            			keysPressed[4]=0;
            	}
            	if ( event.key.keysym.sym == NES_DOWN ){
            			cout << "DOWN UP" <<endl; 
            			keysPressed[5]=0;
            	}
            	if ( event.key.keysym.sym == NES_LEFT ){
            			cout << "LEFT UP" <<endl; 
            			keysPressed[6]=0;
            	}
            	if ( event.key.keysym.sym == NES_RIGHT ){
            			cout << "RIGHT UP" <<endl; 
            			keysPressed[7]=0;
            	}
            	if ( event.key.keysym.sym == NES_A ){
            			cout << "A UP" <<endl; 
            			keysPressed[0]=0;
            	}
            	if ( event.key.keysym.sym == NES_B ){
            			cout << "B UP" <<endl; 
            			keysPressed[1]=0;
            	}
        	}
        	
       		if ( event.type == SDL_QUIT ) {
           		//cout << "QUIT" << endl;
           		nesCPU->stop("Window Closed");
        	}
        	
        	
   		}
   	//cout << "Keys Reported: " << keysPressed[0] << keysPressed[1] << keysPressed[2] << keysPressed[3] << keysPressed[4] << keysPressed[5] << keysPressed[6] << keysPressed[7] << endl;
	nesCPU->setKeys(keysPressed);
}


void System::tick(){
	//Run CPU then run PPU 3 times.
	nesCPU->cycle();
	nesPPU->cycle();
	nesPPU->cycle();
	nesPPU->cycle();
	
	tickCount++;
}

CPU* System::getCPU(){
	return nesCPU;
}

PPU* System::getPPU(){
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
