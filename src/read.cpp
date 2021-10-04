#include <fstream>
#include <string>

#include "datafile.hpp"
#include "proto.hpp"


/*********************************************
               read_input_files
*********************************************/
DataFile* read_input_files(int argc, char** argv)
{
    /***
    Reads and saves each line of data from each given data file.

    Arguments
    ---------
        argc : int
            The number of arguments passed to the program. This is the
            number of data files we wish to align plus one for the
            program name.

        argv : char**
            Array containing the program name followed by the names of
            each data file we wish to align.

    Returns
    -------
        dataFiles : DataFile*
            An array of DataFile instances. Each DataFile stores the
            parsed data from that file as well as metadata about the
            file.
    ***/
    int i;
    int prevLine;
    int nLines;
    int dataStart;
    bool headerLine1;
    bool headerLine2;
    std::fstream inputFile;
    std::string line;
    DataFile* dataFiles = new DataFile[argc - 1];

    // Loop over every data file (start at 1 to skip program name)
    for (i=1; i < argc; i++)
    {
        nLines = 0;
        inputFile.open(argv[i], std::ios::in);
        prevLine = inputFile.tellg();
        // Figure out how many lines are in the file
        while (getline(inputFile, line))
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
                        dataStart = prevLine;
                    }
                    nLines++;
                }
            }
            prevLine = inputFile.tellg();
            line.clear();
        }
        // Create the DataFile instance
        dataFiles[i-1] = DataFile(nLines, argv[i]);
        // Go back to the beginning of the data. Have to clear the
        // stream first to reset the fail bit before trying to seek
        // since the above loop hits EOF. Otherwise the seek doesn't do
        // anything and getline fails
        inputFile.clear();
        inputFile.seekg(dataStart);
        // Read the data
        while (getline(inputFile, line))
        {
            // Skip blank lines and lines with only a carriage return
            if (line.length() > 1)
            {
                dataFiles[i-1].record(line);
            }
            line.clear();
        }
        // Close the file
        inputFile.close();
        line.clear();
    }

    return dataFiles;
}
