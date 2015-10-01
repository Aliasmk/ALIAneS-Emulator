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


int spacing = 15;
int startoverride=0xc000;
bool logging = true;
ofstream lout;

//Constructor which not used yet
CPU::CPU(){}
	
//Startup procedure for the CPU, following what is on NESDEV wiki
void CPU::start(){
	cycleCount=0;
	//Initialize Register Values
	setP(0x24); //TODO change back to 0x34
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
	
	if(startoverride==0)
		setPC(toAddress(readMem(0xFFFC),readMem(0xFFFD)));
	else
		setPC(startoverride);
	
	

	//Set the program counter to equal the reset vector, located at 0xFFFC
	//Because the addressing in the 6502 is backwards, FFFC is concatenated to the end of 
	//FFFD using bitwise shifts and ORs. If the override is specified, set it to that (in
	//the case of cpu only nestest for example)
	
	
	if(logging)
		lout.open("../res/logs/alianesLast.log");
	
	//This is a very long line to output a short amount of debug text
	cout << "PC: "  <<setfill('0')<< setw(4)<<hex<< (int)getPC() << " reset vectors: " << setw(2)<<setfill('0') <<hex << (int)readMem(0xfffc) <<setw(2)<<setfill('0')<<hex<< (int)readMem(0xfffd)<<endl;
	cout << "Startup: Completed"<<endl;
	//DEBUG
	if(logging)
		cout << "Execution logging is enabled. This will drastically slow performance and should only be used for small scale debug." << endl;
	cout << setfill (' ') << "Address" <<setw(spacing)<<"OPCode" <<setw(spacing) << "AddressMode" << setw(spacing) << "OpAddr" << setw(spacing) << "Operand" << setw(spacing) << "NextByte" << setw(spacing) << "ByteAfter" << endl;
	
	
	
	wake();
	
	//Tests go below:
	//decodeAt(getPC());
	//cout << "Output (6e) : " << (int)getA()<<endl;
	//stop("Tests complete");
	//END tests
}
void CPU::stop(string reason){
	cout << "Stopping CPU: " << reason << endl;
	running=false;
}


//Function to be called each time the system cycles, to perform CPU tasks.
//TODO: remove this var and the 20000 cycle code in cycle() (temporary)

void CPU::cycle(){
	if(cycleCount<100)
	{
		if(!sleeping()){
			
			decodeAt(getPC());
			incPC();
		}
	}
	else{
		stop("Finished 100 cycles");
		if(logging)
			lout.close();	
	}
		
	cycleCount++;
	
}

//---MEMORY ACCESS---//

//Returns a memory at selected address
byte CPU::readMem(int address){
	return memory[address];
}

//Give <instruction> (firstbyte) (secondbyte) this returns memory at secondbyte:firstbyte
byte CPU::readMem(byte firstByte, byte secondByte)
{
	int address = firstByte | (secondByte<<8); 
	return memory[address];
}
//Increments the program counter and reads the next bit
byte CPU::readNext(){
	incPC();
	return readMem(getPC());
}

//Writes given data memory to the selected address
void CPU::writeMem(int address, byte value){
	//TODO implement memory mirroring for RAM and PPU registers
	//if(value != 0x00)
		
		//cout << "Writing data: " << hex << (int)value << " to address " << hex << address << endl;	
	memory[address] = value;
}
//Writes given data to a range on addresses between start and end
void CPU::writeMem(int addressStart, int addressEnd, byte value){
	for(int i=addressStart; i<=addressEnd; i++)
	{
		//cout << i;
		memory[i] = value;
		if(value != 0x00)
			cout << "Writing non-zero data: " << hex << value << " to address " << hex << i << endl;
	}
}

//---DECODING---//

//uint16_t  get16bit(uint16_t pc){
//	return (readMem(pc+1)<<4)&(readMem(pc));
//}

//OPCode decoding methods. TODO: Implement these
void CPU::decode(byte opCode){}
void CPU::decode(byte opCode, byte param1){}
void CPU::decode(byte opCode, byte param1, byte param2){}
void CPU::decode(byte opCode, byte param1, byte param2, byte param3){}


void CPU::decodeAt(int address){
	byte opcode = readMem(address);
	//Split opcode up into bits based on info from here: http://www.llx.com/~nparker/a2/opcodes.html
	//By decoding at a opcode bit level we save a lot on switch and if statements.
	byte aaa = (opcode & 0xe0) >> 5;
	byte bbb = (opcode & 0x1c) >> 2 ;
	byte cc  = (opcode & 0x03);
	//cout  << (int)opcode << " -aaa: " << aaa << " -bbb: " << bbb << " -cc: " << cc << endl; 
	
	bool valid = false;
	string operation = "";
	string addressmode = "implied";

	
	//CC: General Set. AAA: Instruction. BBB: Addressing mode. Text strings passed later.
	//First decode the address and operands required based on the address mode asked for and store it. Then use it in the specific instruction set.
	
	if(cc == 1) //cc = 01
	{
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
						addressmode = "absolute,y";
					break;
				}
		switch(aaa)	{
			case 0:
				operation = "ASL";
				valid = true;
			break;
			case 1:
				operation = "ROL";
				valid = true;
			break;
			case 2:
				operation = "LSR";
				valid = true;
			break;
			case 3:
				operation = "ROR";
				valid = true;
			break;
			case 4:
				operation = "STX";
				valid = true;
			break;
			case 5:
				operation = "LDX";
				valid = true;
			break;
			case 6:
				operation = "DEC";
				valid = true;
			break;
			case 7:
				operation = "INC";
				valid = true;
			break;
		}
	}
	else if(cc == 0) //cc = 00
	{
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
		switch(aaa)	{
			case 1:
				operation = "BIT";
				valid = true;
			break;
			case 2:
				operation = "JMPa";
				valid = true;
			break;
			case 3:
				operation = "JMP";
				valid = true;
			break;
			case 4:
				operation = "STY";
				valid = true;
			break;
			case 5:
				operation = "LDY";
				valid = true;
			break;
			case 6:
				operation = "CPY";
				valid = true;
			break;
			case 7:
				operation = "CPX";
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
				valid=true; operation="PHP";
			break;
			case 0x28:
				valid=true; operation="PLP";
			break;
			case 0x48:
				valid=true; operation="PHA";
			break;
			case 0x68:
				valid=true; operation="PLA";
			break;
			case 0x88:
				valid=true; operation="DEY";
			break;
			case 0xA8:
				valid=true; operation="TAY";
			break;
			case 0xC8:
				valid=true; operation="INY";
			break;
			case 0xE8:
				valid=true; operation="INX";
			break;
			case 0x18:
				valid=true; operation="CLC";
			break;
			case 0x38:
				valid=true; operation="SEC";
			break;
			case 0x58:
				valid=true; operation="CLI";
			break;
			case 0x78:
				valid=true; operation="SEI";
			break;
			case 0x98:
				valid=true; operation="TYA";
			break;
			case 0xb8:
				valid=true; operation="CLV";
			break;
			case 0xD8:
				valid=true; operation="CLD";
			break;
			case 0xF8:
				valid=true; operation="SED";
			break;
			
			case 0x8a:
				valid=true; operation="TXA";
			break;
			case 0x9a:
				valid=true; operation="TXS";
			break;
			case 0xaa:
				valid=true; operation="TAX";
			break;
			case 0xba:
				valid=true; operation="TSX";
			break;
			case 0xca:
				valid=true; operation="DEX";
			break;
			case 0xea:
				valid=true; operation="NOP"; addressmode = "";
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
				valid=true; operation="RTI";
			break;
			case 0x60:
				valid=true; operation="RTS";
			break;
		}
	
	if(standardInstruction == false || valid == false) 
	{
		//addressmode = "";
	}	
	
	
	//DEBUG
	cout << dec <<cycleCount << " - " << hex << address << ":";
	if(logging)
		lout << hex << address;
	if(!valid)
		cout << setw(spacing) << "UKN" << hex << (int)opcode;
	else {
		cout << setw(spacing) << operation << " " << hex << (int)opcode << setw(spacing) << addressmode;
		if(logging)
			lout << setw(spacing) << operation << " " << hex << (int)opcode << setw(spacing) << addressmode;
	}
	execute(operation, addressmode);
	cout << endl;
	if(logging)
		lout << endl;
	
}

/*DEBUG - Cycle Timings Implementation
done	Immediate: 		2
		Zero Page: 		3, 5(ASL, DEC, INC, LSR, ROL, ROR)
		Zero Page,X: 	4, 6(ASL, DEC, INC, LSR, ROL, ROR)
		Absolute:		4, 6(ASL, DEC, INC, LSR, ROL, ROR)
		Absolute,X:		4+1, 7(ASL, DEC, INC, LSR, ROL, ROR, JMP)
		Absolute,Y:		4+1, 5(STA)
done	(Indirect,X):	6
		(Indirect),Y:	5+1, 6(STA)
		Implied:		2, 3(PHA, PHP), 4(PLA, PLP), 6(RTI, RTS), 7(BRK)
done	Accumulator:	2
*/

void CPU::execute(string operation, string addressmode)
{
	//Will migrate execution code here after the next backup
	
	//ADDRESSING MODES - Gives both a decoded address and a value to work with, methods use whichever they need.
	byte firstByte, secondByte;
	int opAddress = 0;
	int operand = 0;
	int sleepCycles=0;
	
	//Grab the first and second byte after the operation, we may only need one, or none. Remember most significant byte last
	firstByte = readMem(getPC()+1);
	secondByte = readMem(getPC()+2);
	if(addressmode == "immediate"){ //Immediate uses the next byte as a direct data argument
			//operand = readNext();
			incPC();
			opAddress = getPC();
			setWaitCycles(2);
			
	}else if (addressmode == "absolute"){ //Absolute is an explicit definition of the location in memory to use
		//firstByte = readNext();
		//secondByte = readNext();
		//operand = toAddress(firstByte, secondByte);
		incPC();
		incPC();
		opAddress = toAddress(firstByte, secondByte);
		
	} else if (addressmode == "zeropage") { //Zero Page is absolute but only for the first 256 bits of memory (0x0000 - 0x00ff)
		//firstByte = readNext();
		//operand = toAddress(firstByte, 0);
		incPC();
		opAddress = toAddress(firstByte,0);
	
	} else if (addressmode == "accumulator") { //Targets the accumulator
		//operand = getA();
		incPC();
		setWaitCycles(2);
	} else if (addressmode == "absolute,x") { //Absolute value added to the X register
		//firstByte = readNext();
		//secondByte = readNext();
		//operand = readMem(toAddress(firstByte, secondByte) + getX());
		incPC();
		incPC();
		opAddress = toAddress(firstByte,secondByte)+getX();
		
	} else if (addressmode == "absolute,y") { //Absolute value added to the Y register
		//firstByte = readNext();
		//secondByte = readNext();
		//operand = readMem(toAddress(firstByte, secondByte) + getY());
		incPC();
		incPC();
		opAddress = toAddress(firstByte,secondByte)+getY();
		
	} else if (addressmode == "zeropage,x") { 
		//firstByte = readNext();
		//operand = readMem(toAddress(firstByte, 0)+getX());
		incPC();
		opAddress = toAddress(firstByte,0)+getX();
		
	} else if (addressmode == "(zeropage,x)") { 
		//AKA (Indirect, X)
		firstByte = readNext();
		byte tempAddress = firstByte+getX();
		
		if(tempAddress > 0xFF)
			tempAddress-=0xFF;
		byte tempAddress2 = tempAddress+1;
		if(tempAddress2 > 0xFF)
			tempAddress-=0xFF;
		
		//DEBUG	
		//cout << (int)tempAddress << endl;
		//cout << (int)tempAddress2 << endl;
		//cout << toAddress(readMem(tempAddress), readMem(tempAddress2));
		//cout << (int)readMem(toAddress(readMem(tempAddress), readMem(tempAddress2))) <<endl;
		opAddress = toAddress(readMem(tempAddress), readMem(tempAddress2));
		setWaitCycles(6)
	} else if (addressmode == "(zeropage),y") { 
		//AKA (Indirect),Y)
		firstByte = readNext(); //start address
		byte firstAddress = readMem(firstByte, 0);
		byte secondAddress = readMem(firstByte+1, 0);
		
		//DEBUG
		//cout << (int)firstAddress << endl;
		//cout << (int)secondAddress << endl;

		operand = readMem(toAddress(firstAddress, secondAddress)+getY());
	} else {
		//firstByte = -1;
		//secondByte = -1;
		//operand = -1;
	}
	operand = readMem(opAddress);
	if(addressmode != "implied" && addressmode != "accumulator"){
			
			cout << setw(spacing)<< hex << (int)opAddress << setw(spacing) << (int)operand; // << setw(spacing) <<(int)firstByte << setw(spacing) << hex << (int)secondByte;
			printDebugStatus(getPC());
	}
	
	
	//INSTRUCTIONS
	if(operation == "ADC"){ //Add with Carry
		byte carry;
		int resultTest;
		
		
		if(checkP(CARRY))
			carry=1;
		else
			carry=0;
			
		resultTest=getA()+operand+carry;
		setA(getA()+operand+carry);
		if(resultTest > 255)
			setP(CARRY);
					
		setFlags(getA());
	
	} else if(operation == "AND") { //Perform an AND with the read byte on the Accumulator. Uses: Value
		//cout << endl << "AND: " << bitset<8>(getA());
		//cout << endl << " ON: " << bitset<8>(operand);
		
		setA(getA()&operand);
		setFlags(getA());
		
		
	} else if(operation == "ASL") { //Perform a left shift on the accumulator or memory, store old bit 7 in the carry flag
		if(addressmode == "accumulator"){
			if((128&getA())==128)
				setP(CARRY);
			else
				clearP(CARRY);
			setA(getA()<1);
		}
		else
		{
			if((128&operand)==128)
				setP(CARRY);
			else
				clearP(CARRY);
			writeMem(opAddress, readMem(opAddress)<1);
		}
	} else if(operation == "BIT") { //Bit Test, set ZERO if none match. Then set OVERFLOW to memory bit 6 and NEGATIVE to memory bit 7
		if((getA()&operand)==0)
			setP(ZERO);
		if((operand&0x40)==0x40) //if bit 6 set in operand
			setP(OVERFLOW);
		else
			clearP(OVERFLOW);
		if((operand&0x80)==0x80) //if bit 7 set in operand
			setP(NEGATIVE);
		else
			clearP(NEGATIVE);
	} else if(operation == "BPL") { //Branch if Positive BRANCH \/
		//incPC();
		//incPC();
		if(!checkP(NEGATIVE))
			setPC(getPC()+toSInt(operand));
	} else if(operation == "BMI") { //Branch if Minus
		//incPC();
		//incPC();
		if(checkP(NEGATIVE))
			setPC(getPC()+toSInt(operand));
	} else if(operation == "BVC") { //Branch on Overflow Clear
		//incPC();
		//incPC();
		if(!checkP(OVERFLOW))
			setPC(getPC()+toSInt(operand));
	} else if(operation == "BVS") { //Branch on Overflow Set
		//incPC();
		//incPC();
		if(checkP(OVERFLOW))
			setPC(getPC()+toSInt(operand));
	} else if(operation == "BCC") { //Branch if Carry Clear
		//incPC();
		//incPC();
		if(!checkP(CARRY))
			setPC(getPC()+toSInt(operand));
	} else if(operation == "BCS") { //Branch if Carry Set
		//incPC();
		//incPC();
		if(checkP(CARRY))
			setPC(getPC()+toSInt(operand));	
	} else if(operation == "BNE") { //Branch if Not Equal (Zero Clear)
		//incPC();
		//incPC();
		if(!checkP(ZERO))
			setPC(getPC()+toSInt(operand));
	} else if(operation == "BEQ") { //Branch if Equal (Zero Set) BRANCH 
		//incPC();
		//incPC();
		if(checkP(ZERO))
			setPC(getPC()+toSInt(operand));
	} else if(operation == "BRK") { //Trigger software interrupt
		byte firstPart, secondPart;
		firstPart = (getPC()&0xF0)>4;
		secondPart = (getPC()&0xF);
		stackPush(secondPart);
		stackPush(firstPart);
		
		stackPush(getP());
		
		setPC(toAddress(readMem(0xFFFe),readMem(0xFFFF)));
		setP(BRK);
	} else if(operation == "CMP") { //Compare (Accumulator)
		
		int temp = getA()-operand;
		setFlags(temp);
		if(temp >=0) //a>=operand
			setP(CARRY);
		
	} else if(operation == "CPX") { //Compare X
		int temp = getX()-operand;
		setFlags(temp);
		if(temp >=0) //a>=operand
			setP(CARRY);
	
	} else if(operation == "CPY") { //Compare Y
		int temp = getY()-operand;
		setFlags(temp);
		if(temp >=0) //a>=operand
			setP(CARRY);
	
	} else if(operation == "DEC") { //Decrement Memory
		int result = operand - 1;
		writeMem(opAddress, result);
		setFlags(result);
	} else if(operation == "EOR") { //Exclusive OR (XOR)
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
	} else if(operation == "CLV") { //Clear Overflow
		clearP(OVERFLOW);
	} else if(operation == "CLD") { //Clear Decimal
		clearP(DECIMAL);
	} else if(operation == "SED") { //Set Decimal
		setP(DECIMAL);
	} else if(operation == "INC") { //Increment Memory
		int result = operand + 1;
		writeMem(opAddress, result);
		setFlags(result);
	} else if(operation == "JMP") { //Jump
		addressmode = "indirect";
		firstByte=readNext();
		secondByte=readNext();
		int indirectAddress = toAddress(firstByte,secondByte);
		operand=toAddress(indirectAddress, indirectAddress+1);
		setPC(operand);
		
		//TOTDO setPC(toAddress(readMem(toAddress(firstByte, secondByte)), readMem(toAddress(firstByte, secondByte)+1)));
		//setPC(operand);
	} else if(operation == "JMPa") { //Jump Absolute
		setPC(opAddress-1);
	} else if(operation == "JSR") { //Jump to Subroutine
		byte firstPart, secondPart;
		firstPart = (getPC()&0xF0)>4;
		secondPart = (getPC()&0xF);
		stackPush(secondPart);
		stackPush(firstPart);
		setPC(opAddress-1); 
	} else if(operation == "LDA") { //Load A
		setA(operand);
		setFlags(operand);
	} else if(operation == "LDX") { //Load X
		setX(operand);
		setFlags(operand);
	} else if(operation == "LDY") { //Load Y
		setY(operand);
		setFlags(operand);
	} else if(operation == "LSR") { //Logical Shift Right
		if(addressmode == "accumulator"){
			if((1&getA())==1)
				setP(CARRY);
			else
				clearP(CARRY);
			setA(getA()>1);
		}
		else
		{
			if((1&operand)==1)
				setP(CARRY);
			else
				clearP(CARRY);
			writeMem(opAddress, readMem(opAddress)>1);
		}
	
	} else if(operation == "NOP") { //No Operation
		//Does nothing
	} else if(operation == "ORA") { //Inclusive OR  
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
		bool oldcarry = checkP(CARRY);
		bool newcarry;
		if((BIT7&getA())==BIT7)
			newcarry = true;
		else
			newcarry = false;
		if(newcarry)
			setP(CARRY);
		else
			clearP(CARRY);
		
		if(addressmode == "accumulator"){
			setA(getA()<1);
			if(oldcarry)
				setA(getA()|BIT0);
			else
				setA((getA()&(~BIT0)));	
			setFlags(getA());		
		}
		else
		{
			writeMem(opAddress, readMem(opAddress)<1);
			if(oldcarry)
				writeMem(opAddress, (getA()|BIT0));
			else
				writeMem(opAddress, (getA()&(~BIT0)));	
			setFlags(readMem(opAddress));
		}
		
	} else if(operation == "ROR") { //Rotate Right TODO Test
		bool oldcarry = checkP(CARRY);
		bool newcarry;
		if((BIT0&getA())==BIT0)
			newcarry = true;
		else
			newcarry = false;
		if(newcarry)
			setP(CARRY);
		else
			clearP(CARRY);
		
		if(addressmode == "accumulator"){
			setA(getA()>1);
			if(oldcarry)
				setA(getA()|BIT7);
			else
				setA((getA()&(~BIT7)));
				
			setFlags(getA());		
		}
		else
		{
			writeMem(opAddress, readMem(opAddress)>1);
			if(oldcarry)
				writeMem(opAddress, (getA()|BIT0));
			else
				writeMem(opAddress, (getA()&(~BIT0)));	
			setFlags(readMem(opAddress));
		}
	} else if(operation == "RTI") { //Return from Interrupt 
		setP(0);
		setP(stackPop());
		int firstPart = stackPop();
		int secondPart = stackPop();
		setPC(toAddress(firstPart,secondPart));
	} else if(operation == "RTS") {	//Return from Subroutine
		int firstPart = stackPop();
		int secondPart = stackPop();
		int address = toAddress(firstPart,secondPart)-1;
		setPC(address);
	} else if(operation == "SBC") { //Subtract With Carry
		byte carry;
		int resultTest;
		
		
		if(checkP(CARRY))
			carry=1;
		else
			carry=0;
			
		resultTest=getA()-operand-(1-carry);
		
		setA(getA()-operand-(1-carry));
		if(resultTest < 0)
			clearP(CARRY);
					
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
	} else if(operation == "PLA") { //Pull the value from the stack into the Accumulator
		setA(stackPop());
		setFlags(getA());
	} else if(operation == "PHP") { //Push the status flags onto the stack
		stackPush(getP());
	} else if(operation == "PLP") { //Pull the value from the stack into the status flags
		setP(stackPop());
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
bool CPU::sleeping(){
	if(cycleWait>0){
		cout << dec << cycleCount << " - sleeping for " << cycleWait << " more cycles" << endl;
		cycleWait--;
		return true;
	}
	else
		return false;
}



//Resets the counter for waiting, useful for resetting and possibly interrupts
void CPU::wake(){
	cycleWait=0;
}

//Transforms a byte into an signed integer
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
//Set and clear status flags 
void CPU::setP(byte add){
	//1:10010101
	//2:01001001
	//R:11011101
	
	//OR Orig with args
	
	//1:10010101
	//2:01001001
	//OR 1,2
	//3:11011101
	//cout << endl << "Set Procstat with " << hex << (int)add << " " << bitset<8>(add) << ", before: " << bitset<8>(p) << endl;
	p = p | add;
	//cout << endl<< "Procstat after: " << bitset<8>(p) << endl;
}

void CPU::clearP(byte sub){
	
	//cout << "clear Procstat with " << hex << (int)sub << " " << bitset<8>(sub) << ", before: " << bitset<8>(p) << endl;
	sub = ~sub;
	p = p & sub;
	//cout << "Procstat after: " << bitset<8>(p) << endl;

	//1:01001001
	//2:01101010
	//R:00000001
	
	//Invert Search Arg, then AND the Orig arg with the inverted search
	
	//1:01001001
	//2:01101010
	//Invert 2:
	//3:10010101
	
	//1:01001001
	//3:10010101
	//AND 1,3
	//4:00000001
}
 
bool CPU::checkP(byte chk){
	//1:01001001
	//2:01000000
//AND  :01000000 = original chk

	//1:01001001
	//2:00100000
//AND  :00000000

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
//Returns all 8 bits of the 6 bit status 
byte CPU::getP(){
	return p;
}

void CPU::setFlags(byte operand){
	if(operand == 0)
		setP(ZERO);
	else
		clearP(ZERO);
		
	if((operand&0x80) == 0x80)
		setP(NEGATIVE);
	else
		clearP(NEGATIVE);
}

//Pops the byte off the "bottom" of the stack, then increments the stack pointer.
byte CPU::stackPop(){
	byte temp = readMem(toAddress(getS(),0x01));
	setS(getS()+1);
	return temp;
}

//Pushs a value onto the "bottom" of the stack and decrements the stack pointer.
void CPU::stackPush(byte toPush){
	writeMem(toAddress(getS(),0x01), toPush);
	setS(getS()-1);
	
}

//Returns the value on the "bottom" of the stack, but does not change/remove it, and does not move the stack pointer.
byte CPU::stackPeek(){
	return readMem(toAddress(getS(),0x01));
}

bool CPU::ifPageCrossed(){
	return false;
}

//DEBUG
void CPU::printDebugStatus(int address){
	//cout << endl << address << ": " << hex << (int)readMem(address) << " " << hex << (int)readMem(address+1) << " " << hex << (int)readMem(address+2);
	cout << setw(5) << "A:" << hex << (int)getA() << " X:" << hex << (int)getX() <<  " Y:" << hex << (int)getY() << " P:" << hex << (int)getP() << " SP:" << hex << (int)getS();
	lout << setw(5) << "A:" << hex << (int)getA() << " X:" << hex << (int)getX() <<  " Y:" << hex << (int)getY() << " P:" << hex << (int)getP() << " SP:" << hex << (int)getS();
}