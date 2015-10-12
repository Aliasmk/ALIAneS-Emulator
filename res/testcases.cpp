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
 #include <string>

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
	string address1, address2, memory;
	
	cout << "ALIAneS - BaseLog" << endl;
	bool status = true;
	string alianesEnd, baseEnd;
	while(status){
	alianes >> memory >> address1; 
	baselog >> address2;

	
	
	
	cout << memory << ":" << address1 << " - " << address2 << " :: ";
	if(address1 == address2){
		cout << "GOOD";
		alianes.ignore(256,'\n');
		baselog.ignore(256,'\n');
	}
	else{
		cout << "MISMATCH" << endl;
		status = false;
		getline(alianes, alianesEnd);
		getline(baselog, baseEnd);
	}
	
	cout <<endl;
	
	
	}
	
	alianes.close();
	baselog.close();

	return 0;
}