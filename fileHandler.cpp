#include "fileHandler.h"

int fileHandler::parseCommandlineArguments(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "No command provided." << std::endl;
        return -1;
    }
    std::vector<std::string> arguments{};
    for (int i = 1; i < argc; ++i)
    {
        arguments.push_back(argv[i]);
    }

    if (arguments.size() >= 1)
    {

        if (arguments[0] == "create")
        {
            if (arguments.size() == 2)
            {
                // read the argument[i+1] , verify it is a good integer and
                // convert it into integer and assign it to fileSizeKB
                try
                {
                    fileSizeB = std::stoi(arguments[1]) * 1024;
                }
                catch (const std::exception &e)
                {
                    std::cout << __func__ << ": Commandline error" << std::endl;
                    std::cerr << e.what() << '\n';
                }

                std::string filePath{};
                create(fileSizeB, filePath);
                std::cout << __func__ << ": file created with file name: " << filePath << std::endl;
            }
            else
            {
                std::cout << __func__ << ": wrong number of input arguments provided.";
            }
            return 0;
        }
        else if (arguments[0] == "load")
        {
            // read argument arguments[i+1], verify that it is a valid path
            // and then write it at the end of the config file list
            //(and assign it to the filename member? it won't be used though)
            if (arguments.size() == 2)
            {

                filename = arguments[1];
                if (std::filesystem::exists(filename))
                {
                    load();
                }
                else
                {
                    std::cout << __func__ << ": filename provided does not exist." << std::endl;
                }
            }
            else
            {
                std::cout << __func__ << ": wrong number of input arguments provided.";
            }
            return 0;
        }
        else if (arguments[0] == "write_raw")
        {
            if (arguments.size() == 3)
            {
                std::string address = arguments[1];
                std::string String = arguments[2];
                getFilenameFromConfigFile();
                write_raw(std::atoi(address.c_str()), String);
            }
            else
            {
                std::cout << __func__ << ": wrong number of input arguments provided." << std::endl;
            }
            return 0;
        }
        else if (arguments[0] == "read_raw")
        {
            if (arguments.size() == 3)
            {
                std::string address = arguments[1];
                std::string length = arguments[2];
                getFilenameFromConfigFile();
                std::string stringFromFile{};
                read_raw(std::atoi(address.c_str()), std::atoi(length.c_str()), stringFromFile);
            }
            else
            {
                std::cout << __func__ << ": wrong number of input arguments provided." << std::endl;
            }
            return 0;
        }
        else if (arguments[0] == "write")
        {
            if (arguments.size() == 2)
            {
                getFilenameFromConfigFile();
                std::string filepath = arguments[1];
                write(filepath);
            }
            else
            {
                std::cout << __func__ << ": wrong number of input arguments provided." << std::endl;
            }
            return 0;
        }
        else if (arguments[0] == "list")
        {
            getFilenameFromConfigFile();
            list();
            return 0;
        }
        else if (arguments[0] == "read")
        {
            if (arguments.size() == 2)
            {
                getFilenameFromConfigFile();
                std::string file = arguments[1];
                read(file);
            }
            else
            {
                std::cout << __func__ << ": wrong number of input arguments provided." << std::endl;
            }

            return 0;
        }
        else if (arguments[0] == "erase")
        {
            getFilenameFromConfigFile();
            erase();
            return 0;
        }
        else
        {
            std::cout << __func__ << ": Commandline error" << std::endl;
            return 0;
        }
    }

    return 0;
}

int createBinaryFile(const std::string &filename, int fileSize)
{
    // Open the file in binary mode
    std::ofstream file(filename, std::ios::binary);

    if (file.is_open())
    {
        for (int i = 0; i < fileSize; i++)
        {
            file.put(0);
        }
        // write int 0 in the first 4 bytes to indicate there is no file in this EPROM
        file.seekp(0, std::ios::beg);
        int numFiles = 0;
        file.write(reinterpret_cast<const char *>(&numFiles), sizeof(numFiles));
        // Close the file
        file.close();
        std::cout << "Binary file created successfully with size " << fileSize << " bytes." << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "Error: Unable to create binary file." << std::endl;
        return -1;
    }
}

int fileHandler::create(int size, std::string &filePath)
{
    int fileNumber = 0;
    std::string baseFilename = std::to_string(fileNumber);
    while (std::filesystem::exists(baseFilename))
    {
        baseFilename = std::to_string(fileNumber++);
    }
    int rcode = createBinaryFile(baseFilename, size);
    if (rcode == 0)
    {
        filePath = baseFilename;
        WriteStringToEndOfFile(filePath);
    }

    return rcode;
};

int fileHandler::load()
{
    // write the fileHandler::filename to the end of the dephy.cfg file
    WriteStringToEndOfFile(filename);
    return 0;
};

int fileHandler::write_raw(int address, const std::string &String)
{
    std::ofstream outputfile(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!outputfile.is_open())
    {
        std::cout << __func__ << ": Error: Unable to open file: " << filename << std::endl;
        return 0;
    }

    // check to see there is enough space in the file to write the string.
    outputfile.seekp(0, std::ios::end);
    int filesize = outputfile.tellp();
    if (address + String.size() > filesize)
    {
        std::cout << __func__ << ": There is not enough space in EPROM " << filename << " to write the string to it." << std::endl;
        return -1;
    }

    outputfile.seekp(0, std::ios::beg);
    outputfile.seekp(address);
    outputfile.write(String.c_str(), String.size());
    std::cout << __func__ << ": Successfully wrote the string " << String << " to the address " << address << " of EPROM " << filename << std::endl;
    outputfile.close();
    return 0;
};

int fileHandler::read_raw(int address, int length, std::string &String)
{
    std::ifstream inputfile(filename, std::ios::binary | std::ios::in);
    if (!inputfile.is_open())
    {
        std::cout << __func__ << ": Error: Unable to open file: " << filename << std::endl;
        return 0;
    }

    inputfile.seekg(address);
    std::vector<char> bytes;
    bytes.resize(length);
    inputfile.read(&bytes[0], length);
    std::string Str(bytes.begin(), bytes.end());
    String = Str;
    std::cout << length << " bytes read from " << filename << " are: " << String << std::endl;
    inputfile.close();
    return 0;
};

std::string getFilenameFromPath(const std::string &filePath)
{
    std::filesystem::path pathObj(filePath);
    return pathObj.filename().string();
}

int fileHandler::write(std::string &filePath)
{
    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file)
    {
        std::cout << __func__ << ": Error: Unable to open " << filename << " for reading and writing." << std::endl;
        return -1;
    }

    file.seekg(0, std::ios::end);
    fileSizeB = file.tellg();
    file.seekg(0, std::ios::beg);
    // Read the number of files from the first byte of filename
    int numFiles;
    file.read(reinterpret_cast<char *>(&numFiles), sizeof(numFiles));

    // Find the position to insert the new file
    int insertPosition = sizeof(numFiles); // account for the bytes used to keep the number of files in filename
    for (int i = 0; i < numFiles; i++)
    {
        // Read file name
        std::string fileName;
        std::getline(file, fileName, '\0');

        // Read file size
        int fileSize;
        file.read(reinterpret_cast<char *>(&fileSize), sizeof(fileSize));

        // Move to the next file entry
        insertPosition += fileName.size() + 1 + sizeof(fileSize) + fileSize;
        file.seekg(insertPosition);
    }

    // Seek to the insert position
    file.seekp(insertPosition);

    // Open the new file for reading
    std::ifstream newFile(filePath, std::ios::binary | std::ios::in);
    if (!newFile)
    {
        std::cout << __func__ << ": Error: Unable to open file " << filePath << " for reading." << std::endl;
        return -1;
    }

    // Get the size of the new file
    newFile.seekg(0, std::ios::end);
    int fileSize = newFile.tellg();
    newFile.seekg(0, std::ios::beg);

    int remainingSpaceInBytes = fileSizeB - insertPosition - filePath.size() - 1 - sizeof(fileSize);

    // Check if there is enough space in the EPROM to add the new file
    if (fileSize > remainingSpaceInBytes)
    {
        std::cout << __func__ << ": Error: Not enough space in the EPROM " << filename << " to add the file " << filePath << std::endl;
        return -1;
    }

    // Write the new file's name and size to the filename
    filePath = getFilenameFromPath(filePath);
    file.write(filePath.c_str(), filePath.size() + 1);                       // Write filepath including the null terminator
    file.write(reinterpret_cast<const char *>(&fileSize), sizeof(fileSize)); // Write file size

    // Write the new file's content
    char buffer[1024]; // Buffer to read and write chunks of data
    size_t remaining = fileSize;
    while (remaining > 0)
    {
        size_t chunkSize = std::min(sizeof(buffer), remaining);
        newFile.read(buffer, chunkSize);
        file.write(buffer, chunkSize);
        remaining -= chunkSize;
    }

    // increment the number of files on EPROM
    file.seekp(0, std::ios::beg);
    numFiles++;
    file.write(reinterpret_cast<const char *>(&numFiles), sizeof(numFiles));

    file.close();

    std::cout << "File " << filePath << " inserted into EPROM successfully." << std::endl;
    return 0;
};

int fileHandler::list()
{
    // open EPROM for reading in binary mode
    std::ifstream file(filename, std::ios::binary | std::ios::in);
    if (!file)
    {
        std::cout << __func__ << ": Error: Unable to open EPROM file " << filename << std::endl;
        return -1;
    }
    file.seekg(0, std::ios::end);
    fileSizeB = file.tellg();
    file.seekg(0, std::ios::beg);
    // Read the number of files from the first byte of filename
    int numFiles;
    file.read(reinterpret_cast<char *>(&numFiles), sizeof(numFiles));

    if (numFiles > 0)
    {
        std::cout << "Number of File on EPROM is " << numFiles << std::endl;
        std::cout << std::setw(20) << " File Name "
                  << " | " << std::setw(20) << " File Size In Bytes "
                  << " | "
                  << " Percent Space on EPROM " << std::endl;
        std::cout << "---------------------|----------------------|--------------------------" << std::endl;
    }
    else
    {
        std::cout << "No file is found on EPROM." << std::endl;
        return 0;
    }
    // Iterate through each file entry in the EPROM
    for (int i = 0; i < numFiles; i++)
    {
        // Reading file's name
        std::string fileName;
        std::getline(file, fileName, '\0');
        // Read file size
        int fileSize;
        file.read(reinterpret_cast<char *>(&fileSize), sizeof(fileSize));
        // Read file contents
        std::vector<char> buffer(fileSize);
        file.read(buffer.data(), fileSize);
        // ouput the file contents to stdout
        // std::cout << buffer << std::endl;
        // delete[] buffer;
        // print the file name and size to the screen
        double percentage = static_cast<double>(fileSize) / static_cast<double>(fileSizeB) * 100.0;
        std::cout << std::setw(20) << fileName << " | " << std::setw(20) << fileSize << " | " << std::setw(20) << percentage << " % ";
        for (size_t j = 0; j < std::ceil(percentage); j++)
        {
            std::cout << "+";
        }
        std::cout << std::endl;
    }
    double percentage = static_cast<double>(fileSizeB - file.tellg()) / static_cast<double>(fileSizeB) * 100.0;
    std::cout << std::setw(20) << "Empty Space "
              << " | " << std::setw(20) << fileSizeB - file.tellg() << " | " << std::setw(20) << percentage << " % ";
    for (size_t j = 0; j < std::ceil(percentage); j++)
    {
        std::cout << "+";
    }
    std::cout << std::endl;
    std::cout << "---------------------|----------------------|--------------------------";
    return 0;
};

int fileHandler::read(const std::string &fileToRead)
{
    // open EPROM for reading in binary mode
    std::ifstream file(filename, std::ios::binary | std::ios::in);
    if (!file)
    {
        std::cout << __func__ << ": Error: Unable to open EPROM file " << filename << std::endl;
        return -1;
    }
    // Read the number of files from the first byte of filename
    int numFiles;
    file.read(reinterpret_cast<char *>(&numFiles), sizeof(numFiles));
    int numFilesFound = 0;
    // Iterate through each file entry in the EPROM
    for (int i = 0; i < numFiles; i++)
    {
        // Readthe file's name
        std::string fileName;
        std::getline(file, fileName, '\0');
        // Read file size
        int fileSize;
        file.read(reinterpret_cast<char *>(&fileSize), sizeof(fileSize));
        // Check if this is the desired file
        if (fileName == fileToRead)
        {
            // Read file contents
            std::cout << "Contents of : " << fileToRead << std::endl;
            std::vector<char> buffer(fileSize, 0);
            file.read(buffer.data(), fileSize);
            for (char c : buffer)
            {
                std::cout << c;
            }
            std::cout << std::endl
                      << std::endl;
            numFilesFound++;
        }
        else
        {
            // skip to the next file entry
            file.seekg(fileSize, std::ios::cur);
        }
    }
    file.close();
    if (numFilesFound == 0)
    {
        // if $FILE is not found
        std::cout << __func__ << ": Error: File " << fileToRead << " not found in EPROM." << std::endl;
    }
    else
    {
        std::cout << __func__ << ": Found " << numFilesFound << " files with name " << fileToRead << " in " << filename << " EPROM." << std::endl;
    }

    return 0;
};

int fileHandler::erase()
{
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary | std::ios::ate); // Open the file in binary mode and seek to the end

    if (!file.is_open())
    {
        std::cout << __func__ << ": Error: Unable to open file: " << filename << std::endl;
        return 0;
    }

    // Get the current position, which is the size of the file
    int fileSize = file.tellg();

    file.close();

    std::ofstream output_file(filename, std::ios::binary | std::ios::out); // Open the file in binary mode for output

    if (!output_file.is_open())
    {
        std::cout << __func__ << ": Error: Unable to open file for writing: " << filename << std::endl;
        return 0;
    }

    // Write '0' characters to the file until the desired number of bytes is written
    for (int i = 0; i < fileSize; ++i)
    {
        output_file.put(0);
    }
    // write int 0 in the first 4 bytes to indicate there is no file in this EPROM
    output_file.seekp(0, std::ios::beg);
    int numFiles = 0;
    output_file.write(reinterpret_cast<const char *>(&numFiles), sizeof(numFiles));
    output_file.close();

    std::cout << "Successfully wrote '0' to all bytes of the file." << std::endl;
    return 0;
};

int fileHandler::getFilenameFromConfigFile()
{
    filename = ReadStringFromEndOfFile(configFile);
    std::cout << __func__ << ": EPROM file " << filename << " loaded." << std::endl;
    return 0;
}

void fileHandler::WriteStringToEndOfFile(const std::string &str)
{
    std::ofstream file(configFile); //, std::ios::app);

    if (file.is_open())
    {
        file << str;
        file.close();
        std::cout << __func__ << ": veprom is added to the config file." << std::endl;
    }
    else
    {
        std::cerr << __func__ << ": Error: Unable to open config file for writing." << std::endl;
    }
}

std::string fileHandler::ReadStringFromEndOfFile(const std::string &fname)
{
    std::ifstream file(fname);
    std::string lastline{};

    if (file.is_open())
    {
        std::string line{};
        while (std::getline(file, line))
            ;
        {
            if (!line.empty())
            {
                lastline = line;
            }
        }
        file.close();
    }
    return lastline;
}
