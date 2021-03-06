/**
 * @file  img-zone66_tile.cpp
 * @brief Image implementation for Zone66 tiles.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Zone_66_Tileset_Format
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

#include <camoto/iostream_helpers.hpp>
#include "pal-vga-raw.hpp"
#include "img-zone66_tile.hpp"

/// Offset where the VGA image data begins
#define Z66_IMG_OFFSET 4

/// Palette index to make transparent
#define Z66_TRANSPARENT_COLOUR 0

namespace camoto {
namespace gamegraphics {

ImageType_Zone66Tile::ImageType_Zone66Tile()
{
}

ImageType_Zone66Tile::~ImageType_Zone66Tile()
{
}

std::string ImageType_Zone66Tile::getCode() const
{
	return "img-zone66_tile";
}

std::string ImageType_Zone66Tile::getFriendlyName() const
{
	return "Zone 66 tile";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> ImageType_Zone66Tile::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> ImageType_Zone66Tile::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Zone 66");
	return vcGames;
}

ImageType::Certainty ImageType_Zone66Tile::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	psImage->seekg(0, stream::start);
	uint16_t width, height;
	psImage >> u16le(width) >> u16le(height);
	int imgSize = width * height;
	int pos = 0;
	while (pos < imgSize) {
		uint8_t code;
		psImage >> u8(code);
		len--;
		switch (code) {
			case 0x00: return DefinitelyNo; // doesn't make sense
			case 0xFF: // end of image
				if (len == 0) return DefinitelyYes; // EOF marker at EOF
				return DefinitelyNo; // EOF marker with trailing data
			case 0xFE: // skip to EOL
				pos += width - (pos % width);
				break;
			case 0xFD:
				psImage >> u8(code);
				pos += code;
				len -= code+1;
				break;
			default:
				pos += code;
				len -= code;
				break;
		}
	}

	// Read all data but didn't have 0xFF at EOF
	if ((len == 0) && (pos == imgSize)) return DefinitelyYes;

	// TESTED BY: TODO //fmt_grp_duke3d_isinstance_c01
	return DefinitelyNo;
}

ImagePtr ImageType_Zone66Tile::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage
		<< u16le(0)  // width
		<< u16le(0)  // height
		<< u8(0xFF)  // end-of-image code
	;
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
		pal->at(Z66_TRANSPARENT_COLOUR).alpha = 0;
	}
	return ImagePtr(new Image_Zone66Tile(psImage, pal));
}

ImagePtr ImageType_Zone66Tile::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], 6));
		pal = palFile->getPalette();
		pal->at(Z66_TRANSPARENT_COLOUR).alpha = 0;
	}
	return ImagePtr(new Image_Zone66Tile(psImage, pal));
}

SuppFilenames ImageType_Zone66Tile::getRequiredSupps(const std::string& filenameImage) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "tpal.z66"; // TODO: case sensitivity?
	return supps;
}


Image_Zone66Tile::Image_Zone66Tile(stream::inout_sptr data,
	PaletteTablePtr pal)
	:	data(data),
		pal(pal)
{
	stream::len lenData = data->size();
	if (lenData == 64000) {
		// Headerless fullscreen image
		this->width = 320;
		this->height = 200;
	} else {
		try {
			this->data >> u16le(this->width) >> u16le(this->height);
		} catch (const stream::incomplete_read&) {
			this->width = 0;
			this->height = 0;
		}
	}
}


Image_Zone66Tile::~Image_Zone66Tile()
{
}

int Image_Zone66Tile::getCaps()
{
	return ColourDepthVGA | CanSetDimensions | HasPalette;
}

void Image_Zone66Tile::getDimensions(unsigned int *width, unsigned int *height)
{
	*width = this->width;
	*height = this->height;
	return;
}

void Image_Zone66Tile::setDimensions(unsigned int width, unsigned int height)
{
	this->data->seekg(0, stream::end);
	if ((width == 320) && (height == 200)) {
		this->data->truncate(64000);
	} else {
		if (this->data->tellg() < 4) {
			// Need to enlarge stream to write image size
			this->data->truncate(4);
		}

		this->data->seekp(0, stream::start);
		this->data << u16le(width) << u16le(height);
	}
	this->width = width;
	this->height = height;
	return;
}

StdImageDataPtr Image_Zone66Tile::toStandard()
{
	assert((this->width != 0) && (this->height != 0));

	int dataSize = this->width * this->height;
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);

	if ((this->width == 320) && (this->height == 200)) {
		// Special case for headerless fullscreen images
		this->data->seekg(0, stream::start);
		this->data->read(imgData, 64000);
		return ret;
	}

	// Make any skipped pixels black
	memset(imgData, 0, dataSize);

	this->data->seekg(Z66_IMG_OFFSET, stream::start);
	unsigned int y = 0;
	for (int i = 0; i < dataSize; ) {
		uint8_t code;
		this->data >> u8(code);
		switch (code) {
			case 0xFD: // Skip the given number of pixels
				this->data >> u8(code);
				i += code;
				// Note: i may now be >= dataSize
				break;

			case 0xFE: // Go to the next line
				i = ++y * this->width;
				break;

			case 0xFF: // End of image
				i = dataSize;
				break;

			case 0x00: // shouldn't happen
				throw stream::error("corrupted data");

			default:
				if (i + code <= dataSize) {
					this->data->read(&imgData[i], code);
					i += code;
				} else {
					throw stream::error("bad data, tried to write past end of image");
				}
				break;
		}
	}

	return ret;
}

StdImageDataPtr Image_Zone66Tile::toStandardMask()
{
	assert((this->width != 0) && (this->height != 0));
	int dataSize = this->width * this->height;

	// Return an entirely opaque mask
	uint8_t *imgData = new uint8_t[dataSize];
	StdImageDataPtr ret(imgData);
	memset(imgData, Image::Mask_Vis_Opaque, dataSize);

	return ret;
}

void Image_Zone66Tile::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask)
{
	assert((this->width != 0) && (this->height != 0));
	this->data->seekp(0, stream::start);

	if ((this->width == 320) && (this->height == 200)) {
		// Special case for headerless fullscreen images
		this->data->truncate(64000);
		this->data->write(newContent.get(), 64000);
		return;
	}

	// Start off with enough space for the worst-case size
	this->data->truncate(4 + (this->width + 2) * this->height + 1);
	int finalSize = 4; // for header

	this->data->seekp(Z66_IMG_OFFSET, stream::start);
	uint8_t *imgData = (uint8_t *)newContent.get();

	// Find the last non-black pixel in the image
	uint8_t *imgEnd = imgData + this->width * this->height - 1;
	while ((*imgEnd == 0) && (imgEnd > imgData)) imgEnd--;
	imgEnd++; // still want current (non-black) pixel

	for (int y = 0; y < this->height; y++) {
		int dw = this->width;
		while (dw > 0) {
			// Count how many black pixels are in a row starting at the current pos
			int amt = 0;
			while ((*imgData == 0) && (amt < 254) && (dw > 0)) {
				imgData++;
				amt++;
				dw--;
			}
			// If there were black pixels, figure out the best way to write them
			if (amt) {
				if (dw == 0) {
					// Got blanks until EOL
					// TESTED BY: img_zone66_tile_from_standard_8x4
					break; // go to next line (will write 0xFE or 0xFF)
				} else {
					// Encountered at least one blank pixel
					if (amt > 1) {
						// More efficient to write as RLE
						// TESTED BY: img_zone66_tile_from_standard_8x4
						this->data << u8(0xFD) << u8(amt);
						finalSize += 2;
						// If there were enough blanks, keep looking for more.
						// TESTED BY: TODO
						if (amt == 255) continue;
					} else {
						// Less/same efficiency to write code, so don't
						// TESTED BY: img_zone66_tile_from_standard_8x4
						imgData -= amt; // reprocess data in next step
						dw += amt;
					}
				}
			}
			if (dw == 0) break;
			// This next pixel is not blank (or it is but there's only 1-2 of them)
			// so write out as straight pixel data.  If this assertion fails then
			// the code above is broken because three blanks in a row should've been
			// handled up there.
			assert(
				(imgData[0] != 0) ||
				(imgData[1] != 0) ||
				(imgData[2] != 0)
			);
			amt = (dw > 255) ? 255 : dw;
			for (int l = 1; l < amt; l++) { // assume [0] is not blank - if it is [1] or [2] won't be
				if (imgData[l] == 0) {
					// found first empty
					int lenBlank = 1;
					for (int ll = l + 1; ll < amt; ll++) {
						if (imgData[ll] == 0) lenBlank++;
						else break; // TESTED BY: img_zone66_tile_from_standard_8x4
					}
					if (lenBlank > 2) {
						// More than two blanks start at pos l, so only write up to pos l
						// as normal pixels (then the loop will run again and the blanks
						// will get picked up by the condition above.)
						// TESTED BY: img_zone66_tile_from_standard_8x4
						amt = l;
						break;
					}
				}
			}
			this->data << u8(amt);
			this->data->write(imgData, amt);
			imgData += amt;
			dw -= amt;
			finalSize += amt + 1;
		}

		// TESTED BY: img_zone66_tile_from_standard_8x5
		// TESTED BY: img_zone66_tile_from_standard_8x7
		if (imgData >= imgEnd) break; // just write EOF

		assert(dw == 0); // make sure we read everything
		this->data << u8(0xFE); // end of line
		finalSize++;
	}
	this->data << u8(0xFF); // end of file
	finalSize++;

	this->data->flush();

	// Then shrink back to actual size
	this->data->truncate(finalSize);

	return;
}

PaletteTablePtr Image_Zone66Tile::getPalette()
{
	return this->pal;
}

void Image_Zone66Tile::setPalette(PaletteTablePtr newPalette)
{
	// This doesn't save anything to the file as the palette is stored externally.
	this->pal = newPalette;
	return;
}

} // namespace gamegraphics
} // namespace camoto
