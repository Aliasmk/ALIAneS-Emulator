#ifndef NES_SYSTEM
#define NES_SYSTEM
class System {
	private:
		//CPU nesCPU;
		//PPU nesPPU;
		//APU newAPU
		
		bool powerState;
		
		//File currentCart;
		
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
		
		//void openCartridge(File cartFile);
		//void unloadCartridge(File cartFile);		
};
#endif