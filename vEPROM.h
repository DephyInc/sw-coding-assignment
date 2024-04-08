//============================================================================
// Name        : vEPROM.h
// Author      : tom
// Version     : 00
//===========================================================================
#ifndef VEPROM_H
#define VEPROM_H
#include <bits/stdc++.h>
#ifdef GUI_VEPROM
#include <QTextEdit>
#endif

class vEPROM {
	unsigned int capacity; 	// capacity of a virtual EPROM
	std::string file_name;
	std::ifstream file4_veprom;	// file for the EPROM
	unsigned int bytes_loaded;	// number of bytes loaded to the EPROM
	std::vector<std::pair<std::string, int>> files; // files with their sizes loaded to EPROM
	unsigned int last_valid_pos; // last valid/written position in EPROM
#ifdef GUI_VEPROM
    QTextEdit *guiOut;  // for output to gui
#endif
	/**
	 * @brief Set vEPROM to initial state (each byte to '0', etc.)
     *
     * @return: 0: succes; -1 failed
	 */
    int init();
	/**
	 * @brief Set bytes_loaded to a given size.
	 *
	 * @param size is the number of bytes to be loaded to vEPROM
	 * @return 0: success; -1: fail if size > capacity.
	 */
	int set_bytes_loaded(unsigned int size);

public:
	/**
	 * @brief Construct vEPROM with a given capacity and set it to initial state
	 *
	 * @param cap is the given capacity in k bytes
	 */
	vEPROM(unsigned int cap);
#ifdef GUI_VEPROM
    vEPROM(unsigned int cap, QTextEdit *guiOutText);
#endif

    /**
     * @brief Output a string line to cout
     *
     * @param line is the string to be output
     */
    virtual void out_normal(std::string line) {
        std::cout << line; std::cout << std::flush;
#ifdef GUI_VEPROM
        if (guiOut != nullptr) {
            QString qStr = QString::fromStdString(line);
            guiOut->append(qStr);
        }
#endif
    }

    /**
     * @brief Output a string line to cerr
     *
     * @param line is the string to be output
     */
    virtual void out_err(std::string line) {
        std::cerr << line; std::cerr << std::flush;
#ifdef GUI_VEPROM
        if (guiOut != nullptr) {
            QString qStr = QString::fromStdString(line);
            guiOut->append(qStr);
        }
#endif
    }

	/**
	 * @brief Get file name for vEPROM
	 *
	 * @return: file name string for vEPROM
	 */
	std::string get_file_name();

	/**
	 * @brief Get file names with their sizes loaded/written to vEPROM
	 *
	 * @return: file names with their sizes in vector<pair<string, int>>
	 */
	std::vector<std::pair<std::string, int>> get_files();

	/**
	 * @brief Write a raw string to an address/position on vEPROM.
	 *
	 * @param position is the given address/position
	 * @param s is the given string
	 * @return 0: success; -1: fail to write if values to be written is out of vEPROM range, etc.
	 */
	int write_raw(unsigned int position, std::string s);

	/**
	 * @brief Show vEPROM's contents of n bytes from address on standard output.
	 *  The contents shown are formated. Each line has a starting Address field,
	 *  value of each of 16 bytes in hex and printable chars (. for char not printable).
	 *   Address        |                 Values in Hex                 | Values in Char
	 *   Hex   (Decimal)|0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F |0123456789012345
	 *   ---------------+-----------------------------------------------+----------------
	 *        0(      0)|xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx|abcdefghijk=12AB
	 *       10(     16)|xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx xx|..XYX9876...?*&)
	 *
	 * @param address is the given address/position
	 * @param n is the number of bytes
	 */
	void format_out(unsigned int address, unsigned int n, std::ifstream &fin);

	/**
	 * @brief Read values at an address and length on EPROM and show them on standard output.
	 *
	 * @param address is the given address/position
	 * @param n is the length
	 * @return 0: success; -1: fail to read if values to be read is out of vEPROM range, etc.
	 */
	int read_raw(unsigned int address, unsigned int n);

	/**
	 * @brief Write/Append a file to the EPROM
	 *
	 * @param file is the name of the file
	 * @return 0: success; -1: fail to write/append if file is too big, etc.
	 */
	int append_file(std::string file) ;

	/**
	 * @brief List the files with their sizes loaded/written to the EPROM
	 */
	void list();

	/**
     * @brief Show each byte of file in vEPROM in standard output
	 */
    void read(std::string file);

	/**
	 * @brief Set the EPROM back to its original state
	 */
	void erase();
};

#endif // VEPROM_H
