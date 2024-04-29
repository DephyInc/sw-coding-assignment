#include <iostream>
#include <string>
#include <cstring>
#include "veprom.h"

using namespace std;

extern VEprom veprom;

static void usage (const char* name) {
    std::cerr << "Usage:\n"
              << "\t" << name << " [-v] help (also -h or --help or command is absent)\n"
              << "\t" << name << " [-v] create <size-in-KB>\n"
              << "\t" << name << " [-v] load <eprom-file-name>\n"
	      << "\t" << name << " [-v] write_raw <address> <string>\n"
              << "\t" << name << " [-v] read_raw <address> <length-in-bytes>\n"
              << "\t" << name << " [-v] write <local-file>\n"
              << "\t" << name << " [-v] list\n"
              << "\t" << name << " [-v] read <file-name-in-eprom>\n"
              << "\t" << name << " [-v] erase\n"
	      << "\t\t" << "where optional -v turns verbosity on\n"
              << std::endl;
    std::cerr << "Note:\n"
	      << "\twrite_raw doesn't add terminating null\n"
	      << "\twrite_raw is not allowed to overwrite content, nor names of files already written\n"
	      << "\tAs files are added, content altered with write_raw may be overwritten\n"
	      << "\tA file name with the same name as a file name already written is appended with a version number \"(1)\", etc.\n"
	      << std::endl;
}

static bool check_num (const char* arg, int* ptr_num) {
    std::size_t pos = 0 ;
    int N = -1;
    try {
        N = stoi(arg, &pos, 10) ;
        if (N == -1 || pos != strlen(arg)) {
            return false;
	}
    } catch(...) {
	return false;
    }

    *ptr_num = N;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage(argv[0]);
        return 0; // no error
    }

    string cmd = argv[1];
    int idx_param = 2;

    // set verbose option
    if (cmd == "-v") {
        veprom.Verbose();
	if (argc > 2) {
            cmd = argv[2];
            --argc;
	    idx_param = 3;
	}
    }

    if ((cmd == "-h") || (cmd == "--help") || (cmd == "help")) {
	if (argc > 2) {
	    cerr << argv[2] <<": unexpected parameter\n";
	    return 1; // syntax error
	}
        usage(argv[0]);
        return 0; // no error
    }

    if (argc < 2) {
        usage(argv[0]);
        return 2; // syntax error
    }
    
    // execute commands
    if (cmd == "create") {
        // initialize the eprom by size
        if (argc < 3) {
            cerr << cmd <<": integer parameter is needed\n";
            return 3; // syntax error
	}
        if (argc > 3) {
            cerr << argv[idx_param + 1] <<": invalid parameter\n";
            return 4; // syntax error
        }

	int num = 0;
        if (!check_num(argv[idx_param], &num)) {
            cerr << argv[idx_param] << ": integer parameter is expected\n";
            return 5; // syntax error
        }
	if (num <= 0) {
            cerr << num << ": positive integer parameter is expected\n";
            return 6; // syntax error
        }
	// execute
	int rcode = veprom.Create(num);
        if (rcode) {
	    // report internal error
            cerr << "error code " << rcode << " from Create\n";
            return 7; // syntax error
	}
    } else if (cmd == "load") {
        // initialize the eprom from file
        if (argc < 3) {
            cerr << cmd <<": file name is needed\n";
            return 8; // syntax error
        }
        if (argc > 3) {
            cerr << argv[idx_param + 1] <<": invalid parameter\n";
            return 9; // syntax error
        }

        // execute
        int rcode = veprom.Load(argv[idx_param]);
        if (rcode) {
            // report internal error
            cerr << "error code " << rcode << " from Load\n";
            return 10; // syntax error
        }
	    
    } else if (cmd == "write_raw") {
        // write data at address
        if (argc < 4) {
            cerr << cmd <<": integer and data parameters are needed\n";
            return 11; // syntax error
        }
        if (argc > 4) {
            cerr << argv[idx_param + 2] <<": invalid parameter\n";
            return 12; // syntax error
        }

        int addr = 0;
        if (!check_num(argv[idx_param], &addr)) {
            cerr << argv[idx_param] << ": integer address is expected\n";
            return 13; // syntax error
        }
        if (addr < 0) {
            cerr << addr << ": nonnegative integer address is expected\n";
            return 14; // syntax error
        }
        // execute
        int rcode = veprom.WriteRaw(addr, argv[idx_param + 1] );
        if (rcode) {
            // report internal error
            cerr << "error code " << rcode << " from WriteRaw\n";
            return 16; // syntax error
        }
    } else if (cmd == "read_raw") {
        // read data at address of given length
        if (argc < 4) {
            cerr << cmd <<": integer and data parameters are needed\n";
            return 17; // syntax error
        }
        if (argc > 4) {
            cerr << argv[idx_param + 2] <<": invalid parameter\n";
            return 18; // syntax error
        }

        int addr = 0;
        if (!check_num(argv[idx_param], &addr)) {
            cerr << argv[idx_param] << ": integer address is expected\n";
            return 19; // syntax error
        }
        if (addr < 0) {
            cerr << addr << ": nonnegative integer address is expected\n";
            return 20; // syntax error
        }
        int len = 0;
        if (!check_num(argv[idx_param + 1], &len)) {
            cerr << argv[idx_param] << ": integer length is expected\n";
            return 22; // syntax error
        }
        if (len <= 0) {
            cerr << addr << ": positive integer length is expected\n";
            return 23; // syntax error
        }
	
        // execute
        int rcode = veprom.ReadRaw(addr, len);
        if (rcode) {
            // report internal error
            cerr << "error code " << rcode << " from ReadRaw\n";
            return 24; // syntax error
        }
	    
    } else if (cmd == "write") {
        // write data from file
        if (argc < 3) {
            cerr << cmd <<": file name is needed\n";
            return 25; // syntax error
        }
        if (argc > 3) {
            cerr << argv[idx_param + 1] <<": invalid parameter\n";
            return 26; // syntax error
        }
        // execute
        int rcode = veprom.Write(argv[idx_param]);
        if (rcode) {
            // report internal error
            cerr << "error code " << rcode << " from Write\n";
            return 27; // syntax error
        }    
    } else if (cmd == "list") {
        // list files
        if (argc > 2) {
            cerr << argv[idx_param] <<": invalid parameter\n";
            return 28; // syntax error
        }
        // execute
        int rcode = veprom.List(true, false);
        if (rcode) {
            // report internal error
            cerr << "error code " << rcode << " from List\n";
            return 29; // syntax error
        }
    } else if (cmd == "read") {
        // read data from file
        if (argc < 3) {
            cerr << cmd <<": file name is needed\n";
            return 30; // syntax error
        }
        if (argc > 3) {
            cerr << argv[idx_param + 1] <<": invalid parameter\n";
            return 31; // syntax error
        }
        // execute
        int rcode = veprom.Read(argv[idx_param]);
        if (rcode) {
            // report internal error
            cerr << "error code " << rcode << " from Read\n";
            return 32; // syntax error
        }

    } else if (cmd == "erase") {
        // list files
        if (argc > 2) {
            cerr << argv[idx_param] <<": invalid parameter\n";
            return 33; // syntax error
        }
        // execute
        int rcode = veprom.Erase();
        if (rcode) {
            // report internal error
            cerr << "error code " << rcode << " from Erase\n";
            return 34; // syntax error
        }	    
    } else if (cmd == "-v") {
        cerr << cmd <<": command is missing after verbose option\n";
        return 35;
    } else {
	cerr << cmd <<": invalid command\n";
	return 36;
    }

    return 0;
}
