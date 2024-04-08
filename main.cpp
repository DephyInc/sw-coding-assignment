//============================================================================
// Name        : main.cpp
// Author      : tom
// Version     : 00
//===========================================================================
#include <QApplication>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include "window.h"

/**
 * @brief (for testing) create file with file_name, containing n word(s). E.g.,
 *  for file_name = file1.txt, word = Abcd1111, n = 2, file1.txt contains
 *  Abcd1111Abcd1111
 *
 * @param file_name is the name of file to be created
 * @param word is the given string
 * @return n is the number of words
 */
void generate_file(std::string file_name, std::string word, unsigned int n) {
    std::ofstream fout(file_name, std::ios::binary);
    if (!fout) {
        std::cout << "Cannot open file: " << file_name << std::endl;
    }
    fout.seekp(0, std::ios::beg);
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < word.size(); j++) {
            fout.put(word[j]);
        }
    }
    fout.close();
}

/**
 * @brief (for testing) show the contents of file with file_name. E.g.,
 *  for file_name = file1.txt, the following is shown in standard output
 *  Abcd1111Abcd1111
 *
 * @param file_name is the name of file to be displayed
 */
void show_file(std::string file_name) {
    std::ifstream fin(file_name, std::ios::binary);
    if (!fin) {
        std::cout << "Cannot open file: " << file_name << std::endl;
    }
    std::cout << "File " << file_name << ": ";
    fin.seekg(0, std::ios::beg);
    char c;
    while (fin.get(c)) std::cout << c;
    fin.close();
    std::cout << std::endl;
}


int main(int argc, char *argv[])
{
    // Start of code for testing, generate a few files
    std::string f1 = "file1.txt", f2 = "file2.txt", f3 = "file3.txt", f4 = "file4.txt",f5 = "file5.txt";
    std::string w1 = "-a-file1", w2 = "-B-file2", w3 = "-C-file3", w4 = "-D-file4", w5 = "-X-file5";
    unsigned int n1 = 10, n2 = 20, n3 = 30, n4 = 50, n5 = 200;
    generate_file(f1,w1,n1); generate_file(f2,w2,n2); generate_file(f3,w3,n3);
    generate_file(f4,w4,n4); generate_file(f5,w5,n5);
    //show_file(f1); show_file(f2); show_file(f3); show_file(f4); show_file(f5);

    // End of code for testing
    std::string line;
    line = f1 + "  (80 bytes): " + w1 + " (10 times)\n"; std::cout << line;
    line = f2 + " (160 bytes): " + w2 + " (20 times)\n"; std::cout << line;
    line = f3 + " (240 bytes): " + w3 + " (30 times)\n"; std::cout << line;
    line = f4 + " (400 bytes): " + w4 + " (50 times)\n"; std::cout << line;
    line = f5 + "(1600 bytes): " + w5 + " (200 times)\n"; std::cout << line;
    std::cout << std::flush;
    //vEPROM_cmds vEPROM_cmds_o;
    //vEPROM_cmds_o.process_cmds();

    QApplication a(argc, argv);
    Window w;
    w.show();
    return a.exec();
}
