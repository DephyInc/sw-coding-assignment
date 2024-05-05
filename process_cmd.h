#include <vector>
#include <string>

using namespace std;

int process_create(vector<string> args); 

int process_load(vector<string> args); 

int process_write_raw(vector<string> args); 

int process_read_raw(vector<string> args); 

int process_write(vector<string> args); 

int process_list(vector<string> args); 

int process_read(vector<string> args);

int process_erase(vector<string> args);

int process_help();
