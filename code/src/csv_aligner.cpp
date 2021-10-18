#include <stdio.h>
#include <inttypes.h>
#include <cstring>
#include <cstdlib>

#include "csv_aligner.h"

/***
 * @brief main
 * @param argc
 * @param argv
 * @return 0 on success, 1 if error
 */
int main(int argc, char **argv)
{
	//check for valid input arguments
	if(argc < 2)
	{
		printf("Invalid input arguments. Must include 1 or more input files\r\nUsage: csv_aligner /PATH/TO/FILE1 /PATH/TO/FILE2 /PATH/TO/FILE3 ...\r\n");
		return ERROR;
	}

	//create global timestamp vector
	time_stamp_aggregator global_time;

	//create array of sensor_file objects
	sensor_file *sensor_file_arr = new sensor_file[argc-1];
	if(!sensor_file_arr)
		return ERROR;

	//loop through each filename argument
	int number_files = argc - 1;
	for(int filenum = 0; filenum < number_files; filenum++)
	{
		printf("File %u of %u: %s\r\n",filenum+1,number_files,argv[filenum+1]);

		//read csv file
		if(sensor_file_arr[filenum].parse_file(argv[filenum+1]))
		{
			printf("\tError: Failed to parse file\r\n");
			return ERROR;
		}

		//apply median filter to data
		if(sensor_file_arr[filenum].filter_data())
		{
			printf("\tError: Failed to filter data\r\n");
			return ERROR;
		}

		//find all steps in sensor data
		uint32_t num_steps = 0;
		while(!sensor_file_arr[filenum].add_step())
		{
			printf("\tFound step at t = %ums\r\n",sensor_file_arr[filenum].get_last_step_time());
			num_steps++;
		}
		printf("\tFound %u step events\r\n",num_steps);
		if(sensor_file_arr[filenum].get_first_step_time() == 0)
		{
			printf("\tError: Did not find any steps in data\r\n");
			return ERROR;
		}

		//subtract offset of first step from timestamp vector
		sensor_file_arr[filenum].set_timestamp_offset(sensor_file_arr[filenum].get_first_step_time());

		//add times to global vector
		sensor_file_arr[filenum].add_times_to_global_vector(&global_time);
	}

	//sort global time
	global_time.sort_array();

	//open new output file
	FILE *fp = fopen(OUTPUT_FILE_NAME,"w");
	if(!fp)
	{
		printf("Error: Failed to open output file: %s\r\n",OUTPUT_FILE_NAME);
		return ERROR;
	}

	//print header
	fprintf(fp,"File Number,File Path,Number of rows in file,Number of steps in file" CRLF);

	//print filenames to file
	for(int cnt=0;cnt<number_files;cnt++)
		sensor_file_arr[cnt].print_header(fp);

	//print file header
	fprintf(fp,"Timestamp (ms),");
	for(int cnt=0;cnt<number_files;cnt++)
		fprintf(fp,"%02u accel.x,%02u accel.y,%02u accel.z,%02u gyro.x,%02u gyro.y,%02u gyro.z,%02u is_step_impulse,%02u interpolated,%02u noisy,",cnt,cnt,cnt,cnt,cnt,cnt,cnt,cnt,cnt);
	fprintf(fp,CRLF);

	//loop through all global time points
	for(uint32_t point = 0;point < global_time.num_points;point++)
	{
		fprintf(fp,"%i,",global_time.ts_ms[point]);
		//add each files data points
		for(int cnt=0;cnt<number_files;cnt++)
			sensor_file_arr[cnt].print_data_line(global_time.ts_ms[point],fp);
		fprintf(fp,CRLF);
	}

	//close output file
	fclose(fp);

	//delete sensor dataset
	delete [] sensor_file_arr;

	printf("Finished: Aligned data written to %s\r\n",OUTPUT_FILE_NAME);

	return SUCCESS;
}

/**
 * @brief Calculates median of 3 integers
 * @param a - first integer
 * @param b - second integer
 * @param c - third integer
 * @return median value
 * @note Taken from stack overflow
 */
int median3(int a,int b,int c)
{
    if ((a <= b) && (b <= c)) return b;  // a b c
    if ((a <= c) && (c <= b)) return c;  // a c b
    if ((b <= a) && (a <= c)) return a;  // b a c
    if ((b <= c) && (c <= a)) return c;  // b c a
    if ((c <= a) && (a <= b)) return a;  // c a b
    return b;                            // c b a
}
