#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include <unordered_map>
#include "Veprom.h"

using namespace std;


class Emulator
{
	public:
		Emulator();		// for eproms
		~Emulator();

		void create_Veprom(int size);
		void erase_Veprom();
		void write_to_Veprom(long address, const char* data);
		void read_from_Veprom(long address, int length);
		void write_file_to_Veprom(string filename);
		void read_file_from_Veprom(string filename);
		void list_Files();

		bool load_File(string filename);
		bool isValidFile(string filename);

	private:
		string m_file_loaded;
		string m_filepath;
		char buff[200];
		unordered_map <string ,Veprom*> veprom_list;
};

#endif