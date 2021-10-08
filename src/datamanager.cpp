#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "datamanager.hpp"


DataManager::DataManager(int nF, char** files)
{
    nFiles = nF;
    dataFiles = files;
    dateTimes = new std::string* [nFiles];
    times = new float* [nFiles];
    xAccels = new float* [nFiles];
    yAccels = new float* [nFiles];
    zAccels = new float* [nFiles];
    xGyros = new float* [nFiles];
    yGyros = new float* [nFiles];
    zGyros = new float* [nFiles];
    shiftedTimes = new float* [nFiles];
    nRecords = new int [nFiles];
}


void DataManager::read(void)
{
    int i;
    int j;
    int start;
    int* dataStart;
    std::string line;
    std::fstream inputFile;

    dataStart = &start;

    // Loop over each input file
    for (i=0; i<nFiles; i++)
    {
        inputFile.open(dataFiles[i], std::ios::in);
        // Get the number of lines in the file
        nRecords[i] = get_nlines(&inputFile, dataStart);
        // Allocate to hold data from current file
        allocate(i, nRecords[i]);
        // Go back to the beginning of the data
        inputFile.clear();
        inputFile.seekg(start);
        // Read the data
        j = 0;
        while (getline(inputFile, line))
        {
            // Skip blank lines and lines with only a carriage return
            if (line.length() > 1)
            {
                record(i, j, line);
                j++;
            }
            line.clear();
        }
        // Close the file
        inputFile.close();
        line.clear();
    }
}


int DataManager::get_nlines(std::fstream* inputFile, int* dataStart)
{
    int nLines = 0;
    int prevLineLoc;
    bool headerLine1;
    bool headerLine2;
    std::string line;

    prevLineLoc = inputFile->tellg();
    while (getline(*inputFile, line))
    {
        // Skip blank lines and lines with only a carriage return
        if (line.length() > 1)
        {
            // Skip header lines
            headerLine1 = line.find("Datalogging") != std::string::npos;
            headerLine2 = line.find("Timestamp") != std::string::npos;
            if (!headerLine1 && !headerLine2)
            {
                if (nLines == 0)
                {
                    *dataStart = prevLineLoc;
                }
                nLines++;
            }
        }
        prevLineLoc = inputFile->tellg();
        line.clear();
    }

    return nLines;
}


void DataManager::allocate(int i, int nLines)
{
    dateTimes[i] = new std::string [nLines];
    times[i]= new float [nLines];
    xAccels[i] = new float [nLines];
    yAccels[i] = new float [nLines];
    zAccels[i] = new float [nLines];
    xGyros[i] = new float [nLines];
    yGyros[i] = new float [nLines];
    zGyros[i] = new float [nLines];
    shiftedTimes[i] = new float [nLines];
}


void DataManager::record(int fileNum, int recordNum, std::string line)
{
    std::stringstream stream(line);
    std::string temp;
    std::vector<std::string> data;

    while (getline(stream, temp, ','))
    {
        data.push_back(temp);
    }

    dateTimes[fileNum][recordNum] = data[0];
    times[fileNum][recordNum] = stof(data[1]);
    xAccels[fileNum][recordNum] = stof(data[2]);
    yAccels[fileNum][recordNum] = stof(data[3]);
    zAccels[fileNum][recordNum] = stof(data[4]);
    xGyros[fileNum][recordNum] = stof(data[5]);
    yGyros[fileNum][recordNum] = stof(data[6]);
    zGyros[fileNum][recordNum] = stof(data[7]);
}


void DataManager::align(int* firstPeakIndices)
{
    /***
    Determines which file has the earliest first peak and then shifts
    the arrays of every file such that the first peak occurs at the
    same time.
    ***/
    int i;
    int j;
    int idx;
    float earliestTime = times[0][firstPeakIndices[0]];
    float curTime;
    float diff;

    // Find out which file's first peak occurs at the earliest time
    for (i=1; i<nFiles; i++)
    {
        idx = firstPeakIndices[i];
        curTime = times[i][idx];

        if (curTime < earliestTime)
        {
            earliestTime = curTime;
        }
    }

    // Determine the difference between the first peak time for each
    // file and the file with the earliest first peak. We then shift
    // the times of the later file by that amount
    for (i=0; i<nFiles; i++)
    {
        idx = firstPeakIndices[i];
        curTime = times[i][idx];
        diff = curTime - earliestTime;

        for (j=0; j<nRecords[i]; j++)
        {
            shiftedTimes[i][j] = times[i][j] - diff;
        }
    }
}

void DataManager::write(void)
{
    int i;
    int j;
    std::string record;
    std::fstream outputFile;

    outputFile.open("aligned_walk_data.csv", std::ios::out);

    // Add column headings
    outputFile << "original_filename,";
    outputFile << "date,";
    outputFile << "shifted_time,";
    outputFile << "x_acceleration,";
    outputFile << "y_acceleration,";
    outputFile << "z_acceleration,";
    outputFile << "x_gyro,";
    outputFile << "y_gyro,";
    outputFile << "z_gyro\n";

    // Write data
    for (i=0; i<nFiles; i++)
    {
        for (j=0; j<nRecords[i]; j++)
        {
            record = get_shifted_record(i, j);
            outputFile << record;
            outputFile << "\n";
            record.clear();
        }
    }
    outputFile.close();
}

std::string DataManager::get_shifted_record(int fileNum, int recordNum)
{
    std::string record;
    std::string fName;
    std::stringstream ss;

    ss << dataFiles[fileNum];
    fName = ss.str();

    record = fName + "," +
        dateTimes[fileNum][recordNum] + "," +
        std::to_string(shiftedTimes[fileNum][recordNum]) + "," +
        std::to_string(xAccels[fileNum][recordNum]) + "," +
        std::to_string(yAccels[fileNum][recordNum]) + "," +
        std::to_string(zAccels[fileNum][recordNum]) + "," +
        std::to_string(xGyros[fileNum][recordNum]) + "," +
        std::to_string(yGyros[fileNum][recordNum]) + "," +
        std::to_string(zGyros[fileNum][recordNum]);

    return record;
}
