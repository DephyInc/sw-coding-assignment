#ifndef _DATAFILE_
#define _DATAFILE_

#ifndef _DATARECORD_
#include "datarecord.hpp"
#endif

#include <sstream>
#include <string>
#include <vector>

class DataFile
{
    private:
        int curRec;
    public:
        std::string fileName;
        int nRecords;
        DataRecord* dataRecords;

        // Dummy constructor for use in dynamic array construction
        DataFile() {}

        DataFile(int n, std::string fName)
        {
            nRecords = n;
            fileName = fName;
            dataRecords = new DataRecord [nRecords];
            curRec = 0;
        }

        void record(std::string line)
        {
            std::stringstream stream(line);
            std::vector<std::string> data;
            std::string temp;

            while (getline(stream, temp, ','))
            {
                data.push_back(temp);
            }
            
            dataRecords[curRec] = DataRecord(data);
            curRec++;
        }
};

#endif
