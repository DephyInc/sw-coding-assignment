// This file contains the main driver code for Veprom device emulator.
#include "veprom.hpp"
#include <cctype>
#include <sstream>

// Initialize the global instance pointer of the MasterDevice singleton class.
MasterDevice* MasterDevice::instance = nullptr;
// Command List: tokens list 
vector<vector<string>> cmdList = { 	{ "veprom", "create", "match" },
					{ "veprom", "load", "match"},
					{ "veprom", "write_raw", "match"},
					{ "veprom", "read_raw", "match"},
					{ "veprom", "write", "match"},
					{ "veprom", "list", "match"},
					{ "veprom", "read", "match"},
					{ "veprom", "erase", "match"},
					{ "quit", "match" },
					{ "no-match", "no-match"}
				}; 



void DrawMainMenu() {

	cout << "\t\t\t\t\t Main Menu" << endl;
	cout << "====================================================================" <<
			"=================================" << endl;
	cout << "1 - Create a new virtual EPROM chip. Usage: veprom create <size> "
			<< "size is in KB" << endl;
	cout << "2 - Load a vEPROM file. Usage: veprom load </path/to/veprom/file>" 
			<< endl;
	cout << "3 - Write a raw string of bytes to an address. Usage: veprom " <<
			" write_raw <ADDRESS> <STRING>" << endl;
   	cout << "4 - Read the values stored at a specific address and length. "
			<< "Usage: veprom read_raw <ADDRESS> <length>" << endl;
   	cout << "5 - Write a file to the virtual EPROM chip. Usage: veprom write " <<
				"</path/to/local/file>" << endl;
	cout << "6 - List the files on the virtual EPROM chip. Usage: veprom list" 
				<< endl; 
   	cout << "7 - Read a file from the virtual EPROM chip. Usage: veprom read " <<
       	"<FILE>" << endl;
	cout << "8 - Sets the EPROM back to its original state. Usage: veprom erase" << 
			endl;
	cout << "9 - Quit the application. Usage: quit "<< endl;
	cout << endl;
	cout << endl;
}

string ParseUserInput(string userInput, vector<string>& args) {
	// Create a string stream from the input string
	istringstream stream(userInput);
	string token;
	int numArgs;
	vector<string> tokens;
	bool matchFound = false;
	int row;
	int col;
	
	// Tokenize the string using space as the delimiter
	while (stream >> token) {
   		tokens.push_back(token);
   }
	// Set the number of args.
	numArgs = tokens.size() < 3 ? 0 : tokens.size() - 2;
	int tokNum;
	for(row = 0; (row < cmdList.size() && matchFound == false); row++) {
		tokNum = 0;
		for(col = 0; col < tokens.size(); col++) {
			// Check if end of the list is reached.
			if (cmdList[row][col] == "match") {
				break;
			}
			// Check for token match.
			if (cmdList[row][col] == tokens[tokNum++]) {
				// Token matched, check if token match is complete.
				if (cmdList[row][col+1] == "match") {
					// Match found
					matchFound = true;
					break;
				}
			} else {
				break;
			}
		}
	}
	if (matchFound) {
		args.clear();
		for (int argList = 0; argList < numArgs; argList++) {
			args.push_back(tokens[tokNum++]);
		}
		// Return the Command
		return cmdList[row-1][col];
	}
	return "no-match"; 
}
	


int main(int arc, char **argv) {
	string command;
	vector<string> args;
	MasterDevice* master = MasterDevice::GetInstance();
   // Check if got the object allocated
	if (master == nullptr) {
		cout << "Out-Of-Memory" << endl;
		exit(0);
	}
	string userInput;
	while(1) {
		DrawMainMenu();
		cout << "Please enter the command from above menu or type quit to exit " <<
				"(Use lower case for command tokens): " << endl;
      		getline(cin, userInput);
		command = ParseUserInput(userInput, args);
		if (command == "create") {
			if (args.size() < 1)	{
				cerr << "Error: Create Veprom needs size arg, Please try again!!" << endl;
				continue;
			}
			int size = stoi(args[0]);
			if (size < 0) {
				cerr << "Error: Size arg can't be less that 0, Please try again!!" << endl;
				continue;
			}
			master->CreateVepromDeviceApi(size);
		
		} else if (command == "load") {
			if (args.size() < 1)	{
				cerr << "Error: Load Veprom needs path-to-file arg, Please try again!!" << endl;
				continue;
			}
			// Invoke the API
			master->LoadVepromDeviceApi(args[0]);		
			
		} else if (command == "write_raw") {
			if (args.size() < 2)	{
				cout << "Error: Write Raw Veprom needs address and string args, Please try again!!" 
					<< endl;
				continue;
			}
			unsigned int addr = stoi(args[0]);
			// Invoke the API
			master->WriteRawApi(addr, args[1]);		
		} else if (command == "read_raw") {
			if (args.size() < 2)	{
				cerr << "Error:Read Raw Veprom needs address and length args, Please try again!!" 
					<< endl;
				continue;
			}
			unsigned int addr = stoi(args[0]);
			unsigned int length = stoi(args[1]);
			// Invoke the API
			master->ReadRawApi(addr, length);		
		} else if (command == "write") {
			if (args.size() < 1)	{
				cerr << "Error: Write File Veprom needs path-to-file arg, Please try again!!" << endl;
				continue;
			}
			// Invoke the API
			master->WriteFileApi(args[0]);		
		}	else if (command == "list") {
			// Invoke the API
			master->FileListApi();		
		}	else if (command == "read") {
			if (args.size() < 1)	{
				cerr << "Error: Read File Veprom needs filename arg, Please try again!!" << endl;
				continue;
			}
			// Invoke the API
			master->ReadFileApi(args[0]);		
		}	else if (command == "erase") {
			// Invoke the API
			master->EraseApi();		
		} else if (command == "quit") {
			cout << "Thanks for tying the Veprom Emulator App" << endl;
			break;
		} 
		else {
			cerr << "Error: Invalid Input, Please try again!!" << endl;
			continue;
		}
	}
}
