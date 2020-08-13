/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzSRL
**************************************************************************
*   dk64_rom.cpp
*
*   This file is part of Mr.Patcher.
*
*   Mr.Patcher is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   Mr.Patcher is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with Mr.Patcher.  If not, see <https://www.gnu.org/licenses/>.
*************************************************************************/
#include "dk64_rom.h"
#include "dk64_asset.h"
#include <utility>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
namespace fs = std::filesystem;

dk64_rom::dk64_rom(char* filename)
	:n64_rom(filename)
{
	if (gameID != supported_games_t::DONKEYKONG64_USA
		//TODO other versions
		) {
		delete this;
		throw "Not a recognized version of Donkey Kong 64";
		return;
	}
	std::cout << "N64 ROM identified as Donkey Kong 64" << std::endl;
	_init();
}

dk64_rom::dk64_rom(n64_rom&& src)
	:n64_rom(std::move(src))
{
	if (gameID != supported_games_t::DONKEYKONG64_USA
		//TODO other versions
		) {
		delete this;
		throw "Not a recognized version of Donkey Kong 64";
		return;
	}

	_init();
}

dk64_rom::~dk64_rom()
{
}



void dk64_rom::_write_bin(std::string str, const n64_span& span){
	std::stringstream stream;
	stream << std::setfill ('0') << std::setw(8) << std::hex << &span[0]-_buffer;
	std::string bin_str = str + stream.str() + ".bin";
	//std::cout << bin_str << std::endl;
	std::fstream of(bin_str, std::ios::out | std::ios::binary);
	of.write((char*) &span[0], span.size());
	of.close();
};

void dk64_rom::_write_bin(std::string str, dk64_asset* asset, std::string ext_str){
		std::stringstream stream;
		stream << std::setfill ('0') << std::setw(8) << std::hex << &asset->compress()[0] -_buffer;
		std::string bin_str = str + stream.str() + ext_str + ".bin";
	try{
		asset->decompress();
		std::fstream of(bin_str, std::ios::out | std::ios::binary);
		of.write((char*) &(asset->decompress())[0], asset->decompress().size());
		of.close();
	}
	catch (char const* s){
		std::cout << "Error: " << bin_str << ": " << s;
		if(asset->compress().size() > 0){
			std::cout << " Exporting compressed bin" << std::endl;
			std::fstream of(str + stream.str() + ".raw.bin", std::ios::out | std::ios::binary);
			of.write((char*) &(asset->compress())[0], asset->compress().size());
			of.close();
		}
	}
	
};

void dk64_rom::export_files(char* export_path){

	std::cout << "Exporting ROM to " << export_path << std::endl;
	fs::create_directory(export_path);
	std::string x_path(export_path);

	_write_bin(x_path + "/n64_header.", _header);

	_write_bin(x_path + "/n64_boot.", _boot);

	_write_bin(x_path + "/dk64_boot.", _bootcode_span);

	_write_bin(x_path + "/exp_pak_pic.", _exp_pak_pic_span);

	std::cout << "Exporting asm ..." << std::endl;
	std::cout << "Coming Soon" << std::endl;
	fs::create_directory(x_path + "/asm");
	std::for_each(_asm_code.begin(), _asm_code.end(),
		[&](dk64_asset*x)->void{ _write_bin(x_path + "/asm/", x,".s"); }
	);

	std::for_each(_asm_data.begin(), _asm_data.end(),
		[&](dk64_asset*x)->void{ _write_bin(x_path + "/asm/", x,".d"); }
	);


	std::cout << "Exporting assets ..." << std::endl;
	std::string asset_path = std::string(export_path) + "/assets";
	fs::create_directory(asset_path);

	for(int i = 0; i < 32; i++){
		std::string path = asset_path + "/" + std::to_string(i);
		if(std::any_of(_assets.begin(), _assets.end(),[i](dk64_asset* x)->bool{return x->_type == i;})){
			fs::create_directory(path);
		
			std::for_each(_assets.begin(), _assets.end(),
			[&](dk64_asset* x){
					if(x->_type == i)
						_write_bin(path + "/", x);
				}
			);
		}
	}
	
	
	

}


void dk64_rom::_init(void)
{
	//77C - CRC Check 1
	//	788 - CRC Check 2

	//	1000->E8AC - Low - Level / N64 OS Functions(decompressed)
	_bootcode_span = buffer.slice(0x1000, 0xE8AC - 0x1000);

	//	EA88->F908 - Expansion Pak Pic
	_exp_pak_pic_span = buffer.slice(0xEA88, 0xF908 - 0xEA88);


	///*	-Assets and Assembly -
	std::cout << "Finding asm..." << std::endl;
	u32 tmp = 0;
	// Globals Assembly Code
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0x113F0, buffer.slice(0x113F0, 0xC29D4 - 0x113F0)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xC29D4, buffer.slice(0xC29D4, 0xCBE70 - 0xC29D4)));

	// Affects OKAY!, Main Menu(DONKEY KONG!), and shops
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xCBE70, buffer.slice(0xCBE70, 0xD4554 - 0xCBE70)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xD4554, buffer.slice(0xD4554, 0xD4B00 - 0xD4554)));

	//Multiplayer Mode
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xD4B00, buffer.slice(0xD4B00, 0xD69F8 - 0xD4B00)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xD69F8, buffer.slice(0xD69F8, 0xD6B00 - 0xD69F8)));

	//Mincarts
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xD6B00, buffer.slice(0xD6B00, 0xD98A0 - 0xD6B00)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xD98A0, buffer.slice(0xD98A0, 0xD9A40 - 0xD98A0)));

	//Rambi / Enguarde arena / Minigames
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xD9A40, buffer.slice(0xD9A40, 0xDF346 - 0xD9A40)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xDF346, buffer.slice(0xDF346, 0xDF600 - 0xDF346)));

	//Slide / Car / Boat Race
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xDF600, buffer.slice(0xDF600, 0xE649A - 0xDF600)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xE649A, buffer.slice(0xE649A, 0xE6780 - 0xE649A)));

	//Training Barrels, Tag Barrels, DK RAP
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xE6780, buffer.slice(0xE6780, 0xE9D17 - 0xE6780)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xE9D17, buffer.slice(0xE9D17, 0xEA0B0 - 0xE9D17)));

	//Boss
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xEA0B0, buffer.slice(0xEA0B0, 0xF388F - 0xEA0B0)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xF388F, buffer.slice(0xF388F, 0xF41A0 - 0xF388F)));
	
	//DK Arcade
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xF41A0, buffer.slice(0xF41A0, 0xFB42C - 0xF41A0)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0xFB42C, buffer.slice(0xFB42C, 0xFD2F0 - 0xFB42C)));
	
	//Jetpac
	_asm_code.push_back(new dk64_asset(0xDEADBEEF, tmp, 0xFD2F0, buffer.slice(0xFD2F0, 0x1010FD - 0xFD2F0)));
	_asm_data.push_back(new dk64_asset(0xDEADBEEF, tmp++, 0x1010FD, buffer.slice(0x1010FD, 0x101C50 - 0x1010FD)));


	///*-Main Pointer Table -
	//	Initial Address : 0x101C50*/
	std::cout << "Finding assets..." << std::endl;
	_asset_table = buffer.slice(0x101C50, buffer.size() - 0x101C50);
	_assets = dk64_asset_section::parse(_asset_table);
	std::cout << std::dec << _assets.size() << " assets found" << std::endl;
}
