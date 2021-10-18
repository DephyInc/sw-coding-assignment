#ifndef CSV_ALIGNER_H
#define CSV_ALIGNER_H

#include <inttypes.h>
#ifdef WIN32
#include <algorithm>
#define CRLF "\n"
#else
#include <bits/stdc++.h>
#define CRLF "\r\n"
#endif

using namespace std;

#define OUTPUT_FILE_NAME "aligned.csv"
#define MAX_FILENAME_LENGTH 1024
#define MAX_LINE_LENGTH 1024
#define MAX_TS_VECTOR_POINTS 10000

//indicates the negative going threshold in y gyroscope data for framing step beginning
#define GYRO_Y_STEP_START_THRESHOLD -1000
//indicates the positive going threshold in y gyroscope data for framing step ending
#define GYRO_Y_STEP_FINISH_THRESHOLD 0
//indicates minimum duration of y gyroscope data below start threshold to be counted as valid step
#define GYRO_Y_STEP_DURATION_MS_THRESHOLD 150
//indicates the minimum threshold for a spike in z accelerometer data after a gyro pattern is found to indicate impact moment
#define ACCEL_Z_IMPACT_THRESSHOLD 10000

#define SUCCESS 0
#define ERROR 1

int median3(int a,int b,int c);

/**
 * Indicates sensor order in struct array
 */
typedef enum
{
	accel_x = 0,
	accel_y,
	accel_z,
	gyro_x,
	gyro_y,
	gyro_z,
}sensor_index_t;

/**
 * Struct for holding sensor data
 */
typedef struct sensor_data_t
{
	int32_t ts_ms;
	int32_t sensor_data[6]; //accelx,accely,accelz,gyrox,gyroy,gyroz
	int32_t filtered_data[6]; //accelx,accely,accelz,gyrox,gyroy,gyroz
}sensor_data_t;

/**
 * Class for forming the output timestamp vector
 */
class time_stamp_aggregator
{
public:
	int32_t ts_ms[MAX_TS_VECTOR_POINTS];
	uint32_t num_points;

	time_stamp_aggregator ();
	void add_point(int32_t point);
	void sort_array ();
};

/**
 * @brief default initializer
 */
time_stamp_aggregator::time_stamp_aggregator ()
{
	//zero out points tracker
	num_points = 0;
}

/**
 * @brief Adds a point to timestamp vector
 * @param point - time to add in ms
 */
void time_stamp_aggregator::add_point(int32_t point)
{
	//verify space
	if(num_points == MAX_TS_VECTOR_POINTS)
	{
		printf("\tError: Ran out of space for timestamp vector. Increase MAX_TS_VECTOR_POINTS\r\n");
		return;
	}

	//add point
	ts_ms[num_points++] = point;
}

/**
 * @brief Sorts and finds unique values in timestamp array
 */
void time_stamp_aggregator::sort_array()
{
	//sort all points numerically
	sort(ts_ms,ts_ms + num_points);
	//find unique points
	int32_t *new_last = unique(ts_ms,ts_ms + num_points);
	//update length based on unique output
	num_points = new_last - ts_ms;
}

/**
 * Class for holding info about a step event
 */
class step
{
	uint32_t rotation_start_index;  //index of negative going rotation
	uint32_t rotation_finish_index; //index of positive going rotation
	uint32_t impact_index;          //index when impact occurred
	int32_t impact_time;            //time ms when impact occurred
	step *next_step;                //pointer to next step in list
public:
	step ();
	~step ();
	int find_next_step_in_data(sensor_data_t *sensor_data, uint32_t num_entries, uint32_t start_index);
	void set_next_step(step *next);
	step *get_next_step();
	uint32_t get_impact_index() {return impact_index;}
	int32_t get_impact_time() {return impact_time;}
};

/**
 * @brief Default constructor
 */
step::step ()
{
	//doesn't point to anything yet
	next_step = NULL;
}

/**
 * @brief Default destructor
 */
step::~step ()
{
	//delete next step in list so full list destroyed at once
	if(next_step)
		delete next_step;
}

/**
 * @brief Sets pointer to next step in list
 * @param next - point to the next step
 */
void step::set_next_step(step *next)
{
	next_step = next;
}

/**
 * @brief returns next step in list
 * @return next step
 */
step *step::get_next_step()
{
	return next_step;
}

//state tracking for step search algorithm
typedef enum
{
	gyro_search_state_inactive,
	gyro_search_state_active,
	gyro_search_state_valid_duration,
	gyro_search_state_peak_search,
}gyro_search_state_t;

/**
 * @brief Finds the next step event in dataset after given index
 * @param sensor_data - data to search through
 * @param num_entries - number of entries in dataset
 * @param start_index - index in dataset which to start searching after
 * @return SUCCESS on finding step event, ERROR otherwise
 */
int step::find_next_step_in_data(sensor_data_t *sensor_data, uint32_t num_entries, uint32_t start_index)
{
	//initialize state
	gyro_search_state_t search_state = gyro_search_state_inactive;

	//loop through all points starting at start_index
	for(uint32_t index=start_index;index<num_entries;index++)
	{
		switch(search_state)
		{
		case gyro_search_state_inactive:
			//look for negative threshold crossing
			if(sensor_data[index].filtered_data[gyro_y] < GYRO_Y_STEP_START_THRESHOLD)
			{
				//record start of pattern and advance to next state
				rotation_start_index = index;
				search_state = gyro_search_state_active;
			}
			break;
		case gyro_search_state_active:
			//look for gyro data leaving negative threshold
			if(sensor_data[index].filtered_data[gyro_y] >= GYRO_Y_STEP_START_THRESHOLD)
			{
				//make sure duration was long enough (filters out noise)
				uint32_t duration = sensor_data[index].ts_ms - sensor_data[rotation_start_index].ts_ms;
				if(duration < GYRO_Y_STEP_DURATION_MS_THRESHOLD)
					search_state = gyro_search_state_inactive;
				else
					search_state = gyro_search_state_valid_duration;
			}
			break;
		case gyro_search_state_valid_duration:
			//look for gyro data to cross positive threshold
			if(sensor_data[index].filtered_data[gyro_y] > GYRO_Y_STEP_FINISH_THRESHOLD)
			{
				//record index and advance to next state
				rotation_finish_index = index;
				search_state = gyro_search_state_peak_search;
			}
			break;
		case gyro_search_state_peak_search:
			//look for start of impulse in accelerometer data
			if(sensor_data[index].sensor_data[accel_z] >= ACCEL_Z_IMPACT_THRESSHOLD)
			{
				//record impact and return success
				impact_index = index;
				impact_time = sensor_data[index].ts_ms;
				return SUCCESS;
			}
			break;
		}
	}

	//reached end of file without finding step event
	return ERROR;
}

/**
 * Class to hold data for each input file
 */
class sensor_file
{
	char filename[MAX_FILENAME_LENGTH]; //path to csv file
	sensor_data_t *sensor_data;         //array of sensor data
	uint32_t num_entries;               //length of sensor data
	step *first_step;                   //pointer to first step event in data
	step *last_step;                    //pointer to last step event in data
	uint32_t num_steps;                 //number of steps found in dataset
	static uint32_t num_files;          //static var so each dataset gets a new number
	uint32_t file_number;               //unique number per dataset
	uint32_t output_search_index;       //used for tracking where we left off when writing data to output file
	step *output_search_step;           //used for tracking the step file where we left off when writing data to output file
public:
	sensor_file ();
	~sensor_file ();
	int parse_file(const char *file_name);
	int filter_data ();
	int add_step ();
	uint32_t get_first_step_time() {return first_step ? first_step->get_impact_time() : 0;}
	uint32_t get_last_step_time() {return last_step ? last_step->get_impact_time() : 0;}
	void set_timestamp_offset(int32_t offset);
	void add_times_to_global_vector(time_stamp_aggregator *time_vector);
	void print_header(FILE *fp);
	void print_data_line(int ts,FILE *fp);
};

//initialize the static num_files
uint32_t sensor_file::num_files = 0;

/**
 * @brief Default constructor
 */
sensor_file::sensor_file ()
{
	sensor_data = NULL;
	num_entries = 0;
	first_step = NULL;
	last_step = NULL;
	num_steps = 0;
	file_number = num_files++;
	output_search_index = 0;
	output_search_step = NULL;
}

/**
 * @brief Default destructor
 */
sensor_file::~sensor_file ()
{
	//delete the sensor struct array
	if(sensor_data)
		delete sensor_data;

	//delete the first step in array which in turn will delete the full array
	if(first_step)
		delete first_step;
}

/**
 * @brief Parsed data from the csv file
 * @param file_name - path to csv file
 * @return SUCCESS on valid parsing, ERROR otherwise
 */
int sensor_file::parse_file(const char *file_name)
{
	//holds ascii data for each parsed line
	char str_line[MAX_LINE_LENGTH];

	//save filename
	strncpy(filename,file_name,sizeof(filename));
	//make sure filename was null terminated in case of overflow in strncpy
	filename[MAX_FILENAME_LENGTH-1] = '\0';

	//attempt to open file
	FILE *fp = fopen(filename,"r");
	if(!fp)
	{
		printf("\tError: Could not open file: %s\r\n",filename);
		return ERROR;
	}

	//quickly loop through each line to determine number of entries
	num_entries = 0;
	while(fgets(str_line,MAX_LINE_LENGTH,fp))
	{
		//ignore lines with only newline characters (at end of file)
		if(strlen(str_line) > 2)
			num_entries++;
	}

	//the first 2 lines are headers. Verify there is sufficient data and subtract 2 from num_entries
	if(num_entries <= 2)
	{
		fclose(fp);
		return ERROR;
	}
	num_entries -= 2;

	printf("\tNum entries: %u\r\n",num_entries);

	//allocate struct for sensor data
	sensor_data = new sensor_data_t[num_entries];
	if(!sensor_data)
	{
		printf("\tError: allocate space for sensor data failed\r\n");
		fclose(fp);
		return ERROR;
	}

	//reset to beginning of file and skip over 2 header rows
	fseek(fp, 0, SEEK_SET);
	fgets(str_line,MAX_LINE_LENGTH,fp);
	fgets(str_line,MAX_LINE_LENGTH,fp);

	//loop through each entry in file
	for(uint32_t cnt=0; cnt<num_entries; cnt++)
	{
		//read line
		if(!fgets(str_line,MAX_LINE_LENGTH,fp))
		{
			free(sensor_data);
			fclose(fp);
			return ERROR;
		}

		//parse line
		char *sub_string;
		//date
		sub_string = strtok(str_line,",");
		//timestamp (ms)
		sub_string = strtok(NULL,",");
		if(!sub_string)
		{
			free(sensor_data);
			fclose(fp);
			return ERROR;
		}
		sensor_data[cnt].ts_ms = atoi(sub_string);
		//sensor data
		for(uint8_t sensor_index=0; sensor_index<6; sensor_index++)
		{
			sub_string = strtok(NULL,",");
			if(!sub_string)
			{
				free(sensor_data);
				fclose(fp);
				return ERROR;
			}
			sensor_data[cnt].sensor_data[sensor_index] = atoi(sub_string);
		}
	}

	fclose(fp);

	return SUCCESS;
}

/**
 * @brief Creates a copy of data that is median filtered
 * @return Always returns SUCCESS
 */
int sensor_file::filter_data()
{
	uint8_t offset = 1; //for 3 point median
	//todo: update to allow flexible filter type

	//loop through each entry in dataset
	for(uint32_t cnt=0;cnt<num_entries;cnt++)
	{
		//loop through all axes
		for(uint32_t axis=0;axis<6;axis++)
		{
			//if too close to edge, just use the original value, else perform median filter
			if(cnt<offset || cnt>=num_entries-offset)
				sensor_data[cnt].filtered_data[axis] = sensor_data[cnt].sensor_data[axis];
			else
				sensor_data[cnt].filtered_data[axis] = median3(sensor_data[cnt-1].sensor_data[axis],sensor_data[cnt].sensor_data[axis],sensor_data[cnt+1].sensor_data[axis]);
		}
	}

	return SUCCESS;
}

/**
 * @brief Finds are stores the next step event in the file
 * @return SUCCESS if step event was found, ERROR otherwise
 */
int sensor_file::add_step()
{
	//init a step
	step *curr_step = new step();
	int result;

	//find the next step in the dataset starting after the last step if it exists
	if(last_step)
		result = curr_step->find_next_step_in_data(sensor_data,num_entries,last_step->get_impact_index());
	else
		result = curr_step->find_next_step_in_data(sensor_data,num_entries,0);

	//check if step was found
	if(result == SUCCESS)
	{
		//on success, add the step to array by updating first/last step
		if(!first_step)
		{
			first_step = curr_step;
			last_step = curr_step;
		}
		else
		{
			last_step->set_next_step(curr_step);
			last_step = curr_step;
		}
		//increment step count
		num_steps++;
	}
	else
	{
		//delete the step if none was found
		delete curr_step;
	}

	//return if step was found
	return result;
}

/**
 * @brief Applies offset to all timestamps in file to zero first step event
 * @param offset - time in ms to subtract
 */
void sensor_file::set_timestamp_offset(int32_t offset)
{
	for(uint32_t cnt=0;cnt<num_entries;cnt++)
		sensor_data[cnt].ts_ms -= offset;
}

/**
 * @brief Adds all of the timestamps from a file to the global vector
 * @param time_vector - pointer to the global vector
 */
void sensor_file::add_times_to_global_vector(time_stamp_aggregator *time_vector)
{
	for(uint32_t cnt=0;cnt<num_entries;cnt++)
		time_vector->add_point(sensor_data[cnt].ts_ms);
}

/**
 * @brief Prints header about file to output file
 * @param fp - pointer to output file
 */
void sensor_file::print_header(FILE *fp)
{
	fprintf(fp,"%02u,%s,%u,%u" CRLF,file_number,filename,num_entries,num_steps);
}

/**
 * @brief Prints 1 row of dataset to output file for the given input time
 * @param ts - time in ms
 * @param fp - pointer to output file
 */
void sensor_file::print_data_line(int ts,FILE *fp)
{
	//initialize the search times
	int32_t search_time = sensor_data[output_search_index].ts_ms;;
	int32_t search_time_next = (output_search_index + 1) < num_entries ? sensor_data[output_search_index+1].ts_ms : search_time;

	//initialize the search step
	if(!output_search_step)
		output_search_step = first_step;

	//update search_index/time until the input ts is within range
	while(ts >= search_time_next && search_time != search_time_next)
	{
		output_search_index++;
		search_time = sensor_data[output_search_index].ts_ms;
		search_time_next = (output_search_index + 1) < num_entries ? sensor_data[output_search_index+1].ts_ms : search_time;
	}

	//check if ts is outside of data range
	if(ts < search_time || ts > search_time_next)
	{
		//just print all zeros for sensor data and flag as interpolated
		fprintf(fp,"0,0,0,0,0,0,0,1,0,");
	}
	//check for matching time
	else if(ts == search_time)
	{
		//check if this time matches a step event
		uint8_t is_step = 0;
		if(output_search_step)
		{
			if(ts == sensor_data[output_search_step->get_impact_index()].ts_ms)
			{
				is_step = 1;
				output_search_step = output_search_step->get_next_step();
			}
		}
		//print dataset for this timestamp
		fprintf(fp,"%i,%i,%i,%i,%i,%i,%u,0,0,",
				sensor_data[output_search_index].sensor_data[accel_x],
				sensor_data[output_search_index].sensor_data[accel_y],
				sensor_data[output_search_index].sensor_data[accel_z],
				sensor_data[output_search_index].sensor_data[gyro_x],
				sensor_data[output_search_index].sensor_data[gyro_y],
				sensor_data[output_search_index].sensor_data[gyro_z],
				is_step
		);
	}
	//time is in between two datapoints
	else
	{
		//perform interpolation and print data
		float fraction = (float)(ts - search_time) / (float)(search_time_next - search_time);
		fprintf(fp,"%i,%i,%i,%i,%i,%i,0,1,0,",
				(int32_t)(sensor_data[output_search_index].sensor_data[accel_x] + fraction * (sensor_data[output_search_index+1].sensor_data[accel_x] - sensor_data[output_search_index].sensor_data[accel_x])),
				(int32_t)(sensor_data[output_search_index].sensor_data[accel_y] + fraction * (sensor_data[output_search_index+1].sensor_data[accel_y] - sensor_data[output_search_index].sensor_data[accel_y])),
				(int32_t)(sensor_data[output_search_index].sensor_data[accel_z] + fraction * (sensor_data[output_search_index+1].sensor_data[accel_z] - sensor_data[output_search_index].sensor_data[accel_z])),
				(int32_t)(sensor_data[output_search_index].sensor_data[gyro_x] + fraction * (sensor_data[output_search_index+1].sensor_data[gyro_x] - sensor_data[output_search_index].sensor_data[gyro_x])),
				(int32_t)(sensor_data[output_search_index].sensor_data[gyro_y] + fraction * (sensor_data[output_search_index+1].sensor_data[gyro_y] - sensor_data[output_search_index].sensor_data[gyro_y])),
				(int32_t)(sensor_data[output_search_index].sensor_data[gyro_z] + fraction * (sensor_data[output_search_index+1].sensor_data[gyro_z] - sensor_data[output_search_index].sensor_data[gyro_z]))
		);
	}
}

#endif //#ifndef CSV_ALIGNER_H
