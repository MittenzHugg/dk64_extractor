/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzSRL
**************************************************************************
*   dk64_rom.h
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
#include "../n64/n64_rom.h"
#include "../n64/n64_span.h"
#include "dk64_asset.h"

class dk64_rom :
	public n64_rom
{
public:
	dk64_rom(char* filename);
	dk64_rom(n64_rom&& src);
	~dk64_rom();

	void export_binaries(char* export_path);
	void export_files(char* export_path);

private:
	void _init(void);

	void _write_bin(std::string str, const n64_span& span);
	
	void _write_bin(std::string str, dk64_asset* asset, std::string ext_str);
	void _write_bin(std::string str, dk64_asset* asset){_write_bin(str, asset, "");};

	n64_span _bootcode_span;

	n64_span _exp_pak_pic_span;

	n64_span _asset_table;
	std::vector<dk64_asset*> _assets = {};
	std::vector<dk64_asset*> _asm_code = {};
	std::vector<dk64_asset*> _asm_data = {};
};

