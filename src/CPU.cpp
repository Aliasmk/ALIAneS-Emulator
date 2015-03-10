#include <iostream>
#include <iomanip>
#include "CPU.hpp"


//Define "byte" to be a bitset of 8
typedef std::bitset<8> byte;
		
//Constructor which not used yet
CPU::CPU(){}
	
//Startup procedure for the CPU, following what is on NESDEV wiki
void CPU::start(){
	//Initialize Register Values
	setP(0x34);
	setA(0);
	setX(0);
	setY(0);
	setS(0xFD);
	//Write internal memory apart from a few addresses
	writeMem(0x0000,0x07FF,0x00);
	writeMem(0x0008,0xF7);
	writeMem(0x0009,0xEF);
	writeMem(0x000a,0xDF);
	writeMem(0x000b,0xBF);
	//Zero out the rest of memory
	writeMem(0x4017,0x00);
	writeMem(0x4015,0x00);
	writeMem(0x4000,0x400F,0x00);
	std::cout << "Startup: Completed"<<std::endl;
}

//Function to be called each time the system cycles, to perform CPU tasks.
void CPU::cycle(){}

//---MEMORY ACCESS---//

byte CPU::readMem(short address)
{
	return memory[address];
}
void CPU::writeMem(short address, byte value){
	if(value != 0x00)
		std::cout << "Writing non-zero data: " << value << " to address " << std::hex << address << std::endl;	
	memory[address] = value;
}
void CPU::writeMem(short addressStart, short addressEnd, byte value){
	for(int i=addressStart; i<=addressEnd; i++)
	{
		//std::cout << i;
		memory[i] = value;
		if(value != 0x00)
			std::cout << "Writing non-zero data: " << value << " to address " << std::hex << i << std::endl;
	}
}

//---DECODING---//

//OPCode decoding methods. TODO: Implement these
void CPU::decode(byte opCode){}
void CPU::decode(byte opCode, byte param1){}
void CPU::decode(byte opCode, byte param1, byte param2){}
void CPU::decode(byte opCode, byte param1, byte param2, byte param3){}
void CPU::decodeAt(short address){}

//Some OPCodes require a certain number of cycles to pass to complete the operation.
//Theoretically the operation can be completed on the same cycle, so we wait for the
//operation to 'complete' before accepting other operation. Wasted CPU cycles but
//its the best I can think of right now. TODO: RETHINK THIS SYSTEM			
void CPU::waitForCycles(short toWait){
	cycleWait = toWait;
}	
//Boolean to check if the program is waiting to complete an operation.	
bool CPU::sleeping(){
	if(cycleWait>0)
		return true;
	else
		return false;
}	


//---GET & SET---//

//Load accumulator with i
void CPU::setA(byte i){
	a = i;
}
//Load X index with i
void CPU::setX(byte i){
	x = i;
}
//Load Y index with i
void CPU::setY(byte i){
	y = i;
}
//Set the program counter to the specified address
void CPU::setPC(short address){
	pc = address;
}
//Set the stack pointer to the location at i
void CPU::setS(byte i){
	s = i;
}
//Set status flags as i
void CPU::setP(byte i){
	p = i;
}

//Returns value in the accumulator
byte CPU::getA(){
	return a;
}
//Returns value in the X index
byte CPU::getX(){
	return x;
}
//Returns value in the Y index
byte CPU::getY(){
	return y;
}
//Returns the program counter
short CPU::getPC(){
	return pc;
}
//Returns stack pointer location
byte CPU::getS(){
	return s;
}
//Returns all 8 bits of the 6 bit status 
byte CPU::getP(){
	return p;
}
	

