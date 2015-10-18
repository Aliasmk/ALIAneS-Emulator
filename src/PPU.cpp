/* PPU Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "PPU.hpp"
using namespace std;

typedef uint8_t byte;



PPU::PPU(){}

void PPU::start(){
	cout << "PPU Initialization: Complete" << endl;
}

void PPU::cycle(){

}


void PPU::writeMem(int address, byte value){
	ppuMemory[address] = value;
	cout << "writing value " << hex << (int)value << " to PPU address " << (int)address << endl;
}

byte PPU::readMem(int address){
	return ppuMemory[address];
}