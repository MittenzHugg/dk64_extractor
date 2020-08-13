ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    CC = x86_64-w64-mingw32-gcc
else
    CC = gcc
endif
CXX = g++
CXXFLAGS = -Wall -g -std=c++2a

OBJ_DIR = build
N64_DIR = src/n64
DK64_DIR = src/dk64


SRCS = dk_extractor.cpp
OBJS = $(addprefix build/,$(pathsubst %.c, %.o, $(SRCS)))



main: make_libdeflate build/dk_extractor.o dk64_rom.o dk64_asset.o n64_file.o n64_rom.o md5.o
	$(CXX) $(CXXFLAGS) -o dk_extractor $(OBJ_DIR)/dk_extractor.o $(OBJ_DIR)/dk64_rom.o $(OBJ_DIR)/dk64_asset.o $(OBJ_DIR)/n64_file.o $(OBJ_DIR)/n64_rom.o $(OBJ_DIR)/md5.o libdeflate/libdeflate.a

clean:
	rm -rf build/*.o
	rm -f dk_extractor

make_libdeflate:
	echo $(OS)
	echo $(CC)
	cd libdeflate && make CC=$(CC)
	cp libdeflate/libdeflate.h src/dk64/libdeflate.h

build/.:
	mkdir -p $@

build/%/.:
	mkdir -p $@

build/dk_extractor.o: src/dk_extractor.cpp | build/.
	$(CXX) $(CXXFLAGS) -o $@ -c src/dk_extractor.cpp

dk64_rom.o: $(DK64_DIR)/dk64_rom.cpp
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/dk64_rom.o -c $(DK64_DIR)/dk64_rom.cpp

dk64_asset.o: $(DK64_DIR)/dk64_asset.cpp libdeflate/libdeflate.h
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/dk64_asset.o -c $(DK64_DIR)/dk64_asset.cpp -Ilibdeflate

n64_file.o:  $(N64_DIR)/n64_file.cpp
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/n64_file.o -c $(N64_DIR)/n64_file.cpp

n64_rom.o: $(N64_DIR)/n64_rom.cpp
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/n64_rom.o -c $(N64_DIR)/n64_rom.cpp

md5.o: $(N64_DIR)/md5.c $(N64_DIR)/md5.h
	$(CXX) $(CXXFLAGS) -o $(OBJ_DIR)/md5.o -c $(N64_DIR)/md5.c

