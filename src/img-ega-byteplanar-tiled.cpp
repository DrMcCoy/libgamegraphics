/**
 * @file  img-ega-byteplanar.cpp
 * @brief Image composed of 8x8 EGA byte-planar tiles.
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

#include <cstring>  // memset
#include <cassert>
#include <camoto/iostream_helpers.hpp>
#include "img-ega-byteplanar-tiled.hpp"

namespace camoto {
namespace gamegraphics {

Image_EGABytePlanarTiled::Image_EGABytePlanarTiled()
{
}

Image_EGABytePlanarTiled::~Image_EGABytePlanarTiled()
{
}

void Image_EGABytePlanarTiled::setParams(stream::inout_sptr data,
	stream::pos offset, int width, int height, const PLANE_LAYOUT& planes,
	PaletteTablePtr pal
)
{
	this->data = data;
	this->offset = offset;
	this->width = width;
	this->height = height;
	memcpy(this->planes, planes, sizeof(PLANE_LAYOUT));
	this->pal = pal;
	return;
}

int Image_EGABytePlanarTiled::getCaps()
{
	return Image::ColourDepthEGA | (this->pal ? Image::HasPalette : 0);
}

void Image_EGABytePlanarTiled::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void Image_EGABytePlanarTiled::setDimensions(unsigned int width, unsigned int height)
{
	assert(this->getCaps() & Image::CanSetDimensions);

	this->width = width;
	this->height = height;

	int numPlanes = 0;
	for (int p = 0; p < PLANE_MAX; p++) {
		// Count the plane if its order is nonzero, otherwise ignore it
		if (this->planes[p]) numPlanes++;
	}

	// TODO: Confirm this is correct
	this->data->truncate((this->width + 7) / 8 * this->height * numPlanes);
	return;
}

StdImageDataPtr Image_EGABytePlanarTiled::toStandard()
{
	return this->doConversion(false);
}

StdImageDataPtr Image_EGABytePlanarTiled::toStandardMask()
{
	return this->doConversion(true);
}

void Image_EGABytePlanarTiled::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
{
	// Sort out all the values we need to output for each plane
	int numPlanes = 0;
	int planeValue[PLANE_MAX];
	bool planeMask[PLANE_MAX]; // true == use newMask, false == use newContent
	bool planeSwap[PLANE_MAX]; // true == invert bits, false == leave alone
	memset(planeValue, 0, sizeof(planeValue));
	for (int p = 0; p < PLANE_MAX; p++) {
		// Count the plane if its order is nonzero, otherwise ignore it
		if (!this->planes[p]) continue;
		if (this->planes[p] > numPlanes) numPlanes = this->planes[p];

		// Handle negative values
		int order;
		bool swap;
		if (this->planes[p] < 0) {
			swap = true;
			order = -this->planes[p];
		} else {
			swap = false;
			order = this->planes[p];
		}
		order--;

		// Sanity check
		assert(order < PLANE_MAX);

		// Figure out which bit this plane should set in the 8bpp output data
		int value;
		bool mask;
		switch (p) {
			case PLANE_BLUE:      mask = false; value = 0x01; break;
			case PLANE_GREEN:     mask = false; value = 0x02; break;
			case PLANE_RED:       mask = false; value = 0x04; break;
			case PLANE_INTENSITY: mask = false; value = 0x08; break;

			case PLANE_OPACITY:   mask = true;  value = 0x01; break;
			case PLANE_HITMAP:    mask = true;  value = 0x02; break;

			default:              value = 0x00; break;
		}
		planeMask[order] = mask;
		planeSwap[order] = swap;
		planeValue[order] = value;
	}

	this->data->seekp(0, stream::start);
	uint8_t *imgDataTile = (uint8_t *)newContent.get();
	uint8_t *maskDataTile = (uint8_t *)newMask.get();
	uint8_t *rowData;

	for (unsigned int y = 0; y < this->height / 8; y++) {

		// Run through each lot of eight pixels (a "cell")
		for (unsigned int x = 0; x < (this->width + 7) / 8; x++) {

			uint8_t *imgData = imgDataTile;
			uint8_t *maskData = maskDataTile;

			for (unsigned int y2 = 0; y2 < 8; y2++) {

				// For each pixel, set the appropriate bits in the cell
				for (int p = 0; p < numPlanes; p++) {

					uint8_t c = 0;

					// Work out if this plane will read from the input image or mask data.
					if (planeMask[p]) rowData = maskData;
					else rowData = imgData;

					// See how many bits we should run through.  This is only used
					// when the image is not an even multiple of 8.
					int bits = 8;
					if (x * 8 + 8 > this->width) bits = this->width % 8;

					// Run through each pixel in the group
					for (int b = 0; b < bits; b++) {
						if (rowData[x * 8 + b] & planeValue[p]) {
							// The pixel is on in this plane
							c |= 0x80 >> b;
						} // else the pixel is off in this plane
					}
					// If the plane index was negative, the bits need to be inverted.  If
					// we invert the whole number (c = ~c, or c ^= 0xff) then we could
					// invert too many bits (e.g. all eight bits will be inverted when an
					// image might only be four pixels wide.)  By XORing the value below,
					// only the bits used by the image will be flipped, with the unused
					// bits remaining as zero.
					if (planeSwap[p]) c ^= ~((1 << (8-bits)) - 1);

					this->data << u8(c);
				}

				imgData += this->width;
				maskData += this->width;
			}
		}
		imgDataTile += this->width * 8;
		maskDataTile += this->width * 8;
	}

	return;
}

PaletteTablePtr Image_EGABytePlanarTiled::getPalette()
{
	return this->pal;
}

StdImageDataPtr Image_EGABytePlanarTiled::doConversion(bool mask)
{
	// Sort out all the values we need to output for each plane
	int numPlanes = 0;
	int planeValue[PLANE_MAX], notPlaneValue[PLANE_MAX];
	memset(planeValue, 0, PLANE_MAX);
	memset(notPlaneValue, 0, PLANE_MAX);
	for (int p = 0; p < PLANE_MAX; p++) {
		// Count the plane if its order is nonzero, otherwise ignore it
		if (this->planes[p]) numPlanes++; else continue;

		// Handle negative values
		int order;
		bool swap;
		if (this->planes[p] < 0) {
			swap = true;
			order = -this->planes[p];
		} else {
			swap = false;
			order = this->planes[p];
		}
		order--;

		// Sanity check
		assert(order < PLANE_MAX);

		// Figure out which bit this plane should set in the 8bpp output data
		int value;
		if (mask) {
			switch (p) {
				case PLANE_OPACITY:   value = 0x01; break;
				case PLANE_HITMAP:    value = 0x02; break;
				default:              value = 0x00; break;
			}
		} else {
			switch (p) {
				case PLANE_BLUE:      value = 0x01; break;
				case PLANE_GREEN:     value = 0x02; break;
				case PLANE_RED:       value = 0x04; break;
				case PLANE_INTENSITY: value = 0x08; break;
				default:              value = 0x00; break;
			}
		}
		if (swap) {
			planeValue[order] = 0;
			notPlaneValue[order] = value;
		} else {
			planeValue[order] = value;
			notPlaneValue[order] = 0;
		}
	}

	int imgSizeBytes = (this->width * 8) * this->height;
	uint8_t *rowData = new uint8_t[imgSizeBytes];
	StdImageDataPtr ret(rowData);
	memset(rowData, 0, imgSizeBytes);
	this->data->seekg(0, stream::start);

	// Adding 7 means a width that's not an even multiple of eight will
	// effectively be rounded up to the next byte - so an eight pixel wide
	// image will use one byte (8 + 7 = 15, 15 / 8 == 1) but a nine pixel
	// wide image will use two bytes (9 + 7 = 16, 16 / 8 == 2).
	unsigned int widthBytes = (this->width + 7) / 8;

	for (unsigned int y = 0; y < this->height / 8; y++) {
		// Run through each lot of eight pixels (a "cell")
		uint8_t *rowDataOrig = rowData;
		for (unsigned int x = 0; x < widthBytes; x++) {
			// Reset to row 0 in current 8x8 tile
			rowData = rowDataOrig;// + x * 8 * numPlanes;
			for (unsigned int y2 = 0; y2 < 8; y2++) {
				for (int p = 0; p < numPlanes; p++) {
					uint8_t nextByte;
					this->data >> u8(nextByte);

					// Don't waste time processing a plane we're ignoring
					if (!(planeValue[p] || notPlaneValue[p])) continue;

					// See how many bits we should run through.  This is only used
					// when the image is not an even multiple of 8.
					int bits = 8;
					if (x * 8 + 8 > this->width) bits = this->width % 8;

					// Run through all the (valid) bits in this byte
					for (int b = 0; b < bits; b++) {
						rowData[x * 8 + b] |=
							(nextByte & (0x80 >> b)) ? planeValue[p] : notPlaneValue[p];
					}
				}
				rowData += this->width;
			}
		}
		// Jump down to next tile (if any)
		rowDataOrig += widthBytes * 8 * numPlanes;
	}
	return ret;
}

} // namespace gamegraphics
} // namespace camoto
