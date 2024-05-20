rm data/*
./eprom load data/ROM01.bin

echo "Current epron.ini target:"
cat data/eprom.ini

./eprom create 2

echo "Current epron.ini target:"
cat data/eprom.ini

./eprom load data/ROM01.bin

echo "Current epron.ini target:"
cat data/eprom.ini

./eprom create 22

echo "Current epron.ini target:"
cat data/eprom.ini

./eprom create 25

echo "Current epron.ini target:"
cat data/eprom.ini