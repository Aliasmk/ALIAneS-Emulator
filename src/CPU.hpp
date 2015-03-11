#ifndef NESCPU
#define	NESCPU

#include <string>
#include <cstdint>
#include <iostream>
#include <iomanip>

class CPU {
	private:
		
		typedef std::uint8_t byte;
		
		byte a;
		byte x;
		byte y;
		short pc;
		byte s;
		byte p;
		byte memory[0xFFFF];
		short cycleWait;
				
		
	
	public:
		CPU();
		void start();
		void stop(std::string reason);
		bool running;
		
		void cycle();
		
		//Memory
		void writeMem(int address, byte value);
		void writeMem(int address1, int address2, byte value);
		byte readMem(int address);
		
		//Decoding and running OPcodes
		void decode(byte opCode);
		void decode(byte opCode, byte param1);
		void decode(byte opCode, byte param1, byte param2);
		void decode(byte opCode, byte param1, byte param2, byte param3);
		void decodeAt(short address);
		void waitForCycles(short toWait);		
		bool sleeping();
		
		//Setter Functions
		void setA(byte a);
		void setX(byte x);
		void setY(byte y);
		void setPC(short address);
		void setS(byte s);
		void setP(byte p);
		//Getter Functions
		byte getA();
		byte getX();
		byte getY();
		short getPC();
		byte getS();
		byte getP();
	

};
#endif