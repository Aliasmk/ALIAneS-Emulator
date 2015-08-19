/* CPU Header :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#ifndef NESCPU
#define	NESCPU

#include <string>
#include <cstdint>
#include <iostream>
#include <iomanip>

class CPU {
	private:
		typedef std::uint8_t byte;
		
		enum statusReg {
			CARRY = 1,	//
			ZERO = 2,		//Set to 1 when any arithmetic or logical operation produces a zero result
			INTERUPT = 4,	//If it is set, interrupts are disabled.
			DECIMAL = 8,	//
			BRK = 16,	//This is set when a software interrupt (BRK instruction) is executed.
			UNUSED = 32,	//Not used. Supposed to be logical 1 at all times.
			OVERFLOW = 64,//Set when an arithmetic operation produces a result too large to be represented in a byte
			SIGN = 128	//this is set if the result of an operation is negative, cleared if positive.
		};

		
		byte a; //Accumulator
		byte x; //X Register
		byte y; //Y Register
		uint16_t pc; //Program Counter
		byte s; //Stack Counter
		byte p; //Processor Flags
		byte memory[0xFFFF]; //Memory
		short cycleWait;
		byte resetVector; //Memory location to begin execution from
	public:
		CPU();
		
		//Normal functions
		void start(); //Begin the CPU startup sequence
		void stop(std::string reason); //Begin the CPU shutdown sequence
		bool running; 
		void cycle();
		
		//Memory
		void writeMem(int address, byte value); //Write to address a byte
		void writeMem(int address1, int address2, byte value); //Write to range of addresses a byte
		byte readMem(int address); //Read from an address
		byte readMem(byte firstByte, byte secondByte); //read from two bytes, reversing the significance.
		byte readNext(); //Increment the PC and read at that location
		
		//Decoding and running OPcodes
		void decode(byte opCode);
		void decode(byte opCode, byte param1);
		void decode(byte opCode, byte param1, byte param2);
		void decode(byte opCode, byte param1, byte param2, byte param3);
		void decodeAt(int address);
		void waitForCycles(short toWait);		
		bool sleeping();
		void wake();
		void execute(std::string operation, std::string addressmode);
		
		int toAddress(byte firstByte, byte secondByte);
		
		//Setter Functions
		void setA(byte a);
		void setX(byte x);
		void setY(byte y);
		void setPC(uint16_t address);
		void incPC();
		void setS(byte s);
		void setP(byte add);
		void clearP(byte sub);
		bool checkP(byte chk);
		//void setP(byte p);
		//Getter Functions
		byte getA();
		byte getX();
		byte getY();
		uint16_t getPC();
		byte getS();
		byte getP();
	

};
#endif