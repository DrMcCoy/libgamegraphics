/**
 * @file   test-filter-ccomic.cpp
 * @brief  Test code for Captain Comic RLE algorithm.
 *
 * Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>
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

#include <boost/test/unit_test.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <camoto/util.hpp>

#include "tests.hpp"
#include "test-filter.hpp"
#include "../src/filter-ccomic.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

struct ccomic_compress_sample: public filter_sample {
	ccomic_compress_sample()
	{
		this->filter.reset(new filter_ccomic_rle());
	}
};

struct ccomic_decompress_sample: public filter_sample {
	ccomic_decompress_sample()
	{
		this->filter.reset(new filter_ccomic_unrle());
	}
};

// A single encoded plane (expands to exactly 8000 bytes)
#define DATA_ENCODED_ONEPLANE \
	"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00" \
	"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00" \
	"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00" \
	"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00" \
	"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00" \
	"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00" \
	"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00" \
	"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFE\x00" \

#define DATA_ENCODED \
	"\x40\x1F" \
	DATA_ENCODED_ONEPLANE \
	DATA_ENCODED_ONEPLANE \
	DATA_ENCODED_ONEPLANE \
	DATA_ENCODED_ONEPLANE

#define EGAPLANE_BLANKLINE \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
	"\x00\x00\x00\x00\x00\x00\x00\x00"

#define EGAPLANE_10BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE \
	EGAPLANE_BLANKLINE

#define EGAPLANE_190LINES \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_10BLANKLINE

#define DATA_DECODED \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_190LINES \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_190LINES \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_190LINES \
	EGAPLANE_10BLANKLINE \
	EGAPLANE_190LINES

BOOST_FIXTURE_TEST_SUITE(ccomic_decompress_suite, ccomic_decompress_sample)

BOOST_AUTO_TEST_CASE(decode)
{
	BOOST_TEST_MESSAGE("Expand some Captain Comic RLE data");

	in << makeString(DATA_ENCODED);

	BOOST_CHECK_MESSAGE(is_equal(makeString(DATA_DECODED)),
		"Expanding Captain Comic RLE data failed");
}

BOOST_AUTO_TEST_CASE(decode_alt)
{
	BOOST_TEST_MESSAGE("Expand some Captain Comic RLE data");

	in << makeString(
		"\x40\x1F"
		"\x0F\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		"\xF0\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00\xFF\x00"
		"\xFF\x00\xFF\x00\xFF\x00"
		"\xFA\x00\x01\x00"
	);

	BOOST_CHECK_MESSAGE(is_equal(makeString(DATA_DECODED)),
		"Expanding Captain Comic RLE data failed");
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(ccomic_compress_suite, ccomic_compress_sample)

BOOST_AUTO_TEST_CASE(encode)
{
	BOOST_TEST_MESSAGE("RLE some data");

	BOOST_REQUIRE_EQUAL(makeString(DATA_DECODED).length(), 32000);

	in << makeString(DATA_DECODED);

	BOOST_CHECK_MESSAGE(is_equal(makeString(DATA_ENCODED)),
		"Encoding RLE data failed");
}

BOOST_AUTO_TEST_SUITE_END()
