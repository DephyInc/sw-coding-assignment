#include "Emulator.h"
#include<string>
#include <iostream>
#include <direct.h>
#include<limits.h>

Emulator::Emulator()
{
	// create new folder/emulator

	char buff[200];
	_getcwd(buff, 200);
	m_filepath = buff;
	m_filepath = m_filepath + "\\Emulator\\" ;
	

	_mkdir(m_filepath.c_str());		//may have already been created - _rmdir
	cout << "file added " << m_filepath << endl;
	m_file_loaded = "";

}

Emulator::~Emulator() {
	// destroy any veprom objects in veprom_list

}
void Emulator::create_Veprom(int size)
{
	/* creates new file name and eprom
	unless file already exists does nothing
	closes other files if there is one loaded*/

	// create new file
	// generate random tag id for new file
	srand((unsigned)time(NULL));
	long random = 9999 + (rand() % 10000);

	//create file, add to list of veproms
	string filename = "epromV" + to_string(random) + ".txt";
	Veprom *eprom = new Veprom(size, filename, m_filepath);

	veprom_list[filename] = eprom;

	// load file
	m_file_loaded = filename;

}

void Emulator::erase_Veprom()
{
	// wipes eprom, empty file count
	if (m_file_loaded != "")
		veprom_list[m_file_loaded]->erase();
	else
		cout << "cant erase!";
}

void Emulator::list_Files()
{
	if (m_file_loaded != "")
		veprom_list[m_file_loaded]->list();
	else
		cout << "cant list!";
}

void Emulator::write_to_Veprom(long address, const char* data)
{
	// if writing address is within
	if (m_file_loaded != "")
		veprom_list[m_file_loaded]->write(address, data);
	else
		cout << "cant write!";

}

void Emulator::read_from_Veprom(long address, int length)
{
	// if reading address is within
	if (m_file_loaded != "")
		veprom_list[m_file_loaded]->read(address, length);
	else
		cout << "cant read!";
}

void Emulator::write_file_to_Veprom(string filename)
{
	if (m_file_loaded != "")
		veprom_list[m_file_loaded]->write_file(filename);
	else
		cout << "cant write to file!";
}

void Emulator::read_file_from_Veprom(string filename)
{
	if (m_file_loaded != "")
		veprom_list[m_file_loaded]->read_file(filename);
	else
		cout << "cant read to file!";

}

bool Emulator::load_File(string filename)
{
	// if filename is present then 
	string f;
	if (isValidFile(filename)) {
		//veprom_list[m_file_loaded]->close_file();
		cout << "loading" << endl;
		size_t i = filename.rfind("\\", filename.length());
		if (i != string::npos) {
			f = (filename.substr(i + 1, filename.length() - i));
		}

		for (auto x : veprom_list) {
			cout << "eprom: " << x.first;
		}
		if (veprom_list.find(f) == veprom_list.end()) {
		//	Veprom* eprom = new Veprom(100, f, dir);
		//	veprom_list[filename] = eprom;
			return false;
		}

		m_file_loaded = f;
		return true;
	}
	else {
		cout << "This file cannot be loaded: " << filename << endl;
		return false;
	}

	return false;

}

bool Emulator::isValidFile(string filename)
{
	// search directory for this file
	// unimplemented
	return true;
}
