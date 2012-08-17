/**
 * @file   test-img.hpp
 * @brief  Generic image conversion test code.
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

#include <boost/bind.hpp>
#include "test-img-defines.hpp"

// These constants should be defined in the file that #includes this one.

// Define if converting the mask should be tested.
//#define IMG_HAS_MASK

// Run particular code instead of the standard openImage() function below.
// Use this when testing image formats that aren't exposed via the library's
// public interface.
//#define IMG_CREATE_CODE  <C++ code>

// Number of bytes wide to display mismatched data in failed test when
// converting *from* standard data.  This should be set to the number of
// bytes in the image format that hold one row of data (if possible) to
// make error diagnosis easier.  Defaults to 8.
//#define IMG_DATA_WIDTH 8

using namespace camoto;
using namespace camoto::gamegraphics;

// Test images in standard (8bpp) format
const uint8_t stdformat_test_image_8x8[] = {
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A
};
const uint8_t stdformat_test_mask_8x8[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t stdformat_test_image_16x16[] = {
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,  0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,  0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A
};

const uint8_t stdformat_test_mask_16x16[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const uint8_t stdformat_test_image_9x9[] = {
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A
};
const uint8_t stdformat_test_mask_9x9[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t stdformat_test_image_8x4[] = {
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A,
	0x0C, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0A
};
const uint8_t stdformat_test_mask_8x4[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct FIXTURE_NAME: public default_sample {

	stream::string_sptr base;
	ImagePtr img;
	SuppData suppData;

	/// Number of bytes wide to display mismatched data in a failed test.
	/// Defaults to IMG_DATA_WIDTH or can be overridden when redefining
	/// IMG_CREATE_CODE.
	int dataWidth;

	FIXTURE_NAME()
		:	base(new stream::string())
	{
#ifdef IMG_DATA_WIDTH
		this->dataWidth = IMG_DATA_WIDTH;
#else
		this->dataWidth = 8;
#endif
	}

	void openImage(int width, int height)
	{
#ifdef IMG_OPEN_CODE
	IMG_OPEN_CODE
#else
		ManagerPtr pManager(getManager());
		ImageTypePtr pTestType(pManager->getImageTypeByCode(IMG_TYPE));
		BOOST_REQUIRE_MESSAGE(pTestType, "Invalid image code " IMG_TYPE);
		this->img = pTestType->open(this->base, this->suppData);
#endif
		BOOST_REQUIRE_MESSAGE(this->img, "Could not open image instance");
	}

	void createImage(int width, int height)
	{
#ifdef IMG_CREATE_CODE
	IMG_CREATE_CODE
#else
		ManagerPtr pManager(getManager());
		ImageTypePtr pTestType(pManager->getImageTypeByCode(IMG_TYPE));
		BOOST_REQUIRE_MESSAGE(pTestType, "Invalid image code " IMG_TYPE);
		this->img = pTestType->create(this->base, this->suppData);
#endif
		BOOST_REQUIRE_MESSAGE(this->img, "Could not create image instance");
	}

};

BOOST_FIXTURE_TEST_SUITE(SUITE_NAME, FIXTURE_NAME)

#define TO_STANDARD_TEST(w, h) \
BOOST_AUTO_TEST_CASE(TEST_NAME(to_standard_ ## w ## x ## h)) \
{ \
	BOOST_TEST_MESSAGE("Converting " TOSTRING(IMG_CLASS) " to stdformat " __STRING(w) "x" __STRING(h)); \
\
	std::string d = makeString(TESTDATA_INITIAL_ ## w ## x ## h); \
	this->base->open(&d); \
	this->openImage(w, h); \
\
	StdImageDataPtr output = this->img->toStandard(); \
\
	BOOST_CHECK_MESSAGE( \
		default_sample::is_equal( \
			stdformat_test_image_ ## w ## x ## h, \
			output.get(), \
			w * h, \
			w \
		), \
		"Error converting " __STRING(w) "x" __STRING(h) " image to standard format" \
	); \
}


TO_STANDARD_TEST(8, 8);
TO_STANDARD_TEST(16, 16);
TO_STANDARD_TEST(9, 9);
TO_STANDARD_TEST(8, 4);

// The image mask tests are only run for those image formats which actually
// have masks.
#ifdef IMG_HAS_MASK

#define TO_MASK_TEST(w, h) \
BOOST_AUTO_TEST_CASE(TEST_NAME(to_mask_ ## w ## x ## h)) \
{ \
	BOOST_TEST_MESSAGE("Converting " TOSTRING(IMG_CLASS) " to stdmask " __STRING(w) "x" __STRING(h)); \
\
	std::string d = makeString(TESTDATA_INITIAL_ ## w ## x ## h); \
	this->base->open(&d); \
	this->openImage(w, h); \
\
	StdImageDataPtr output = this->img->toStandardMask(); \
\
	BOOST_CHECK_MESSAGE( \
		default_sample::is_equal( \
			stdformat_test_mask_ ## w ## x ## h, \
			output.get(), \
			w * h, \
			w \
		), \
		"Error converting " __STRING(w) "x" __STRING(h) " image to standard mask format" \
	); \
}

TO_MASK_TEST(8, 8);
TO_MASK_TEST(16, 16);
TO_MASK_TEST(9, 9);
TO_MASK_TEST(8, 4);

#endif // IMG_HAS_MASK

#define FROM_STANDARD_TEST(w, h) \
BOOST_AUTO_TEST_CASE(TEST_NAME(from_standard_ ## w ## x ## h)) \
{ \
	BOOST_TEST_MESSAGE("Converting stdmask " __STRING(w) "x" __STRING(h) " to " TOSTRING(IMG_CLASS)); \
\
	StdImageDataPtr stddata(new uint8_t[w*h]); \
	memcpy(stddata.get(), stdformat_test_image_ ## w ## x ## h, w*h); \
\
	StdImageDataPtr stdmask(new uint8_t[w*h]); \
	memcpy(stdmask.get(), stdformat_test_mask_ ## w ## x ## h, w*h); \
\
	this->createImage(w, h); \
\
	if (this->img->getCaps() & Image::CanSetDimensions) { \
		this->img->setDimensions(w, h); \
	} \
	this->img->fromStandard(stddata, stdmask); \
\
	int targetSize = sizeof(TESTDATA_INITIAL_ ## w ## x ## h) - 1; \
	BOOST_CHECK_MESSAGE( \
		default_sample::is_equal( \
			(const uint8_t *)TESTDATA_INITIAL_ ## w ## x ## h, \
			targetSize, \
			this->base->str(), \
			this->dataWidth \
		), \
		"Error converting " __STRING(w) "x" __STRING(h) " standard format to " TOSTRING(IMG_CLASS) \
	); \
\
	/* Make sure the right amount of data was written out */ \
	BOOST_REQUIRE_EQUAL(this->base->str().length(), targetSize); \
}

FROM_STANDARD_TEST(8, 8)
FROM_STANDARD_TEST(16, 16)
FROM_STANDARD_TEST(9, 9)
FROM_STANDARD_TEST(8, 4)

BOOST_AUTO_TEST_SUITE_END()
