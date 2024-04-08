//============================================================================
// Name        : vEPROM_cmds.h
// Author      : tom
// Version     : 00
//===========================================================================
#ifndef VEPROM_CMDS_H
#define VEPROM_CMDS_H
#include <bits/stdc++.h>
#include "vEPROM.h"
#ifdef GUI_VEPROM
#include <QTextEdit>
#endif

#define MAX_CAP 1024000

class vEPROM_cmds {
	std::map <std::string, vEPROM*> m_vEPROMs; // file names and pointers to their vEPROM objects
    vEPROM *cur_vEPROM_p; 		// pointer to the current vEPROM to be operated
#ifdef GUI_VEPROM
    QTextEdit *guiOut;  // for output to gui
#endif
public:
	vEPROM_cmds ();
#ifdef GUI_VEPROM
    vEPROM_cmds(QTextEdit *guiOutText);
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
	 * @brief Get tokens from given string line by delimiter char (e.g., ' ') and "
	 *  e.g, from the line below, tokens = {"veprom", "write_raw", "32", "AB 12"}
	 *  veprom write_raw 32 "AB 12"
	 *
	 * @param line is the given string
	 * @param delimiter is the given delimiter char
	 * @return tokens is the tokens vector<string> from the given string
	 */
	void get_tokens(std::vector<std::string> &tokens, std::string line, char delimiter);

	/**
	 * @brief Process and execute "veprom create capacity" command
	 *  create a vEPROM object with capacity given in command
	 *  display file name for the vEPROM in standard output
	 *
	 * @param tokens contains parameters of the command
	 */
	void create_cmd(std::vector<std::string> &tokens);

	/**
	 * @brief Process and execute "veprom load /path/to/veprom/file" command
	 *  set the vEPROM with file to be the current vEPROM to be operated
	 *
	 * @param tokens contains parameters of the command
	 */
	void load_cmd(std::vector<std::string> &tokens);

	/**
	 * @brief Process and execute "veprom write_raw $ADDRESS $STRING" command
	 *  write STRING to the current vEPROM (file) from ADDRESS
	 *
	 * @param tokens contains parameters of the command
	 */
	void write_raw_cmd(std::vector<std::string> &tokens);

	/**
	 * @brief Process and execute "veprom read_raw $ADDRESS $length" command
	 *  display length bytes in the current vEPROM (file) from ADDRESS
	 *
	 * @param tokens contains parameters of the command
	 */
	void read_raw_cmd(std::vector<std::string> &tokens);

	/**
	 * @brief Process and execute "veprom write /path/to/local/file" command
	 *  write/load the given file to the current vEPROM (file)
	 *  first file is loaded to start of vEPROM, second file is appended to first one
	 *
	 * @param tokens contains parameters of the command
	 */
	void write_cmd(std::vector<std::string> &tokens);

	/**
	 * @brief Process and execute "veprom list" command
	 *  list the files for all the vEPROMs created and the files loaded for current vEPROM
	 */
	void list_cmd();

	/**
	 * @brief Process and execute "veprom read $FILE" command
	 *  display contents of $FILE for vEPROM to standard output
	 *
	 * @param tokens contains parameters of the command
	 */
	void read_cmd(std::vector<std::string> &tokens);

	/**
	 * @brief Process and execute "veprom erase" command
	 *  set the current vEPROM to initial state
	 */
	void erase_cmd();

	/**
	 * @brief Input, parse and execute every veprom command
	 *  exit after quit command
	 */
	void process_cmds();
};
#endif // VEPROM_CMDS_H
