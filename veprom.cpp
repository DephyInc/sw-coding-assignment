#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "datafile.h"
#include "utils.h"
#include "graphical_interface.h"


using namespace std;


class Emulator {
    private:
        string generateDataFileName() {
            string fileName;

            char buffer[PATH_MAX];

            if (getcwd(buffer, sizeof(buffer)) != NULL) {
                // for the purpose of this utility
                // we will limit to 9999 veprom in the local path
                // if we need >9999, we have other problems :-)
                for(int i = 0; i < 9999; i++) {
                    fileName = string(buffer) + "/veprom_" + to_string(i) + ".bin";

                    if (!Utils::isFileExists(fileName)) {
                        break;
                    }
                }

                return fileName;
            }
            else {
                throw runtime_error("Error evaluating current working directory!");
            }
        }

        void setCurrentDataFile(DataFile df) {
            ofstream file("active_veprom.cfg", ios::trunc);
            file << df.getFileName();
            file.close();
        }

        string getActiveDataFileName() {
            ifstream file("active_veprom.cfg");
            string currentFile;
            if(file && file.good() && file.is_open()) {
                while(file) {
                    getline(file, currentFile);
                }
                return currentFile;
            }
            throw runtime_error("Unable to open active_veprom.cfg file");
        }

        DataFile getCurrentDataFile() {
            string currentFile = getActiveDataFileName();
            return DataFile(currentFile);
        }

    public:
        Emulator() {

        }

        bool isActiveFileConfigExists () {
            ifstream file("active_veprom.cfg");
            return file.good();
        }

        string getFilesList() {
            DataFile df = getCurrentDataFile();
            vector<FileRepresentationDTO*> files = df.getFilesDTO();

            string output;
            for(const auto &el :files) {
                output += el->fileName + "\n";
            } 

            return output;
        }

        char* readFile(char* sourcePath) {
            DataFile df = getCurrentDataFile();
            return df.readFile(sourcePath);           
        }

        void writeFile(char* sourcePath) {
            DataFile df = getCurrentDataFile();
            df.writeFile(sourcePath);           
        }

        char* readRaw(int bytesStart, int length) {
            DataFile df = getCurrentDataFile();
            return df.readRawAtAddress(bytesStart, length);
        }

        void writeRaw(int start, int end, char* input) {
            DataFile df = getCurrentDataFile();
            df.writeRawAtAddress(start, end, input);
        }

        string erase() {
            DataFile df = getCurrentDataFile();
            df.erase();
            return df.getFileName();
        }

        string status_report() {
            try {
                DataFile df = getCurrentDataFile();
                return df.getStatusReport();
            }
            catch (const std::exception& e) {
                printf("Active vEPROM does not exist (or invalid/corrupt file)!\n");
                printf("Filename: %s\n", getActiveDataFileName().c_str());
                throw e;
            }
        }

        string load_veprom(string fileName) {
            DataFile df(fileName);
            setCurrentDataFile(df);
            return df.getFileName();
        }

        string create_veprom(int kBytes) {
            string fileName = generateDataFileName();

            DataFile df = DataFile::init(fileName, kBytes);

            setCurrentDataFile(df);
            
            return df.getFileName();
        }

        void generateGraphicalInterface() {
            GraphicalInterface gi(getCurrentDataFile());
            gi.render();
        }
};

class EmulatorConsoleInterpreterImpl {

    private:
        const int DEFAULT_CREATE_KBYTES = 256;

        Emulator my_emulator;
        string valid_commands[10] = {
            "create" ,
            "load",
            "status",
            "erase",
            "write_raw",
            "read_raw",
            "write",
            "read",
            "list",
            "gui"
        };

    public:
        EmulatorConsoleInterpreterImpl(Emulator emulator) {
            my_emulator = emulator;
        }

        /**
         * 
         * Probably would have done something different with this. Instead maybe encapsulate each command option
         * and have each command be responsible to deliver an instruction to support it.
        */
        void printInstructions() {
            printf("vEPROM Instructions\n");
            printf("vEPROM 1.0 (Feb 1, 2023). Usage:\n");
            printf("./veprom [option]\n\nOptions:\n");
            printf("status\t\t\t\t\tShow current veprom file name and internal characteristics\n");
            printf("create <int>\t\t\t\tCreate a new vEPROM with the desired length (in KB) - default=256\n");
            printf("\t\t\t\t\tThis new created file will be considered the current active vEPROM\n");
            printf("load <path_to_file>\t\t\tSet the current active vEPROM\n");
            printf("\t\t\t\t\tThe path_to_file is the file path to a veprom_*.bin\n");
            printf("write_raw <address> <string>\t\tWrite a string starting at a specific location in the active vEPROM\n");
            printf("\t\t\t\t\tThe address provided may be in int or hex\n");
            printf("\t\t\t\t\tThe string may be wrapped in double-quotes to preserve spaces\n");
            printf("read_raw <address> <length>\t\tRead raw data starting at a specific location in the active vEPROM\n");
            printf("\t\t\t\t\tThe address provided may be in int or hex\n");
            printf("\t\t\t\t\tThe length must be an int\n");
            printf("write <path_to_file>\t\t\tWrite contents of the specified file to the active vEPROM\n");
            printf("\t\t\t\t\tThe path_to_file must point to a valid and accessible file\n");
            printf("list\t\t\t\t\tRead the list of files saved to the vEPROM\n");
            printf("read <file_name>\t\t\tRead a file in its entirety which has been saved to the active vEPROM\n");
            printf("\t\t\t\t\tThe file_name must refer to a name returned using the 'list' option\n");
            printf("erase\t\t\t\t\tClean all memory from the current active vEPROM\n");
            printf("gui\t\t\t\t\tView a graphical representation of the filesystem for the active vEPROM\n");
        }

        /*
         * Handles interaction between console->emulator
        */
        void processArguments(int argc, char** argv) {

            cout << endl;

            if (argc == 1) {
                printInstructions();
            }
            else if(argc > 1) {

                string cmd = argv[1];

                bool is_valid = (find(begin(valid_commands), end(valid_commands), cmd) != end(valid_commands));

                if(!is_valid) {
                    printf("Invalid command specified: %s\n", cmd.c_str());
                    return;
                }

                try {
                    if(cmd == valid_commands[0] || cmd == valid_commands[1]) {
                        if(cmd == valid_commands[0]) {
                            processCreateHandler(argc, argv);
                        }
                        else if(cmd == valid_commands[1]) {
                            processLoadHandler(argc, argv);
                        }
                    }
                    else {
                        if(!my_emulator.isActiveFileConfigExists()) {
                            printf("It seems this may be the first time you are using vEPROM (thanks, btw).\n");
                            printf("Try creating a new vEPROM by using the <create> option.\n");
                            return;
                        }

                        if(cmd == valid_commands[2]) {
                            processStatusReportHandler(argc, argv);
                        }
                        else if(cmd == valid_commands[3]) {
                            processEraseHandler(argc, argv);
                        }
                        else if(cmd == valid_commands[4]) {
                            processWriteRawHandler(argc, argv);
                        }
                        else if(cmd == valid_commands[5]) {
                            processReadRawHandler(argc, argv);
                        }
                        else if(cmd == valid_commands[6]) {
                            processWriteFileHandler(argc, argv);
                        }
                        else if(cmd == valid_commands[7]) {
                            processReadFileHandler(argc, argv);
                        }
                        else if(cmd == valid_commands[8]) {
                            processListFilesHandler(argc, argv);
                        }
                        else if(cmd == valid_commands[9]) {
                            processGUIHandler(argc, argv);
                        }
                        else {
                            printf("%s", Localization::INVALID_CMD_ARGS_MSG);
                        }
                    }
                }
                catch (const std::exception& e) {
                    printf("Fatal Exception occured while processing vEPROM command\n%s\n", e.what());
                    printf("An invalid command or parameter may have been used...\n");
                    printf("\n");
                }
                
            }
        }

        void processCreateHandler(int argc, char** argv) {

            // default for console use
            int kbytes_size = DEFAULT_CREATE_KBYTES;

            if(argc == 3) {
                istringstream iss(argv[2]);
                int val;

                if (iss >> val) {
                   kbytes_size = val;
                }
            }
            string fileName = my_emulator.create_veprom(kbytes_size);
            printf("Successfully created vEPROM. File: %s\n", fileName.c_str());
        }

        void processLoadHandler(int argc, char** argv) {
            if(argc == 3) {
                string fileLoaded = my_emulator.load_veprom(argv[2]);
                printf("Successfully loaded vEPROM from file: %s\n", fileLoaded.c_str());
            }
            else {
                printf("%s", Localization::INVALID_CMD_ARGS_MSG);
            }
        }

        void processStatusReportHandler(int argc, char** argv) {
            string statusReport = my_emulator.status_report();
            printf("Successfully retrieved status from vEPROM:\n%s\n", statusReport.c_str());
        }

        void processEraseHandler(int argc, char** argv) {
            string fileErased = my_emulator.erase();
            printf("Successfully erased vEPROM:\n%s\n", fileErased.c_str());
        }

        void processWriteRawHandler(int argc, char** argv) {
            if(argc == 4) {
                string address = argv[2];
                string input = argv[3];
                cout << input << endl;
                int bytesStart = Utils::convertStringHexToInt(address);

                const int length = input.size();
                char* char_array = new char[length + 1];
                strcpy(char_array, input.c_str());

                my_emulator.writeRaw(bytesStart, bytesStart + length, char_array);
                printf("Data has been written. Length: %s\n", to_string(length).c_str());
            }
            else {
                printf("%s", Localization::INVALID_CMD_ARGS_MSG);
            }
        }

        void processReadRawHandler(int argc, char** argv) {
            if(argc == 4) {
                string address = argv[2];
                int length = stoi(argv[3]);

                int bytesStart = Utils::convertStringHexToInt(address);

                char* data = my_emulator.readRaw(bytesStart, length);
                cout.write(data, length);
                cout << endl;
            }
            else {
                printf("%s", Localization::INVALID_CMD_ARGS_MSG);
            }
        }

        void processWriteFileHandler(int argc, char** argv) {
            if(argc == 3) {
                char* sourcePath = argv[2];
                my_emulator.writeFile(sourcePath);
                printf("File has been written.\n");
            }
            else {
                printf("%s", Localization::INVALID_CMD_ARGS_MSG);
            }
        }

        void processReadFileHandler(int argc, char** argv) {
            if(argc == 3) {
                char* sourcePath = argv[2];
                char* output = my_emulator.readFile(sourcePath);
                cout << output << endl;
            }
            else {
                printf("%s", Localization::INVALID_CMD_ARGS_MSG);
            }
        }

        void processListFilesHandler(int argc, char** argv) {
            string filesList = my_emulator.getFilesList();
            if(filesList.size() > 0) {
                printf("Files found in vEPROM:\n\n%s\n", filesList.c_str());
            }
            else {
                cout << Localization::NO_FILES_FOUND_MSG << endl;
            }
        }

        void processGUIHandler(int argc, char** argv) {
            my_emulator.generateGraphicalInterface();
        }
};


/**
 * Main Entry Point to start emulator+cmdline interpreter
*/
int main(int argc, char** argv) {

    Emulator emulator;
    EmulatorConsoleInterpreterImpl interpreter(emulator);
    
    interpreter.processArguments(argc, argv);
    
}
