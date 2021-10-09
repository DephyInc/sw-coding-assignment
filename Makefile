# Makefile for csv_aligner


csv_aligner:
	g++ ./src/datamanager.cpp ./src/main.cpp ./src/matrices.cpp ./src/regressor.cpp -g3 -Wall -o csv_aligner
