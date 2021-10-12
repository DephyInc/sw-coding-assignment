/*
*	File: Data.h
*	Author: Scott Laboe
*	Date: 10/10/2021
*/

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include "DataProcessor.h"

/*
*	Data Class
*
*	Sensor data fields can be easily modifiyed by updating the dataIndicies enumeration.
*	Any change to non-data fields should be added to the dataPoint struct and nonDataIndicies
*	enumeration as well as updated in the Data.cpp logic. Input data should be formatted
*	so that non-data is group together and proceeds sensor data. NUMBER_OF_PARAMS should be updated
*	to reflect the total number of columns in a file.
*
*/
class Data {
public:
	//default constructor
	Data() {};
	struct nonDataPoint_t {
		std::string timeStamp;
		int timeStamp_ms;
	};
	std::vector<std::string> columnNames = {
			"TimeStamp",
			"timeStamp (ms)",
			"accelX",
			"accelY",
			"accelZ",
			"gyroX",
			"gyroY",
			"gyroZ"
	};
	//to index data in nonDataPoint
	enum nonDataIndicies {
		TIMESTAMP = 0,
		TIMESTAMP_MS,
		NUM_NON_DATA_FIELDS
	};
	//to index data in sensorData vector
	enum dataIndicies {
		ACCEL_X,
		ACCEL_Y,
		ACCEL_Z,
		GYRO_X,
		GYRO_Y,
		GYRO_Z,
		NUM_DATA_FIELDS
	};
	struct dataFile_t {
		//data is indexed according to column indicies
		std::vector<std::vector<int>> sensorData;
		std::vector<nonDataPoint_t> nonSensorData;
		std::string fileName;
		int peakIndex;
		friend bool operator<(const dataFile_t& l, const dataFile_t& r)
		{
			return l.sensorData.front().size() < r.sensorData.front().size();
		};
		DataProcessor::stats_t<int> stats;
	};
	std::vector<dataFile_t> dataCollection;

	void insertFile(dataFile_t fileToInsert);
	bool addDataFile(std::string fileName);
	bool alignDataToPeak(void);
	friend std::ostream& operator<<(std::ostream& os, const Data& dt);
private:
	const int NUMBER_OF_PARAMS = 20;
};