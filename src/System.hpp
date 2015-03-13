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

class System {
	private:
		bool powerState;
			
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
};
#endif