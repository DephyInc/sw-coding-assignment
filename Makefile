
CC=g++
CCFLAGS=-g
LDFLAGS=

SRC_DIR=src
BLD_DIR=build
OBJ_DIR=build/obj

TARGET=$(BLD_DIR)/veprom

SOURCES=$(SRC_DIR)/main.cpp \
	$(SRC_DIR)/create_command.cpp \
	$(SRC_DIR)/load_command.cpp \
	$(SRC_DIR)/list_command.cpp \
	$(SRC_DIR)/erase_command.cpp \
	$(SRC_DIR)/read_raw_command.cpp \
	$(SRC_DIR)/write_raw_command.cpp \
	$(SRC_DIR)/read_file_command.cpp \
	$(SRC_DIR)/write_file_command.cpp \
	$(SRC_DIR)/virtual_eprom.cpp


XOBJECTS=$(SOURCES:.cpp=.o)
OBJECTS=$(subst $(SRC_DIR),$(OBJ_DIR),$(XOBJECTS))

all: $(TARGET)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BLD_DIR)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BLD_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< $(CCFLAGS) -o $@
