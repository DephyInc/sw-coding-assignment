#include "Veprom.h"

Veprom::Veprom(int size, string filename, string file_location)
{
	m_size = size * 1000;			// size of eprom in bytes
	m_filename = filename;
	m_file_location = file_location;

	write(m_size-1, "0");

}

bool Veprom::open_file()
{
	m_text_in.open(m_file_location + m_filename, ios::out);
	if (m_text_in.is_open()) {
		return true;
	}
	
	return false;
}

void Veprom::close_file()
{
	m_text_in.close();
	m_text_out.close();
}

void Veprom::write(long address, const char* data)
{
	m_text_in.open(m_file_location + m_filename, ios::out);
	if (m_text_in.is_open()) {
		m_text_in.seekp(address);
		m_text_in << data;
		m_text_in.close();
		cout << "success!" << endl;
	}
	else
		cout << "unable to write" << endl;
}

void Veprom::read(long address, int length)
{
	m_text_out.open(m_file_location + m_filename, std::ios::out | std::ios::in);
	if (m_text_out.is_open()) {
		string data;
		m_text_out.seekg(address);
		data.resize(length);
		m_text_out.read(&data[0], length);
		m_text_out.close();
		cout << "raw data: " << data << endl;

	}
	else
		cout << "cant read" << endl;
}

void Veprom::list()
{
	// lists filenames
	for (auto i : files_stored)
	{
		cout << "file: " << i.filename << endl;
	}
}

void Veprom::erase()
{
	// deletes file and filenames
	remove(m_filename.c_str());
	files_stored.erase(files_stored.begin());
	// new file
	//open_file();
	write(m_size - 1, "0");
}

void Veprom::write_file(string filename)
{
	// find if the eprom has space to contain the new file

	// how large is the new file?
	ifstream in_file(filename, ios::binary);
	in_file.seekg(0, ios::end);
	long file_size = in_file.tellg();
	in_file.close();

	if (file_size == -1){
		cout << "error: invalid file given";
		return;
	}

	// can we make it fit? greedy algo

	textfile text;
	text.starting_address = 0;
	text.filename = filename;
	text.length = file_size;

	bool space_left = false;
	int pos = 0;
	if (files_stored.size() == 0 && text.length < m_size)
		space_left = true;
	else {
		for (int i = 0; i < files_stored.size(); i++) {
			if (files_stored[i].starting_address - text.starting_address > text.length) {
				pos = i;
				space_left = true;
				break;
			}
			else
				text.starting_address = files_stored[i].starting_address + files_stored[i].length;
		}
		if (space_left == false && files_stored.size() > 0) {
			int val = files_stored[files_stored.size() - 1].starting_address + files_stored[files_stored.size() - 1].length;
			int m = m_size - val;

			if ( text.length < m)
			{
				pos = files_stored.size();
				space_left = true;
			}
		}
	}

	if (!space_left) {
		cout << "no room for this file" << endl;
		return;
	}

	files_stored.insert(files_stored.begin() + pos, text);
	
	cout << "start copying: " << text.starting_address << endl;
	ifstream file;
	file.open(text.filename);
	
		m_text_in.open(m_file_location + m_filename, std::ios::out | std::ios::in);
		m_text_in.seekp(text.starting_address);
		while (!file.eof())
		{
			string c;

			getline(file, c);
			//cout << "raw data: " << c << endl;

			m_text_in << c;

		}

	file.close();
	m_text_in.close();
}

void Veprom::read_file(string filename)
{

	for (auto i : files_stored)
	{
		if (filename == i.filename) {
			//read_chunks(i.starting_address, i.length);
			read(i.starting_address, i.length);
		}

	}
}

void Veprom::read_chunks(long address, long length) {

	for (int i = 0; i < length/1000 -1; i++) {
		long offset = i * 1000;
		read(address + offset, 1000);
	}
	long last = length % 1000;
	read(address + length - last, last);
}