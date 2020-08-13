/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzSRL
**************************************************************************
*   dk64_asset.cpp
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
#include "dk64_asset.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

//libdeflate_compressor* dk64_asset::comper = libdeflate_alloc_compressor(12); 			//TODO: relax compression?
libdeflate_decompressor* dk64_asset::decomper = libdeflate_alloc_decompressor();

dk64_asset::dk64_asset(u32 type, u32 index, u32 offset, const n64_span& data, bool compressed, bool referenced) 
: n64_file(data, compressed)
, _type{type}
, _index{index}
, _offset{offset}
, _refer{referenced}{
	//initialization stuff
}

void dk64_asset::_comp_method() const
{
	n64_file::_comp_method(); //TMP: this just copies the decomp span ptr to the comp span ptr

	//TODO below is the BK compression method for example. Process will be similar

	////u8* tmpBuffer = (u8*)calloc(_decomp_buffer->size() + 0xC, sizeof(u8));
	////if (tmpBuffer == nullptr)
	////	throw "Bad memory allocation";

	////n64_span tmpSpan = n64_span(tmpBuffer, tmpBuffer + _decomp_buffer->size() + 0x06);

	////tmpSpan.set<u16>(0x1172);
	////u32 outsize = libdeflate_deflate_compress(comper, _decomp_buffer->begin(), _decomp_buffer->size(), tmpSpan.begin() + 0x06, tmpSpan.size() - 0x06);
	////if (outsize == 0)
	////	throw "Unable to compress code buffer.";
	////tmpSpan.set<u32>(outsize, 0x02);
	////size_t comp_size = outsize + 0x06;
	////tmpSpan.set<u16>(0x1172);

	////tmpBuffer = (u8*)realloc(tmpBuffer, comp_size);
	////if (tmpBuffer == nullptr)
	////	throw "Bad memory reallocation";

	////_comp_buffer = new n64_span(tmpBuffer, (size_t)comp_size);
	////_comp_own = true;

	return;
}

void dk64_asset::_decomp_method() const {
	if(_refer){
		throw "Asset reference.";
		return;
	}	
	if (_comp_buffer->size() == 0x0A){
		throw "File too small.";
		return;
	}
	if (_comp_buffer->get<u64>() == 0x1F8B080000000000 && _comp_buffer->get<u16>(8) != 0x0203){
		throw "Unknown file header.";
		return;
	}

	//find start //TODO: WHAT ARE THE values in here 
	u32 comp_begin = 0x0A;
	if (_comp_buffer->get<u32>() == 0x1F8B0808) {
		while (comp_begin < _comp_buffer->size() && _comp_buffer->operator[](comp_begin++) != 0);
		if(comp_begin >= _comp_buffer->size()){
			throw "Could not find compression start.";
		}
	}

	//std::cout << "Decompressing Asset " << _type << "." << _index << std::endl;
	uint8_t* tmpBuffer = (uint8_t*)malloc(0x800000);
	if (tmpBuffer == nullptr)
		throw "Bad memory allocation 1.";
	size_t tmpCompSize = _comp_buffer->size();
	if (tmpCompSize == 0) { return; }
	size_t tmpDecompSize;




	//decompress Code
	enum libdeflate_result decompResult = libdeflate_deflate_decompress_ex(decomper, _comp_buffer->begin() + comp_begin, _comp_buffer->size() - comp_begin
		, tmpBuffer, 0x800000, &tmpCompSize, &tmpDecompSize);
	if(decompResult){
		free(tmpBuffer);	
		throw "Unknown compression type.";
		return;
	}
	
	//decompress data
	tmpCompSize += comp_begin;
	tmpBuffer = (uint8_t*)realloc(tmpBuffer, tmpDecompSize);
	if (tmpBuffer == nullptr)
		throw "Bad memory reallocation 2.";
	_decomp_buffer = new n64_span(tmpBuffer, tmpDecompSize);
	_decomp_own = true;
	return;
	
}

std::vector<dk64_asset*> dk64_asset_section::parse(const n64_span& section){
	//seperate asset bins from asset section
	auto assets = std::vector<dk64_asset*>();

	std::vector<u32> type_offsets = section.to_vector<u32>(0,32);
	//std::vector<u32> type_info = section.to_vector<u32>(0x80, 32);

	auto type_end = std::remove_if(type_offsets.begin(), type_offsets.end(), 
		[](u32 x)->bool{return (x == 0);}
	);
	type_offsets.erase(type_end, type_offsets.end());

	//for(u32 type = 0; type < 4; type++ ){
	for(u32 type = 0; type < type_offsets.size(); type++ ){
		u32 offset = type_offsets[type];
		
		u32 first_file = section.get<u32>(offset) & 0x0FFFFFFF;
		std::vector<u32> file_offsets = section.slice(offset, first_file-offset).to_vector<u32>();
		auto file_list_end = std::find_first_of(file_offsets.begin(), file_offsets.end(),file_offsets.end(), file_offsets.end());
		file_offsets.erase(file_list_end, file_offsets.end());
		
		for(u32 iFile = 0; (iFile < file_offsets.size()-1); iFile++){
			u32 curr_offset = file_offsets[iFile] & 0x0FFFFFFF;
			bool ref = file_offsets[iFile] & 0x80000000;
			u32 curr_size = (file_offsets[iFile + 1] & 0x0FFFFFFF) - curr_offset;
			if(file_offsets[iFile] != file_offsets[iFile + 1]){
				assets.push_back(new dk64_asset(type, iFile, curr_offset
								 	 , section.slice(curr_offset, curr_size),true, ref));
			}
		}
	}
	return assets;
}