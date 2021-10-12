/*
*	File: DataProcessor.h
*	Author: Scott Laboe
*	Date: 10/10/2021
*/

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <utility>
#include <algorithm>

static const std::string delimiter(", ");

/**
*	Data Processor Class
*
*	This is a static class used to calculate statistics in the stats_t structure.
*
*/
class DataProcessor {
public:
	// data statistics are calculated for every field in the "sensorData" vector
	template <typename T>
	struct stats_t {
		std::vector<T> min;
		std::vector<T> max;
		std::vector<double> mean;
		std::vector<double> goodToCorruptDataRatio;
		std::vector<std::string> dataNames;
		//this will trigger the min/max values to be set to initial conditions
		bool areStatsInitialized = false;
	};
	template <typename T>
	friend std::ostream& operator<<(std::ostream& os, const stats_t<T>& stats) {
		//header
		for (auto name : stats.dataNames) {
			os << delimiter << name;
		}
		os << std::endl;
		//minimum values
		os << "Min";
		for (auto value : stats.min) {
			os << delimiter << value;
		}
		os << std::endl;
		//maximum values
		os << "Max";
		for (auto value : stats.max) {
			os << delimiter << value;
		}
		os << std::endl;
		//mean values
		os << "Mean";
		for (auto value : stats.mean) {
			os << delimiter << value;
		}
		os << std::endl;
		//Good vs Corrupt Data
		os << "Good to Corrupt Data Ratio";
		for (auto value : stats.goodToCorruptDataRatio) {
			os << delimiter << value;
		}
		return os;
	}

	/**
	 *	Calculate Data Statistics
	 *
	 *	This calculates the min and max value of the input array and stores it in the
	 *	stats parameter.
	 *
	 *	@param statsToUpdate is a pointer to the statistics structure
	 *	@param data is a pointer to new data that needs to be processed into the stat structure
	 */
	template <typename T>
	static void calculateStats(stats_t<T> * statsToUpdate, std::vector<std::vector<T>> * data) {
		if (statsToUpdate->areStatsInitialized == false) {
			for (int i = 0; i < data->size(); ++i) {
				statsToUpdate->min.push_back(data->at(i).front());
				statsToUpdate->max.push_back(data->at(i).front());
				statsToUpdate->mean.push_back(data->at(i).front());
			}

			statsToUpdate->areStatsInitialized = true;
		}
		for (int i = 0; i < data->size(); ++i) {
			for (auto& value : data->at(i)) {
				//check minimum
				if (value < statsToUpdate->min.at(i)) {
					statsToUpdate->min.at(i) = value;
				}
				//check maximum
				if (value > statsToUpdate->max.at(i)) {
					statsToUpdate->max.at(i) = value;
				}
			}
			//calculate mean
			statsToUpdate->mean.at(i) = calculateMean(data->at(i).begin(), data->at(i).end());
		}
	}

	/**
	 *	Finds peaks in data
	 *
	 *	Measures dispertion by calculating a moving average every "stride" length and checking if it is 
	 *	"threshold" standard deviations above the previously calculated mean. "Influence" determines 
	 *	how much influence a peak has on the moving average. Once a peak is detected, the local maxima 
	 *	is varified to confirm we have the index at the peak of the signal. 
	 * 
	 *	Derived from: 
	 *	Brakel, J.P.G. van (2014). "Robust peak detection algorithm using z-scores". Stack Overflow. 
	 *	Available at: https://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data/22640362#22640362 (version: 2020-11-08).
	 *
	 * @param input data to be processed
	 * @param lag determines filtering stride
	 * @param threshold set the number of standard deviations above the mean to be counted as a peak
	 * @param influence determines how much effect a peak has on the moving average
	 * @param getNegativePeaks sets whether the algorithm adds negative peaks to the output vector
	 * @return vector with indicies of peaks
	 */
	template <typename T>
	static std::vector<int> findPeaks(const std::vector<T> * input, int lag, float threshold, double influence, bool getNegativePeaks){

		if (input->size() <= lag + 2){
			std::vector<int> empty;
			return empty;
		}

		//Initialize variables
		std::vector<int> output;
		std::vector<double> filteredY(input->size(), 0.0);
		std::vector<double> avgFilter(input->size(), 0.0);
		std::vector<double> stdFilter(input->size(), 0.0);
		avgFilter.at(lag) = calculateMean(input->begin(), input->begin() + lag);
		stdFilter.at(lag) = calculateSD(input->begin(), input->begin() + lag);

		for (int i = lag + 1; i < input->size(); i++)
		{
			if (std::abs(input->at(i) - avgFilter.at(i - 1)) > threshold * stdFilter.at(i - 1))
			{
				int max = i;
				int maxIndex = i - lag;
				if (input->at(i) > avgFilter.at(i - 1)){
					//find local maxima with range of +/- lag about i
					for (; maxIndex < i + lag; ++maxIndex) {
						if (input->at(maxIndex) > max) {
							max = maxIndex;
						}
					}
					//add peak index
					output.push_back(max);
				}
				else if (getNegativePeaks) {
					//find local minima with range of +/- lag about i
					for (; maxIndex < i + lag; ++maxIndex) {
						if (input->at(maxIndex) < max) {
							max = maxIndex;
						}
					}
					//add peak index
					output.push_back(max);
				}
				//Make influence lower
				filteredY.at(i) = (influence * input->at(i) + (1 - influence) * filteredY.at(i - 1));
			}
			else
			{
				filteredY.at(i) = input->at(i);
			}
			//Adjust the filters
			avgFilter[i] = calculateMean(filteredY.begin() + i - lag, filteredY.begin() + i);
			stdFilter[i] = calculateSD(filteredY.begin() + i - lag, filteredY.begin() + i);
		}
		return output;
	}

	/**
	 *	Calculate standard deviation
	 *
	 * @param start iterator for beginning of data to process
	 * @param end iterator for end of data to process
	 * @return calculated standard deviation
	 */
	template <typename Iterator>
	static double calculateSD(Iterator start, Iterator end) {
		double standardDeviation = 0.0;

		double mean = calculateMean(start, end);
		
		for (Iterator it = start; it != end; ++it){
			standardDeviation += pow(*it - mean, 2);
		}

		return sqrt(standardDeviation / std::distance(start, end));
	}

	/**
	 *	Calculate mean
	 *
	 * @param start iterator for beginning of data to process
	 * @param end iterator for end of data to process
	 * @return mean value
	 */
	template <typename Iterator>
	static double calculateMean(Iterator start, Iterator end) {
		if (start != end) {
			return std::accumulate(start, end, 0.0) / std::distance(start, end);
		}
		//if data is only a single element, return element
		return *start;
	}

	/**
	 *	Find anomalous data
	 * 
	 *	Find data peaks then check if they are known signal peaks. If the peak index is within
	 *	"validTolerance" remove that peak from the valid anomalies vector.
	 *
	 * @param input pointer to data to find anomalies
	 * @param knownSignalPeaks vector of valid peaks
	 * @return vector with indicies of anomalies
	 */
	template <typename T>
	static std::vector<int> findAnomolies(std::vector<T> * input, std::vector<int> knownSignalPeaks) {
		//number of indicies outside of known peak to be counted as valid anomaly
		int validTolerance = 3;

		std::vector<int> anomolies = findPeaks(input, 3, 10, 1, true);
		std::vector<int> validAnomolies;

		if (knownSignalPeaks.size()) {
			for (auto& value : anomolies) {
				if (value > knownSignalPeaks.front() && value < knownSignalPeaks.back()) {
					auto it = upper_bound(knownSignalPeaks.begin(), knownSignalPeaks.end(), value);
					if (value < (*it - validTolerance) || value >(*it + validTolerance)) {
						//only indicies outside of knownSignalPeaks are added
						validAnomolies.push_back(value);
					}
				}
			}
		}
		return validAnomolies;
	}
};