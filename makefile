vEPROM: EPROM.o
	c++ vEPROM_Chip_Reader.o EPROM.o main.cpp -o vEPROM

EPROM.o: vEPROM_Chip_Reader.o
	c++ -c EPROM.cpp -o EPROM.o

vEPROM_Chip_Reader.o: vEPROM_Chip_Reader.cpp
	c++ -c vEPROM_Chip_Reader.cpp -o vEPROM_Chip_Reader.o


