//============================================================================
// Name        : vEPROM_cmds.cpp
// Author      : tom
// Version     : 00
//===========================================================================
#include <iostream>
#include <bits/stdc++.h>
#include "vEPROM_cmds.h"

vEPROM_cmds::vEPROM_cmds () {
	cur_vEPROM_p = nullptr;
}
#ifdef GUI_VEPROM
vEPROM_cmds::vEPROM_cmds(QTextEdit *guiOutText) {
    cur_vEPROM_p = nullptr;
    guiOut = guiOutText;
}
#endif

/**
 * @brief Get tokens from given string line by delimiter char (e.g., ' ') and "
 *  e.g, from the line below, tokens = {"veprom", "write_raw", "32", "AB 12"}
 *  veprom write_raw 32 "AB 12"
 *
 * @param line is the given string
 * @param delimiter is the given delimiter char
 * @return tokens is the tokens vector<string> from the given string
 */
void vEPROM_cmds::get_tokens(std::vector<std::string> &tokens, std::string line, char delimiter) {
	unsigned int i = 0;
	unsigned int n = line.size();
	unsigned int j;
	std::string token;

	while (i < n) {
		if (line[i] == delimiter) { // skip delimiter
			i++;
		} else {
			if (line[i] == '"') { // left ", find right "
				i++; j = i;
				while (j < n && line[j] != '"') { // find right "
					j++;
				} // j >= n or line[j] == '"'
				if (line[j] =='"') { // got right "
					token = line.substr(i, j-i); // token has every char inside "
					tokens.push_back(token);
				}
			} else { // find next delimiter
				j = i;
				while (j < n && line[j] != delimiter) { // find next delimiter
					j++;
				} // // j >= n or line[j] == delimiter
				token = line.substr(i, j-i); // token has every char to delimiter/end
				tokens.push_back(token);
			}
			i = j + 1;
		}
	}
}


/**
 * @brief Process and execute "veprom create capacity" command
 *  create a vEPROM object with capacity given in command
 *  display file name for the vEPROM in standard output
 *
 * @param args contains parameters of the command
 */
void vEPROM_cmds::create_cmd(std::vector<std::string> &args) {
	unsigned int n = args.size();
    std::string line;
	if (n != 1) { // number of parameters is not one (veprom create not parameter)
        line = "Wrong vEPROM command (# parameters incorrect)\n";
        out_err(line);
	} else {
		try {
			unsigned int cap = (unsigned int) stoul(args[0])*1024; // get capacity
            if (cap > MAX_CAP || cap <= 0) {
                line = "Capacity is out of range.\n";
                out_err(line);
            } else {

#ifdef GUI_VEPROM
                vEPROM *vEPROM_p = new vEPROM(cap, guiOut); // create a vEPROM object with file name
#else
                vEPROM *vEPROM_p = new vEPROM(cap); // create a vEPROM object with file name
#endif
                if (vEPROM_p) {
                    std::string file_name = vEPROM_p->get_file_name();
                    m_vEPROMs[file_name] = vEPROM_p; //<file name, vEPROM_p> to map
                    // display file name for vEPROM
                    line = file_name + "\n";
                 out_normal(line);
                } else {
                    line = "vEPROM cannot be created, Try again.\n";
                    out_err(line);
                }
            }
		}
		catch (const std::exception &e) {
            line = "Capacity must be a number\n";
            out_err(line);
		}
	}
}

/**
 * @brief Process and execute "veprom load /path/to/veprom/file" command
 *  set the vEPROM with file to be the current vEPROM to be operated
 *
 * @param args contains parameters of the command
 */
void vEPROM_cmds::load_cmd(std::vector<std::string> &args) {
	unsigned int n = args.size();
    std::string line;
	if (n != 1) {
        line = "Wrong vEPROM command (# parameters incorrect)\n";
        out_err(line);
	} else {
		cur_vEPROM_p = m_vEPROMs[args[0]];//point to vEPROM with the file
		if (cur_vEPROM_p == nullptr) {
            line = "Wrong vEPROM command (file incorrect)\n";
            m_vEPROMs.erase(args[0]);
            out_err(line);
		} else {
            line = "current vEPROM is: " + cur_vEPROM_p->get_file_name() + "\n";
            out_normal(line);
		}
	}
}

/**
 * @brief Process and execute "veprom write_raw $ADDRESS $STRING" command
 *  write STRING to the current vEPROM (file) from ADDRESS
 *
 * @param args contains parameters of the command
 */
void vEPROM_cmds::write_raw_cmd(std::vector<std::string> &args) {
	unsigned int n = args.size();
    std::string line;
	if (n != 2) {
        line = "Wrong vEPROM command (# parameters incorrect)\n";
        out_err(line);
	} else {
		try {
			unsigned int address = (unsigned int) stoul(args[0]);// get ADDRESS
  			if (cur_vEPROM_p == nullptr) {
                line = "No vEPROM/file loaded\n";
                out_err(line);
    		} else {
    			cur_vEPROM_p->write_raw(address, args[1]); // write STRING
    		}
		}
		catch (const std::exception &e) {
            line = "Address must be a number\n";
            out_err(line);
		}
	}
}

/**
 * @brief Process and execute "veprom read_raw $ADDRESS $length" command
 *  display length bytes in the current vEPROM (file) from ADDRESS
 *
 * @param args contains parameters of the command
 */
void vEPROM_cmds::read_raw_cmd(std::vector<std::string> &args) {
	unsigned int n = args.size();
    std::string line;
	if (n != 2) {
        line = "Wrong vEPROM command (# parameters incorrect)\n";
        out_err(line);
	} else {
		try {
			unsigned int address = (unsigned int)stoul(args[0]);
			unsigned int length = (unsigned int)stoul(args[1]);
  			if (cur_vEPROM_p == nullptr) {
                line = "No vEPROM/file loaded\n";
                out_err(line);
    		} else {
    			cur_vEPROM_p->read_raw(address, length); // display length bytes
    		}
		}
		catch (const std::exception &e) {
            line = "Address and length must be a number\n";
            out_err(line);
		}
	}
}

/**
 * @brief Process and execute "veprom write /path/to/local/file" command
 *  write/load the given file to the current vEPROM (file)
 *  first file is loaded to start of vEPROM, second file is appended to first one
 *
 * @param args contains parameters of the command
 */
void vEPROM_cmds::write_cmd(std::vector<std::string> &args) {
	unsigned int n = args.size();
    std::string line;
	if (n != 1) {
        line =  "Wrong vEPROM command (# parameters incorrect)\n";
        out_err(line);
	} else {
  		if (cur_vEPROM_p == nullptr) {
            line = "No vEPROM/file loaded\n";
            out_err(line);
    	} else {
    		cur_vEPROM_p->append_file(args[0]); //write/load the given file
    	}
	}
}

/**
 * @brief Process and execute "veprom list" command
 *  list the files for all the vEPROMs created and the files loaded for current vEPROM
 */
void vEPROM_cmds::list_cmd() {
    std::string line;
    for (auto x : m_vEPROMs) { // list the files for all the vEPROMs created
        line = x.first + "\n";
        out_normal(line);
    }
	if (cur_vEPROM_p != nullptr) {
        line =  "Current vEPROM file is: " + cur_vEPROM_p->get_file_name() + "\n";
        out_normal(line);
		cur_vEPROM_p->list();
    } else {
        line = "No vEPROM is loaded/selected as current vEPROM\n";
        out_err(line);
    }
}

/**
 * @brief Process and execute "veprom read $FILE" command
 *  display contents of $FILE in vEPROM to standard output
 *
 * @param args contains parameters of the command
 */
void vEPROM_cmds::read_cmd(std::vector<std::string> &args) {
	unsigned int n = args.size();
    std::string line;
	if (n != 1) {
        line = "Wrong vEPROM command (# parameters incorrect)\n";
        out_err(line);
        return;
    }
    if (cur_vEPROM_p != nullptr) {
        cur_vEPROM_p->read(args[0]); // outputs contents of $FILE in vEPROM to stdout;
    }  else {
        line = "No vEPROM is loaded/selected as current vEPROM\n";
        out_err(line);
	}
}

/**
 * @brief Process and execute "veprom erase" command
 *  set the current vEPROM to initial state
 */
void vEPROM_cmds::erase_cmd() {
	if (cur_vEPROM_p == nullptr) {
        std::string line = "No vEPROM/file loaded\n";
        out_err(line);
	} else {
		cur_vEPROM_p->erase(); // set vEPROM to initial state
	}
}

/**
 * @brief Input, parse and execute every veprom command
 *  exit after quit command
 */
void vEPROM_cmds::process_cmds() {
    std::string line;
	while (true) {
		std::string s;
        line = "Enter a vEPROM command:\n";
        out_normal(line);
		do {
			getline(std::cin, s);
		} while (s.size() == 0);

		std::vector<std::string> tokens;
		char delimiter = ' ';
		get_tokens(tokens, s, delimiter);
    	unsigned int n = tokens.size();

    	if (tokens[0] == "quit") {
    		break;
    	}
    	if (tokens[0] != "veprom" || n < 2) {
            line = "Wrong vEPROM command\n";
            out_err(line);
    		continue;
    	}

    	std::string cmd = tokens[1];
    	tokens.erase(tokens.begin(), tokens.begin()+2); //keep args by removing the first 2 tokens
    	if (cmd == "create") { // veprom create 256
    		create_cmd(tokens);
    	} else if (cmd == "load") { // veprom load /path/to/veprom/file
    		load_cmd(tokens);
    	} else if (cmd == "write_raw") { // veprom write_raw $ADDRESS $STRING
    		write_raw_cmd(tokens);
    	}else if (cmd == "read_raw") { // veprom read_raw $ADDRESS $length
    		read_raw_cmd(tokens);
    	} else if (cmd == "write") { // veprom write /path/to/local/file
    		write_cmd(tokens);
    	} else if (cmd == "list") { // veprom list
    		list_cmd();
    	} else if (cmd == "read") { // veprom read $FILE
    		read_cmd(tokens);
    	} else if (cmd == "erase") { // veprom erase
    		erase_cmd();
    	} else {
            line = "Wrong vEPROM command\n";
            out_err(line);
    	}
	}
}
