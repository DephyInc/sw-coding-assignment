#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "datamanager.hpp"


/*********************************************
                constructor
*********************************************/
DataManager::DataManager(int nF, char** files)
{
    /***
    Arguments:
    ----------
        nF : int
            The number of data files to align.
        files : char**
            The list of file names to align.
    ***/
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


/*********************************************
                 desctructor
*********************************************/
DataManager::~DataManager()
{
    int i;

    for (i=0; i<nFiles; i++)
    {
        delete[] times[i];
        delete[] xAccels[i];
        delete[] yAccels[i];
        delete[] zAccels[i];
        delete[] xGyros[i];
        delete[] yGyros[i];
        delete[] zGyros[i];
        delete[] shiftedTimes[i];
    }
        delete[] times;
        delete[] xAccels;
        delete[] yAccels;
        delete[] zAccels;
        delete[] xGyros;
        delete[] yGyros;
        delete[] zGyros;
        delete[] shiftedTimes;
        delete[] nRecords;
}


/*********************************************
                     read
*********************************************/
void DataManager::read(void)
{
    /***
    Driver function for reading in each data file.

    The data files have the following layout: two lines of header
    followed by the data. Each line has all of the data gathered at a
    single time step.

    Columns:
    date, time, x-accel, y-accel, z-accel, x-gyro, y-gyro, z-gyro,...

    For this project, only the first eight columns are relevant.
    ***/
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


/*********************************************
                  get_nlines
*********************************************/
int DataManager::get_nlines(std::fstream* inputFile, int* dataStart)
{
    /***
    Determines the number of lines of data in the given file.

    Additionally, by having to skip the first two lines of header,
    we also save the location in the file where the actual data starts.
    This means that, after allocating memory to hold the data, we can
    begin reading at the proper point.

    Arguments:
    ----------
        inputFile : fstream*
            The pointer to the file object used to read in the current
            data file.
        dataStart : int*
            Holds the location in the file where the actual data
            begins. Is a pointer because C++ can't return multiple
            values from a function.

    Returns:
    --------
        nLines : int
            The number of lines of data in the file.
    ***/
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


/*********************************************
                  allocate
*********************************************/
void DataManager::allocate(int i, int nLines)
{
    /***
    Handles allocating the proper amount of memory needed to hold the
    data.

    Arguments:
    ----------
        i : int
            Index giving the current file being read.
        nLines : int
            The number of data entries we need to store for this file.
    ***/
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


/*********************************************
                    record
*********************************************/
void DataManager::record(int fileNum, int recordNum, std::string line)
{
    /***
    Parses a given line into the appropriate arrays.

    Arguments:
    ----------
        fileNum : int
            Index giving the current file being read.
        recordNum : int
            Index giving the current row of data being read (does not
            include header lines).
        line : string
            The line of data to be parsed.
    ***/
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


/*********************************************
                    align
*********************************************/
void DataManager::align(int* firstPeakIndices)
{
    /***
    Determines which file has the earliest first peak and then shifts
    the arrays of every file such that the first peak occurs at the
    same time.

    Arguments:
    ----------
        firstPeakIndices : int*
            Array containing the index corresponding to the value
            closest to the peak value determined by sinusoidal
            regression. There's one index per data file.
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


/*********************************************
                    write
*********************************************/
void DataManager::write(void)
{
    /***
    Writes the shifted data from each data file to a new csv file.

    Columns: Original data file name, date, shifted time, x-accel,
    y-accel, z-accel, x-gyro, y-gyro, and z-gyro.
    ***/
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


/*********************************************
              get_shifted_record
*********************************************/
std::string DataManager::get_shifted_record(int fileNum, int recordNum)
{
    /***
    Constructs the string to be written to the output file from the
    constituent arrays.

    Arguments:
    ----------
        fileNum : int
            Index giving the original data file.
        recordNum : int
            Index giving the original line of data.

    Returns:
    --------
        record : string
            The line of data containing: original file name, date,
            shifted time, x-accel, y-accel, z-accel, x-gyro, y-gyro,
            and z-gyro.
    ***/
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
