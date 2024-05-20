rm data/*
./eprom create 2
./eprom create 2
./eprom create 2

./eprom load data/ROM02.bin
./eprom write_raw 100 abcdefghijklmnopqrstuvwxyz
./eprom read_raw 100 26
./eprom read_raw 100 10
./eprom read_raw 110 10

# write violation error
./eprom write_raw 2040 abcdefghijklmnopqrstuvwxyz 

# read violation error
./eprom read_raw 2040 26 

# should read nothing
./eprom read_raw 2040 8 

./eprom write_raw 2040 abcdefgh
./eprom read_raw 2040 8 

./eprom erase
./eprom read_raw 100 26



