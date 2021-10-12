/*
*	File: Data.cpp
*	Author: Scott Laboe
*	Date: 10/10/2021
*/

#include <fstream>
#include <iterator>
#include <regex>
#include <string>
#include <locale>
#include <iomanip>
#include <sstream>

#include "Data.h"

const std::regex inputDelimiter(",");
const std::string outputDelimiter(", ");

/**
 * Output Data
 *
 * Overload insertion operator to send data to output stream. Output column names then process data
 *
 * @param os input stream
 * @param dat data object
 * @return output stream
 */
std::ostream& operator<<(std::ostream& os, const Data& dat){
	size_t numberOfFilesToProcess = dat.dataCollection.size();

	//Add headers for each file
	for (auto dFile = dat.dataCollection.begin(); dFile != dat.dataCollection.end(); ++dFile) {
		for (auto column = dat.columnNames.begin(); column != dat.columnNames.end(); ++column) {
				os << dFile->fileName << "." << *column;
				//don't place delimiter after last element of the last file (i.e. the end of the list)
				if (!(dFile == (dat.dataCollection.end()-1) && column == (dat.columnNames.end()-1))) os << outputDelimiter;
		}
	}
	long currentLine = 0;
	const Data::nonDataPoint_t * tmpNonDataPoint;

	//Iterate through every data point. The files are sorted so the first has the most points
	for (int pointIndex = 0; pointIndex < dat.dataCollection.front().sensorData.front().size(); ++pointIndex) {
		os << std::endl;
		++currentLine;
		for (int fileIndex = 0; fileIndex < numberOfFilesToProcess; ++fileIndex) {
			tmpNonDataPoint = &dat.dataCollection.at(fileIndex).nonSensorData.at(pointIndex);

			os << tmpNonDataPoint->timeStamp << outputDelimiter;
			os << tmpNonDataPoint->timeStamp_ms;

			//numeric data
			for (int i = 0; i < Data::NUM_DATA_FIELDS; ++i) {
				os << outputDelimiter;
				os << dat.dataCollection.at(fileIndex).sensorData.at(i).at(pointIndex);
			}

			if (fileIndex < numberOfFilesToProcess - 1) {
				os << outputDelimiter;
			}
			//the data set at the end of the vector is exausted and doesn't need additional processing
			if (fileIndex == numberOfFilesToProcess-1 
				&& dat.dataCollection.at(fileIndex).sensorData.front().size() <= currentLine) {
				--numberOfFilesToProcess;
			}
		}
	}
	return os;
}

/**
 * Insert File
 *
 * Files are sorted longest to shortest. This searches for the correct location to insert the
 * file parameter to maintain sorting. It uses an overloaded "<" operator defined in the class which
 * compares the length of the data point array in each file and returns true if the size of the left
 * operand is greater than the right.
 *
 * @param fileToInsert is a pointer to the file to be inserted into the collection
 */
void Data::insertFile(dataFile_t fileToInsert) {
	//find insertion point and insert file
	dataCollection.insert(
		std::lower_bound(dataCollection.rbegin(), dataCollection.rend(), fileToInsert).base(),
		fileToInsert
		);
}

/**
 * Add Data file to data collection
 *
 * Parse input file and add the data to the data collection
 *
 * @param filename is the path to the file being imported
 * @return true if file is successfully parsed and false if there is an error
 */
bool Data::addDataFile(std::string filePath){
	std::ifstream inputFile(filePath);
	if (inputFile.fail() || !inputFile.is_open() ) {
		std::cerr << "Problem opening file: " << filePath;
		return false;
	}
	
	//extract name from file path
	std::string base_filename = filePath.substr(filePath.find_last_of("/\\") + 1);
	std::string::size_type const p(base_filename.find_last_of('.'));
	std::string file_without_extension = base_filename.substr(0, p);

	dataFile_t tmpFile;
	tmpFile.fileName = file_without_extension;
	tmpFile.sensorData = std::vector<std::vector<int>>(NUM_DATA_FIELDS);

	long currentLine = 0;
	std::string line{};
	int headerRow = -1;
	//start gathering data
	while (inputFile && getline(inputFile, line)) {
		// Tokenize the line and store result in vector. Use range constructor of std::vector
		std::vector<std::string> row{ std::sregex_token_iterator(line.begin(),line.end(),inputDelimiter,-1), std::sregex_token_iterator() };
		//Only add elements of interest if there is the correct number of elements
		if (row.size() == NUMBER_OF_PARAMS) {
			++currentLine;
			//When we come to the header line we can note where it is and skip it
			if (headerRow < 0) {
				headerRow = currentLine;
			}
			else {
				nonDataPoint_t tmpNonPoint;
				try {
					//add data to structure
					tmpNonPoint.timeStamp = row.at(TIMESTAMP);
					tmpNonPoint.timeStamp_ms = stoi(row.at(TIMESTAMP_MS));
					//add numeric data
					for (uint64_t i = NUM_NON_DATA_FIELDS; i < (NUM_DATA_FIELDS+NUM_NON_DATA_FIELDS); ++i) {
						tmpFile.sensorData.at(i-NUM_NON_DATA_FIELDS).push_back(stoi(row.at(i)));
					}
				}
				catch (...) {
					std::cerr << "Data in file: \"" << tmpFile.fileName << "\" is corrupted or improperly formatted " <<
						"at line:" << currentLine << std::endl;
					std::cerr << "Attempting to continue..." << std::endl;
				}
				tmpFile.nonSensorData.push_back(tmpNonPoint);
			}
		}
		
	}
	inputFile.close();

	//add data names pointer to data stats
	std::vector<std::string> dataNames(columnNames.begin() + NUM_NON_DATA_FIELDS, columnNames.end());
	tmpFile.stats.dataNames = dataNames;

	//insert file into collection
	insertFile(tmpFile);

	return true;
}

/**
 * Align data off the first peak
 *
 * This modifies the "timestamp_ms" for each data file so that the data is aligned
 * off the first peak in the X accelerometer data. It first locates the peak with
 * the greatest time to the first peak and offsets all other files with respect to
 * that point.
 *
 * @return true if file is successfully aligned and false if it could not align the files
 */
bool Data::alignDataToPeak(void) {
	if (dataCollection.size() <= 1) {
		return false;
	}
	dataFile_t* referenceFile = &dataCollection.at(0);
	//find data file with the latest peak
	for (auto& dataFile : dataCollection) {
		//we are only interested in aligning the data off accelerometer X
		if (dataFile.nonSensorData.at(dataFile.peakIndex).timeStamp_ms > referenceFile->nonSensorData.at(referenceFile->peakIndex).timeStamp_ms) {
			referenceFile = &dataFile;
		}
	}
	int timeOffset = referenceFile->nonSensorData.at(referenceFile->peakIndex).timeStamp_ms;
	int currentFileTimeOffset;
	//modify data files to align with reference file peak
	for (auto& dataFile : dataCollection) {
		currentFileTimeOffset = timeOffset - dataFile.nonSensorData.at(dataFile.peakIndex).timeStamp_ms;
		for (auto& field : dataFile.nonSensorData) {
			field.timeStamp_ms += currentFileTimeOffset;
		}
	}
	return true;
}