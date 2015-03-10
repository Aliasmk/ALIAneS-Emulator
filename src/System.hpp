#ifndef NES_SYSTEM
#define NES_SYSTEM
class System {
	private:
		CPU nesCPU;
		bool powerState;
		//File currentCart;
		//PPU nesPPU;
		//APU newAPU
		
	public: 
		int main();
		System();
		void start();
		void setPowerState(bool state);
		bool getPowerState();
		//void openCartridge(File cartFile);
		//void unloadCartridge(File cartFile);
		void sendReset();		
		
		
};
#endif