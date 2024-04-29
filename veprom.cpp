#include <stdexcept>
#include <string.h>
#include <cstdint>
#include <filesystem>
#include <iterator>
#include "veprom.h"

VEprom veprom;

VEprom::VEprom() {
    // cout << "VEprom constructor start\n";
    v_ = false;
    memtop_ = 0; // no writable memory
    eprom_name_ = "";
    dirty_ = false;
    RestoreMem();
    // cout << "memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
    // cout << "VEprom constructor done\n";
};

VEprom::~VEprom() {
    if (v_) cout << "VEprom destructor start\n";
    if (v_) cout << "memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
    dirty_ = true;
    SaveMem();
    int rcode = WriteHTML();
    if (rcode) {
	cerr << "error in WriteHTML, rcode = " << rcode << "\n";
    }	
    if (v_) cout << "After SaveMem memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
    if (v_) cout << "VEprom destructor done\n";
};

void VEprom::Verbose() {
    v_ = true;
}

int VEprom::Create (int size) {
    try {
        mem_.resize(1000*size, 0); 
    } catch  (const exception& e) {
	cerr << "exception in Create: " << e.what();
	return 1;
    }

    memtop_ = mem_.size() - 2; // top limit past available for data before directory size
    memend_ = mem_.size();     // end of original vector size 

    if (v_) cout << "Create memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
    dirty_ = true;
    SaveMem();
    return 0;
}

int VEprom::Load (const char* eprom_name) {
    ifstream ifs;
    ifs.open(eprom_name, ifstream::in);
    if (!ifs) {
	cerr << "cannot open '" << eprom_name << "\n";
	return 1;
    }
    mem_.clear();
    unsigned char c = ifs.get();
    while (ifs.good()) {
        mem_.push_back(c);
        c = ifs.get();
    }

    ifs.close();
    memtop_ = mem_.size() - 2; // top available for data
    memend_ = memtop_;         // end of original vector size 
    if (v_) cout << memtop_ << " bytes read from " << eprom_name << "\n";
    SaveMem();
    return 0;
}

int VEprom::WriteRaw (int address, const char* str) {
    int dir_size = (mem_[memend_ - 2] << 8) | mem_[memend_ - 1];
    int mem_available = FindAvailable();
    if ((address < mem_available) || ((size_t)address + strlen(str) > memend_ - dir_size)) {
	cerr << "cannot write between addresses " << mem_available << " and " << memend_ - dir_size << "\n";
	return 1;
    }

    int bytes = 0;
    while (*str != '\0') {
        mem_[address++] = *str;
	++str;
	++bytes;
    }

    if (v_) cout << bytes << " bytes altered\n";

    if (bytes) {
        dirty_ = true;
    }
    return 0;
}

int VEprom::ReadRaw (int address, int len) {
    int bytes = 0;
    while (len-- > 0) {
	if (v_) cout << "ReadRaw reading at addr " << address << "\n";
	cout << mem_[address++];
	++bytes;
    }

    if (v_) cout << "\n" << bytes << " bytes read\n";

    return 0;
}

int VEprom::Write (const char* fname) {
    // verify file exists
    error_code ec;
    if (!filesystem::exists(fname, ec)) {
	cerr << "file not found: " << fname << " error code " << ec <<"\n";
	return 1;
    }

    // verify access to file
    ifstream ifs;
    ifs.open(fname, ifstream::in);
    if (!ifs || !ifs.is_open()) {
        cerr << "cannot open '" << fname << "\n";
        return 2;
    }

    // modify name if duplicate
    string fname_orig = fname;
    string fname_relative_path = filesystem::path(fname).relative_path();
    string fname_filename = filesystem::path(fname).filename();
    string fname_stem = filesystem::path(fname).stem();
    string fname_ext = filesystem::path(fname).extension();
    size_t lastext = fname_relative_path.find_last_of(".");
    string fname_noext = fname_relative_path.substr(0, lastext);

    if (v_) cout << "relative_path is '" << fname_relative_path << "' filename is '" << fname_filename << "' stem is '" << fname_stem << "' ext '" << fname_ext << "' fname_noext is '"<< fname_noext << "'\n";

    string fname_mod  = fname_orig;
    int suffix = 0;
    while (FindFile(fname_mod.c_str())) {
	++suffix;
	fname_mod = fname_noext + "(" + to_string(suffix) + ")" + fname_ext;
    }

    if (v_) cout << "modified name is '" << fname_mod << "'\n";

    // verify the file size, including the space for directory entry
    uintmax_t size_dir = fname_mod.length() + 1 + 2 + 2 + 1;  // fname_mod, \0, address, length, \0
    uintmax_t size_data = filesystem::file_size(fname);
    if (size_dir + size_data >= memtop_) {
        cerr << "no room on device with available size " << memtop_ << " for directory entry "<< size_dir << " +  file data " << size_data << "\n";
	ifs.close();
        return 3;
    }

    memtop_ -= size_dir;
    int begin = memtop_;
    int fname_ptr = 0;
    while (fname_mod[fname_ptr] != '\0') {
        if (v_) cout << "Writing file letter " << fname_mod[fname_ptr] << " at fname_ptr " << fname_ptr << " to dir addr " << begin << "\n";
        mem_[begin++] = fname_mod[fname_ptr++];
    }

    if (v_) cout << "writing file name terminating null at " << begin << "\n";
    mem_[begin++] = '\0';

    // write file data addr
    int mem_available = FindAvailable();
    if (v_) cout << "mem_available " << mem_available << "\n";

    size_t idx = mem_available;
    if (v_) cout << "writing file data address mem_available " << mem_available << " bytes at " << begin << "\n";

    mem_[begin++] = idx >> 8;
    mem_[begin++] = idx & 0x0000ffff;
    size_t file_data_addr = (mem_[begin] << 8) | mem_[begin + 1];
    if (v_) cout << "wrote bytes at " << begin - 2 << " : " << (int)mem_[begin - 2] << " " << (int)mem_[begin - 1] << " file_data_addr " << file_data_addr << "\n";

    // write file data len
    if (v_) cout << "writing file data length " << size_data << " at " << begin << "\n";
    idx = begin;
    mem_[begin++] = size_data >> 8;
    mem_[begin++] = size_data & 0x0000ffff;
    if (v_) cout << "writing bytes at " << idx << " : " << (int)mem_[idx] << " " << (int)mem_[idx + 1] << " size_data " << size_data << "\n";

    if (v_) cout << "writing directory record terminating null at " << begin << "\n";
    mem_[begin++] = '\0';
    
    if (v_) cout << "memend_ is " << memend_ << " memtop_ becomes " << memtop_ << " size_dir is " << size_dir << "\n";

    // write file data
    begin = mem_available;
    unsigned char c = ifs.get();
    while (ifs.good()) {
        mem_[begin++] = c;
        c = ifs.get();
    }

    if (v_) cout << "last unwritten data address " << begin << "\n";

    // write new directory size
    size_t new_size = memend_ - memtop_;
    mem_[memend_ - 2] = new_size >> 8;
    mem_[memend_ - 1] = new_size & 0x0000ffff;
    if (v_) cout << "directory size bytes become " << (int)mem_[memend_ - 2] << " " << (int)mem_[memend_ - 1] << " at " << memend_ - 1 << "\n";
    dirty_ = true;
    return 0;
}

int VEprom::List (bool print, bool updateHTML) {
    // this method also builds a vector for files discovered used in HTML feedback
    if (v_) cout << "List: memend_ " << memend_ << " memtop_ " << memtop_ << " directory size bytes are " << (int)mem_[memend_ - 2] << " " << (int)mem_[memend_ - 1] << "\n";

    int dir_size = (mem_[memend_ - 2] << 8) | mem_[memend_ - 1];

    if (v_) cout << "List: directory size is " << dir_size << "\n";
    size_t idx = memend_ - 2 - 1 - 2 - 2 - 1 - 1; // past directory size bytes, three nulls, data address and data length
    if (v_) cout << "List: idx " << idx << " memtop_ " << memtop_ << "\n";
    int nfiles = 0;
    files_.clear();

    while (true) {
	size_t last_letter_idx = idx;
        if (v_) cout << "List: last_letter_idx " << last_letter_idx << "\n";
	string fname = "";
	unsigned char letter[2] = {'\0', '\0'};
	int n = 0;
        while (mem_[idx] != '\0') {
            letter[0] = mem_[idx];
            string the_byte(reinterpret_cast<char*>(letter), 2);
	    if (v_) cout << "List: idx " << idx << " the_byte '" << the_byte << "'\n";
	    fname = the_byte + fname;
	    ++n;
            --idx;
        }

        size_t fname_begin_idx = idx;
        if (v_) cout << "List: fname_begin_idx " << fname_begin_idx << "\n";
	
	if (updateHTML) {
	    idx = last_letter_idx + 1;
	    ++idx; // past the null
	    // now idx points to data section for the next file
	    size_t data_idx = idx;
	    if (v_) cout << "List: n " << n <<" data_idx " << data_idx << "\n";
            idx += 2;
            // now idx points to length section for the next file
	    size_t len_idx = idx;
            if (v_) cout << "List: len_idx " << len_idx << "\n";

            FileRecord fr;
	    // add FileRecord
            int file_data_addr = (mem_[data_idx] << 8) | mem_[data_idx + 1];
            if (v_) cout << "List: file_data_addr " << file_data_addr << " at " << data_idx << "\n";
            int file_data_len = (mem_[len_idx] << 8) | mem_[len_idx + 1];
            if (v_) cout << "List: file_data_len " << file_data_len << " at " << len_idx << "\n";

            fr.address_ = fname_begin_idx + 1;
	    fr.fname_ = fname;
	    fr.data_ = file_data_addr;
	    fr.len_ = file_data_len;
            if (n > 0) { // substantive fname
                files_.push_back(fr);
	        if (v_) cout << "List: record pushed " << fr.fname_ << " data " << fr.data_ << " len " << fr.len_ << "\n";
	    } else {
		if (v_) cout << "List: record not pushed \n";
	    }
            ++nfiles;
	}

	if (print) {
            if (n > 0) { // substantive fname
                // print file name
                cout << fname << "\n";
            } else {
                cerr << "expected file name, found null\n";
	    }
        }

        if (n > 0) {	
            if (v_) cout << "List: idx " << idx << "\n";
	    idx -= (1 + 2 + 2 + 1 + n + 2 + 2);
            if (v_) cout << "List: idx for the next file " << idx << " memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
	} else {
	    if (v_) cout << "can't go back to preceding file " << n << "\n";
	    if (v_) cout << "List: idx for the next file idx " << idx << " memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
	    break;
	}
    }

    if (nfiles == 0) {
	if (v_) cout << "no files\n";
    }
    return 0;
}

int VEprom::Read (const char* fname) {
    size_t data_addr = FindFile(fname); // this is address of the directory record for file data
    if (!data_addr) {
	cerr << "file not found \n";
	return 1;
    }

    size_t len_addr = data_addr + 2;
    size_t start = (mem_[data_addr] << 8) | mem_[data_addr + 1];
    size_t len = (mem_[len_addr] << 8) | mem_[len_addr + 1];
    if (v_) cout << "Read data_addr " << data_addr << " len_addr " << len_addr << " len " << len << " len bytes " << (int)mem_[len_addr] << " " << (int)mem_[len_addr+1] << "\n";
    size_t idx = start;
    while (idx < start + len) {
	cout << mem_[idx++];
    }	    
    return 0;
}

int VEprom::Erase() {
    mem_.clear();
    memtop_ = memend_ - 2;
    dirty_ = true;
    SaveMem();
    return 0;
}

int VEprom::SaveMem () {
    if (!dirty_) {
	return 0; // not an error -- nothing to save
    }

    string eprom_dat = ".mem";
    ofstream ofs;
    ofs.close();
    ofs.open(eprom_dat, ofstream::out | ofstream::binary | ofstream::trunc);
    if (!ofs) {
        cerr << "SaveMem cannot open '" << eprom_dat << "'\n";
	return 1;
    }

    if (!ofs.write((char*)&mem_[0], memend_)) {
	cerr << "failure to write '" << eprom_dat << "'\n";
	ofs.close();
	return 2;
    }
    ofs.close();

    mem_.resize(memend_, 0);
    error_code ec;
    filesystem::resize_file(eprom_dat, memend_, ec);
    if (ec) {
	cerr << "resize_file error code " << ec << "\n";
    }

    if (v_) cout << mem_.size() << " bytes saved to '" << eprom_dat << "' while memend_ is " << memend_ <<" \n";

    int dir_size = (mem_[memend_ - 2] << 8) | mem_[memend_ - 1];
    if (v_) cout << "saved directory size is " << dir_size << " memend_ " << memend_ << " memend bytes " << (int)mem_[memend_ - 2] << " " << (int) mem_[memend_ - 1] << "\n";

    dirty_ = false;
    return 0;
}

int VEprom::RestoreMem () {
    string eprom_dat = ".mem";
    ifstream ifs;
    ifs.open(eprom_dat, ifstream::in | ifstream::binary);
    if (!ifs) {
        cerr << "RestoreMem cannot open '" << eprom_dat << "'\n";
	return 1;
    }

    mem_.clear();
    unsigned char c = ifs.get();
    while (ifs.good()) {
        mem_.push_back(c);
        c = ifs.get();
    }
    memend_ = mem_.size();
    int dir_size = (mem_[memend_ - 2] << 8) | mem_[memend_ - 1];
    if (dir_size < 2) {
	dir_size = 2;
        mem_[memend_ - 2] = 0;
	mem_[memend_ - 1] = 2;
    }
    if (v_) cout << "restored directory size is " << dir_size << " memend_ " << memend_ << " memend bytes " << (int)mem_[memend_ - 2] << " " << (int) mem_[memend_ - 1] << "\n";


    memtop_ = memend_ - dir_size; // allow for directory size
    ifs.close();
    if (v_) cout << mem_.size() << " bytes restored from '" << eprom_dat << "'\n";

    return 0;
}

int VEprom::FindAvailable() {
    size_t idx = memtop_;
    int sum_data_size = 0;

    while (idx < memend_ - 2) {
	if (v_) cout << "FindAvailable looking at directory idx " << idx << " memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
	int n = 0;
        while (mem_[idx] != '\0') {
            ++idx;
	    ++n;
	}

	if (n == 0) {
	    if (v_) cout << "no file name at idx " << idx << "\n";
	    break;
	}
        ++idx; // skip terminating null after the file name
	// now idx points to data section for this file
	// int file_data_addr = (mem_[idx] << 8) | mem_[idx + 1];
	idx += 2;
        // now idx points to length section for this file
        int file_data_len = (mem_[idx] << 8) | mem_[idx + 1];
	if (v_) cout << "FindAvailable file_data_len " << file_data_len << "\n";
	idx += 2; // skip the length section
        ++idx;    // skip the terminating null after the length section
        sum_data_size += file_data_len;
    }
    return sum_data_size;
}

size_t VEprom::FindFile(const char* fname) {
    size_t idx = memtop_;

    if (v_) cout << "FindFile idx " << idx << " memtop_ " << memtop_ << " memend_ " << memend_ << " find " << fname << "\n";
    while (idx < memend_) {
        int n = 0;
	bool found = true; // assume they match
        while (mem_[idx] != '\0') {
            if (v_) cout << "FindFile reading fname " << (int)fname[n] <<" idx " << idx << " memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
	    if (mem_[idx] != fname[n]) {
		found = false; // still have to skan whole name
	    }
            ++n;
            ++idx;
        }

	if (n > 0 && mem_[idx] == '\0') {
            if (v_) cout << "FindFile found null idx " << idx << " memtop_ " << memtop_ << " memend_ " << memend_ << " matching letters " << n <<"\n";
            ++idx;

            if (found) {
                size_t file_data_addr = (mem_[idx] << 8) | mem_[idx + 1];
                if (v_) cout << "FindFile returning bytes at " << idx << " : " << (int)mem_[idx] << " " << (int)mem_[idx + 1] << " file_data_addr " << file_data_addr << "\n";
                return idx; // return address in the directory section!
	    }
	}

	idx += 5; // skip data pointer and length, and another null
	if (v_) cout << "FindFile moving to next file idx " << idx << " memtop_ " << memtop_ << " memend_ " << memend_ << "\n";
    }
    return 0;
}

int VEprom::WriteHTML() {
    bool save_v = v_;
    v_ = false;
    List(false, true);
    string html_dat = ".html";
    ofstream ofs;
    ofs.open(html_dat, ofstream::out);
    if (!ofs) {
        cerr << "WriteHTML cannot open '" << html_dat << "'\n";
        return 1;
    }

    ofs << "<!DOCTYPE html>\n";
    ofs << "<html>\n"; 
    ofs << "<body>\n";
    ofs << "<h1>VEPROM memory</h1>\n";
    /*
    ofs << "files_size " << files_.size() << "\n"; // remove me
    for (size_t f = 0; f < files_.size(); ++f) {
        ofs << "address " << files_[f].address_ << " name " << files_[f].fname_ << " data " << files_[f].data_ << " len " << files_[f].len_ << "\n";
    }
    */
    ofs << "<table>\n";
    ofs << "<th>Address</th>\n";
    ofs << "<th>Data</th>\n";
    ofs << "<th>Comment</th>\n";
    ofs << "<tr>\n";
    int mem_available = FindAvailable();
    size_t n = 0;
    for (int idx = 0; idx < mem_available; ++idx) {
        ofs << "<td>" << idx << "</td>\n";
        ofs << "<td>" << (int)mem_[idx] << "</td>\n";
	if (n < files_.size() && (size_t)idx == files_[n].data_) {
            ofs << "<td>" << "file storage begins for " << files_[n].fname_ << "</td>\n";
	    ++n;
	}

        ofs << "<tr>\n";
    }

    ofs << "<td>" << mem_available << "</td>\n";
    ofs << "<td>" << (int)mem_[mem_available] << "</td>\n";
    ofs << "<td>" << "random access storage begins" << "</td>\n";
    ofs << "<tr>\n";

    ofs << "<td></td>\n";
    ofs << "<td>...</td>\n";
    ofs << "<tr>\n";

    ofs << "<td>" << memtop_ - 1 << "</td>\n";
    ofs << "<td>" << (int)mem_[memtop_ - 1] << "</td>\n";
    ofs << "<td>" << "random access storage ends" << "</td>\n";
    ofs << "<tr>\n";

    if (files_.size() > 0) {
        ofs << "<td> size " << files_.size() << "</td>\n";
	ofs << "<tr>\n";
        std::vector<FileRecord>::reverse_iterator rit = files_.rbegin();
	for (; rit!= files_.rend(); ++rit) {
            ofs << "<td>" << rit->address_ << "</td>\n";
            ofs << "<td>" << (int)mem_[rit->address_] << "</td>\n";
            ofs << "<td>" << "directory for " << rit->fname_ <<  " begins" << "</td>\n";
            ofs << "<tr>\n";
        }
    }

    ofs << "<td>" << memend_ - 2 << "</td>\n";
    ofs << "<td>" << (int)mem_[memend_ - 2] << "</td>\n";
    ofs << "<td>" << "directory size byte1" << "</td>\n";
    ofs << "<tr>\n";

    ofs << "<td>" << memend_ - 1 << "</td>\n";
    ofs << "<td>" << (int)mem_[memend_ - 1] << "</td>\n";
    ofs << "<td>" << "directory size byte2;  top address reached" << "</td>\n";
    ofs << "<tr>\n";

    ofs << "</table>\n";
    ofs << "</body>\n";
    ofs << "</html>\n";
    ofs.close();
    v_ = save_v;
    return 0;
}
