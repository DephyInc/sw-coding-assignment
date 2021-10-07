#include "datafile.hpp"
#include "proto.hpp"


/*********************************************
                     main
*********************************************/
int main(int argc, char** argv)
{
    DataFile* dataFiles;

    // Read input files
    dataFiles = read_input_files(argc, argv);

    // Perform sinusoidal regression on the data
    dataFiles = sinusoidal_regression(dataFiles, argc-1);
    
    // Find peaks
    dataFiles = find_peaks(dataFiles, argc-1);

    // Align the data
    dataFiles = align_data(dataFiles, argc-1);

    // Save the aligned data to a new file
    save_aligned(dataFiles, argc-1);

    return 0;
}
