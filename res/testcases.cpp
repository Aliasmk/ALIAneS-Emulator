/* Testcases Class :: ALIAneS Emulator Project
 *
 * http://aliasmk.blogspot.com
 * http://michael.kafarowski.com
 *
 */
 
 //Simple class to compare execution of ALIAneS to Nintenulators logs for NesTest
 //Will spit out the location of the first deviation in the program counter as well as the respective information etc.
 //Requires a file log of ALIAneS and Nintendulator for comparisons. 

 
 #include <fstream>
 #include <iostream> 

using namespace std;

int main(){
	ifstream alianes, baselog;
	string alianespath = "../res/logs/alianesLast.log";
	string baselogpath = "../res/logs/nestest.log";
	
	alianes.open(alianespath);
	baselog.open(baselogpath);
	
	if(!baselog){
		cout << "No base log found at " << baselogpath <<endl;
		return 1;
	}
	if(!alianes){
		cout << "No alianes log found at " << alianespath << endl;
		return 1;
	}
	
	//Init testing
	string address = "";
	string test = "";
	
	
	for(int i = 0; i < 10; i++){
	alianes >> address >> test;
	cout << "Alianes output: " << address << test << endl;
	
	
	baselog >> address >> test;
	cout << "Baselog output: " << address << test << endl;
	aliasnes.getLine();
	baselog.getLine();
	}
	
	alianes.close();
	baselog.close();

	return 0;
}