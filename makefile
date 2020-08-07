CXX = g++
CXXFLAGS = -Wall -g -std=c++20

OBJ_DIR = build
N64_DIR = src/n64
DK64_DIR = src/dk64

main: dk_extractor.o dk64_rom.o dk64_asset.o n64_rom.o n64_rom.o md5.o
	$(CXX) $(CXXFLAGS) -o dk_extractor $(OBJ_DIR)/dk_extractor.o $(OBJ_DIR)/dk64_rom.o $(OBJ_DIR)/dk64_asset.o $(OBJ_DIR)/n64_file.o $(OBJ_DIR)/n64_rom.o $(OBJ_DIR)/md5.o 

clean:
	rm -rf *.o

dk_extractor.o: src/dk_extractor.cpp $(DK64_DIR)/dk64_rom.h dk64_rom.o
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/dk_extractor.o -c src/dk_extractor.cpp

dk64_rom.o: $(DK64_DIR)/dk64_rom.cpp $(N64_DIR)/n64_rom.h n64_rom.o
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/dk64_rom.o -c $(DK64_DIR)/dk64_rom.cpp

dk64_asset.o: $(DK64_DIR)/dk64_asset.cpp $(N64_DIR)/n64_file.h n64_file.o
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/dk64_asset.o -c $(DK64_DIR)/dk64_asset.cpp

n64_file.o:  $(N64_DIR)/n64_file.cpp
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/n64_file.o -c $(N64_DIR)/n64_file.cpp

n64_rom.o: $(N64_DIR)/n64_rom.cpp $(N64_DIR)/md5.h md5.o
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/n64_rom.o -c $(N64_DIR)/n64_rom.cpp

md5.o: $(N64_DIR)/md5.c $(N64_DIR)/md5.h
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/md5.o -c $(N64_DIR)/md5.c