#include <math.h>

#include "datafile.hpp"
#include "proto.hpp"


/*********************************************
                  find_peaks
*********************************************/
DataFile* find_peaks(DataFile* dataFiles, int nFiles)
{
    /***
    Locates the index of the data arrays closest to the first peak in
    the model sine wave.

    The sensor data was fit with a generic sine wave:

    y(x) = A + B * sin(C * x + D),

    where A, B, C, and D are the four beta parameters found by
    sinusoidal regression.

    Sine has a max when its argument theta = n * pi / 2. The first peak
    occurs when n = 1, so we have:

    Cx + D = pi / 2 => x = (pi/2 - D) / C.

    We then search our timestamps for the value closest to x. This
    gives us the index corresponding to the first peak.
    ***/
    int i;
    int idx;
    float x;

    for (i=0; i<nFiles; i++)
    {
        x = (M_PI / 2. - dataFiles[i].beta[3]) / dataFiles[i].beta[2];
        idx = find_closest_index(dataFiles[i].extract_times(), x, nFiles);
        dataFiles[i].firstPeakIndex = idx;
    }

    return dataFiles;
}


/*********************************************
              find_closest_index
*********************************************/
int find_closest_index(float* a, float x, int n)
{
    /***
    Searches through the array a in order to find the index
    corresponsing to the value in a closest to x.
    ***/
    int idx;
    int i;
    float diff;
    float temp;

    diff = fabs(x - a[0]);
    idx = 0;

    for (i=1; i<n; i++)
    {
        temp = fabs(x - a[i]);
        if (temp < diff)
        {
            diff = temp;
            idx = i;
        }
    }

    return idx;
}
