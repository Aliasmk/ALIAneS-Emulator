/* CPU Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "CPU.hpp"

#include <bitset>

//Define "byte" to be a bitset of 8
typedef uint8_t byte;

using namespace std;

PPU* ptr_nesPPU;
int spacing = 15;
int startoverride=0; //set to 0 for auto
int cyclesToExecute;
bool logging = false;
bool debugging = false;
ofstream lout;

//Constructor which not used yet
CPU::CPU(PPU* nPPU){
	cout << "NES CPU is now on." <<endl;
	ptr_nesPPU = nPPU;
}
	
void CPU::setConfig(int startAddress, int cycles){
	startoverride=startAddress;
	cyclesToExecute=cycles;
}


//Startup procedure for the CPU, following what is on NESDEV wiki
void CPU::start(){
	cycleCount=0;
	//Initialize Register Values
	setP(0x34);
	clearP(UNUSED);
	setP(UNUSED);
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
	if(startoverride==0)
		setPC(toAddress(readMem(0xFFFC),readMem(0xFFFD)));
	else
		setPC(startoverride);

	//CPU "bootup" output text
	if(logging){
		lout.open("../res/logs/alianesLast.log");
		cout << "Execution logging is enabled. This will drastically slow performance and should only be used for small scale debug." << endl;
	}
	cout << "Reset vector: " << setw(2)<<setfill('0') <<hex << (int)readMem(0xfffc) <<setw(2)<<setfill('0')<<hex<< (int)readMem(0xfffd)<<endl;	
	cout << "Startup: Completed"<<endl;		
	if(debugging)
		cout << setfill (' ') << "Address" <<setw(spacing)<<"OPCode" <<setw(spacing) << "AddressMode" << setw(spacing) << "OpAddr" << setw(spacing) << "Operand" << setw(spacing) << "NextByte" << setw(spacing) << "ByteAfter" << endl;
	oddCycle=false;
	wake();
}

//CPU Shutdown Method
void CPU::stop(string reason){
	cout << "Stopping CPU: " << reason << endl;
	running=false;
}

//Function to be called each time the system cycles, to perform CPU tasks.
void CPU::cycle(){
	if(ptr_nesPPU->vblank==true && ptr_nesPPU->vblankSeen==false && ptr_nesPPU->getNMI()==true){
		ptr_nesPPU->vblankSeen=true;
		triggerInterrupt(NMI);
	}
	
	
	//TODO cycles to execute is temporary. Set using config file
	if(cycleCount<cyclesToExecute)
	{
		if(!sleeping()){
			evaluateInterrupt(currentInterrupt); //Checks for interrupt request before executing any commands
			decodeAt(getPC());
			incPC(); //TODO PC incrementing may want to be done elsewhere.
		}
	}
	else{
		stop("Finished cycles");
		if(logging)
			lout.close();	
		//memoryDump();
	}
	oddCycle = !oddCycle;
	cycleCount++;
}

//---MEMORY ACCESS---//


void CPU::setKeys(int* keysToSet){
	//cout << "set keys: ";
	for(int k = 0; k<8; k++){
		keys[k]=keysToSet[k];
		//cout << keys[k];
	}
	//cout << endl;
}

//Returns a memory value from selected address
byte CPU::readMem(int address){
	
	
	if(address == 0x4016){
		//cout << "Index: " << keyIndex << endl;
		//write goes here
		
		if(keyIndex < 8)
			memory[0x4016] = keys[keyIndex];
		else
			memory[0x4016] = 1;
		
		//read goes here
		//cout << "Reading from 0x4016: " << hex << (int)memory[address] << endl;
		keyIndex++;
	}
	
	//cout << "reading memory - ";
	if(address >= 0x2000 && address <=0x3FFF)
	{
		int offset = address%8;
		//cout << " - ";
		switch(offset)
		{
			
			case 0:
				cout << "2000 PPUCTRL read ERROR! ";
				
			break;
			case 1:
				cout << "2001 PPUMASK read ERROR! ";
			break;
			case 2:
				//cout << "2002 PPUSTATUS read " << endl;
				return ptr_nesPPU->readPPUSTATUS();
			break;
			case 3:
				cout << "2003 OAMADDR read ERROR! ";
			break;
			case 4:
				//cout << "2004 OAMDATA read ";
				return ptr_nesPPU->readOAMDATA();
			break;
			case 5:
				cout << "2005 PPUSCROLL read ERROR! ";
			break;
			case 6:
				cout << "2006 PPUADDR read ERROR! ";
			break;
			case 7:
				//cout << "2007 PPUDATA read ";
				return ptr_nesPPU->readPPUDATA();
			break;
		}
	}
	return memory[address];
}

//Give <instruction> (firstbyte) (secondbyte) this returns memory at secondbyte:firstbyte
//TODO replace this method with a readMem16(int address) method
byte CPU::readMem(byte firstByte, byte secondByte)
{
	int address = firstByte | (secondByte<<8); 
	return memory[address];
}

//Increments the program counter and reads the next bit
byte CPU::readNext(){
	incPC(); //TODO this gets confusing and hard to understand when there are incPC's everywhere
	return readMem(getPC());
}

//Writes given value to the selected address
void CPU::writeMem(int address, byte value){	
	//Memory Mirroring
	//Not sure what this section in memory is for
	
	//DEBUG
	/*if(address >= 0x200 && address <0x300){
		cout << "data (" << hex << (int)value << ") written to " << hex<< address << endl;
	}*/
	
	//If writing to OAM DMA
	if(address==0x4014){
		int readAddress = (value<<8);
		//cout << "OAM DMA has been written to. Copying " << hex << readAddress << " to " << hex <<readAddress+0xFF << " into OAM memory" << endl;
		for(int i = 0; i<256; i++){
			
			ptr_nesPPU->ppuWriteOAM(i, readMem(readAddress+i));
			//cout << "writing " << hex << (int)readMem(readAddress+i) << " from CPU " << readAddress+i << " to OAM " << i << endl; 
			//TODO what if there OAM ADDR is not 0? out of bounds of array!quit
		}
		
		setWaitCycles(513+oddCycle);
		
	}
	
	
	if(address==0x4016){
		//cout << "Writing value " << hex << (int)value << " to address " << hex << address << endl;
		value = value&0x1;
		if(value == 1){
			//cout << "key index was " << keyIndex;
			keyIndex = 0;
			//cout << " now " << keyIndex << endl;
		}
	}
	
	
	if(address >= 0x0800 && address <= 0x1FFF){
		//Mirror every 0x0800 bytes
		int offset = address%0x0800;	
		for(int i = 0x0800; i<0x1FFF; i+=0x0800){
			memory[i+offset] = value;
		}
	}
	//PPU Registers
	else if(address >= 0x2000 && address <= 0x3FFF){
		//Mirror every 8 bytes
		int offset = address%0x8;
		for(int i = 0x2000; i<0x3FFF; i+=8){
			memory[i+offset] = value;
		}
		
		switch(offset)
		{
			case 0:
				//cout << "2000 PPUCTRL write" << endl;
				ptr_nesPPU->writePPUCTRL(value);
			break;
			case 1:
				//cout << "2001 PPUMASK write" << endl;
				ptr_nesPPU->writePPUMASK(value);
			break;
			case 2:
				cout << "2002 PPUSTATUS write ERROR!" << endl;
			break;
			case 3:
			//	cout << "2003 OAMADDR write" << endl;
				ptr_nesPPU->writeOAMADDR(value);
			break;
			case 4:
			//	cout << "2004 OAMDATA write" << endl;
				ptr_nesPPU->writeOAMDATA(value);
			break;
			case 5:
				//cout << "2005 PPUSCROLL write" << endl;
				ptr_nesPPU->writePPUSCROLL(value);
			break;
			case 6:
				//cout << "2006 PPUADDR write" << endl;
				ptr_nesPPU->writePPUADDR(value);
			break;
			case 7:
				//cout << "2007 PPUDATA write" << endl;
				ptr_nesPPU->writePPUDATA(value);
			break;
		}
	}
	else{
		memory[address] = value;
	}
}
//Writes given data to a range on addresses between start and end
void CPU::writeMem(int addressStart, int addressEnd, byte value){
	for(int i=addressStart; i<=addressEnd; i++)
	{
		writeMem(i,value);
	}
}

//Sets the next interrupt to trigger
void CPU::triggerInterrupt(int toTrigger){
	currentInterrupt = toTrigger;
}

//Runs before any decoding every CPU cycle.
void CPU::evaluateInterrupt(int & interruptType){
	byte firstPart, secondPart; //TODO figure out why this scope is like this
	switch(interruptType){
		case NONE:
			//No interrupt, continue as normal
		break;
		case NMI:
			//Push pc and flags to stack, setpc to NMI interrupt vector and set I. 7 cycles
			//TODO implement stackPush16 method
			firstPart = (getPC()&0xFF00)>>8;
			secondPart = (getPC()&0xFF);
			
			stackPush(firstPart);
			stackPush(secondPart);
		
			stackPush(getP());
			setWaitCycles(7);
			setPC(toAddress(readMem(0xFFFA),readMem(0xFFFB)));
			setP(BRK);
			//cout << endl <<"NMI interrupt has been thrown" << endl;
		break;
		case IRQ:
			//Push pc and flags to stack, setpc to IRQ (reset) interrupt vector and set I. 7 cycles
			firstPart = (getPC()&0xFF00)>>8;
			secondPart = (getPC()&0xFF);
			stackPush(firstPart);
			stackPush(secondPart);
			
		
			stackPush(getP());
			setWaitCycles(7);
			setPC(toAddress(readMem(0xFFFE),readMem(0xFFFF)));
			setP(BRK);
		break;
	}
	interruptType = NONE;	
}



//---DECODING---//

//Given a byte, figures out its opcode and addressing mode
void CPU::decodeAt(int address){
	byte opcode = readMem(address);
	//Split opcode up into bits based on info from here: http://www.llx.com/~nparker/a2/opcodes.html
	//By decoding at a opcode bit level we save a lot on switch and if statements.
	byte aaa = (opcode & 0xe0) >> 5;
	byte bbb = (opcode & 0x1c) >> 2 ;
	byte cc  = (opcode & 0x03);
	//cout  << (int)opcode << " -aaa: " << aaa << " -bbb: " << bbb << " -cc: " << cc << endl; 
	
	bool valid = false;
	
	//TODO MAJOR enumerate the OPcodes and addressing modes. I havent profiled this project but I'll eat my hat if using strings aren't slowing this down a lot. 
	string operation = "";
	string addressmode = "implied";

	
	//CC: General Set. AAA: Instruction. BBB: Addressing mode. Text strings passed later.
	//First decode the address and operands required based on the address mode asked for and store it. Then use it in the specific instruction set.
	if(cc == 1) //cc = 01
	{
		switch(bbb){
					default:
						addressmode = "na";
					break;
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
		switch(aaa){
			case 0:
				operation = "ORA";
				valid = true;		
			break;
			case 1:
				operation = "AND";
				valid = true;
			break;
			case 2:
				operation = "EOR";
				valid = true;
			break;
			case 3:
				operation = "ADC";
				valid = true;
				break;
			case 4:
				operation = "STA";
				if(addressmode!="immediate")
					valid = true;
			break;
			case 5:
				operation = "LDA";
				valid = true;
			break;
			case 6:
				operation = "CMP";
				valid = true;
			break;
			case 7:
				operation = "SBC";
				valid = true;
			break;
		}			
	}
	else if(cc == 2) //cc = 10
	{
		switch(bbb){
					default:
						addressmode = "na";
					break;
					case 0: //immediate
						addressmode = "immediate";
					break;
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
		switch(aaa)	{
			case 0:
				operation = "ASL";
				if(addressmode!="immediate")
					valid = true;
			break;
			case 1:
				operation = "ROL";
				if(addressmode!="immediate")
					valid = true;
			break;
			case 2:
				operation = "LSR";
				if(addressmode!="immediate")
					valid = true;
			break;
			case 3:
				operation = "ROR";
				if(addressmode!="immediate")
					valid = true;
			break;
			case 4:
				operation = "STX";
				if(addressmode == "zeropage,x")
					addressmode = "zeropage,y";
				if(addressmode!="immediate")
					valid = true;
			break;
			case 5:
				operation = "LDX";
				if(addressmode == "zeropage,x")
					addressmode = "zeropage,y";
				if(addressmode == "absolute,x")
					addressmode = "absolute,y";
				valid = true;
			break;
			case 6:
				operation = "DEC";
				if(addressmode!="immediate")
					valid = true;
			break;
			case 7:
				operation = "INC";
				if(addressmode!="immediate")
					valid = true;
			break;
		}
	}
	else if(cc == 0) //cc = 00
	{
		switch(bbb){
					default:
						addressmode = "na";
					break;
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
		switch(aaa)	{
			case 1:
				operation = "BIT";
				if(addressmode!="immediate" && addressmode!="zeropage,x" && addressmode!="absolute,x")
					valid = true;
			break;
			//TODO fix the bug that causes below to be required
			/*case 2:
				operation = "JMPa";
				valid = true;
			break;*/
			/*case 3: -- Moving down to other cases
				operation = "JMP"; 
				//addressmode = "jumpindirect";
				valid = true;
			break;*/
			case 4:
				operation = "STY";
				if(addressmode!="immediate" && addressmode!="absolute,x")
					valid = true;
			break;
			case 5:
				operation = "LDY";
				valid = true;
			break;
			case 6:
				operation = "CPY";
				if(addressmode!="zeropage,x" && addressmode!="absolute,x")
					valid = true;
			break;
			case 7:
				operation = "CPX";
				if(addressmode!="zeropage,x" && addressmode!="absolute,x")
					valid = true;
			break;
		}
	}
	bool standardInstruction = false;
		switch(opcode){
			//Single Byte Instructions
			default:
				standardInstruction = true;
			break;
			case 0x08:
				valid=true; operation="PHP"; addressmode = "implied";
			break;
			case 0x28:
				valid=true; operation="PLP"; addressmode = "implied";
			break;
			case 0x48:
				valid=true; operation="PHA"; addressmode = "implied";
			break;
			case 0x68:
				valid=true; operation="PLA"; addressmode = "implied";
			break;
			case 0x88:
				valid=true; operation="DEY"; addressmode = "implied";
			break;
			case 0xA8:
				valid=true; operation="TAY"; addressmode = "implied";
			break;
			case 0xC8:
				valid=true; operation="INY"; addressmode = "implied";
			break;
			case 0xE8:
				valid=true; operation="INX"; addressmode = "implied";
			break;
			case 0x18:
				valid=true; operation="CLC"; addressmode = "implied";
			break;
			case 0x38:
				valid=true; operation="SEC"; addressmode = "implied";
			break;
			case 0x58:
				valid=true; operation="CLI"; addressmode = "implied";
			break;
			case 0x78:
				valid=true; operation="SEI"; addressmode = "implied";
			break;
			case 0x98:
				valid=true; operation="TYA"; addressmode = "implied";
			break;
			case 0xb8:
				valid=true; operation="CLV"; addressmode = "implied";
			break;
			case 0xD8:
				valid=true; operation="CLD"; addressmode = "implied";
			break;
			case 0xF8:
				valid=true; operation="SED"; addressmode = "implied";
			break;
			
			case 0x8a:
				valid=true; operation="TXA"; addressmode = "implied";
			break;
			case 0x9a:
				valid=true; operation="TXS"; addressmode = "implied";
			break;
			case 0xaa:
				valid=true; operation="TAX"; addressmode = "implied";
			break;
			case 0xba:
				valid=true; operation="TSX"; addressmode = "implied";
			break;
			case 0xca:
				valid=true; operation="DEX"; addressmode = "implied";
			break;
			case 0xea:
				valid=true; operation="NOP"; addressmode = "implied";
			break;
			
			case 0x6c:
				valid=true; operation="JMP"; addressmode = "jumpindirect";
			break;
			case 0x4c:
				valid=true; operation="JMPa"; addressmode = "absolute";
			break;
			
			//Branch on - instructions
			
			case 0x10:
				valid=true; operation="BPL"; addressmode = "immediate";
			break;
			case 0x30:
				valid=true; operation="BMI"; addressmode  = "immediate";
			break;
			case 0x50:
				valid=true; operation="BVC"; addressmode = "immediate";
			break;
			case 0x70:
				valid=true; operation="BVS"; addressmode = "immediate";
			break;
			case 0x90:
				valid=true; operation="BCC"; addressmode = "immediate";
			break;
			case 0xb0:
				valid=true; operation="BCS"; addressmode = "immediate";
			break;
			case 0xd0:
				valid=true; operation="BNE"; addressmode = "immediate";
			break;
			case 0xf0:
				valid=true; operation="BEQ"; addressmode = "immediate";
			break;
			
			//Other instructions
			
			case 0x00:
				valid=true; operation="BRK"; addressmode="";
			break;
			case 0x20:
				valid=true; operation="JSR"; addressmode="absolute";
			break;
			case 0x40:
				valid=true; operation="RTI"; addressmode="implied";
			break;
			case 0x60:
				valid=true; operation="RTS"; addressmode="implied";
			break;
		}
	if(addressmode == "na")
		valid = false;	
	if(!valid)
		operation = "UKN";
	
	//DEBUG Output
	
	if(debugging)
		cout << dec <<cycleCount << " - " << hex << uppercase << setw(4)<<setfill('0') <<address << setfill(' ')<< ":"<< setw(spacing) << operation << " " << hex << (int)opcode << setw(spacing) << addressmode;
	
	
	if(logging)
		lout << dec << cycleCount << " " << hex  << uppercase<< setw(4) << setfill('0') << address<< setfill(' ') << setw(spacing) << operation << " " << hex << (int)opcode << setw(spacing) << addressmode;
	execute(operation, addressmode);
	
	if(debugging)
		cout << endl;
	
	if(logging)
		lout << endl;
	
}

//Given the operation and addressmode, fetches arguments and executes commands
void CPU::execute(string operation, string addressmode)
{
	//ADDRESSING MODES - Gives both a decoded address and a value to work with, methods use whichever they need.
	byte firstByte, secondByte;
	int opAddress = 0;
	int operand = 0;
	int sleepCycles=0;	
	
	//Grab the first and second byte after the operation, we may only need one, or none. Remember most significant byte last
	firstByte = readMem(getPC()+1);
	secondByte = readMem(getPC()+2);
	if(addressmode == "immediate"){ //Immediate uses the next byte as a direct data argument
			incPC();
			opAddress = getPC();
			setWaitCycles(2);
	}else if (addressmode == "absolute"){ //Absolute is an explicit definition of the location in memory to use
		incPC();
		incPC();
		opAddress = toAddress(firstByte, secondByte);
		if(operation == "ASL" || operation == "DEC" || operation == "INC" || operation == "LSR" || operation == "ROL" || operation == "ROR"){
			setWaitCycles(6);
		} else {
			setWaitCycles(4);
		} 
		
	} else if (addressmode == "zeropage") { //Zero Page is absolute but only for the first 256 bits of memory (0x0000 - 0x00ff)
		incPC();
		opAddress = toAddress(firstByte,0);
		
		if(operation == "ASL" || operation == "DEC" || operation == "INC" || operation == "LSR" || operation == "ROL" || operation == "ROR"){
			setWaitCycles(5);
		} else {
			setWaitCycles(3);
		}
	
	} else if (addressmode == "accumulator") { //Targets the accumulator
		//TODO im not sure why this is commented out...
		//operand = getA();
		//incPC();
		setWaitCycles(2);
	} else if (addressmode == "absolute,x") { //Absolute value added to the X register
		incPC();
		incPC();
		opAddress = toAddress(firstByte,secondByte)+getX();
		
		if(operation == "ASL" || operation == "DEC" || operation == "INC" || operation == "LSR" || operation == "ROL" || operation == "ROR" || operation == "JMP"){
			setWaitCycles(6);
		} else {
			setWaitCycles(4);
			if(pageCrossed(opAddress, opAddress-getX()))
				addWaitCycles(1);
		}
		
	} else if (addressmode == "absolute,y") { //Absolute value added to the Y register
		incPC();
		incPC();
		opAddress = toAddress(firstByte,secondByte)+getY();
		if(opAddress > 0xFFFF)
			opAddress-=0x10000;
		if(operation == "STA"){
			setWaitCycles(5);
		} else {
			setWaitCycles(4);
			if(pageCrossed(opAddress, opAddress-getY()))
				addWaitCycles(1);
		}
		
	} else if (addressmode == "zeropage,x") { 
		incPC();
		opAddress = toAddress(firstByte,0)+getX();
		if(opAddress > 0xFF)
			opAddress -= 0x100;
			
		if(operation == "ASL" || operation == "DEC" || operation == "INC" || operation == "LSR" || operation == "ROL" || operation == "ROR"){
			setWaitCycles(6);
		} else {
			setWaitCycles(4);
		} 
		
	} else if (addressmode == "zeropage,y") { 
		incPC();
		opAddress = toAddress(firstByte,0)+getY();
		if(opAddress > 0xFF)
			opAddress -= 0x100;
			
		setWaitCycles(4);
		
	} else if (addressmode == "(zeropage,x)") { 
		//AKA (Indirect, X)
		firstByte = readNext();
		byte tempAddress = firstByte+getX();
		
		if(tempAddress > 0xFF)
			tempAddress-=0xFF;
		byte tempAddress2 = tempAddress+1;
		if(tempAddress2 > 0xFF)
			tempAddress-=0xFF;
		
		opAddress = toAddress(readMem(tempAddress), readMem(tempAddress2));
		
		setWaitCycles(6);
	} else if (addressmode == "(zeropage),y") { 
		//AKA (Indirect),Y)
		firstByte = readNext(); //start address
		byte firstAddress = readMem(toAddress(firstByte,0)); //reads address from zeropage next
		byte secondAddress = readMem(toAddress((firstByte+1),0));
		
		
		opAddress = toAddress(firstAddress, secondAddress)+getY();
		if(opAddress > 0xFFFF)
			opAddress-=0x10000;
		
		if(operation == "STA"){
			setWaitCycles(6);
		} else {
			setWaitCycles(5);
			if(pageCrossed(opAddress, opAddress-getY()))
				addWaitCycles(1);
		}
	
	} else if (addressmode == "implied"){
		setWaitCycles(2);
	} else if (addressmode == "jumpindirect"){ //JMP 6C only

		byte firstPart = readNext();
		byte secondPart = readNext();
		int startAddress = toAddress(firstPart,secondPart);
		int nextAddress;
		if(firstPart==0xFF){
			nextAddress = startAddress-0xFF;
		}
		else{		
			nextAddress = startAddress+1;
		}
		opAddress = toAddress(readMem(startAddress), readMem(nextAddress));
	}
	
	//TODO DETECT A PPU ADDRESS READ

	//operand = readMem(opAddress);
	
	//DEBUG
	if(addressmode != "implied" && addressmode != "accumulator"){	
		if(debugging)
			cout << setw(spacing)<< hex << (int)opAddress << setw(spacing) << (int)operand; // << setw(spacing) <<(int)firstByte << setw(spacing) << hex << (int)secondByte;
	}
	else
	{
		if(debugging)
			cout << setw(spacing)<< hex << "-" << setw(spacing) << "-";
	}
	
	if(debugging)
		printDebugStatus(getPC());
	
	//INSTRUCTIONS
	if(operation == "ADC"){ //Add with Carry
		operand = readMem(opAddress);
		byte carry;
		int resultTest;
		
		if(checkP(CARRY))
			carry=1;
		else
			carry=0;
		resultTest=getA()+operand+carry;
		
		if((getA() ^ resultTest) & (operand ^ s) & 0x80)
			setP(AOVERFLOW);
		else
			clearP(AOVERFLOW);
		setA(getA()+operand+carry);
		if(resultTest > 0xFF)
			setP(CARRY);
		else
			clearP(CARRY);			
		setFlags(getA());
	
	} else if(operation == "AND") { //Perform an AND with the read byte on the Accumulator. Uses: Value
		operand = readMem(opAddress);
		setA(getA()&operand);
		setFlags(getA());
		
	} else if(operation == "ASL") { //Perform a left shift on the accumulator or memory, store old bit 7 in the carry flag
		operand = readMem(opAddress);
		if(addressmode == "accumulator"){
			//TODO chane to enumerated bits instead of 128
			if((128&getA())==128)
				setP(CARRY);
			else
				clearP(CARRY);
			setA(getA()<<1);
			setFlags(getA());
		}
		else
		{
			if((128&operand)==128)
				setP(CARRY);
			else
				clearP(CARRY);
			writeMem(opAddress, readMem(opAddress)<<1);
			setFlags(readMem(opAddress));
		}
	} else if(operation == "BIT") { //Bit Test, set ZERO if none match. Then set AOVERFLOW to memory bit 6 and NEGATIVE to memory bit 7
		operand = readMem(opAddress);
		if((getA()&operand)==0)
			setP(ZERO);
		else
			clearP(ZERO);
		if((operand&BIT6)==BIT6) //if bit 6 set in operand
			setP(AOVERFLOW);
		else
			clearP(AOVERFLOW);
		if((operand&BIT7)==BIT7) //if bit 7 set in operand
			setP(NEGATIVE);
		else
			clearP(NEGATIVE);
			
	} else if(operation == "BPL") { //Branch if Positive BRANCH \/
		//TODO add cycles if branch succeeeds.
		operand = readMem(opAddress);
		if(!checkP(NEGATIVE))
			setPC(getPC()+toSInt(operand));
			
	} else if(operation == "BMI") { //Branch if Minus
		operand = readMem(opAddress);
		if(checkP(NEGATIVE))
			setPC(getPC()+toSInt(operand));
			
	} else if(operation == "BVC") { //Branch on AOVERFLOW Clear
		operand = readMem(opAddress);
		if(!checkP(AOVERFLOW))
			setPC(getPC()+toSInt(operand));
			
	} else if(operation == "BVS") { //Branch on AOVERFLOW Set
		operand = readMem(opAddress);
		if(checkP(AOVERFLOW))
			setPC(getPC()+toSInt(operand));
			
	} else if(operation == "BCC") { //Branch if Carry Clear
		operand = readMem(opAddress);
		if(!checkP(CARRY))
			setPC(getPC()+toSInt(operand));
			
	} else if(operation == "BCS") { //Branch if Carry Set
		operand = readMem(opAddress);
		if(checkP(CARRY))
			setPC(getPC()+toSInt(operand));	
			
	} else if(operation == "BNE") { //Branch if Not Equal (Zero Clear)
		operand = readMem(opAddress);
		if(!checkP(ZERO))
			setPC(getPC()+toSInt(operand));
			
	} else if(operation == "BEQ") { //Branch if Equal (Zero Set) BRANCH 
		operand = readMem(opAddress);
		if(checkP(ZERO))
			setPC(getPC()+toSInt(operand));
			
	} else if(operation == "BRK") { //Trigger software interrupt
	//TODO add this as an interrupt request
		byte firstPart, secondPart;
		firstPart = (getPC()&0xF0)>4;
		secondPart = (getPC()&0xF);
		stackPush(secondPart);
		stackPush(firstPart);
		
		stackPush(getP());
		setWaitCycles(7);
		setPC(toAddress(readMem(0xFFFe),readMem(0xFFFF)));
		setP(BRK);
		
	} else if(operation == "CMP") { //Compare (Accumulator)
		operand = readMem(opAddress);
		int temp = getA()-operand;
		setFlags(temp);
		if(temp >=0) 
			setP(CARRY);
		else
			clearP(CARRY);
		
	} else if(operation == "CPX") { //Compare X
		operand = readMem(opAddress);
		int temp = getX()-operand;
		setFlags(temp);
		if(temp >=0) 
			setP(CARRY);
		else
			clearP(CARRY);
	
	} else if(operation == "CPY") { //Compare Y
		operand = readMem(opAddress);
		int temp = getY()-operand;
		setFlags(temp);
		if(temp >=0) 
			setP(CARRY);
		else
			clearP(CARRY);
			
	} else if(operation == "DEC") { //Decrement Memory
		operand = readMem(opAddress);
		int result = operand - 1;
		writeMem(opAddress, result);
		setFlags(result);
		
	} else if(operation == "EOR") { //Exclusive OR (XOR)
		operand = readMem(opAddress);
		setA(getA()^operand);
		setFlags(getA());
		
	} else if(operation == "CLC") { //Clear Carry
		clearP(CARRY);
		
	} else if(operation == "SEC") { //Set Carry
		setP(CARRY);
		
	} else if(operation == "CLI") { //Clear Interrupt
		clearP(INTERRUPT);
		
	} else if(operation == "SEI") { //Set Interrupt
		setP(INTERRUPT);
		
	} else if(operation == "CLV") { //Clear AOVERFLOW
		clearP(AOVERFLOW);
		
	} else if(operation == "CLD") { //Clear Decimal
		clearP(DECIMAL);
		
	} else if(operation == "SED") { //Set Decimal
		setP(DECIMAL);
		
	} else if(operation == "INC") { //Increment Memory
		operand = readMem(opAddress);
		int result = operand + 1;
		writeMem(opAddress, result);
		setFlags(result);
		
	} else if(operation == "JMP") { //Jump
		setPC(opAddress-1);
		
	} else if(operation == "JMPa") { //Jump Absolute
	//TODO combine jumps?
		setPC(opAddress-1);
		
	} else if(operation == "JSR") { //Jump to Subroutine
		byte firstPart, secondPart;
		firstPart = (getPC());
		secondPart = (getPC())>>8;
		stackPush(secondPart);
		stackPush(firstPart);
		setPC(opAddress-1); 
		
	} else if(operation == "LDA") { //Load A
		//TODO when writing to registers, are flags always set if so combine them
		operand = readMem(opAddress);
		setA(operand);
		setFlags(operand);
		
	} else if(operation == "LDX") { //Load X
		operand = readMem(opAddress);
		setX(operand);
		setFlags(operand);
		
	} else if(operation == "LDY") { //Load Y
		operand = readMem(opAddress);
		setY(operand);
		setFlags(operand);
		
	} else if(operation == "LSR") { //Logical Shift Right
		operand = readMem(opAddress);
		if(addressmode == "accumulator"){
			if((1&getA())==1)
				setP(CARRY);
			else
				clearP(CARRY);
			setA(getA()>>1);
			setFlags(getA());
		}
		else
		{
			if((1&operand)==1)
				setP(CARRY);
			else
				clearP(CARRY);
			writeMem(opAddress, readMem(opAddress)>>1);
			setFlags(readMem(opAddress));
		}
	
	} else if(operation == "NOP") { //No Operation
		//Does nothing
	} else if(operation == "ORA") { //Inclusive OR  
		operand = readMem(opAddress);
		setA(getA()|operand);
		setFlags(getA());
		
	} else if(operation == "TAX") { //Transfer A to X
		setX(getA());
		setFlags(getX());
		
	} else if(operation == "TXA") { //Transfer X to A
		setA(getX());
		setFlags(getA());
		
	} else if(operation == "DEX") { //Decrement X
		setX(getX()-1);
		setFlags(getX());
		
	} else if(operation == "INX") { //Increment X
		setX(getX()+1);
		setFlags(getX());
		
	} else if(operation == "TAY") { //Transfer A to Y
		setY(getA());
		setFlags(getY());
		
	} else if(operation == "TYA") { //Transfer Y to A
		setA(getY());
		setFlags(getY());
		
	} else if(operation == "DEY") {	//Decrement Y
		setY(getY()-1);
		setFlags(getY());
		
	} else if(operation == "INY") { //Increment Y
		setY(getY()+1);
		setFlags(getY());
		
	} else if(operation == "ROL") { //Rotate Left TODO Test
		operand = readMem(opAddress);
		bool oldcarry = checkP(CARRY);
		bool newcarry;
		if(addressmode == "accumulator"){
			if((BIT7&getA())==BIT7)
			newcarry = true;
		else
			newcarry = false;
		if(newcarry)
			setP(CARRY);
		else
			clearP(CARRY);
			
			setA(getA()<<1);
			if(oldcarry)
				setA(getA()|BIT0);
			else
				setA((getA()&(~BIT0)));	
			setFlags(getA());		
		}
		else
		{	
			if((BIT7&operand)==BIT7)
				newcarry = true;
			else
				newcarry = false;
			if(newcarry)
				setP(CARRY);
			else
				clearP(CARRY);
			
			byte temp = readMem(opAddress);		
			writeMem(opAddress, temp<<1);
			temp = readMem(opAddress);
				
			if(oldcarry)
				writeMem(opAddress, (temp|BIT0));
			else
				writeMem(opAddress, (temp&(~BIT0)));	
			setFlags(temp);
		}
		
	} else if(operation == "ROR") { //Rotate Right TODO Test
		operand = readMem(opAddress);
		bool oldcarry = checkP(CARRY);
		bool newcarry;	
		if(addressmode == "accumulator"){
			//check carry bit
			if((BIT0&getA())==BIT0)
				newcarry = true;
			else
				newcarry = false;
			
			if(newcarry)
				setP(CARRY);
			else
				clearP(CARRY);
			
			//write
			setA(getA()>>1);
			if(oldcarry)
				setA(getA()|BIT7);
			else
				setA((getA()&(~BIT7)));
				
			setFlags(getA());
					
		}
		else
		{
			if((BIT0&operand)==BIT0)
				newcarry = true;
			else
				newcarry = false;
			if(newcarry)
				setP(CARRY);
			else
				clearP(CARRY);	
			
			byte temp = readMem(opAddress);		
			writeMem(opAddress, temp>>1);
			temp = readMem(opAddress);
			
			if(oldcarry)
				writeMem(opAddress, (temp|BIT7));
			else
				writeMem(opAddress, (temp&(~BIT7)));
					
			setFlags(temp);
			
		}
	} else if(operation == "RTI") { //Return from Interrupt 
		clearP(0xFF);
		setP(stackPop());
		int firstPart = stackPop();
		int secondPart = stackPop();
		setWaitCycles(6);
		setPC(toAddress(firstPart,secondPart)-1);
	} else if(operation == "RTS") {	//Return from Subroutine
		int firstPart = stackPop();
		int secondPart = stackPop();
		int address = toAddress(firstPart,secondPart);
		setWaitCycles(6);
		setPC(address);
		
	} else if(operation == "SBC") { //Subtract With Carry
		operand = readMem(opAddress);
		byte carry;
		int resultTest;
		
		if(checkP(CARRY))
			carry=1;
		else
			carry=0;
			
		resultTest=getA()-operand-(1-carry);
		
		if((getA() ^ resultTest) & (getA() ^ operand) & 0x80)
			setP(AOVERFLOW);
		else
			clearP(AOVERFLOW);
		
		setA(getA()-operand-(1-carry));
		
		if(resultTest < 0)
			clearP(CARRY);
		else
			setP(CARRY);				
		setFlags(getA());
		
	} else if(operation == "STA") { //Store A into Memory
		writeMem(opAddress,getA());
		
	} else if(operation == "TXS") { //Transfer X register to Stack Register
		setS(getX());
		
	} else if(operation == "TSX") { //Transfer Stack Register to X
		setX(getS());
		setFlags(getX());
		
	} else if(operation == "PHA") { //Push the accumulator on to the stack
		stackPush(getA());
		setWaitCycles(3);
		
	} else if(operation == "PLA") { //Pull the value from the stack into the Accumulator
		setA(stackPop());
		setFlags(getA());
		setWaitCycles(4);
		
	} else if(operation == "PHP") { //Push the status flags onto the stack
		stackPush((getP()|0x10)); //For some reason the break flag is or'd...  http://forums.nesdev.com/viewtopic.php?f=3&t=8448&hilit=PLP+Break
		setWaitCycles(3);
		
	} else if(operation == "PLP") { //Pull the value from the stack into the status flags
		clearP(0xFF);
		setP(stackPop());
		setWaitCycles(4);
		
	} else if(operation == "STX") { //Store X into Memory
		writeMem(opAddress,getX());
		
	} else if(operation == "STY") {	//Store Y into Memory
		writeMem(opAddress,getY());
	}
}

//Converts a 0xAA, 0xBB into a 0xBBAA Little Eidiean address
int CPU::toAddress(byte firstByte, byte secondByte){
	return firstByte | (secondByte << 8);
}

/*Some OPCodes require a certain number of cycles to pass to complete the operation.
 *Theoretically the operation candbe completed on the same cycle, so we wait for the
 *operation to 'complete' before accepting other operation. Wasted CPU cycles but
 *its the best I can think of right now.	*/		

void CPU::setWaitCycles(short toWait){
	cycleWait = toWait;
}
void CPU::addWaitCycles(short toAdd){
	cycleWait += toAdd;
}	

//Boolean to check if the program is waiting to complete an operation.	
//TODO getter method should not modify anything - what if called twice in a cycle?
bool CPU::sleeping(){
	if(cycleWait>0){
		cycleWait--;
		return true;
	}
	else
		return false;
}

//Immediately resets the counter for waiting, useful for resetting and possibly interrupts
void CPU::wake(){
	cycleWait=0;
}

//Transforms a byte into an signed integer
//TODO is this used?
signed char CPU::toSInt(byte input)
{
	signed char out = 0;
	//cout << endl << hex << bitset<8>(input) << " to int = " ;
	if((input&0x80)==0x80)
		out = -(~input+1); //Two's Complement
	else
		out = (input&0x7F);
	
	//cout << bitset<8>(out) << endl;
	return out; 
}

//---GET & SET METHODS---//

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

//TODO This status flag stuff really needs to be fixed so I don't have to clear and then set to avoid errors.
//Set and clear status flags 
void CPU::setP(byte add){
	p = p | add;
	p = p | 0x20;
}

void CPU::clearP(byte sub){
	sub = ~sub;
	p = p & sub;
}
 
bool CPU::checkP(byte chk){
	if((p&chk) == chk)
		return true;
	else
		return false;
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

//Returns stack pointer location. Stack pointer is the first location of free space, starting from 0x0100 to 0x01FF.
//Note, stack is hardcoded on page 0x01 so getS() = FF actually means 0x01FF.
//Also, the 6502 Stack is descending, so the first byte of free space is FF.
byte CPU::getS(){
	return s;
}
//Returns all "8" bits of the "6" bit status register
byte CPU::getP(){
	return p;
}

//Given the input set the negative and zero flags as needed.
void CPU::setFlags(byte operand){
	if(operand == 0){
		clearP(ZERO);
		setP(ZERO);
	}else
		clearP(ZERO);
		
	if((operand&0x80) == 0x80)
		setP(NEGATIVE);
	else
		clearP(NEGATIVE);
}

//Pops the byte off the "bottom" of the stack, then increments the stack pointer.
//TODO rename to pull to avoid confusion and mesh with instruction names
byte CPU::stackPop(){
	byte temp = readMem(toAddress(getS()+1,0x01));
	//cout << endl << "Popping value: " << hex << (int)temp << " from address " << toAddress(getS()+1,0x01) << endl; 
	setS(getS()+1);
	return temp;
}

//Pushs a value onto the "bottom" of the stack and decrements the stack pointer.
void CPU::stackPush(byte toPush){
	writeMem(toAddress(getS(),0x01), (int)toPush);
	//cout <<endl <<"Pushing value: " << hex << (int)toPush << " to address " << toAddress(getS(),0x01) <<endl; 
	setS(getS()-1);
	
}

//Returns the value on the "bottom" of the stack, but does not change/remove it, and does not move the stack pointer. Unsed but could be useful
byte CPU::stackPeek(){
	return readMem(toAddress(getS(),0x01));
}

//Page Cross Detection
bool CPU::pageCrossed(int address1, int address2){
	if((address1&0xFF00) == (address2&0xFF00))
		return false;
	else
		return true;
}
 
//Dumps the entire memory of the CPU to a file.
void CPU::memoryDump(){
	ofstream dump;
	dump.open("../res/logs/alianesLast.dump");
	for(int i = 0; i < 0xFFFF; i++)
	{
		dump << memory[i];
	}
	dump.close();
}

//DEBUG
//TODO include all output in one function for finer and more organized debug
void CPU::printDebugStatus(int address){
	//cout << endl << address << ": " << hex << (int)readMem(address) << " " << hex << (int)readMem(address+1) << " " << hex << (int)readMem(address+2);
	stringstream stack;
	stack << " - stack: top ||<- ";

	for(int i = getS(); i<s+8; i++){
		stack << hex << (int)readMem(toAddress(i,0x01)) << ",";
	}
	
	string stack2 = stack.str();
	
	cout << setw(5) << "A:" << hex << (int)getA() << " X:" << hex << (int)getX() <<  " Y:" << hex << (int)getY() << " P:" << hex << (int)getP() << " SP:" << hex << (int)getS() << stack2;
	lout << setw(5) << "A:" << hex << (int)getA() << " X:" << hex << (int)getX() <<  " Y:" << hex << (int)getY() << " P:" << hex << (int)getP() << " SP:" << hex << (int)getS() << stack2;
}