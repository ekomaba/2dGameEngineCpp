## Variables
CC = g++
LANG_STD = -std=c++17
COMPILER_FLAGS_DEBUG = -Wall -Wfatal-errors -g -DDEBUG
COMPILER_FLAGS_RELEASE = -Wall -O2
INCLUDE_PATH = -I"./libs/"
SRC_FILES = ./src/*.cpp \
			./src/Game/*.cpp \
			./src/Logger/*.cpp
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3
OBJ_NAME = gameengine			

## Declare some Makefile rules
debug:
	$(CC) $(COMPILER_FLAGS_DEBUG) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS) -o $(OBJ_NAME)

release:
	$(CC) $(COMPILER_FLAGS_RELEASE) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS) -o $(OBJ_NAME)

run:
	./$(OBJ_NAME)

clean:
	rm $(OBJ_NAME)