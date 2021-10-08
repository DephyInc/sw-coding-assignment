#include "datamanager.hpp"
#include "regressor.hpp"


/*********************************************
                     main
*********************************************/
int main(int argc, char** argv)
{
    DataManager dataManager(argc-1, argv+1);
    SineRegressor regressor;

    // Read input files
    dataManager.read();

    // Perform sinusoidal regression on the data
    regressor.do_regression(
        dataManager.times,
        dataManager.xAccels,
        dataManager.nRecords,
        dataManager.nFiles
    );

    // Find the first peaks in each dataset
    regressor.find_peaks(
        dataManager.times,
        dataManager.nRecords,
        dataManager.nFiles
    );
    
    // Align the data
    dataManager.align(regressor.firstPeakIndices);

    // Save the aligned data to a new file
    dataManager.write();

    return 0;
}
