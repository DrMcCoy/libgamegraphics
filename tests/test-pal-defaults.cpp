/**
 * @file   test-pal-defaults.cpp
 * @brief  Test code for the default palette generation code.
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
#include <boost/bind.hpp>

#include <camoto/gamegraphics/palettetable.hpp>

#include "tests.hpp"

using namespace camoto;
using namespace camoto::gamegraphics;

/// Require a palette entry to contain the given red, green and blue values
#define REQUIRE_PAL(i, r, g, b) \
	BOOST_REQUIRE_EQUAL((*pal)[i].red,    r); \
	BOOST_REQUIRE_EQUAL((*pal)[i].green,  g); \
	BOOST_REQUIRE_EQUAL((*pal)[i].blue,   b);

BOOST_AUTO_TEST_CASE(pal_full_cga)
{
	BOOST_TEST_MESSAGE("Generate full CGA palette");

 PaletteTablePtr pal = createPalette_FullCGA();

	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x00, 0x00, 0xAA);
	REQUIRE_PAL( 6, 0xAA, 0x55, 0x00);
	REQUIRE_PAL(14, 0xFF, 0xFF, 0x55);
	REQUIRE_PAL(15, 0xFF, 0xFF, 0xFF);
}

BOOST_AUTO_TEST_CASE(pal_full_ega)
{
	BOOST_TEST_MESSAGE("Generate full EGA palette");

	PaletteTablePtr pal = createPalette_FullEGA();

	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x00, 0x00, 0xAA);
	REQUIRE_PAL( 6, 0xAA, 0xAA, 0x00);
	REQUIRE_PAL(20, 0xAA, 0x55, 0x00);
	REQUIRE_PAL(33, 0x55, 0x00, 0xAA);
	REQUIRE_PAL(46, 0xFF, 0xAA, 0x55);
	REQUIRE_PAL(62, 0xFF, 0xFF, 0x55);
	REQUIRE_PAL(63, 0xFF, 0xFF, 0xFF);
}

BOOST_AUTO_TEST_CASE(pal_default_cga)
{
	BOOST_TEST_MESSAGE("Generate CGA palettes");

	PaletteTablePtr pal;

	pal = createPalette_CGA(CGAPal_GreenRed);
	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x00, 0xAA, 0x00);
	REQUIRE_PAL( 2, 0xAA, 0x00, 0x00);
	REQUIRE_PAL( 3, 0xAA, 0x55, 0x00);

	pal = createPalette_CGA(CGAPal_GreenRedBright);
	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x55, 0xFF, 0x55);
	REQUIRE_PAL( 2, 0xFF, 0x55, 0x55);
	REQUIRE_PAL( 3, 0xFF, 0xFF, 0x55);

	pal = createPalette_CGA(CGAPal_CyanMagenta);
	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x00, 0xAA, 0xAA);
	REQUIRE_PAL( 2, 0xAA, 0x00, 0xAA);
	REQUIRE_PAL( 3, 0xAA, 0xAA, 0xAA);

	pal = createPalette_CGA(CGAPal_CyanMagentaBright);
	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x55, 0xFF, 0xFF);
	REQUIRE_PAL( 2, 0xFF, 0x55, 0xFF);
	REQUIRE_PAL( 3, 0xFF, 0xFF, 0xFF);

	pal = createPalette_CGA(CGAPal_CyanRed);
	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x00, 0xAA, 0xAA);
	REQUIRE_PAL( 2, 0xAA, 0x00, 0x00);
	REQUIRE_PAL( 3, 0xAA, 0xAA, 0xAA);

	pal = createPalette_CGA(CGAPal_CyanRedBright);
	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x55, 0xFF, 0xFF);
	REQUIRE_PAL( 2, 0xFF, 0x55, 0x55);
	REQUIRE_PAL( 3, 0xFF, 0xFF, 0xFF);

	// Try different background colours
	pal = createPalette_CGA((CGAPaletteType)(CGAPal_GreenRed | 12));
	REQUIRE_PAL( 0, 0xFF, 0x55, 0x55);
	REQUIRE_PAL( 1, 0x00, 0xAA, 0x00);
	REQUIRE_PAL( 2, 0xAA, 0x00, 0x00);
	REQUIRE_PAL( 3, 0xAA, 0x55, 0x00);

	pal = createPalette_CGA((CGAPaletteType)(CGAPal_GreenRedBright | 9));
	REQUIRE_PAL( 0, 0x55, 0x55, 0xFF);
	REQUIRE_PAL( 1, 0x55, 0xFF, 0x55);
	REQUIRE_PAL( 2, 0xFF, 0x55, 0x55);
	REQUIRE_PAL( 3, 0xFF, 0xFF, 0x55);
}

BOOST_AUTO_TEST_CASE(pal_default_ega)
{
	BOOST_TEST_MESSAGE("Generate default EGA palette");

	PaletteTablePtr pal = createPalette_DefaultEGA();

	REQUIRE_PAL( 0, 0x00, 0x00, 0x00);
	REQUIRE_PAL( 1, 0x00, 0x00, 0xAA);
	REQUIRE_PAL( 2, 0x00, 0xAA, 0x00);
	REQUIRE_PAL( 3, 0x00, 0xAA, 0xAA);
	REQUIRE_PAL( 4, 0xAA, 0x00, 0x00);
	REQUIRE_PAL( 5, 0xAA, 0x00, 0xAA);
	REQUIRE_PAL( 6, 0xAA, 0x55, 0x00);
	REQUIRE_PAL( 7, 0xAA, 0xAA, 0xAA);
	REQUIRE_PAL( 8, 0x55, 0x55, 0x55);
	REQUIRE_PAL( 9, 0x55, 0x55, 0xFF);
	REQUIRE_PAL(10, 0x55, 0xFF, 0x55);
	REQUIRE_PAL(11, 0x55, 0xFF, 0xFF);
	REQUIRE_PAL(12, 0xFF, 0x55, 0x55);
	REQUIRE_PAL(13, 0xFF, 0x55, 0xFF);
	REQUIRE_PAL(14, 0xFF, 0xFF, 0x55);
	REQUIRE_PAL(15, 0xFF, 0xFF, 0xFF);
}

BOOST_AUTO_TEST_CASE(pal_default_vga)
{
	BOOST_TEST_MESSAGE("Generate default VGA palette");

	PaletteTablePtr pal = createPalette_DefaultVGA();

#define VGA_PAL(i, vr, vg, vb) REQUIRE_PAL(i, \
	(vr << 2) | (vr >> 2), \
	(vg << 2) | (vg >> 2), \
	(vb << 2) | (vb >> 2))

	VGA_PAL(  0, 0x00, 0x00, 0x00);
	VGA_PAL(  1, 0x00, 0x00, 0x2A);
	VGA_PAL(  2, 0x00, 0x2A, 0x00);
	VGA_PAL(  3, 0x00, 0x2A, 0x2A);
	VGA_PAL(  4, 0x2A, 0x00, 0x00);
	VGA_PAL(  5, 0x2A, 0x00, 0x2A);
	VGA_PAL(  6, 0x2A, 0x15, 0x00);
	VGA_PAL(  7, 0x2A, 0x2A, 0x2A);
	VGA_PAL(  8, 0x15, 0x15, 0x15);
	VGA_PAL(  9, 0x15, 0x15, 0x3F);
	VGA_PAL( 10, 0x15, 0x3F, 0x15);
	VGA_PAL( 11, 0x15, 0x3F, 0x3F);
	VGA_PAL( 12, 0x3F, 0x15, 0x15);
	VGA_PAL( 13, 0x3F, 0x15, 0x3F);
	VGA_PAL( 14, 0x3F, 0x3F, 0x15);
	VGA_PAL( 15, 0x3F, 0x3F, 0x3F);
	VGA_PAL( 16, 0x00, 0x00, 0x00);
	VGA_PAL( 17, 0x05, 0x05, 0x05);
	VGA_PAL( 18, 0x08, 0x08, 0x08);
	VGA_PAL( 19, 0x0B, 0x0B, 0x0B);
	VGA_PAL( 20, 0x0E, 0x0E, 0x0E);
	VGA_PAL( 21, 0x11, 0x11, 0x11);
	VGA_PAL( 22, 0x14, 0x14, 0x14);
	VGA_PAL( 23, 0x18, 0x18, 0x18);
	VGA_PAL( 24, 0x1C, 0x1C, 0x1C);
	VGA_PAL( 25, 0x20, 0x20, 0x20);
	VGA_PAL( 26, 0x24, 0x24, 0x24);
	VGA_PAL( 27, 0x28, 0x28, 0x28);
	VGA_PAL( 28, 0x2D, 0x2D, 0x2D);
	VGA_PAL( 29, 0x32, 0x32, 0x32);
	VGA_PAL( 30, 0x38, 0x38, 0x38);
	VGA_PAL( 31, 0x3F, 0x3F, 0x3F);
	VGA_PAL( 32, 0x00, 0x00, 0x3F);
	VGA_PAL( 33, 0x10, 0x00, 0x3F);
	VGA_PAL( 34, 0x1F, 0x00, 0x3F);
	VGA_PAL( 35, 0x2F, 0x00, 0x3F);
	VGA_PAL( 36, 0x3F, 0x00, 0x3F);
	VGA_PAL( 37, 0x3F, 0x00, 0x2F);
	VGA_PAL( 38, 0x3F, 0x00, 0x1F);
	VGA_PAL( 39, 0x3F, 0x00, 0x10);
	VGA_PAL( 40, 0x3F, 0x00, 0x00);
	VGA_PAL( 41, 0x3F, 0x10, 0x00);
	VGA_PAL( 42, 0x3F, 0x1F, 0x00);
	VGA_PAL( 43, 0x3F, 0x2F, 0x00);
	VGA_PAL( 44, 0x3F, 0x3F, 0x00);
	VGA_PAL( 45, 0x2F, 0x3F, 0x00);
	VGA_PAL( 46, 0x1F, 0x3F, 0x00);
	VGA_PAL( 47, 0x10, 0x3F, 0x00);
	VGA_PAL( 48, 0x00, 0x3F, 0x00);
	VGA_PAL( 49, 0x00, 0x3F, 0x10);
	VGA_PAL( 50, 0x00, 0x3F, 0x1F);
	VGA_PAL( 51, 0x00, 0x3F, 0x2F);
	VGA_PAL( 52, 0x00, 0x3F, 0x3F);
	VGA_PAL( 53, 0x00, 0x2F, 0x3F);
	VGA_PAL( 54, 0x00, 0x1F, 0x3F);
	VGA_PAL( 55, 0x00, 0x10, 0x3F);
	VGA_PAL( 56, 0x1F, 0x1F, 0x3F);
	VGA_PAL( 57, 0x27, 0x1F, 0x3F);
	VGA_PAL( 58, 0x2F, 0x1F, 0x3F);
	VGA_PAL( 59, 0x37, 0x1F, 0x3F);
	VGA_PAL( 60, 0x3F, 0x1F, 0x3F);
	VGA_PAL( 61, 0x3F, 0x1F, 0x37);
	VGA_PAL( 62, 0x3F, 0x1F, 0x2F);
	VGA_PAL( 63, 0x3F, 0x1F, 0x27);
	VGA_PAL( 64, 0x3F, 0x1F, 0x1F);
	VGA_PAL( 65, 0x3F, 0x27, 0x1F);
	VGA_PAL( 66, 0x3F, 0x2F, 0x1F);
	VGA_PAL( 67, 0x3F, 0x37, 0x1F);
	VGA_PAL( 68, 0x3F, 0x3F, 0x1F);
	VGA_PAL( 69, 0x37, 0x3F, 0x1F);
	VGA_PAL( 70, 0x2F, 0x3F, 0x1F);
	VGA_PAL( 71, 0x27, 0x3F, 0x1F);
	VGA_PAL( 72, 0x1F, 0x3F, 0x1F);
	VGA_PAL( 73, 0x1F, 0x3F, 0x27);
	VGA_PAL( 74, 0x1F, 0x3F, 0x2F);
	VGA_PAL( 75, 0x1F, 0x3F, 0x37);
	VGA_PAL( 76, 0x1F, 0x3F, 0x3F);
	VGA_PAL( 77, 0x1F, 0x37, 0x3F);
	VGA_PAL( 78, 0x1F, 0x2F, 0x3F);
	VGA_PAL( 79, 0x1F, 0x27, 0x3F);
	VGA_PAL( 80, 0x2D, 0x2D, 0x3F);
	VGA_PAL( 81, 0x31, 0x2D, 0x3F);
	VGA_PAL( 82, 0x36, 0x2D, 0x3F);
	VGA_PAL( 83, 0x3A, 0x2D, 0x3F);
	VGA_PAL( 84, 0x3F, 0x2D, 0x3F);
	VGA_PAL( 85, 0x3F, 0x2D, 0x3A);
	VGA_PAL( 86, 0x3F, 0x2D, 0x36);
	VGA_PAL( 87, 0x3F, 0x2D, 0x31);
	VGA_PAL( 88, 0x3F, 0x2D, 0x2D);
	VGA_PAL( 89, 0x3F, 0x31, 0x2D);
	VGA_PAL( 90, 0x3F, 0x36, 0x2D);
	VGA_PAL( 91, 0x3F, 0x3A, 0x2D);
	VGA_PAL( 92, 0x3F, 0x3F, 0x2D);
	VGA_PAL( 93, 0x3A, 0x3F, 0x2D);
	VGA_PAL( 94, 0x36, 0x3F, 0x2D);
	VGA_PAL( 95, 0x31, 0x3F, 0x2D);
	VGA_PAL( 96, 0x2D, 0x3F, 0x2D);
	VGA_PAL( 97, 0x2D, 0x3F, 0x31);
	VGA_PAL( 98, 0x2D, 0x3F, 0x36);
	VGA_PAL( 99, 0x2D, 0x3F, 0x3A);
	VGA_PAL(100, 0x2D, 0x3F, 0x3F);
	VGA_PAL(101, 0x2D, 0x3A, 0x3F);
	VGA_PAL(102, 0x2D, 0x36, 0x3F);
	VGA_PAL(103, 0x2D, 0x31, 0x3F);
	VGA_PAL(104, 0x00, 0x00, 0x1C);
	VGA_PAL(105, 0x07, 0x00, 0x1C);
	VGA_PAL(106, 0x0E, 0x00, 0x1C);
	VGA_PAL(107, 0x15, 0x00, 0x1C);
	VGA_PAL(108, 0x1C, 0x00, 0x1C);
	VGA_PAL(109, 0x1C, 0x00, 0x15);
	VGA_PAL(110, 0x1C, 0x00, 0x0E);
	VGA_PAL(111, 0x1C, 0x00, 0x07);
	VGA_PAL(112, 0x1C, 0x00, 0x00);
	VGA_PAL(113, 0x1C, 0x07, 0x00);
	VGA_PAL(114, 0x1C, 0x0E, 0x00);
	VGA_PAL(115, 0x1C, 0x15, 0x00);
	VGA_PAL(116, 0x1C, 0x1C, 0x00);
	VGA_PAL(117, 0x15, 0x1C, 0x00);
	VGA_PAL(118, 0x0E, 0x1C, 0x00);
	VGA_PAL(119, 0x07, 0x1C, 0x00);
	VGA_PAL(120, 0x00, 0x1C, 0x00);
	VGA_PAL(121, 0x00, 0x1C, 0x07);
	VGA_PAL(122, 0x00, 0x1C, 0x0E);
	VGA_PAL(123, 0x00, 0x1C, 0x15);
	VGA_PAL(124, 0x00, 0x1C, 0x1C);
	VGA_PAL(125, 0x00, 0x15, 0x1C);
	VGA_PAL(126, 0x00, 0x0E, 0x1C);
	VGA_PAL(127, 0x00, 0x07, 0x1C);
	VGA_PAL(128, 0x0E, 0x0E, 0x1C);
	VGA_PAL(129, 0x11, 0x0E, 0x1C);
	VGA_PAL(130, 0x15, 0x0E, 0x1C);
	VGA_PAL(131, 0x18, 0x0E, 0x1C);
	VGA_PAL(132, 0x1C, 0x0E, 0x1C);
	VGA_PAL(133, 0x1C, 0x0E, 0x18);
	VGA_PAL(134, 0x1C, 0x0E, 0x15);
	VGA_PAL(135, 0x1C, 0x0E, 0x11);
	VGA_PAL(136, 0x1C, 0x0E, 0x0E);
	VGA_PAL(137, 0x1C, 0x11, 0x0E);
	VGA_PAL(138, 0x1C, 0x15, 0x0E);
	VGA_PAL(139, 0x1C, 0x18, 0x0E);
	VGA_PAL(140, 0x1C, 0x1C, 0x0E);
	VGA_PAL(141, 0x18, 0x1C, 0x0E);
	VGA_PAL(142, 0x15, 0x1C, 0x0E);
	VGA_PAL(143, 0x11, 0x1C, 0x0E);
	VGA_PAL(144, 0x0E, 0x1C, 0x0E);
	VGA_PAL(145, 0x0E, 0x1C, 0x11);
	VGA_PAL(146, 0x0E, 0x1C, 0x15);
	VGA_PAL(147, 0x0E, 0x1C, 0x18);
	VGA_PAL(148, 0x0E, 0x1C, 0x1C);
	VGA_PAL(149, 0x0E, 0x18, 0x1C);
	VGA_PAL(150, 0x0E, 0x15, 0x1C);
	VGA_PAL(151, 0x0E, 0x11, 0x1C);
	VGA_PAL(152, 0x14, 0x14, 0x1C);
	VGA_PAL(153, 0x16, 0x14, 0x1C);
	VGA_PAL(154, 0x18, 0x14, 0x1C);
	VGA_PAL(155, 0x1A, 0x14, 0x1C);
	VGA_PAL(156, 0x1C, 0x14, 0x1C);
	VGA_PAL(157, 0x1C, 0x14, 0x1A);
	VGA_PAL(158, 0x1C, 0x14, 0x18);
	VGA_PAL(159, 0x1C, 0x14, 0x16);
	VGA_PAL(160, 0x1C, 0x14, 0x14);
	VGA_PAL(161, 0x1C, 0x16, 0x14);
	VGA_PAL(162, 0x1C, 0x18, 0x14);
	VGA_PAL(163, 0x1C, 0x1A, 0x14);
	VGA_PAL(164, 0x1C, 0x1C, 0x14);
	VGA_PAL(165, 0x1A, 0x1C, 0x14);
	VGA_PAL(166, 0x18, 0x1C, 0x14);
	VGA_PAL(167, 0x16, 0x1C, 0x14);
	VGA_PAL(168, 0x14, 0x1C, 0x14);
	VGA_PAL(169, 0x14, 0x1C, 0x16);
	VGA_PAL(170, 0x14, 0x1C, 0x18);
	VGA_PAL(171, 0x14, 0x1C, 0x1A);
	VGA_PAL(172, 0x14, 0x1C, 0x1C);
	VGA_PAL(173, 0x14, 0x1A, 0x1C);
	VGA_PAL(174, 0x14, 0x18, 0x1C);
	VGA_PAL(175, 0x14, 0x16, 0x1C);
	VGA_PAL(176, 0x00, 0x00, 0x10);
	VGA_PAL(177, 0x04, 0x00, 0x10);
	VGA_PAL(178, 0x08, 0x00, 0x10);
	VGA_PAL(179, 0x0C, 0x00, 0x10);
	VGA_PAL(180, 0x10, 0x00, 0x10);
	VGA_PAL(181, 0x10, 0x00, 0x0C);
	VGA_PAL(182, 0x10, 0x00, 0x08);
	VGA_PAL(183, 0x10, 0x00, 0x04);
	VGA_PAL(184, 0x10, 0x00, 0x00);
	VGA_PAL(185, 0x10, 0x04, 0x00);
	VGA_PAL(186, 0x10, 0x08, 0x00);
	VGA_PAL(187, 0x10, 0x0C, 0x00);
	VGA_PAL(188, 0x10, 0x10, 0x00);
	VGA_PAL(189, 0x0C, 0x10, 0x00);
	VGA_PAL(190, 0x08, 0x10, 0x00);
	VGA_PAL(191, 0x04, 0x10, 0x00);
	VGA_PAL(192, 0x00, 0x10, 0x00);
	VGA_PAL(193, 0x00, 0x10, 0x04);
	VGA_PAL(194, 0x00, 0x10, 0x08);
	VGA_PAL(195, 0x00, 0x10, 0x0C);
	VGA_PAL(196, 0x00, 0x10, 0x10);
	VGA_PAL(197, 0x00, 0x0C, 0x10);
	VGA_PAL(198, 0x00, 0x08, 0x10);
	VGA_PAL(199, 0x00, 0x04, 0x10);
	VGA_PAL(200, 0x08, 0x08, 0x10);
	VGA_PAL(201, 0x0A, 0x08, 0x10);
	VGA_PAL(202, 0x0C, 0x08, 0x10);
	VGA_PAL(203, 0x0E, 0x08, 0x10);
	VGA_PAL(204, 0x10, 0x08, 0x10);
	VGA_PAL(205, 0x10, 0x08, 0x0E);
	VGA_PAL(206, 0x10, 0x08, 0x0C);
	VGA_PAL(207, 0x10, 0x08, 0x0A);
	VGA_PAL(208, 0x10, 0x08, 0x08);
	VGA_PAL(209, 0x10, 0x0A, 0x08);
	VGA_PAL(210, 0x10, 0x0C, 0x08);
	VGA_PAL(211, 0x10, 0x0E, 0x08);
	VGA_PAL(212, 0x10, 0x10, 0x08);
	VGA_PAL(213, 0x0E, 0x10, 0x08);
	VGA_PAL(214, 0x0C, 0x10, 0x08);
	VGA_PAL(215, 0x0A, 0x10, 0x08);
	VGA_PAL(216, 0x08, 0x10, 0x08);
	VGA_PAL(217, 0x08, 0x10, 0x0A);
	VGA_PAL(218, 0x08, 0x10, 0x0C);
	VGA_PAL(219, 0x08, 0x10, 0x0E);
	VGA_PAL(220, 0x08, 0x10, 0x10);
	VGA_PAL(221, 0x08, 0x0E, 0x10);
	VGA_PAL(222, 0x08, 0x0C, 0x10);
	VGA_PAL(223, 0x08, 0x0A, 0x10);
	VGA_PAL(224, 0x0B, 0x0B, 0x10);
	VGA_PAL(225, 0x0C, 0x0B, 0x10);
	VGA_PAL(226, 0x0D, 0x0B, 0x10);
	VGA_PAL(227, 0x0F, 0x0B, 0x10);
	VGA_PAL(228, 0x10, 0x0B, 0x10);
	VGA_PAL(229, 0x10, 0x0B, 0x0F);
	VGA_PAL(230, 0x10, 0x0B, 0x0D);
	VGA_PAL(231, 0x10, 0x0B, 0x0C);
	VGA_PAL(232, 0x10, 0x0B, 0x0B);
	VGA_PAL(233, 0x10, 0x0C, 0x0B);
	VGA_PAL(234, 0x10, 0x0D, 0x0B);
	VGA_PAL(235, 0x10, 0x0F, 0x0B);
	VGA_PAL(236, 0x10, 0x10, 0x0B);
	VGA_PAL(237, 0x0F, 0x10, 0x0B);
	VGA_PAL(238, 0x0D, 0x10, 0x0B);
	VGA_PAL(239, 0x0C, 0x10, 0x0B);
	VGA_PAL(240, 0x0B, 0x10, 0x0B);
	VGA_PAL(241, 0x0B, 0x10, 0x0C);
	VGA_PAL(242, 0x0B, 0x10, 0x0D);
	VGA_PAL(243, 0x0B, 0x10, 0x0F);
	VGA_PAL(244, 0x0B, 0x10, 0x10);
	VGA_PAL(245, 0x0B, 0x0F, 0x10);
	VGA_PAL(246, 0x0B, 0x0D, 0x10);
	VGA_PAL(247, 0x0B, 0x0C, 0x10);
	VGA_PAL(248, 0x00, 0x00, 0x00);
	VGA_PAL(249, 0x00, 0x00, 0x00);
	VGA_PAL(250, 0x00, 0x00, 0x00);
	VGA_PAL(251, 0x00, 0x00, 0x00);
	VGA_PAL(252, 0x00, 0x00, 0x00);
	VGA_PAL(253, 0x00, 0x00, 0x00);
	VGA_PAL(254, 0x00, 0x00, 0x00);
	VGA_PAL(255, 0x00, 0x00, 0x00);
}
