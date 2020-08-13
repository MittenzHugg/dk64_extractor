/*************************************************************************
*   Copyright (C) 2020 Michael (Mittenz) Salino-Hugg
*   Contact: Twitter: @MittenzSRL
**************************************************************************
*	n64_rom.h
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
#include <stdio.h>
#include <cstdint>
#include "n64_span.h"

//type definitions
typedef enum supported_games {
	UNKNOWN_GAME,
	BANJOKAZOOIE_NTSC,
	BANJOKAZOOIE_PAL,
	BANJOKAZOOIE_JP,
	BANJOKAZOOIE_NTSC_REV1,

	DONKEYKONG64_USA,
	DONKEYKONG64_JP,
	DONKEYKONG64_PAL,
	DONKEYKONG64_KIOSK,
}supported_games_t;

//static Constants
#define EXPANSION_CODE_ROM_OFFSET 0xFDAA30
#define EXPANSION_PAK_OFFSET 0x400000
#define EXPANSION_PAK_SIZE 0x400000

static const u32 N64_SIG = 0x40123780;
static const u32 V64_SIG = 0x37804012;
static const u32 Z64_SIG = 0x80371240;


//md5 hashes for detecting game
/*static const u64 BK_USA_HASH		= 0xB29599651A13F681;
static const u64 BK_PAL_HASH		= 0x06A43BACF5C0687F;
static const u64 BK_JP_HASH			= 0x3D3855A86FD5A1B4;
static const u64 BK_USA_REV1_HASH	= 0xB11F476D4BC8E039;*/

static const u64 DK64_USA_HASH		= 0x9ec41abf2519fc38;
//TODO static const u64 DK64_JP_HASH	= ;
//TODO static const u64 DK64_PAL_HASH	= ;
//TODO static const u64 DK64_KIOSK_HASH	= ;


class n64_rom
{

public:


	//int size;
	supported_games_t gameID;
	n64_span buffer;

	u64 getHash() const { return _hash; };
	n64_rom() = delete;
	n64_rom(char const* fileName);
    n64_rom(n64_rom&& src); //move constructor
	virtual ~n64_rom();

protected:
	//FILE* ROMFile;
	n64_span _header;
	n64_span _boot;
	uint8_t* _buffer = nullptr; //TODO make protected
	u64 _hash = 0;
};

