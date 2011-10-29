/**
 * @file   test-img-pcx-8b1p.cpp
 * @brief  Test code for PCX file handler (with VGA linear image data.)
 *
 * Copyright (C) 2010-2011 Adam Nielsen <malvineous@shikadi.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <camoto/gamegraphics/image.hpp>
#include "../src/img-pcx.hpp"

using namespace camoto::gamegraphics;
using namespace camoto;

#define PCX_PAL \
	"\x00\x00\x00" \
	"\x00\x00\xAA" \
	"\x00\xAA\x00" \
	"\x00\xAA\xAA" \
	"\xAA\x00\x00" \
	"\xAA\x00\xAA" \
	"\xAA\x55\x00" \
	"\xAA\xAA\xAA" \
	"\x55\x55\x55" \
	"\x55\x55\xFF" \
	"\x55\xFF\x55" \
	"\x55\xFF\xFF" \
	"\xFF\x55\x55" \
	"\xFF\x55\xFF" \
	"\xFF\xFF\x55" \
	"\xFF\xFF\xFF"

#define PCX_PAD \
	"\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00"

#define TESTDATA_INITIAL_8x8 \
	"\x0A\x05\x01" \
	"\x08" \
	"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00" \
	"\x4B\x00" "\x4B\x00" \
	PCX_PAL \
	"\x00" \
	"\x01" \
	"\x08\x00" "\x01\x00" \
	"\x00\x00" "\x00\x00" \
	PCX_PAD \
	"\xC8\x0F" \
	"\x0C\xC6\x00\x0A" \
	"\x0C\xC6\x00\x0A" \
	"\x0C\xC6\x00\x0A" \
	"\x0C\xC6\x00\x0A" \
	"\x0C\xC6\x00\x0A" \
	"\x0C\xC6\x00\x0A" \
	"\x0C\xC6\x09\x0A"

#define TESTDATA_INITIAL_16x16 \
	"\x0A\x05\x01" \
	"\x08" \
	"\x00\x00" "\x00\x00" "\x0F\x00" "\x0F\x00" \
	"\x4B\x00" "\x4B\x00" \
	PCX_PAL \
	"\x00" \
	"\x01" \
	"\x10\x00" "\x01\x00" \
	"\x00\x00" "\x00\x00" \
	PCX_PAD \
	"\xD0\x0F" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x00\x0A" \
	"\x0C\xCE\x09\x0A"

#define TESTDATA_INITIAL_9x9 \
	"\x0A\x05\x01" \
	"\x08" \
	"\x00\x00" "\x00\x00" "\x08\x00" "\x08\x00" \
	"\x4B\x00" "\x4B\x00" \
	PCX_PAL \
	"\x00" \
	"\x01" \
	"\x0A\x00" "\x01\x00" \
	"\x00\x00" "\x00\x00" \
	PCX_PAD \
	"\xCA\x0F" \
	"\x0C\xC7\x00\x0A\x0A" \
	"\x0C\xC7\x00\x0A\x0A" \
	"\x0C\xC7\x00\x0A\x0A" \
	"\x0C\xC7\x00\x0A\x0A" \
	"\x0C\xC7\x00\x0A\x0A" \
	"\x0C\xC7\x00\x0A\x0A" \
	"\x0C\xC7\x00\x0A\x0A" \
	"\x0C\xC7\x09\x0A\x0A"

#define TESTDATA_INITIAL_8x4 \
	"\x0A\x05\x01" \
	"\x08" \
	"\x00\x00" "\x00\x00" "\x07\x00" "\x03\x00" \
	"\x4B\x00" "\x4B\x00" \
	PCX_PAL \
	"\x00" \
	"\x01" \
	"\x08\x00" "\x01\x00" \
	"\x00\x00" "\x00\x00" \
	PCX_PAD \
	"\xC8\x0F" \
	"\x0C\xC6\x00\x0A" \
	"\x0C\xC6\x00\x0A" \
	"\x0C\xC6\x09\x0A" \

#define IMG_TYPE "img-pcx-8b1p"
#define IMG_CLASS img_pcx_8b1p
#include "test-img.hpp"

// Test some invalid formats to make sure they're not identified as valid
// files.  Note that they can still be opened though (by 'force'), this
// only checks whether they look like valid files or not.

ISINSTANCE_TEST(c00, TESTDATA_INITIAL_8x8, DefinitelyYes);

// Bad signature
ISINSTANCE_TEST(c01,
	"\x0B\x05\x01"
	"\x08"
	"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
	"\x4B\x00" "\x4B\x00"
	PCX_PAL
	"\x00"
	"\x01"
	"\x08\x00" "\x01\x00"
	"\x00\x00" "\x00\x00"
	PCX_PAD
	"\xC8\x0F"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x09\x0A",
	DefinitelyNo
);

// Unsupported version
ISINSTANCE_TEST(c02,
	"\x0A\x06\x01"
	"\x08"
	"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
	"\x4B\x00" "\x4B\x00"
	PCX_PAL
	"\x00"
	"\x01"
	"\x08\x00" "\x01\x00"
	"\x00\x00" "\x00\x00"
	PCX_PAD
	"\xC8\x0F"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x09\x0A",
	DefinitelyNo
);

// Wrong bits-per-plane
ISINSTANCE_TEST(c03,
	"\x0A\x05\x01"
	"\x09"
	"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
	"\x4B\x00" "\x4B\x00"
	PCX_PAL
	"\x00"
	"\x01"
	"\x08\x00" "\x01\x00"
	"\x00\x00" "\x00\x00"
	PCX_PAD
	"\xC8\x0F"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x09\x0A",
	DefinitelyNo
);

// Wrong plane count
ISINSTANCE_TEST(c04,
	"\x0A\x05\x01"
	"\x08"
	"\x00\x00" "\x00\x00" "\x07\x00" "\x07\x00"
	"\x4B\x00" "\x4B\x00"
	PCX_PAL
	"\x00"
	"\x09"
	"\x08\x00" "\x01\x00"
	"\x00\x00" "\x00\x00"
	PCX_PAD
	"\xC8\x0F"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x00\x0A"
	"\x0C\xC6\x09\x0A",
	DefinitelyNo
);