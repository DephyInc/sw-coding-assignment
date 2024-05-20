rm data/*
./eprom create 2
./eprom create 3
./eprom create 2

./eprom load data/ROM02.bin
./eprom list
./eprom write test/test_story1.txt
./eprom list
./eprom write test/test_story2.txt
./eprom list
./eprom write test/test_story3.txt
./eprom list

./eprom read test_story1.txt
./eprom read test_story2.txt
./eprom read test_story3.txt

echo ""
echo "Verification: re-direct the output of read command to a file"
echo "$ ./eprom read test_story1.txt > test/test_story1_read.txt"
echo "$ ./eprom read test_story2.txt > test/test_story2_read.txt"
./eprom read test_story1.txt > test/test_story1_read.txt
./eprom read test_story2.txt > test/test_story2_read.txt

echo "Compare with 'diff input output' command"
echo "$ diff test/test_story1.txt test/test_story1_read.txt"
echo "$ diff test/test_story2.txt test/test_story2_read.txt"
diff test/test_story1.txt test/test_story1_read.txt
diff test/test_story2.txt test/test_story2_read.txt

