#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <utility>
#include <numeric>

using namespace std;

/* 
 string Timestamp: Day Month Date hr:min:sec Year 
    int Timestamp_ms: timestamp of each data recording
    int accel_x: X Axis Accelerometer Data
    int accel_y: Y Axis Accelerometer Data
    int accel_z: Z Axis Accelerometer Data
    int gyro_x: X Axis Gyroscope Data
    int gyro_y: Y Axis Gyroscope Data
    int gyro_z: Z Axis Gyroscope Data
*/
class Results
{
    public: //0-7 columns of dataset
    string Timestamp;
    int Timestamp_ms;
    int accel_x;
    int accel_y;
    int accel_z;
    int gyro_x;
    int gyro_y;
    int gyro_z;
};

/*
    string Header: Header Info
    string Col_header: Read column headers from file
    Results record: See <Results>
*/
class File_Items
{
    public:
    string Header;
    string Col_header; // TODO:make vector for write_csv() column headers.
    Results record;
};

vector<Results> read_csv(string filename);
void write_csv(string csvname, vector<string> filename, vector<vector<Results>> vals, int num_files, vector<int> peak_index);
int find_peak(vector<Results> input);

// Output .csv file 
string result_file = "csv_aligned_output.csv";

int main(int argc, char* argv[])
{
    if(argc > 1)
    { 
        vector<vector<Results>> full_result;
        vector<string> full_files_name;
        vector<int> file_peak_index;
        for(int i = 1; i < argc; i++)
        {
            string path = argv[i];
            string file_name = path.substr(path.find_last_of("/\\") +1);
            full_files_name.push_back(file_name);
            cout << file_name << endl;
            vector<Results> result1 = read_csv(path.c_str());
            file_peak_index.push_back(find_peak(result1));
            full_result.push_back(result1);
        }
        write_csv(result_file, full_files_name, full_result, argc, file_peak_index);
    }
}

vector<Results> read_csv(string filename)
{
    File_Items full_record;
    vector<Results> finalrecord;

    vector<pair<string, vector<int>>> result;
    ifstream myFile(filename);

    if(!myFile.is_open())
    {
        cout <<" " << "not open" << endl;
    }
    
    string line, colname;

    // read file header row
    getline(myFile, line);
    stringstream ss(line);
    getline(ss, full_record.Header, ',');

    // read data headers 
    string token;
    string token_s; // for strings
    getline(myFile, line);
    stringstream ss1(line);
    while(getline(ss1, colname, ','))
    {
        result.push_back({colname, vector<int> {}});
    }

    // reading results columns 0-7
    while(getline(myFile, line)) // TODO: Add in check for data holes/empty space.
    {
        istringstream iss(line);

        getline(iss, token_s, ','); 
        full_record.record.Timestamp = token_s;

        getline(iss, token, ',');
        full_record.record.Timestamp_ms = stoi(token);
        
        getline(iss, token, ',');
        full_record.record.accel_x = stoi(token);

        getline(iss, token, ',');
        full_record.record.accel_y = stoi(token); 
            
        getline(iss, token, ',');
        full_record.record.accel_z = stoi(token);

        getline(iss, token, ',');
        full_record.record.gyro_x = stoi(token); 
            
        getline(iss, token, ',');
        full_record.record.gyro_y = stoi(token); 

        getline(iss, token, ',');
        full_record.record.gyro_z = stoi(token);

        finalrecord.push_back(full_record.record);
    }
    
    myFile.close();
    return finalrecord;
}


// TODO: Use peak_index to organize data based on each file's first peak index.
void write_csv(string csvname, vector<string> filename, vector<vector<Results>> vals, int num_files, vector<int> peak_index)
{
    int max_size = 0;
    ofstream myFile;
    myFile.open(csvname);
    if(!myFile.is_open())
    {
        cout <<" " << "not open" << endl;
    }

    // Writing Headers
    for(int i = 0; i < filename.size(); i++)
    {
        string file_n = filename[i];
        string file = file_n.substr(0,(file_n.find('.')));
        string col1 = file + " Timestamp"; // TODO: Add in struct for result_file column headers.
        string col2 = file + " Timestamp_ms";
        string col3 = file + " accel.x";
        string col4 = file + " accel.y";
        string col5 = file + " accel.z";
        string col6 = file + " gyro.x";
        string col7 = file + " gyro.y";
        string col8 = file + " gyro.z";

        myFile << col1 << ",";
        myFile << col2 << ",";
        myFile << col3 << ",";
        myFile << col4 << ",";
        myFile << col5 << ",";
        myFile << col6 << ",";
        myFile << col7 << ",";
        myFile << col8 << ",";
    }
    myFile << "\n";

    for(int j = 0; j < vals.size(); j++)
    {
        if(vals[j].size() > max_size)
        {
            max_size = vals[j].size(); // max_size is the maximum data set length of all files
        }
    }
    for(int i = 0; i < max_size - 1; i++)
    {
        for(int j = 0; j < vals.size(); j++)
        {
            if((vals[j].size() - 1) < i)
            {
                myFile << " " << ",";
                myFile << " " << ",";
                myFile << " " << ",";
                myFile << " " << ",";
                myFile << " " << ",";
                myFile << " " << ",";
                myFile << " " << ",";
                myFile << " " << ","; // replaces any remaining empty spaces
            }
            else
            {
                myFile << vals[j][i].Timestamp << ","; 
                myFile << vals[j][i].Timestamp_ms << ",";
                myFile << vals[j][i].accel_x << ",";
                myFile << vals[j][i].accel_y << ",";
                myFile << vals[j][i].accel_z << ",";
                myFile << vals[j][i].gyro_x << ",";
                myFile << vals[j][i].gyro_y << ",";
                myFile << vals[j][i].gyro_z << ",";
            }
        }
        myFile << "\n";
    }
    myFile.close();
}

int find_peak(vector<Results> input)
{
    int index = 0;
    int max_value[input.size()];
    int max_val = 0;
    float avg = 0;
    int j = 0;
    for(int i = 0 ; i < input.size(); i++)
    {
        avg += input[i].accel_z; // TODO: Choosing accel_z for now as peak.  Will need to add in comparison between axis of gyro, and accel.
    }
    avg /= input.size();
    max_value[0] = input[0].accel_x;
    for(int i = 0; i < 50; i++) // looking at the first 50 entries for a peak.
    {            
        if((input[i].accel_z > max_val) && (input[i].accel_z > avg)) // TODO: look into a threshold to find first peak. 
        {
            index = i;
            max_val= input[i].accel_z;
        }
    }
    cout << "maximum value:" << max_val << endl;
    cout << "accel_z average:" << avg << endl;
    cout << "data length:" << input.size() << endl;
    return index;
}