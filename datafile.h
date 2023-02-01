#pragma once

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>

#include "utils.h"

using namespace std;


class FileRepresentationDTO {
    public:
        FileRepresentationDTO() {}
        int step = -1;
        string fileName;
        int startBytes = -1;
        int endBytes = -1;
};


/**
 * Data Transformation Object (DTO) used for external/graphical representation of data
*/
class DataRepresentationAggDTO {
    public:
        DataRepresentationAggDTO() {
            isUsrSpc = false;
            isCtlSpc = false;
            isSysSpc = false;
            isFile = false;
            files.clear();
        }
        int step = -1;
        int freeBytes = -1;
        bool isCompletelyFree;
        bool isUsrSpc;
        bool isCtlSpc;
        bool isSysSpc;
        bool isFile;
        vector<FileRepresentationDTO*> files;
};


class SpaceReader {
    private:
        int my_start;
        int my_end;
        bool my_isIncludeNulls = false;
        char* data;

        char* _getData(ifstream& iFile) {
            int length = my_end - my_start;
            iFile.seekg(my_start, iFile.beg);
            vector<char> buffer;
            buffer.resize(length);
            iFile.read(buffer.data(), length);

            char* char_array = new char[length + 1];

            for (int i = 0; i < buffer.size(); i++) {
                char my_char = buffer[i];

                if(my_isIncludeNulls && my_char == '\0') {
                    my_char = ' ';
                }

                char_array[i] = my_char;
            }
            return char_array;
        }

    public:
        /**
         * Responsible for closing stream
        */
        SpaceReader(ifstream& iFile, int start, int end, bool isIncludeNulls = false) {
            my_start = start;
            my_end = end;
            my_isIncludeNulls = isIncludeNulls;
            data = _getData(iFile);
            iFile.close();
        }

        char* getData() {
            return data;
        }
};

class SpaceWriter {
    private:
        int my_start;
        int my_end;

        void writeData(ofstream& oFile, char* data) {
            oFile.seekp(my_start, oFile.beg);
            oFile.write(data, my_end - my_start);
        }

        void writeData(ofstream& oFile, ifstream& iFile) {
            oFile.seekp(my_start, oFile.beg);
            while(iFile) {
                std::vector<char> buffer(8096);
                iFile.read(buffer.data(), buffer.size());
                oFile.write(buffer.data(), buffer.size());
            }
        }

    public:
        SpaceWriter(ofstream& oFile, char* data, int start, int end) {
            my_start = start;
            my_end = end;
            writeData(oFile, data);
            oFile.close();
        }

        /* Caller is responsible for closing input file */
        SpaceWriter(ofstream& oFile, ifstream& iFile, int start, int end) {
            my_start = start;
            my_end = end;
            writeData(oFile, iFile);
            oFile.close();
        }
};


class DataFile {
    private:
        static inline char* sys_make_context = "vEPROM_em1";
        
        const static inline int default_kBytesChunk = 256;
        
        // pct of space in the vEPROM for organizing what data exists and locating it
        const static inline float default_control_space_bytes_pct = .10;

        // changing this may result in never being able to interpret a data file
        // as this contains system referential data used to interpret the control space
        const static inline int hard_system_space_bytes = 256;

        string my_fileName;

        static void validateFileSize(string fileName, int expectedKBytes) {
            int fileSize = getFileSize(fileName);

            int expectedBytes = expectedKBytes * Utils::bytesInKB;
            if(fileSize != expectedBytes) {
                throw runtime_error("Error encountered while validating vEPROM file size. Expected (bytes): " + to_string(expectedBytes) + ", Actual (bytes): " + to_string(fileSize));
            }
        }

        static void writeSystemSpaceData(string fileName, char* data) {
            ifstream iFile(fileName, ios::binary);
            int start;
            int end;

            // get range for system space
            if(iFile && iFile.is_open()) {
                iFile.seekg(0, iFile.end);
                end = iFile.tellg();
                iFile.seekg(-1 * hard_system_space_bytes, iFile.end);
                start = iFile.tellg();
                iFile.close();    
            }

            write(fileName, data, start, end);
        }

        /**
         * System Space is intended for 1 time only initialization
        */
        static void initSystemSpace(int length, string fileName) {
            char data[hard_system_space_bytes];

            int usr_spc_start = 0;
            int usr_spc_end = usr_spc_start + (length * (1.00 - default_control_space_bytes_pct));
            int ctl_spc_start = usr_spc_end + 1;
            int ctl_spc_end = ctl_spc_start + (length * default_control_space_bytes_pct - hard_system_space_bytes - 1);
            int sys_spc_start = ctl_spc_end + 1;
            int sys_spc_end = length - 1;

            appendToSystemSpace(data, "start", "_");
            appendToSystemSpace(data, "context", sys_make_context);
            appendToSystemSpace(data, "version", "v1");
            appendToSystemSpace(data, "length", to_string(length));
            appendToSystemSpace(data, "user_spc_start", to_string(usr_spc_start));
            appendToSystemSpace(data, "user_spc_end", to_string(usr_spc_end));
            appendToSystemSpace(data, "ctl_spc_start", to_string(ctl_spc_start));
            appendToSystemSpace(data, "ctl_spc_end", to_string(ctl_spc_end));
            appendToSystemSpace(data, "sys_spc_start", to_string(sys_spc_start));
            appendToSystemSpace(data, "sys_spc_end", to_string(sys_spc_end));

            //strncpy(data, output, sizeof(data) - 1);
            
            writeSystemSpaceData(fileName, data);
        }

        static void appendToSystemSpace(char* data, string key, string value) {
            string output = key + ":" + value;
            strcat(data, output.c_str());
            strcat(data, "|");
        }

        /**
         * this is expected to be run internally, as there are no protections around damaging the file system
        */
        static void cleanOrInitFile(string fileName, int kBytes) {
            int kBytesWatermark = 0;
            int seekPointer = 0;

            ofstream oFile(fileName, ios::out | ios::binary);
            if(oFile) {
                // just for sanity sake
                oFile.seekp(seekPointer);
                
                // inititalize (i.e. wipe clean) our entire data file
                while(kBytesWatermark < kBytes) {
                    int nextBytes = min(kBytes - kBytesWatermark, DataFile::default_kBytesChunk);
                    int length = nextBytes * Utils::bytesInKB;
                    
                    char* buffer = new char[length];

                    // init
                    for(int i = 0; i < length; i++) {
                        buffer[i] = '\0';
                    }

                    oFile.write(buffer, length);
                    
                    kBytesWatermark += nextBytes;
                    seekPointer += length;
                    oFile.seekp(seekPointer);
                }
                oFile.close();
            }
            else {
                throw runtime_error("Error while writing to DataFile. File: " + fileName);
            }

            initSystemSpace(seekPointer, fileName);            
        }

        /**
         * Needed a static method for write to support system write.
        */
        static void write(string my_fileName, char* input, int start, int end) {
            ofstream oFile(my_fileName, ios_base::in|ios_base::ate|ios::binary);
            if(oFile && oFile.is_open()) {
                try {
                    SpaceWriter writer(oFile, input, start, end);
                }
                catch(const std::exception& e) {
                    throw runtime_error("Error while writing data to vEPROM. Error: " + string(e.what()));
                }
            }    
        }

        void validateDataFileIntegrity() {
            
        }

        /**
         * Simple check that evaluates system space, retrieves context
         * and performs comparison. If any of this is false, returns false.
        */
        bool isDataFile() {
            try {
                string context = getSystemValue("context");
                return context == sys_make_context;
                return true;
            }
            catch(const std::exception& e) {
                printf("Error encountered while checking validity of DataFile. Error: %s", e.what());
            }
            return false;
        }


        string getSystemValue(string key) {
            vector<vector<string> > spc_data = extractInternalSpaceData(getSystemSpaceReader());

            for(const auto &el :spc_data) {
                string my_key = el.at(0);
                string my_val = el.at(1);

                if(string(key) == string(my_key)) {
                    return my_val;
                }
            }

            throw runtime_error("key not found in system data: " + key);
        }

        ifstream getInputStream() {
            ifstream iFile(my_fileName, ios::binary);
            if(iFile && iFile.is_open()) {
                return iFile;
            }
            throw runtime_error("Error getting file input stream from vEPROM.\n");
        }

        ofstream getOutputStream() {
            ofstream oFile(my_fileName, ios_base::in|ios_base::ate|ios::binary);
            if(oFile && oFile.is_open()) {
                return oFile;
            }
            throw runtime_error("Error getting file output stream from vEPROM.\n");
        }

        /**
         * System Space is always found at the end of the virtual mem and will be consistent in size
         * no matter the user defined size of the vMem that was allocated.
        */
        SpaceReader getSystemSpaceReader() {
            ifstream iFile = getInputStream();
            try {
                // only for the sys space we cheat because we know exactly where it is always expected to be
                iFile.seekg(0, iFile.end);
                int end = iFile.tellg();
                iFile.seekg(-1 * hard_system_space_bytes, iFile.end);
                return SpaceReader(iFile, iFile.tellg(), end);
            } 
            catch(const std::exception& e) {
                throw runtime_error("Error getting SystemSpace from vEPROM. Exception: " + string(e.what()));
            }
        }

        SpaceReader getCtrlSpaceReader() {
            ifstream iFile = getInputStream();
            try {
                int ctl_spc_start = stoi(getSystemValue("ctl_spc_start"));
                int ctl_spc_end = stoi(getSystemValue("ctl_spc_end"));
                return SpaceReader(iFile, ctl_spc_start, ctl_spc_end);
            } 
            catch(const std::exception& e) {
                throw runtime_error("Error getting SystemSpace from vEPROM. Exception: " + string(e.what()));
            }
        }

        void write(char* input, int start, int end) {
            ofstream oFile = getOutputStream();
            if(oFile && oFile.is_open()) {
                try {
                    SpaceWriter writer(oFile, input, start, end);
                }
                catch(const std::exception& e) {
                    throw runtime_error("Error while writing data to vEPROM. Error: " + string(e.what()));
                }
            }    
        }

        void write(ifstream& input, int start, int end) {
            ofstream oFile = getOutputStream();
            if(oFile && oFile.is_open()) {
                try {
                    SpaceWriter writer(oFile, input, start, end);
                }
                catch(const std::exception& e) {
                    throw runtime_error("Error while writing data to vEPROM. Error: " + string(e.what()));
                }
            }    
        }

        bool validateAddressInBounds(int start, int end, int spc_start, int spc_end) {
            bool inBounds = start >= spc_start
                && start <= spc_end
                && end >= spc_start
                && end <= spc_end;

            if(!inBounds) {
                throw runtime_error("Input length could not be contained in space bounds.");
            }
        }

        bool validateAddressInUserBounds(int start, int end) {
            int user_spc_start = stoi(getSystemValue("user_spc_start"));
            int user_spc_end = stoi(getSystemValue("user_spc_end"));

            return validateAddressInBounds(start, end, user_spc_start, user_spc_end);
        }

        bool validateAddressInCtrlBounds(int start, int end) {
            int ctl_spc_start = stoi(getSystemValue("ctl_spc_start"));
            int ctl_spc_end = stoi(getSystemValue("ctl_spc_end"));

            return validateAddressInBounds(start, end, ctl_spc_start, ctl_spc_end);
        }

        void writeRawDataToUserSpace(int start, int end, char* input) {
            validateAddressInUserBounds(start, end);
            write(input, start, end);
        }

        void updateCtrlSpcForFile(char* fileIdentifier, int file_start, int file_end) {
            int ctl_spc_start = stoi(getSystemValue("ctl_spc_start"));
            int ctl_spc_end = stoi(getSystemValue("ctl_spc_end"));
            int ctl_length = ctl_spc_end - ctl_spc_start;

            string data = string(fileIdentifier) + ":" + to_string(file_start).c_str() + "," + to_string(file_end).c_str() + "|";
            int length = data.length();

            int init_start = ctl_spc_start;
            int *start = &init_start;
            findNextContiguousSpan(start, ctl_spc_end, length);

            int ctrl_end = *start + length;

            validateAddressInCtrlBounds(*start, ctrl_end);

            char* str_input = new char[length + 1];
            strcpy(str_input, data.c_str());
            write(str_input, *start, ctrl_end);
        }


        /**
         * This function is critical as it traverses through the vmem to find contiguous space
        */
        void findNextContiguousSpan(int* start, int end, int length) {
            ifstream iFile = getInputStream();
            iFile.seekg(*start, ios::beg);

            int index = 0;
            int loc = *start;
            if(iFile && iFile.is_open()) {
                while(loc <= end) {
                    char c;
                    iFile.get(c);

                    loc++;

                    if(c != '\0') {
                        *start = loc;
                        index = 0;
                    }
                    else if(index >= length) {
                        // we found our start address
                        // where there is enough contiguous space to fill the
                        // the length of the input
                        iFile.close();
                        return;
                    }

                    index++;
                }
            }
            iFile.close();
            throw runtime_error("Unable to find a contiguous block of space");
        }

        void writeFileToUserSpace(char* sourceFilePath) {
            ifstream sourceFile(sourceFilePath, ios::binary);
            if(sourceFile && sourceFile.is_open()) {
                sourceFile.seekg(0, sourceFile.end);
                int length = sourceFile.tellg();
                sourceFile.seekg(0, sourceFile.beg);

                int init_start = 0;
                int *start = &init_start;
                int user_spc_end = stoi(getSystemValue("user_spc_end"));
                findNextContiguousSpan(start, user_spc_end, length);

                int end = *start + length;

                // validate that we are in bounds
                validateAddressInUserBounds(*start, end);

                // need control space to reflect the location/address bounds of the file
                char* fileId = getUniqueFileName(sourceFilePath);
                updateCtrlSpcForFile(fileId, *start, end);

                write(sourceFile, *start, end);

                sourceFile.close();
            }
        }

        char* getUniqueFileName(char* fileName) {
            // limit file path to 260 chars
            char* u_fileName = new char[260];
            strcpy(u_fileName, fileName);

            while(isFileNameExists(u_fileName)) {
                // try again by appending a random int
                strcpy(u_fileName, fileName);
                strcat(u_fileName, "_");
                strcat(u_fileName, to_string(rand()).c_str());
            }
            return u_fileName;
        }

        bool isFileNameExists(char* fileName) {
            vector<vector<string> > spc_data = extractInternalSpaceData(getCtrlSpaceReader());

            for(const auto &el :spc_data) {
                string my_key = el.at(0);

                if(string(fileName) == string(my_key)) {
                    return true;
                }
            }
            return false;
        }

        vector<int> getFileCoordinates(char* fileName) {
            vector<FileRepresentationDTO*> files = getFilesDTO();

            for(const auto &el :files) {
                FileRepresentationDTO* dto = el;

                if(string(fileName) == string(dto->fileName)) {
                    vector<int> output;
                    output.push_back(dto->startBytes);
                    output.push_back(dto->endBytes);
                    return output;
                }
            }

            throw runtime_error("Unable to locate coordinates for file. Name: " + string(fileName));
        }

        char* readFileFromUserSpace(char* fileName) {
            vector<int> coordinates = getFileCoordinates(fileName);
            int start = coordinates.at(0);
            int end = coordinates.at(1);

            return readRawDataFromUserSpace(start, end);
        }

        char* readRawDataFromUserSpace(int start, int end) {
            validateAddressInUserBounds(start, end);
            ifstream iFile = getInputStream();
            SpaceReader reader(iFile, start, end, true);
            return reader.getData();
        }

        vector<vector<string> > extractInternalSpaceData(SpaceReader reader) {
            string sys_space = reader.getData();

            vector<vector<string> > output;

            stringstream sys_data(sys_space);
            string segment;
            vector<string> seglist;

            while(getline(sys_data, segment, '|')) {
                seglist.push_back(segment);
            }

            for(int i = 0; i < seglist.size(); i++) {
                string val = seglist[i];
                stringstream val_s(val);

                string my_key;
                string my_val;
                while (!val_s.eof()) {
                    getline(val_s, my_key, ':');
                    getline(val_s, my_val, ':');

                    vector<string> res;
                    res.push_back(my_key);
                    res.push_back(my_val);
                    output.push_back(res);
                }
            }  

            return output;  
        }

    public:
        DataFile(string fileName) {
            my_fileName = realpath(fileName.c_str(), NULL);
            if(!isDataFile()) {
                throw runtime_error("Invalid vEPROM DataFile. File: " + my_fileName);
            }
        }

        static DataFile init(string fileName, int kBytes) {
            if(Utils::isFileExists(fileName)) {
                throw runtime_error("Unable to initialize DataFile. File already exists. File: " + fileName);
            }

            cleanOrInitFile(fileName, kBytes);

            return DataFile(fileName);
        }

        static int getFileSize(string fileName) {
            int fileSize;
            ifstream iFile(fileName, ios::binary | ios::ate);
            if(iFile) {
                fileSize = iFile.tellg();
                iFile.close();
            }
            else {
                throw runtime_error("Error getting filesize of file: " + fileName);
            }

            return fileSize;
        }

        void erase() {
            if(!isDataFile()) {
                throw runtime_error("Unable to erase non-DataFile");
            }

            int kBytes = stoi(getSystemValue("length")) / Utils::bytesInKB;

            cleanOrInitFile(my_fileName, kBytes);
        }

        string getStatusReport() {
            string output = "vEPROM File: " + getFileName() + "\n";

            vector<vector<string> > spc_data = extractInternalSpaceData(getSystemSpaceReader());

            for(const auto &el :spc_data) {
                string my_key = el.at(0);
                string my_val = el.at(1);

                output += my_key + ":" + my_val + "\n";
            }

            return output;
        }

        /**
         * Get Files DTO (data transformation object) used metadata transfer purposes
        */
        vector<FileRepresentationDTO*> getFilesDTO() {
            vector<FileRepresentationDTO*> filesDTO;
            vector<vector<string> > spc_data = extractInternalSpaceData(getCtrlSpaceReader());

            int step = 0;
            for(const auto &el :spc_data) {
                FileRepresentationDTO* dto = new FileRepresentationDTO();
                dto->step = step;
                dto->fileName = el.at(0);

                stringstream val_s(el.at(1));

                string start;
                string end;
                while (!val_s.eof()) {
                    getline(val_s, start, ',');
                    getline(val_s, end, ':');
                }

                dto->startBytes = stoi(start);
                dto->endBytes = stoi(end);

                filesDTO.push_back(dto);
                step += 1;
            }

            return filesDTO;
        }

        string getFileName() {
            return my_fileName;
        }

        void writeRawAtAddress(int start, int end, char* input) {
            writeRawDataToUserSpace(start, end, input);
        }

        char* readRawAtAddress(int start, int length) {
            return readRawDataFromUserSpace(start, start + length);
        }

        void writeFile(char* sourceFilePath) {
            writeFileToUserSpace(sourceFilePath);
        }

        char* readFile(char* sourceFilePath) {
            return readFileFromUserSpace(sourceFilePath);
        }

        int getAggBytesSpan() {
            int length = stoi(getSystemValue("length"));
            return length / Utils::bytesInKB;
        }

        /**
         * Get bytes DTO (data transformation object) used for graphical display
        */
        vector<DataRepresentationAggDTO*> getBytesDTO() {
            vector<DataRepresentationAggDTO*> output;
            int bytesSpan = getAggBytesSpan();

            int usrSpcStart = stoi(getSystemValue("user_spc_start"));
            int usrSpcEnd = stoi(getSystemValue("user_spc_end"));
            int ctlSpcStart = stoi(getSystemValue("ctl_spc_start"));
            int ctlSpcEnd = stoi(getSystemValue("ctl_spc_end"));
            int sysSpcStart = stoi(getSystemValue("sys_spc_start"));
            int sysSpcEnd = stoi(getSystemValue("sys_spc_end"));

            vector<FileRepresentationDTO*> files = getFilesDTO();

            int byte = 0;

            ifstream iFile = getInputStream();
            if(iFile && iFile.is_open()) {
                for(int el = 0; el < Utils::bytesInKB; el++) {
                    DataRepresentationAggDTO* dto = new DataRepresentationAggDTO();

                    set<int> filesFound;

                    dto->step = el;

                    int freeBytes = 0;
                    
                    for(int i = 0; i < bytesSpan; i++) {
                        char c;
                        iFile.get(c);

                        if(c == '\0') {
                            freeBytes++;
                        }

                        if(Utils::isWithinRange(byte, usrSpcStart, usrSpcEnd)) {
                            dto->isUsrSpc = true;
                        }
                        else if(Utils::isWithinRange(byte, ctlSpcStart, ctlSpcEnd)) {
                            dto->isCtlSpc = true;
                        }
                        else if(Utils::isWithinRange(byte, sysSpcStart, sysSpcEnd)) {
                            dto->isSysSpc = true;
                        }

                        for(const auto &f :files) {
                            FileRepresentationDTO* fileDTO = f;
                            
                            // check if we already added this file.
                            // since we are doing things in aggregate to remove the dups
                            if(filesFound.count(fileDTO->step) == 0 && Utils::isWithinRange(byte, fileDTO->startBytes, fileDTO->endBytes)) {
                                dto->files.push_back(fileDTO);
                                filesFound.insert(fileDTO->step);
                            }
                        }

                        byte += 1;
                    }
                    dto->freeBytes = freeBytes;
                    dto->isCompletelyFree = freeBytes == bytesSpan;

                    output.push_back(dto);
                }
            }

            return output;
        }
};

