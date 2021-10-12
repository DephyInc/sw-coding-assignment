/*
*	File: csv_aligner.cpp
*	Author: Scott Laboe
*	Date: 10/10/2021
*/

#include "Data.h"
#include "DataProcessor.h"
#include <fstream>

std::string outputFileName("outputFile.csv");

/**
 * Program entry point
 *
 * @param argc is the number of parameters passed
 * @param argv is an array of pointers to the commands passed
 * @return exit code is 0 if normal exit
 */
int main(int argc, char** argv)
{
	if (argc <= 2) {
		std::cerr << "Please enter at least two files as arguments" << std::endl;
		return -1;
	}

	Data data;

	for (int i = 1; i < argc; ++i) {
		data.addDataFile(argv[i]);
	}
	std::ofstream outputFile(outputFileName);
	if (outputFile.fail()) {
		std::cerr << "Failed to open output file: " << "outputFile.csv" << std::endl;
	}
	int i = 0;
	size_t numberOfAnomalies;
	//calculate stats for each file
	for (auto& dataFile : data.dataCollection) {
		std::vector<std::vector<int>> peaks;
		for (auto& point : dataFile.sensorData) {
			peaks.push_back(DataProcessor::findPeaks(&point, 15, 10, 0.3, false));
			if (peaks.size()) {
				//set good to corrupt data ratio
				numberOfAnomalies = DataProcessor::findAnomolies(&point, peaks.back()).size();
				dataFile.stats.goodToCorruptDataRatio.push_back(1.0 - static_cast<double>(numberOfAnomalies)/ static_cast<double>(point.size()) );
			}
		}
		//peak index is first peak in accelerometer X
		dataFile.peakIndex = (peaks.front().at(data.ACCEL_X));
		DataProcessor::calculateStats(&dataFile.stats, &dataFile.sensorData);
	}
	
	data.alignDataToPeak();
	
	//output to file
	outputFile << data;
	for (auto& dataFile : data.dataCollection) {
		outputFile << std::endl << std::endl;
		outputFile << dataFile.fileName << std::endl;
		outputFile << dataFile.stats;
	}

	outputFile.close();

	std::cout << "Data output to: " << outputFileName << std::endl;
	return 0;
}
