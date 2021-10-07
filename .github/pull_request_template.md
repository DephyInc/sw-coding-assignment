# Summary

*   My program starts by requesting the user to input the files they wish to align. With this information, 
    it takes in the files and reads them line by line, sending the data we want to keep to an object that
    stores it for use later. With this data we can generate the helpful stats which is also stored in an 
    object of its own. To align the files I intended to use the Z axis of the Accelerator data due to the 
    large peaks it has from the movement of the foot stepping and raising. It is supposed to use the vector
    storing each file's Accel Z data and go through it, outputting a -1, 0, or 1 at each index, indicating a 
    drop in the average line, consistent average line, or a spike. Using the 1's gathered during this process
    I would compare the files to find the file with the furthest first peak in the data, we can call this 
    furthestPeak, and buffer the other files to align with this one. I can align the files by subtracting 
    the index of the other file's first peaks from furthestPeak, and buffering the data by x amount. 
    Once all the data has been adjusted accordingly and calculated, I export the data to two files, one called
    walk_data_aligned.csv, and the second called walk_data_helpfulstats.csv

# Test Plan

* I have provided a compiled file to run from command line, however my instructions include directions
  on how to compile the file incase you would like to do that first. If you would like to just run the file
  without compiling it first, skip step 2.

1. With the coding_assignment-main folder downloaded, open command line and direct to the folder .
    On windows and Linux this can be done by typing "cd", space, and then the path (it can also be pasted).
2. Once in the folder (under the assumption the g++ compiler is installed on the system you are using)
    type the following: g++ csv_aligner.cpp -o csv_aligner.exe
3. When ready to run the file, type the following: csv_aligner.exe
4. Enter the file paths for the files you would like to run the program on, then hit enter.
    I have been using the following during my coding tests: ./data/walk_data_1.csv ./data/walk_data_2.csv
5. Enter the path to the folder you would like the files to be output in, then hit enter.
6. Open the folder you output the files into to open them.


# Expected Results

*   I was not able to complete the alignment of the data as my method of finding the peaks and figuring out their
    indexes needs more work, however I have provided all of the code I intend to use and commented as best as I could 
    within the file to explain how everything works. 

* The results expected are two files: walk_data_aligned.csv, and walk_data_helpfulstats.csv

*   In walk_data_aligned.csv the first line should have the file info, these are the paths of the files used in reference 
    to a number listed on each column that contains data that originates from the respective file. On the second line are
    the headers for each column of data, this tells you what each column contains and from which file the data is from.
    The in each column below the headers is the data from columns 0-7 of the input files

*   In walk_data_helpfulstats.csv the first line is similar to the first file's layout, there are numbers with paths associated
    which specify which piece of data comes from which file. On the second line is the header for the file, this explains what 
    is in each column, the origin of the data, number of data points, max, min, average, good vs corrupted. 

*   Good Vs Corrupted is filled with 0's as I was not able to calculate the percentage of good to corrupted data, or figure out 
    how many corrupted data points there were in the file. This data would have been done through the function that could find the
    peaks as I could narrow the precision and figure out where a spike or drop, that isnt intended would be.

* Feel free to include screenshots and attach binaries.
*   I have attached the files I generated when running the program using the provided walk_data_1 and walk_data_2 files. 

# Requirements
1. [x] Written in C++
2. [x] Uses at least two classes and have them interact together
3. [x] Runs on Linux, and Windows or MacOS
4. [x] Runs fast
5. [x] Outputs the correct data
