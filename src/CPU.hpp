/* CPU Header :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#ifndef NESCPU
#define	NESCPU

#include <string>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <iomanip>


class CPU {
	private:
		typedef std::uint8_t byte;
		
		enum interrupt {
			NONE = 0,
			NMI = 1,
			IRQ = 2
		};
		
		enum statusReg {
			CARRY = 1,	//
			ZERO = 2,		//Set to 1 when any arithmetic or logical operation produces a zero result
			INTERRUPT = 4,	//If it is set, interrupts are disabled.
			DECIMAL = 8,	//
			BRK = 16,	//This is set when a software interrupt (BRK instruction) is executed.
			UNUSED = 32,	//Not used. Supposed to be logical 1 at all times.
			OVERFLOW = 64,//Set when an arithmetic operation produces a result too large to be represented in a byte
			NEGATIVE = 128	//this is set if the result of an operation is negative, cleared if positive.
		};
		
		enum bitPosition {
			BIT0 = 1,
			BIT1 = 2,
			BIT2 = 4,
			BIT3 = 8,
			BIT4 = 16,
			BIT5 = 32,
			BIT6 = 64,
			BIT7 = 128
		};

		int currentInterrupt;
		
		byte a; //Accumulator
		byte x; //X Register
		byte y; //Y Register
		uint16_t pc; //Program Counter
		byte s; //Stack Counter
		byte p; //Processor Flags
		byte memory[0xFFFF]; //Memory
		short cycleWait;
		int cycleCount;
		byte resetVector; //Memory location to begin execution from
	public:
		CPU();
		
		//Normal functions
		void setConfig(int startAddress, int cycles);
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
		
		void evaluateInterrupt(int & interruptType);
		void triggerInterrupt(int toTrigger);
		
		//Decoding and running OPcodes
		void decodeAt(int address);
		void setWaitCycles(short toWait);
		void addWaitCycles(short toAdd);
		bool sleeping();
		void wake();
		void execute(std::string operation, std::string addressmode);
		signed char toSInt(byte input);
		
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
		void setFlags(byte operand);
		
		//Stack functions
		byte stackPop();
		void stackPush(byte toPush);
		byte stackPeek();
		
		bool pageCrossed(int address1, int address2);
		
		void memoryDump();
		
		void printDebugStatus(int address);

};
#endif