#include "CPU.cpp"
#include "System.hpp"

		CPU nesCPU;
		//PPU *nesPPU;
		//APU *newAPU;
		
		System::System(){
			start();
		}
		
		int main(){
			System theNES;
			return 0;
		}
		
		void System::start()
		{
			nesCPU.start();
			//nesPPU = new PPU();
			//nesAPU = new APU();
			setPowerState(true);
		}
		void System::setPowerState(bool state)
		{
			powerState=state;
		}
		bool System::getPowerState()
		{
			return powerState;
		}
		/*void System::openCartridge(File& cartFile){
		}
		void System::unloadCartridge(File& cartFile)
		{
			cartFile.close();
			currentCart = null;
		}*/
		void System::sendReset()
		{
			//delete nesCPU;
			//delete nesPPU;
			//delete nesAPU;
			//unloadCartridge(currentCart);
			setPowerState(false);
			start();
		}
		
		