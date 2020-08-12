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

//libdeflate_compressor* dk64_asset::comper = libdeflate_alloc_compressor(12); 			//TODO: relax compression?
libdeflate_decompressor* dk64_asset::decomper = libdeflate_alloc_decompressor();

/*dk64_asset::dk64_asset(u32 indx, u32 offset, u16 type, const n64_span& data, u16 compressed = true) 
: n64_file(data, compressed) {

}*/

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
	uint8_t* tmpBuffer = (uint8_t*)malloc(0x800000);
	if (tmpBuffer == nullptr)
		throw "Bad memory allocation.";
	size_t tmpCompSize = _comp_buffer->size();
	if (tmpCompSize == 0) { return; }
	size_t tmpDecompSize;


	//find start //TODO: WHAT ARE THE values in here 
	u32 comp_begin = 0x0A;
	if (_comp_buffer->get<u32>() == 0x1F8B0808) {
		while (_comp_buffer->operator[](comp_begin++) != 0);
	}

	//decompress Code
	enum libdeflate_result decompResult = libdeflate_deflate_decompress_ex(decomper, _comp_buffer->begin() + comp_begin, _comp_buffer->size() - comp_begin
		, tmpBuffer, 0x800000, &tmpCompSize, &tmpDecompSize);
	//decompress data
	tmpCompSize += comp_begin;
	tmpBuffer = (uint8_t*)realloc(tmpBuffer, tmpDecompSize);
	if (tmpBuffer == nullptr)
		throw "Bad memory reallocation.";
	_decomp_buffer = new n64_span(tmpBuffer, tmpDecompSize);
	_decomp_own = true;
	return;
	
}

std::vector<const dk64_asset*> dk64_asset_section::parse(const n64_span& table){
	//seperate asset bins from asset table
	for(int i = 0; i < 32; i++){
		u32 _asset_type_offset = table.get<u32>(i*sizeof(u32));
		if(_asset_type_offset == 0) break;
		u32 _next_asset_type_offset = table.get<u32>((i+1)*sizeof(u32));
		if(_next_asset_type_offset == 0) break;

		u32 _asset_type_type = table.get<u32>((i+32)*sizeof(u32));
		
		
		std::cout << "Offset = " << _asset_type_offset << std::endl;
		const n64_span _asset_type_span = table.slice(_asset_type_offset, table.get<u32>((i+1)*sizeof(u32)) -  _asset_type_offset);
		if(_asset_type_offset){
			std::cout << "Extracting File 0x" << std::hex << (0x101C50 + _asset_type_offset) << std::endl;
			for(int i_file = 0; i_file < 32; i_file++){
				std::cout << std::hex << (0x101C50 + _asset_type_offset + i_file*4) << ": " << std::hex << _asset_type_span.get<u32>((i_file*sizeof(u32))) << std::endl;
			}
		}
	} 
	return std::vector<const dk64_asset*>();
}