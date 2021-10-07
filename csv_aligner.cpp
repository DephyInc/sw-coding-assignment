#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <math.h>
using namespace std;

// Class for holding CSV File data that is wanted
class csvData {
    public: 
        vector<string> timestampOne;
        vector<int> timeStampTwo;
        vector<int> accelX;
        vector<int> accelY;
        vector<int> accelZ;
        vector<int> gyroX;
        vector<int> gyroY;
        vector<int> gyroZ;
};

// Class for holding helpful stats based on data stored in csvData Objects
class helpfulStats {
    public: 
        vector<int> dataPoints;
        vector<int> dataMax;
        vector<int> dataMin;
        vector<int> dataAverage;
        vector<int> goodVCorruptdata;
};

// Should be used to track signal peak indexes between files, as well as good vs corrupt data points
// class averagingPeaks {
//     public: 
//         vector<int> averages;
//         vector<int> stepPeaks;
//         vector<int> peakIndex;
//         vector<int> gdVCorrupt;
// };

/*  Function to display csv files to the terminal useful for making sure data is being read correctly.
    I left this here for the scenario that future changes to the file would be done and the function could be
    used to verify if there is data coming in correctly */
void termDisplayCSV(csvData csvOb) {
    int vectLen = csvOb.accelX.size();
    if (vectLen == 0) {
        cout << "Object is empty" << endl;
    }
    cout << "vectlen = " << vectLen << endl;
    for (int i = 0; i < vectLen; i++) {
        cout << "here is csvObj[" << i+3 << "] = " 
            << csvOb.timestampOne[i] << ", " << csvOb.timeStampTwo[i] << ", "
            << csvOb.accelX[i] << ", " << csvOb.accelY[i] << ", "
            << csvOb.accelZ[i] << ", " << csvOb.gyroX[i] << ", "
            << csvOb.gyroY[i] << ", " << csvOb.gyroZ[i] << endl;
    }
    return;
}

/*  Function reads data from csv file.
    The filepaths, the objects to store the data in, and the number of paths is passed to the function.*/
void readFromCSV(vector<string> filepath, csvData csvObjRead[], int sizePath) {
    for (int i = 0; i < sizePath; i ++){
        ifstream csvFrom(filepath[i]);

        if (!csvFrom.is_open()) {
            cout << "File failed to open" << endl;
            return;
        } else { // temporary strings to store the data in order to read the file line by line
            string tempTimeStampOne, tempTimeStampTwo, tempAccelX, 
                tempAccelY, tempAccelZ, tempGyroX, tempGyroY, tempGyroZ;
            string line;

            int count = 0;
            while (getline(csvFrom, line)){
                if (count < 2){ // skips the first two lines of the walking csv files due to header info
                    count++;
                    continue;
                }
                if (line.empty()){ // if line is empty, skip it (applies to csv/ text files with an extra line)
                    continue;
                } else { // separates line by commas and adds wanted data to respective vectors in Object
                    stringstream ss(line);
                    getline(ss, tempTimeStampOne, ',');
                    csvObjRead[i].timestampOne.push_back(tempTimeStampOne);
                    getline(ss, tempTimeStampTwo, ',');
                    csvObjRead[i].timeStampTwo.push_back(stoi(tempTimeStampTwo));
                    getline(ss, tempAccelX, ',');
                    csvObjRead[i].accelX.push_back(stoi(tempAccelX));
                    getline(ss, tempAccelY, ',');
                    csvObjRead[i].accelY.push_back(stoi(tempAccelY));
                    getline(ss, tempAccelZ, ',');
                    csvObjRead[i].accelZ.push_back(stoi(tempAccelZ));
                    getline(ss, tempGyroX, ',');
                    csvObjRead[i].gyroX.push_back(stoi(tempGyroX));
                    getline(ss, tempGyroY, ',');
                    csvObjRead[i].gyroY.push_back(stoi(tempGyroY));
                    getline(ss, tempGyroZ, ',');
                    csvObjRead[i].gyroZ.push_back(stoi(tempGyroZ));
                }
            }
        }
        csvFrom.close();
    }
    return; // returns filled object
}

/* Function finds the length of the longest object.
    Checking the object length here allows you to buffer data
    and still be able to retun the longest length.*/
int longestCsvObject(csvData csvObject[], int sizeObj) {
    unsigned longObject = 0;

    for (int i = 0; i < sizeObj; i++) {
        if (longObject < csvObject[i].timestampOne.size()) {
            longObject = csvObject[i].timestampOne.size();
        }
    }
    return longObject;
}

/*  Function takes in a file path to generate and write the data to, the CSV objects holding the data, amount of objects, where the info is from
    It then writes all data from the objects holding the input file data to a new file*/
void writeToCSV(string filepath, csvData csvObj[], int sizeObj, vector<string> pathInfo) {
    ofstream csvTo(filepath);

    int lenLongFile = longestCsvObject(csvObj, sizeObj);

    if (!csvTo.is_open()) {
        cout << "File failed to open" << endl;
        return;
    } else {
        // The following outputs the file path info as a key
        csvTo << "File Info: ,";
        for (int i = 0; i < sizeObj; i++) {
            csvTo << "[" << i << "] = " << pathInfo[i];
            if(i != sizeObj-1) {
                csvTo << ",";
            } else {
                csvTo << "\n";
            }
        }
        // The following outputs the headers to the file
        for (int j = 0; j < sizeObj; j ++) {
            csvTo << "Timestamp[" << j << "]," << "Timestamp (ms)[" << j << "]," 
                << "accel.x[" << j << "]," << "accel.y[" << j << "]," 
                << "accel.z[" << j << "]," << "gyro.x[" << j << "]," 
                << "gyro.y[" << j << "]," << "gyro.z[" << j << "]";
            if (j != sizeObj-1) {
                csvTo << ",";
            } else {
                csvTo << "\n";
            }
        }

        // The following outputs the data of all objects to their respective rows and columns
        for (int m = 0; m < lenLongFile; m ++) {
            for (int k = 0; k < sizeObj; k++) {
                if (csvObj[k].timestampOne[m].empty()) {
                    csvTo << ",,,,,,,"; // if one file is longer than another it fills the difference with empties
                    if (k != sizeObj-1) {
                        csvTo << ",";
                    } else {
                        csvTo << "\n";
                    }
                } else {
                    csvTo << csvObj[k].timestampOne[m] << "," << csvObj[k].timeStampTwo[m] << "," 
                        << csvObj[k].accelX[m] << "," << csvObj[k].accelY[m] << ","
                        << csvObj[k].accelZ[m] << "," << csvObj[k].gyroX[m] << ","
                        << csvObj[k].gyroY[m] << "," << csvObj[k].gyroZ[m];
                    if(k != sizeObj-1) {
                        csvTo << ",";
                    } else {
                        csvTo << "\n";
                    }
                }
            }
        }
    }
    csvTo.close();
    return;
}

/*  Function takes in csv file objects storing data, number of objects, objects for storing the generated stats.
    The Generated data included number of data points, max, min, average, good Vs corrupt Data.*/
void getHelpfulStats(csvData csvObjects[], int sizeObjects, helpfulStats dataStats[]) {
    for (int i = 0; i < sizeObjects; i ++) {
            int sizeAccelX = csvObjects[i].accelX.size();
            int sumAccelX = accumulate(csvObjects[i].accelX.begin(), csvObjects[i].accelX.end(), 0);
            int averageAccelX = sumAccelX / sizeAccelX; // calculates average for AccelX

            dataStats[i].dataPoints.push_back(csvObjects[i].accelX.size()); // adds AccelX size to vector in object
            dataStats[i].dataMax.push_back(*max_element(csvObjects[i].accelX.begin(), csvObjects[i].accelX.end())); // adds value of AccelX max data point to associated vector in object
            dataStats[i].dataMin.push_back(*min_element(csvObjects[i].accelX.begin(), csvObjects[i].accelX.end())); // adds value of AccelX min data point to associated vector in object
            dataStats[i].dataAverage.push_back(averageAccelX); // adds Average value of AccelX data points to associated vector in object
            dataStats[i].goodVCorruptdata.push_back(0); // adds the percentage of AccelX's good Vs Corrupt data to associated vector in object

            dataStats[i].dataPoints.push_back(csvObjects[i].accelY.size());
            dataStats[i].dataMax.push_back(*max_element(csvObjects[i].accelY.begin(), csvObjects[i].accelY.end()));
            dataStats[i].dataMin.push_back(*min_element(csvObjects[i].accelY.begin(), csvObjects[i].accelY.end()));
            dataStats[i].dataAverage.push_back((accumulate(csvObjects[i].accelY.begin(), csvObjects[i].accelY.end(), 0)) / (csvObjects[i].accelY.size()));
            dataStats[i].goodVCorruptdata.push_back(0);

            dataStats[i].dataPoints.push_back(csvObjects[i].accelZ.size());
            dataStats[i].dataMax.push_back(*max_element(csvObjects[i].accelZ.begin(), csvObjects[i].accelZ.end()));
            dataStats[i].dataMin.push_back(*min_element(csvObjects[i].accelZ.begin(), csvObjects[i].accelZ.end()));
            dataStats[i].dataAverage.push_back((accumulate(csvObjects[i].accelZ.begin(), csvObjects[i].accelZ.end(), 0)) / (csvObjects[i].accelZ.size()));
            dataStats[i].goodVCorruptdata.push_back(0);

            dataStats[i].dataPoints.push_back(csvObjects[i].gyroX.size());
            dataStats[i].dataMax.push_back(*max_element(csvObjects[i].gyroX.begin(), csvObjects[i].gyroX.end()));
            dataStats[i].dataMin.push_back(*min_element(csvObjects[i].gyroX.begin(), csvObjects[i].gyroX.end()));
            dataStats[i].dataAverage.push_back((accumulate(csvObjects[i].gyroX.begin(), csvObjects[i].gyroX.end(), 0)) / (csvObjects[i].gyroX.size()));
            dataStats[i].goodVCorruptdata.push_back(0);

            dataStats[i].dataPoints.push_back(csvObjects[i].gyroY.size());
            dataStats[i].dataMax.push_back(*max_element(csvObjects[i].gyroY.begin(), csvObjects[i].gyroY.end()));
            dataStats[i].dataMin.push_back(*min_element(csvObjects[i].gyroY.begin(), csvObjects[i].gyroY.end()));
            dataStats[i].dataAverage.push_back((accumulate(csvObjects[i].gyroY.begin(), csvObjects[i].gyroY.end(), 0)) / (csvObjects[i].gyroY.size()));
            dataStats[i].goodVCorruptdata.push_back(0);

            dataStats[i].dataPoints.push_back(csvObjects[i].gyroZ.size());
            dataStats[i].dataMax.push_back(*max_element(csvObjects[i].gyroZ.begin(), csvObjects[i].gyroZ.end()));
            dataStats[i].dataMin.push_back(*min_element(csvObjects[i].gyroZ.begin(), csvObjects[i].gyroZ.end()));
            dataStats[i].dataAverage.push_back((accumulate(csvObjects[i].gyroZ.begin(), csvObjects[i].gyroZ.end(), 0)) / (csvObjects[i].gyroZ.size()));
            dataStats[i].goodVCorruptdata.push_back(0);
    }
    return;
}

/*  Function takes in the filepath used to generate the file it should write to, number of objects being used,
    where the original file data is from, the objects storing the helpful stats*/
void writeHelpfulStats(string filepath, int sizeObjs, vector<string> pathInfo, helpfulStats dataStats[]) {
    ofstream csvTo(filepath);
    if (!csvTo.is_open()) {
        cout << "File failed to open" << endl;
        return;
    } else {
        // The following outputs the headers to the csv Stats file
        csvTo << "File Info: ,";
        for(int j = 0; j < sizeObjs; j++) {
            csvTo << "[" << j << "] = " << pathInfo[j];
            if (j != sizeObjs-1) {
                csvTo << ",";
            } else {
                csvTo << "\n";
            }
        }
        csvTo << "Data Origin,Data Points,Max,Min,Average,Good Vs Corrupted\n";

        for(int i = 0; i < sizeObjs; i ++) { // sends labels and data to file
            for(int k = 0; k < 6; k++) {
                if (k == 0) {
                    csvTo << "accel.x[" << i << "],";
                } else if (k == 1) {
                    csvTo << "accel.y[" << i << "],";
                } else if (k == 2) {
                    csvTo << "accel.z[" << i << "],";
                } else if (k == 3) {
                    csvTo << "gyro.x[" << i << "],";
                } else if (k == 4) {
                    csvTo << "gyro.y[" << i << "],";
                } else if (k == 5) {
                    csvTo << "gyro.z[" << i << "],";
                }
                
                csvTo << dataStats[i].dataPoints[k] << ","
                    << dataStats[i].dataMax[k] << ","
                    << dataStats[i].dataMin[k] << ","
                    << dataStats[i].dataAverage[k] << ","
                    << dataStats[i].goodVCorruptdata[k] << "\n";
            }
        }
    csvTo.close();
    return;
    }
}

// takes input filepaths and places each into its own vector index
void tokenize(string const &str, const char delim, vector<string> &out) {
    stringstream ss(str);
    string s;
    while(getline(ss, s, delim)) {
        out.push_back(s);
    }
}

// Function finds the mean value of a vector
int mean(vector<int> vec) {
    int sum = accumulate(vec.begin(), vec.end(), 0);
    int theMean = sum / vec.size();
    return theMean;
}

// Function find the standard deviation of a vector
int stdDev(vector<int> vec) {
    int accum = 0.0;
    for_each (begin(vec), end(vec), [&](const int d) {
        accum += (d - mean(vec)) * (d - mean(vec));
    });

    int stdev = sqrt(accum / (vec.size()-1));

    return stdev;
}

/*  Function is supposed to output a -1, 0, or 1 based on whether it is a drop, neutral value, or peak.
    Using the vector storing the data you wish to align the files by, it goes through trying to find the 
    peaks using mean, stddev, previous inputs, */
vector<int> smoothedZScore(vector<int> input) {   
    //lag 5 for the smoothing functions
    int lag = 25;
    //3.5 standard deviations for signal
    int threshold = 5;
    //between 0 and 1, where 1 is normal influence, 0.5 is half
    int influence = 0;

    int inputSize = input.size();
    if (inputSize <= lag + 2) {
        vector<int> emptyVec;
        return emptyVec;
    }

    //Initialise variables
    vector<int> signals(input.size(), 0);
    vector<int> filteredY(input.size(), 0);
    vector<int> avgFilter(input.size(), 0);
    vector<int> stdFilter(input.size(), 0);
    vector<int> subVecStart(input.begin(), input.begin() + lag);
    avgFilter[lag] = mean(subVecStart);
    stdFilter[lag] = stdDev(subVecStart);

    for (size_t i = lag + 1; i < input.size(); i++) {
        if (abs(input[i] - avgFilter[i - 1]) > threshold * stdFilter[i - 1]) {
            if (input[i] > avgFilter[i - 1]) {
                signals[i] = 1; //# Positive signal
            } else {
                signals[i] = -1; //# Negative signal
            }
            //Make influence lower
            filteredY[i] = influence* input[i] + (1 - influence) * filteredY[i - 1];
        } else {
            signals[i] = 0; //# No signal
            filteredY[i] = input[i];
        }
        //Adjust the filters
        vector<int> subVec(filteredY.begin() + i - lag, filteredY.begin() + i);
        avgFilter[i] = mean(subVec);
        stdFilter[i] = stdDev(subVec);
    }
    return signals;
}

int main() {
    string filePathways;
    vector <string> pathsFrom;

    cout << "Please enter filepaths with spaces between each: ";
    getline(cin, filePathways);
    const char delim = ' '; // what the input is split by (filepaths must not have spaces)
    tokenize(filePathways, delim, pathsFrom);

    string outputFolder;
    cout << "Please the filepath to where you would like the files to output: ";
    cin >> outputFolder;
    
    int pathNumb = pathsFrom.size();
    csvData csvFiles[pathNumb];
    helpfulStats csvStats[pathNumb];

    readFromCSV(pathsFrom, csvFiles, pathNumb);

    getHelpfulStats(csvFiles, pathNumb, csvStats);

    vector<int> signalsOut = smoothedZScore(csvFiles[0].accelZ);

    string pathToAligned;
    string pathtoStats;
    #if defined(_WIN32) || defined(__linus__)
        pathToAligned = outputFolder + "/walk_data_aligned.csv";
        pathtoStats = outputFolder + "/walk_data_helpfulstats.csv";
    #elif (__APPLE__)
        pathToAligned = outputFolder + "\csvOutcome.csv";
        pathtoStats = outputFolder + "\csvHelpfulStats.csv";
    #endif

    writeToCSV(pathToAligned, csvFiles, pathNumb , pathsFrom);
    writeHelpfulStats(pathtoStats, pathNumb, pathsFrom, csvStats);

    return 0;
}
