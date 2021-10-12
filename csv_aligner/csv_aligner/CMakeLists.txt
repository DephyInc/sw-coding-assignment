# CMakeList.txt : CMake project for csv_aligner, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (csv_aligner "csv_aligner.cpp"  "Data.cpp" "Data.h" "DataProcessor.h")

# TODO: Add tests and install targets if needed.
