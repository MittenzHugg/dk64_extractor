//#include "n64/n64_rom.h"
#include "dk64/dk64_rom.h"
#include <iostream>
#include <string>

int main(int argc,  char * argv[]){
	dk64_rom* dk64Rom;
	
	if(argc != 2){
		std::cout << "Usage: dk_extractor <path_to_dk64_rom>\n";
		return 0;
	}
	std::string rom_file = std::string(argv[1]);
	std::cout << "Checking ROM: " << std::string(argv[1]) << "\n";
	try{
		dk64Rom = new dk64_rom(rom_file.c_str());
	}
	catch (char const* s){
		std::cout << "Error: " << s << std::endl;
	}
	
	
	return 0;
}
