all:main.o process_cmd.o
	g++ -o veprom main.o process_cmd.o
main.o: main.cpp
	g++ -c main.cpp
process_cmd.o: enum.h process_cmd.h process_cmd.cpp
	g++ -c process_cmd.h process_cmd.cpp
clean:
	rm *.o
