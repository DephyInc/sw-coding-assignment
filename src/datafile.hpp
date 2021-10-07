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
        int firstPeakIndex;
        float* beta;
        float* shiftedTimes;
        DataRecord* dataRecords;

        // Dummy constructor for use in dynamic array construction
        DataFile() {}

        DataFile(int n, std::string fName)
        {
            nRecords = n;
            fileName = fName;
            dataRecords = new DataRecord [nRecords];
            shiftedTimes = new float [nRecords];
            curRec = 0;
        }

        ~DataFile()
        {
            delete[] beta;
            delete[] shiftedTimes;
            delete[] dataRecords;
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

        float* extract_times(void)
        {
            // Gathers up the timestamps from each record into one
            // contiguous array
            int i;
            float* t;

            t = new float [nRecords];
            for (i=0; i<nRecords; i++)
            {
                t[i] = dataRecords[i].timeStamp;
            }

            return t;
        }

        float* extract_x(void)
        {
            // Gathers up the x accel from each record into one
            // contiguous array
            int i;
            float* x;

            x = new float [nRecords];
            for (i=0; i<nRecords; i++)
            {
                x[i] = dataRecords[i].xAccel;
            }

            return x;
        }

        void save_beta(float* b, int n)
        {
            int i;

            beta = new float [n];

            for (i=0; i <n; i++)
            {
                beta[i] = b[i];
            }
        }

        std::string get_shifted_record(int idx)
        {
            std::string record;

            record = fileName + "," +
                dataRecords[idx].dateTime + "," +
                std::to_string(shiftedTimes[idx]) + "," +
                std::to_string(dataRecords[idx].xAccel) + "," +
                std::to_string(dataRecords[idx].yAccel) + "," +
                std::to_string(dataRecords[idx].zAccel) + "," +
                std::to_string(dataRecords[idx].xGyro) + "," +
                std::to_string(dataRecords[idx].yGyro) + "," +
                std::to_string(dataRecords[idx].zGyro) + "," +
                std::to_string(dataRecords[idx].strain) + "," +
                std::to_string(dataRecords[idx].analog0) + "," +
                std::to_string(dataRecords[idx].analog1) + "," +
                std::to_string(dataRecords[idx].current) + "," +
                std::to_string(dataRecords[idx].encDisp) + "," +
                std::to_string(dataRecords[idx].encCont) + "," +
                std::to_string(dataRecords[idx].encComm) + "," +
                std::to_string(dataRecords[idx].vb) + "," +
                std::to_string(dataRecords[idx].vg) + "," +
                std::to_string(dataRecords[idx].temp) + "," +
                std::to_string(dataRecords[idx].status1) + "," +
                std::to_string(dataRecords[idx].status2) + "\n";

            return record;
        }
};

#endif
