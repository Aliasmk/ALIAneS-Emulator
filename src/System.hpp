#ifndef NES_SYSTEM
#define NES_SYSTEM

#include <fstream>
#include <string>
#include <iostream>

class System {
	private:
		//CPU nesCPU;
		//PPU nesPPU;
		//APU newAPU
		
		bool powerState;
		
		std::ifstream currentCart;
		std::string cartPath;
		
		void tick();
		
		int tickCount;
		
	public: 
		System();
		
		int main();
		
		void init();
		void run();
		
		void setPowerState(bool state);
		bool getPowerState();
		void sendReset();
		
		void openCartridge(std::string cartPath);
		void unloadCartridge(std::ifstream& cartFile);		
};
#endif