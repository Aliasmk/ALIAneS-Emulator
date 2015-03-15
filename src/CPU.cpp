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

	decodeAt(0xfd14);
}
void CPU::stop(std::string reason){
	std::cout << "Stopping CPU: " << reason << std::endl;
	running=false;
}


//Function to be called each time the system cycles, to perform CPU tasks.
//TODO: remove this var and the 20000 cycle code in cycle() (temporary)
int inc = 0;
void CPU::cycle(){
	if(inc<35000)
	{
		decodeAt(getPC());
		incPC();
	}
	else
		stop("Finished 2000000 cycles");
		
	inc++;
}

//---MEMORY ACCESS---//

//Returns a memory at selected address
byte CPU::readMem(int address){
	return memory[address];
}
//Writes given data memory to the selected address
void CPU::writeMem(int address, byte value){
	//if(value != 0x00)
		std::cout << "Writing data: " << std::hex << (int)value << " to address " << std::hex << address << std::endl;	
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

//TODO Finish
void CPU::decodeAt(int address){
	byte opcode = readMem(address);
	//Split opcode up into bits based on info from here: http://www.llx.com/~nparker/a2/opcodes.html
	//By decoding at a opcode bit level we save a lot on switch and if statements.
	byte aaa = (opcode & 0xe0) >> 5;
	byte bbb = (opcode & 0x1c) >> 2 ;
	byte cc  = (opcode & 0x03);
	//std::cout  << (int)opcode << " -aaa: " << aaa << " -bbb: " << bbb << " -cc: " << cc << std::endl; 
	
	bool valid = false;
	std::string operation = "";
	std::string addressmode = "";
	
	if(cc == 1) //cc = 01
	{
		switch(aaa){
			case 0:
				operation = "ORA";
				switch(bbb){
					case 0: //(zeropage,x)
						addressmode = "(zeropage,x)";
					break;
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 2: //immediate
						addressmode = "immediate";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 4: //(zeropage),y
						addressmode = "(zeropage),y";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 6: //absolute,y
						addressmode = "absolute,y";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				valid = true;
			break;
			case 1:
				switch(bbb){
					case 0: //(zeropage,x)
						addressmode = "(zeropage,x)";
					break;
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 2: //immediate
						addressmode = "immediate";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 4: //(zeropage),y
						addressmode = "(zeropage),y";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 6: //absolute,y
						addressmode = "absolute,y";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "AND";
				valid = true;
			break;
			case 2:
				switch(bbb){
					case 0: //(zeropage,x)
						addressmode = "(zeropage,x)";
					break;
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 2: //immediate
						addressmode = "immediate";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 4: //(zeropage),y
						addressmode = "(zeropage),y";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 6: //absolute,y
						addressmode = "absolute,y";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "EOR";
				valid = true;
			break;
			case 3:
				switch(bbb){
						case 0: //(zeropage,x)
							addressmode = "(zeropage,x)";
						break;
						case 1: //zeropage
							addressmode = "zeropage";
						break;
						case 2: //immediate
							addressmode = "immediate";
						break;
						case 3: //absolute
							addressmode = "absolute";
						break;
						case 4: //(zeropage),y
							addressmode = "(zeropage),y";
						break;
						case 5: //zeropage,x
							addressmode = "zeropage,x";
						break;
						case 6: //absolute,y
							addressmode = "absolute,y";
						break;
						case 7: //absolute,x
							addressmode = "absolute,x";
						break;
					}
					operation = "ADC";
					valid = true;
				break;
			case 4:
				switch(bbb){
						case 0: //(zeropage,x)
							addressmode = "(zeropage,x)";
						break;
						case 1: //zeropage
							addressmode = "zeropage";
						break;
						case 3: //absolute
							addressmode = "absolute";
						break;
						case 4: //(zeropage),y
							addressmode = "(zeropage),y";
						break;
						case 5: //zeropage,x
							addressmode = "zeropage,x";
						break;
						case 6: //absolute,y
							addressmode = "absolute,y";
						break;
						case 7: //absolute,x
							addressmode = "absolute,x";
						break;
					}
				operation = "STA";
				valid = true;
			break;
			case 5:
				switch(bbb){
						case 0: //(zeropage,x)
							addressmode = "(zeropage,x)";
						break;
						case 1: //zeropage
							addressmode = "zeropage";
						break;
						case 2: //immediate
							addressmode = "immediate";
						break;
						case 3: //absolute
							addressmode = "absolute";
						break;
						case 4: //(zeropage),y
							addressmode = "(zeropage),y";
						break;
						case 5: //zeropage,x
							addressmode = "zeropage,x";
						break;
						case 6: //absolute,y
							addressmode = "absolute,y";
						break;
						case 7: //absolute,x
							addressmode = "absolute,x";
						break;
					}
				operation = "LDA";
				valid = true;
			break;
			case 6:
				switch(bbb){
						case 0: //(zeropage,x)
							addressmode = "(zeropage,x)";
						break;
						case 1: //zeropage
							addressmode = "zeropage";
						break;
						case 2: //immediate
							addressmode = "immediate";
						break;
						case 3: //absolute
							addressmode = "absolute";
						break;
						case 4: //(zeropage),y
							addressmode = "(zeropage),y";
						break;
						case 5: //zeropage,x
							addressmode = "zeropage,x";
						break;
						case 6: //absolute,y
							addressmode = "absolute,y";
						break;
						case 7: //absolute,x
							addressmode = "absolute,x";
						break;
					}
				operation = "CMP";
				valid = true;
			break;
			case 7:
				switch(bbb){
						case 0: //(zeropage,x)
							addressmode = "(zeropage,x)";
						break;
						case 1: //zeropage
							addressmode = "zeropage";
						break;
						case 2: //immediate
							addressmode = "immediate";
						break;
						case 3: //absolute
							addressmode = "absolute";
						break;
						case 4: //(zeropage),y
							addressmode = "(zeropage),y";
						break;
						case 5: //zeropage,x
							addressmode = "zeropage,x";
						break;
						case 6: //absolute,y
							addressmode = "absolute,y";
						break;
						case 7: //absolute,x
							addressmode = "absolute,x";
						break;
					}
				operation = "SBC";
				valid = true;
			break;
		}
	}
	else if(cc == 2) //cc = 10
	{
		switch(aaa)	{
			case 0:
				switch(bbb){
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 2: //accumulator
						addressmode = "accumulator";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "ASL";
				valid = true;
			break;
			case 1:
				switch(bbb){
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 2: //accumulator
						addressmode = "accumulator";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "ROL";
				valid = true;
			break;
			case 2:
				switch(bbb){
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 2: //accumulator
						addressmode = "accumulator";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "LSR";
				valid = true;
			break;
			case 3:
				switch(bbb){
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 2: //accumulator
						addressmode = "accumulator";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //zeropage,x
						addressmode = "xzeropage,x";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "ROR";
				valid = true;
			break;
			case 4:
				switch(bbb){
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //(zeropage),y CHANGES
						addressmode = "zeropage,y";
					break;
				}
				operation = "STX";
				valid = true;
			break;
			case 5:
				switch(bbb){
					case 0: //immediate
						addressmode = "immediate";
					break;
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //(zeropage),y CHANGES
						addressmode = "zeropage,y";
					break;
					case 7: //absolute,y
						addressmode = "absolute,y";
					break;
				}
				operation = "LDX";
				valid = true;
			break;
			case 6:
				switch(bbb){
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "DEC";
				valid = true;
			break;
			case 7:
				switch(bbb){
					case 0: //immediate
						addressmode = "immediate";
					break;
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "INC";
				valid = true;
			break;
		}
	}
	else if(cc == 0) //cc = 00
	{
		switch(aaa)	{
			case 1:
				switch(bbb){
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
				}
				operation = "BIT";
				valid = true;
			break;
			case 2:
				switch(bbb){
					case 3: //absolute
						addressmode = "absolute";
					break;
				}
				operation = "JMP";
				valid = true;
			break;
			case 3:
				switch(bbb){
					case 3: //absolute
						addressmode = "absolute";
					break;
				}
				operation = "JMP (abs)";
				valid = true;
			break;
			case 4:
				switch(bbb){
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
				}
				operation = "STY";
				valid = true;
			break;
			case 5:
				switch(bbb){
					case 0: //immediate
						addressmode = "immediate";
					break;
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
					case 5: //zeropage,x
						addressmode = "zeropage,x";
					break;
					case 7: //absolute,x
						addressmode = "absolute,x";
					break;
				}
				operation = "LDY";
				valid = true;
			break;
			case 6:
				switch(bbb){
					case 0: //immediate
						addressmode = "immediate";
					break;
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
				}
				operation = "CPY";
				valid = true;
			break;
			case 7:
				switch(bbb){
					case 0: //immediate
						addressmode = "immediate";
					break;
					case 1: //zeropage
						addressmode = "zeropage";
					break;
					case 3: //absolute
						addressmode = "absolute";
					break;
				}
				operation = "CPX";
				valid = true;
			break;
		}
	}
	else //one-off codes
	{
		
	}
	
	if(!valid)
		std::cout << "Unsupported OPcode " << (int)opcode << " at address " << address << ". Skipping..." << std::endl;
	else
		std::cout << "Executed " << operation << " " << addressmode<< " (" << (int)opcode << ") at address " << address << "." << std::endl;
	
}

/*Some OPCodes require a certain number of cycles to pass to complete the operation.
 *Theoretically the operation candbe completed on the same cycle, so we wait for the
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
//Increments the Program counter by one;
void CPU::incPC(){
	pc++;
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