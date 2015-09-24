/* Shell Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */

#include "Shell.hpp"
#include "System.hpp"
#include "CPU.hpp"

typedef std::uint8_t byte;

using namespace std;

int main()
{
	string command;
	//Presents an interactive shell front end to read and write memory as well as execute opcodes in a sandbox.
	cout << endl << "********* Welcome to ALIAneS Testing Shell **********" << endl<< endl;
	
	//Loop a prompt while the user does not want to exit
	while(command != "quit")
	{
		cout << "nes$ ";
		cin >> command;
		
		if(command == "start") //"Start" starts the emulator as normally before
		{
			//Create the Emulator Instance
			System* nes = new System();
			//Clears the memory used by the Emulator
			delete nes;
			nes = NULL;
		} 
		else if (command == "load")
		{
			string cartName;
			cout << "Cartridge File $../res/"; 
			cin >> cartName;
			System* nes = new System("../res/"+cartName);
			delete nes;
			nes = NULL;
		}
		else if(command == "test") //"Test" loads the CPU and a blank set of memory where commands can be executed and memory interacted with
		{
			//Create the CPU Instance
			cout << "Initializing CPU... ";
			CPU* nesCPU = new CPU();
			nesCPU->start();
			cout << "Ready" << endl;
			
			//Displays a nested prompt while the user does not want to quit
			while(command != "end" && command != "quit")
			{
				command = "";
				cout << "nes-test " << nesCPU->getPC() << "$ ";
				cin >> command;
				
				if(command == "write") //Writes a value to memory
				{	
					command = "";
					cout << "<address> <value>$ ";
					int iInput1;
					int iInput2;
					cin >> hex >> iInput1 >> hex >> iInput2;
					nesCPU->writeMem(iInput1, iInput2);
				} 
				else if(command == "read") //Reads a memory value
				{
					command = "";
					cout << "<address> $ ";
					int iInput;
					cin >> hex >> iInput;
					cout << hex << (int)nesCPU->readMem(iInput) << endl;
					
				} else if(command == "decode"){
					command = "";
					cout << "<address> $ ";
					int iInput;
					cin >> hex >> iInput;
					nesCPU->decodeAt(iInput);
					nesCPU->incPC();
					
				} else if(command == "execute"){
					command = "";
					int previousPC = nesCPU->getPC();
					cout << "<command> <addressmode> <writeto1> <writeto2>$";
					string sInput1, sInput2;
					int iInput1, iInput2;
					cin >> sInput1 >> sInput2 >> iInput1 >> iInput2;
					
					nesCPU->writeMem(previousPC+1, iInput1);
					nesCPU->writeMem(previousPC+2, iInput2);
					
					nesCPU->execute(sInput1, sInput2);
				
				} else if(command == "setpc") {
					command = "";
					cout << "<address> $ ";
					int iInput;
					cin >> hex >> iInput;
					nesCPU->setPC(iInput);
				} else if(command == "info")
				{
					nesCPU->printDebugStatus(nesCPU->getPC());
				} 
				
				else if (command == "" || command == "quit" || command == "end") {}
				else {
					cout << "Unknown command" << endl;
				}
				
			}
			//Deletes the CPU instance
			delete nesCPU;
			nesCPU=NULL;
			
		}
		else if(command == "quit"){}
		else
		{
			cout<< "Unknown Command" << endl;
		}
	}
 	
	
	
	return 0;
}


//void readInput(int args, string input, string[] & split)
//{
//	split[] = new string[args];
	
//}
