// This file contains the classes used for creating the virtual eprom device.

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdio>

using namespace std;

// Class definition of the FileObject

class FileObject {

public:
	FileObject(string name, size_t size): fileName(name), fileSize(size) {
		try {
			fileData.reserve(size);
		}
		
		catch(const exception& e) {
			cerr << "Error: Exception caught in FileObject: " << e.what() << endl;
			// Re-Throw the exception
			throw;
		}
	}
		
	// Destructor, no need for virtual destructor here.
	// Destructor will cleanup the fileData vector 
	~FileObject() {} 

	// Member function to read the file to be stored on the Veprom chip
	void ReadFile(string pathName) {
   		// Open the file in binary mode
    		ifstream file(pathName, ios::binary);
    
    		if (!file) {
     			cerr << "Error: Failed to open the file for reading." << endl;
        		return;
    		}
    
    		// Read the specified number of bytes
    		file.read(fileData.data(), fileSize);
    
    		// Check how many bytes were actually read
    		if (file.gcount() < fileSize) {
      			cerr << "Warning: Only " << file.gcount() << " bytes were read." 
						<< endl;
			fileSize = file.gcount();
    		}

    		// Close the file
    		file.close();
	}
	
	// Member function to print the content of the file data on stdout
	void PrintFile() {
    	// Print the bytes read 
    	cout << "Data: " << endl;
    	for (size_t i = 0; i < fileSize; ++i) {
      		cout << fileData[i];
    	}
    	cout << endl;
	}
	
	// Member function to get the file size
	size_t GetFileSize() {
		return fileSize;
	}

   	//static Member function to get the file size of any file.
	static long GetFileSize(const string& filename) {
   		// Open the file in binary mode
   		ifstream file(filename, ios::binary);
    		if (!file) {
      			cerr << "Error: Unable to open file: " << filename << endl;
        			return -1; // Error code
    		}

    		// Move the file pointer to the end of the file
    		file.seekg(0, ios::end);
    
    		// Get the position of the pointer, which represents the file size
    		long size = file.tellg();
    		file.close();
    
    		return size;
	}

private:
	// File Name 
	string fileName;
	// Size of the file
	size_t fileSize;
   	// Dynamically allocated buffer for file data to emulate writing to Eprom 
	// file  system.
	vector<char> fileData;

};

// Class definition of VepromDevice object.

class VepromDevice {

public:

	// Constructor of the VepromDevice object.
	VepromDevice(unsigned int id, string name, unsigned int maxSize):deviceId(id), 
			devName(name), maxSize(maxSize*1024), curUsage(0) {

    	ofstream file(devName);
		// Write the device file to the local file-system
		if (!file) {  // Check if the file was successfully created/opened
        		cerr << "Failed to create the file!" << endl;
        		return;
    		}
		// Store the device Id in the device file.
		file << deviceId;
		// Close the file (not strictly necessary because of RAII, but good 
		// practice)
    		file.close();
	}

	// Destructor , no need for virtual destructor as there never going to be any
	// derived class from this class.
	~VepromDevice() {
		const char* name = devName.c_str();
		Erase();
		// Remove the device filename that was created on the local file system.
    		if (remove(name) == 0) {
      			cout << "File " << name << " device deleted successfully!" << endl;
    		} else {
        		perror("Error deleting device");  // Output the error message
    		}
	}

	// Member function to erase the Veprom device
	void Erase() {
		// Release memory for registers
		vEpromReg.clear();
		
		// Release memory for FileObjects 
		// Deleting the entry in Map will cause the unique_ptr object go out-of
		// scope and deletes the object.
		files.clear();
		
		// Set the usage of the Veprom device to 0
		curUsage = 0;
      		cout << "Successfully erased device  " << devName << endl;
	}
	
	// This member function outputs the string stored at an address 
	// otherwise error message.
	void ReadRaw(unsigned int addr, unsigned int len) {
		if ( vEpromReg.find(addr) == vEpromReg.end()) {
			cerr << "Error: No data found at this address" << endl;
			return;
		}
		cout << "Reading raw data at Addr = " << addr << " Value = " << 
			vEpromReg[addr].substr(0, len) << endl;
		return;
	}
	
	// Member Function to write raw string to the register.
   	void WriteRaw(unsigned int addr, string data) {
		vEpromReg[addr] = data; 
		cout << "Successfully written  " << data << "  at addr = " << addr << endl;
	}
	
	// Member function to print the list of file entries on VEprom device.
	void FileList() {
		string retStr = "List of Files: \n";
		for(const auto& it : files) {
			retStr += it.first + '\n';
		}
		cout <<  retStr;
		return;
	}
	
	// Member function to check if file is already loaded.
	void CheckFileAlreadyLoaded(const string& fileName) {
		auto it = files.find(fileName);
      		// Check if the file is already present
      		if (it != files.end()) {
         		cout << "File was already loaded, over-writing the old file " << endl;
			// Decrement the current usage.
			curUsage -= it->second->GetFileSize();
			files.erase(it);
		}
	}
	
	// Member function write file to Veprom device
	void WriteFile(string pathName) {
		if (curUsage >= maxSize) {
			cerr << "Error: Device Full" << endl;
			return;
		}
		//This filename may contain the full path just extract the file name from 
		// the string
		string fileName;
		size_t fileSize;
		size_t pos = pathName.rfind('/');
		if (pos != string::npos) {
			fileName = pathName.substr(pos+1);
		} else {
			fileName = pathName;
      		}
	  	long retVal = FileObject::GetFileSize(pathName);
      		if (retVal < 0) {
			// Error in getting the file size
			return;
  		}
		fileSize = retVal;
		CheckFileAlreadyLoaded(fileName);
		// Check the current usage
		if (curUsage + fileSize > maxSize){
			// Truncate the file.
			fileSize = maxSize - curUsage;
         		cout << "Warning: File is too big to fit, truncating the file to " 
					<< fileSize << " bytes"  << endl;
		}	 
		try {
      			// Use RAII so in case contructor throws resource is cleaned up.
      			unique_ptr<FileObject> ptr = make_unique<FileObject>(fileName, fileSize);
			// Read the content of the file
			ptr->ReadFile(pathName);	
			// Pass the ownership
			files[fileName] = move(ptr);
			// Increase the current usage.
			curUsage += fileSize;
			cout << "Successfully written file = " << fileName << endl;
		}
		catch(const exception& e) {
			cerr << "Caught exception in WriteFileApi: " << e.what() << endl;
			cerr << "Error: Failed to write file = " << fileName << endl;
		}
	}

	// Member function to read file
	void ReadFile(string fileName) {
		auto it = files.find(fileName);
      		// Check if the file is  present
      		if (it == files.end()) {
			cerr << "Error: File " << fileName << " NOT found on the device" 
					<< endl;
			return;
		}
		// Output the file.
		it->second->PrintFile();
	}	

private:
	
	// Device Id of the device
	unsigned int deviceId;
	// Device Name of this Veprom device
   	string devName;
	// Maximum size of the device in bytes. 
	unsigned int maxSize;
	// Current usage in bytes.
	unsigned int curUsage;
	// Following emulates writting/reading of raw data to control registers of the 
	// Veprom device
	map<unsigned int, string> vEpromReg;
	
	// Following emulates user area, FileObjects are stored with filename as key.
	map<string, unique_ptr<FileObject>>	files;
 
};

// This is a singleton class. MasterDevice will contain the map of all the 
// veprom devices.
class MasterDevice {

public:
	
	// Function to get the instance of the master device.
	static MasterDevice* GetInstance() {
		// Memory allocation failures should be handled gracefully.
		try {
			if (instance == nullptr) {
				instance = new MasterDevice();
			}
			return instance;
		}
		catch (const bad_alloc& e) {
      		cerr << "Error: Memory allocation failed: " << e.what() << endl;
    	}
		return nullptr;
	}
	
	// create a new Veprom device
	void CreateVepromDeviceApi(unsigned int maxSize) {
		string devName = "Veprom-" + to_string(++nextDeviceId);

		try {
			curDev = make_shared<VepromDevice>(nextDeviceId, devName, maxSize);
			// Insert this device into hashmap
			vEpromDeviceMap[devName] = curDev;
			cout << "Device " << devName << " created successfully" << endl;
		}
		catch (const exception& e) {
			cerr << "Error: Exception caught in CreateVepromDeviceApi: " 
					<< e.what() 
					<< endl;
			if (vEpromDeviceMap.find(devName) != vEpromDeviceMap.end()) {
				vEpromDeviceMap[devName] = nullptr;
				vEpromDeviceMap.erase(devName);
			}
			curDev = nullptr;	
		}
	}
	
	// Loading a new Veprom device using a device filename.
	void LoadVepromDeviceApi(string devName) {
		if (vEpromDeviceMap.find(devName) != vEpromDeviceMap.end()) {
			curDev = vEpromDeviceMap[devName];
			cout << "Device " << devName << " loaded successfully" << endl;
		} else {
			cerr << "Error: Device " << devName << " NOT FOUND" << endl;
		}
	}
	
	// Write Raw API to Veprom device 
	// Member Function to write raw string to the register.
	void WriteRawApi(unsigned int addr, string str) {
		if (curDev == nullptr) {
			cerr << "Error: Please Create Veprom device first" << endl;
			return;
		}
		curDev->WriteRaw(addr, str);
	}
	
	// Member Function API to read Raw register values 
	void ReadRawApi(unsigned int addr, unsigned int len) {
		if (curDev == nullptr) {
			cerr << "Error: Please Create Veprom device first" << endl;
			return;
		}
		curDev->ReadRaw(addr, len);
		return;
	}
	
	// Member Function API the List of files 
	void FileListApi() {
		if (curDev == nullptr) {
			cerr << "Error: Please Create Veprom device first" << endl;
			return;
		}
		curDev->FileList();
		return;	
	} 
	
	// Write File API to the Veprom device	
	void WriteFileApi(string pathName) {
		if (curDev == nullptr) {
			cerr << "Error: Please Create Veprom device first" << endl;
			return;
		}
		curDev->WriteFile(pathName);	
		return;
	}
	
	// Read File API to read file from Veprom device
	void ReadFileApi(string fileName) {
		if (curDev == nullptr) {
			cerr << "Error: Please Create Veprom device first" << endl;
			return;
		}
		curDev->ReadFile(fileName);
		return;
	}
	
	// Member function API to erase the Veprom device
	void EraseApi() {
		if (curDev == nullptr) {
			cerr << "Error: Please Create Veprom device first" << endl;
			return;
		}
		curDev->Erase();
		return;
	}

	// Function to cleanup the master device
	void Cleanup() {
		// Cleanup the VepromDevice map
		vEpromDeviceMap.clear();
		// Set the current device pointer to NULL
		curDev = nullptr;
      		nextDeviceId = 0;
	}


private:
 	
	// Private copy constructor and assignment operator to prevent copying
	MasterDevice(const MasterDevice&) = delete;
	MasterDevice& operator=(const MasterDevice&) = delete;
	
	// Private Constructor
	MasterDevice() : nextDeviceId(0), curDev(nullptr) {};

	// Single instance of the master device
	static MasterDevice *instance;
	
	// Map of device-file-name(Key) and VepromDevice object shared pointer.
	map<string, shared_ptr<VepromDevice>> vEpromDeviceMap;

	// Next Device id to be used for the Veprom device 
	unsigned int nextDeviceId;

	// Current Veprom device in use, using shared_ptr as VepromDevice object 
	// pointer is shared here and in the map. 
	// There is no delete operation for Veprom device but this 
	// would be handy in handling ref counting.
	shared_ptr<VepromDevice> curDev;

};
