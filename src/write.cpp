#include <fstream>
#include <string>

#include "datafile.hpp"
#include "proto.hpp"


/*********************************************
                save_aligned
*********************************************/
void save_aligned(DataFile* dataFiles, int n)
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
    outputFile << "z_gyro,";
    outputFile << "Strain,";
    outputFile << "analog0,";
    outputFile << "analog1,";
    outputFile << "current,";
    outputFile << "enc_disp,";
    outputFile << "enc_cont,";
    outputFile << "enc_comm,";
    outputFile << "vb,";
    outputFile << "vg,";
    outputFile << "temp,";
    outputFile << "status1,";
    outputFile << "status2\n";

    // Write data
    for (i=0; i<n; i++)
    {
        for (j=0; j<dataFiles[i].nRecords; j++)
        {
            record = dataFiles[i].get_shifted_record(j);
            outputFile << record;
            outputFile << "\n";
            record.clear();
        }
    }
    outputFile.close();
}
