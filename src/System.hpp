/* System Header :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#ifndef NES_SYSTEM
#define NES_SYSTEM

#include <string>
#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include "CPU.hpp"
#include "PPU.hpp"
#include "SDLrender.hpp"



class System {
	private:
		bool powerState;
			
		void tick();
		void getInput();	
		int tickCount;
	public: 
		
		
		int keysPressed[8];
		
		
		System();
		System(std::string cart);

		int main2();
		
		void loadConfig();
		bool init();
		void run();
		CPU* getCPU();
		PPU* getPPU();
		SDLrender* getRenderer();
		
		void setPowerState(bool state);
		bool getPowerState();
		void sendReset();	
};
#endif