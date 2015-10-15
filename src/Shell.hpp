#ifndef NES_SHELL
#define NES_SHELL

#include <string>
#include <cstdint>
#include <iostream>

class Shell{
	public:
	int main();
	
	private:
		std::string getCommand();
};

#endif