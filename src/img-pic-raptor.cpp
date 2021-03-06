/**
 * @file  img-pic-raptor.cpp
 * @brief Raptor PIC image format.
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
#include "img-pic-raptor.hpp"
#include "pal-vga-raw.hpp"

/// Offset where standard VGA data begins
#define PIC_DATA_OFFSET 20

/// Depth of the Raptor palette file
#define PIC_PALETTE_DEPTH 6

namespace camoto {
namespace gamegraphics {

std::string ImageType_RaptorPIC::getCode() const
{
	return "img-pic-raptor";
}

std::string ImageType_RaptorPIC::getFriendlyName() const
{
	return "Raptor PIC image";
}

std::vector<std::string> ImageType_RaptorPIC::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pic");
	return vcExtensions;
}

std::vector<std::string> ImageType_RaptorPIC::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Raptor");
	return vcGames;
}

ImageType::Certainty ImageType_RaptorPIC::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();
	if (len < 20) return DefinitelyNo;
	uint32_t unknown1, unknown2, unknown3, width, height;
	psImage->seekg(0, stream::start);
	psImage
		>> u32le(unknown1)
		>> u32le(unknown2)
		>> u32le(unknown3)
		>> u32le(width)
		>> u32le(height)
	;
	if (width * height + 20 != len) return DefinitelyNo;

	// TESTED BY: TODO
	return DefinitelyYes;
}

ImagePtr ImageType_RaptorPIC::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	psImage->truncate(20);
	psImage->seekp(0, stream::start);
	psImage
		<< u32le(1)
		<< u32le(1)
		<< u32le(0)
		<< u32le(0)
		<< u32le(0)
	;

	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], PIC_PALETTE_DEPTH));
		pal = palFile->getPalette();
	}
	return ImagePtr(new Image_RaptorPIC(psImage, pal));
}

ImagePtr ImageType_RaptorPIC::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new Palette_VGA(suppData[SuppItem::Palette], PIC_PALETTE_DEPTH));
		pal = palFile->getPalette();
	}
	return ImagePtr(new Image_RaptorPIC(psImage, pal));
}

SuppFilenames ImageType_RaptorPIC::getRequiredSupps(const std::string& filenameImage) const
{
	SuppFilenames supps;
	supps[SuppItem::Palette] = "palette_dat";
	return supps;
}


Image_RaptorPIC::Image_RaptorPIC(stream::inout_sptr data, PaletteTablePtr pal)
	:	Image_VGA(data, PIC_DATA_OFFSET),
		pal(pal)
{
}

Image_RaptorPIC::~Image_RaptorPIC()
{
}

int Image_RaptorPIC::getCaps()
{
	return Image::ColourDepthVGA
		| Image::CanSetDimensions
		| (this->pal ? Image::HasPalette : 0)
	;
}

void Image_RaptorPIC::getDimensions(unsigned int *width, unsigned int *height)
{
	this->data->seekg(12, stream::start);
	this->data
		>> u32le(*width)
		>> u32le(*height)
	;
	return;
}

void Image_RaptorPIC::setDimensions(unsigned int width, unsigned int height)
{
	this->data->seekp(12, stream::start);
	this->data
		<< u32le(width)
		<< u32le(height)
	;
	return;
}

PaletteTablePtr Image_RaptorPIC::getPalette()
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto
