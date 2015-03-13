/* CPU Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */
 
#include "CPU.hpp"

//Define "byte" to be a bitset of 8
typedef std::uint8_t byte;
		
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
	running=true;

	//Set the program counter to equal the reset vector, located at 0xFFFC
	//Because the addressing in the 6502 is backwards, FFFC is concatenated to the end of 
	//FFFD using bitwise shifts and ORs
	setPC((readMem(0xFFFC) | (readMem(0xFFFD) << 8)));
	
	//This is a very long line to output a short amount of debug text
	std::cout << "PC: "  <<std::setfill('0')<< std::setw(4)<<std::hex<< (int)getPC() << " reset vectors: " << std::setw(2)<<std::setfill('0') <<std::hex << (int)readMem(0xfffc) <<std::setw(2)<<std::setfill('0')<<std::hex<< (int)readMem(0xfffd)<<std::endl;
	std::cout << "Startup: Completed"<<std::endl;
	
}
void CPU::stop(std::string reason){
	std::cout << "Stopping CPU: " << reason << std::endl;
	running=false;
}


//Function to be called each time the system cycles, to perform CPU tasks.
//TODO: remove this var and the 20000 cycle code in cycle() (temporary)
int inc = 0;
void CPU::cycle(){
	if(inc<2000000)
	{
		//std::cout << ".";
		inc++;
	}
	else
		stop("Finished 2000000 cycles");
}

//---MEMORY ACCESS---//

//Returns a memory at selected address
byte CPU::readMem(int address){
	return memory[address];
}
//Writes given data memory to the selected address
void CPU::writeMem(int address, byte value){
	//if(value != 0x00)
		//std::cout << "Writing data: " << std::hex << (int)value << " to address " << std::hex << address << std::endl;	
	memory[address] = value;
}
//Writes given data to a range on addresses between start and end
void CPU::writeMem(int addressStart, int addressEnd, byte value){
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

/*Some OPCodes require a certain number of cycles to pass to complete the operation.
 *Theoretically the operation can be completed on the same cycle, so we wait for the
 *operation to 'complete' before accepting other operation. Wasted CPU cycles but
 *its the best I can think of right now.	*/		
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
void CPU::setPC(uint16_t address){
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
uint16_t CPU::getPC(){
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