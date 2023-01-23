#ifndef VEPROM_H
#define VEPROM_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

struct textfile {
	string filename;
	long   starting_address;
	long   length;
};

class Veprom
{
	public:
		Veprom(int size, string filename, string file_location);

		void write(long address, const char* data);					// writes raw data
		void read(long address, int length);					// read raw data
		void list();						// lists files
		void erase();						//erase

		void write_file(string filename);					// writes file
		void read_file(string filename);					// read file to veprom
		bool open_file();
		void close_file();

	private:
		vector<textfile> files_stored;			// name of file and address
		long m_size;
		string m_filename;
		string m_file_location;
		ofstream m_text_in;
		ifstream m_text_out;

		void read_chunks(long address, long length);
};

#endif