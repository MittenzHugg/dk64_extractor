/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzSRL
**************************************************************************
*   dk64_asset.h
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

#pragma once
#include "../n64/n64_file.h"
#include "libdeflate.h"
#include <vector>

class dk64_asset :
	public n64_file
{
public:
	dk64_asset(){};
	//TODO dk64_asset(dk64_asset&& src); //move constructor
	//TODO dk64_asset(const dk64_asset& src); //copy constructor
	//dk64_asset(u32 indx, u32 offset, u16 type, const n64_span& data, u16 compressed = true);
	dk64_asset(u32 type, u32 index, u32 offset, const n64_span& data, bool compressed = true, bool referenced = false);
	~dk64_asset(){n64_file::~n64_file();};
	//TODO dk64_asset& operator = (const dk64_asset& other) {}; //assignment operator
	u32 _offset;
	u32 _index;
	u8 _type;
private:
	
	
	
	bool _refer;

	//static libdeflate_compressor* comper;
	static libdeflate_decompressor* decomper;

	void _comp_method() const override;
	void _decomp_method() const override;
};

class dk64_type_section{
public:
	dk64_type_section(const u32 type, const u32 offset, const n64_span& asset_sec);
	//std::vector<const dk64_asset*> assets(void);
	//void export_bins(char* o_path, bool compressed = true);

private:
	const u32 _type; 
	std::vector<u32> _header;
	std::vector<const dk64_asset*> _assets;
};

class dk64_asset_section {
public: //helper functions for parsing/building entire asset section
	static u32 ROM_offset;
	static std::vector<dk64_asset*> parse(const n64_span& section);
	//TODO static const n64_span& build(std::vector<const dk64_asset*>& assets);
};