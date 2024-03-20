veprom create 20
pause
veprom create 10
pause
veprom create 10
pause
veprom load 1
pause 
cat dephy.cfg
pause 
veprom load 2
pause 
cat dephy.cfg
pause
veprom write dephy.cfg
pause
veprom list
pause
veprom read_raw 4 9
pause
veprom write_raw 4 dephy.ini
pause
veprom list
pause
veprom write data.txt
pause
veprom write data1.txt
pause
veprom list
pause
veprom write data2.txt
pause
veprom write data.txt
pause
veprom list
pause
veprom write data.txt
pause
veprom read_raw 4 9
pause
veprom read dephy.cfg
pause
veprom read data.txt
pause
veprom erase
pause
veprom list