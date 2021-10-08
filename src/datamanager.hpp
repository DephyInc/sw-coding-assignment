#ifndef _DATAMANAGER_
#define _DATAMANAGER_

#include <fstream>
#include <string>


class DataManager
{
    public:
        int nFiles;
        char** dataFiles;
        std::string** dateTimes;
        int* nRecords;
        float** times;
        float** xAccels;
        float** yAccels;
        float** zAccels;
        float** xGyros;
        float** yGyros;
        float** zGyros;
        float** shiftedTimes;

        DataManager(int, char**);
        void read(void);
        int get_nlines(std::fstream*, int*);
        void allocate(int, int);
        void record(int, int, std::string);
        void align(int*);
        void write(void);
        std::string get_shifted_record(int, int);
};
#endif
