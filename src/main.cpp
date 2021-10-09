#include "datamanager.hpp"
#include "regressor.hpp"


/*********************************************
                     main
*********************************************/
int main(int argc, char** argv)
{
    /***
    Reads in sensor data from walking trials, aligns each daataset
    according to the position of the earliest step, and then writes
    the aligned data back out to a new file.

    The general idea is to perform sinusoidal regression on each
    dataset. Once we have the best-fit sine curves we can determine
    where the first peak is for each dataset. So armed, we can then
    perform the desired shift.

    Usage:
    ------
        .csv_aligner file1.csv file2.csv ...
    ***/
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

    // Find the first peak in each dataset
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
