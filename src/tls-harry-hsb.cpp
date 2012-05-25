/**
 * @file   tls-harry-hsb.cpp
 * @brief  Halloween Harry .HSB tileset handler.
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/HSB_Format_(Halloween_Harry)
 *
 * Copyright (C) 2010-2012 Adam Nielsen <malvineous@shikadi.net>
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
#include "pal-gmf-harry.hpp"
#include "img-vga-raw.hpp"
#include "tls-harry-hsb.hpp"

/// Offset of first tile in an empty tileset
#define HSB_FIRST_TILE_OFFSET  0

/// Number of bytes in each image's header
#define HSB_HEADER_LEN 8

namespace camoto {
namespace gamegraphics {

HarryHSBTilesetType::HarryHSBTilesetType()
	throw ()
{
}

HarryHSBTilesetType::~HarryHSBTilesetType()
	throw ()
{
}

std::string HarryHSBTilesetType::getCode() const
	throw ()
{
	return "tls-harry-hsb";
}

std::string HarryHSBTilesetType::getFriendlyName() const
	throw ()
{
	return "Halloween Harry HSB tileset";
}

std::vector<std::string> HarryHSBTilesetType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("hsb");
	return vcExtensions;
}

std::vector<std::string> HarryHSBTilesetType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Alien Carnage");
	vcGames.push_back("Halloween Harry");
	return vcGames;
}

HarryHSBTilesetType::Certainty HarryHSBTilesetType::isInstance(
	stream::input_sptr psTileset) const
	throw (stream::error)
{
	stream::pos len = psTileset->size();

	// Make sure the file is large enough
	// TESTED BY: fmt_harry_hsb_isinstance_c01
	if (len < HSB_HEADER_LEN) return DefinitelyNo;

	psTileset->seekg(0, stream::start);
	stream::pos pos = 0;
	while (pos < len) {
		uint16_t unk1, unk2;
		uint16_t width, height;
		psTileset
			>> u16le(unk1)
			>> u16le(unk2)
			>> u16le(width)
			>> u16le(height)
		;
		int delta = width * height;

		// If this pushes us past EOF it's not a valid file
		// TESTED BY: fmt_harry_hsb_isinstance_c02
		pos += delta + HSB_HEADER_LEN;
		if (pos > len) return DefinitelyNo;

		psTileset->seekg(delta, stream::cur);
	}

	// TESTED BY: fmt_harry_hsb_isinstance_c00
	return DefinitelyYes;

}

TilesetPtr HarryHSBTilesetType::create(stream::inout_sptr psTileset,
	SuppData& suppData) const
	throw (stream::error)
{
	psTileset->truncate(HSB_HEADER_LEN);
	psTileset->seekp(0, stream::start);
	psTileset->write("\0\0\0\0\0\0\0\0", 8);

	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new VGAPalette(suppData[SuppItem::Palette]));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new HarryHSBTileset(psTileset, pal));
}

TilesetPtr HarryHSBTilesetType::open(stream::inout_sptr psTileset,
	SuppData& suppData) const
	throw (stream::error)
{
	PaletteTablePtr pal;
	// Only load the palette if one was given
	if (suppData.find(SuppItem::Palette) != suppData.end()) {
		ImagePtr palFile(new GMFHarryPalette(suppData[SuppItem::Palette]));
		pal = palFile->getPalette();
	}
	return TilesetPtr(new HarryHSBTileset(psTileset, pal));
}

SuppFilenames HarryHSBTilesetType::getRequiredSupps(
	const std::string& filenameTileset) const
	throw ()
{
	SuppFilenames supps;
	return supps;
}


HarryHSBImage::HarryHSBImage(stream::inout_sptr data, PaletteTablePtr pal)
	throw () :
		VGAImage(data, HSB_HEADER_LEN),
		pal(pal)
{
	assert(data->tellg() == 0);

	uint16_t unk1, unk2;
	data
		>> u16le(unk1)
		>> u16le(unk2)
		>> u16le(this->width)
		>> u16le(this->height)
	;
}

HarryHSBImage::~HarryHSBImage()
	throw ()
{
}

int HarryHSBImage::getCaps()
	throw ()
{
	return this->VGAImage::getCaps()
		| Image::CanSetDimensions;
}

void HarryHSBImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void HarryHSBImage::setDimensions(unsigned int width, unsigned int height)
	throw (stream::error)
{
	assert(this->getCaps() & Image::CanSetDimensions);
	this->width = width;
	this->height = height;
	return;
}

void HarryHSBImage::fromStandard(StdImageDataPtr newContent,
	StdImageDataPtr newMask
)
	throw ()
{
	this->VGAImage::fromStandard(newContent, newMask);

	// Update dimensions
	this->data->seekp(0, stream::start);
	this->data
		<< u16le(0) // TODO
		<< u16le(0) // TODO
		<< u16le(this->width)
		<< u16le(this->height)
	;
	return;
}

PaletteTablePtr HarryHSBImage::getPalette()
	throw (stream::error)
{
	return this->pal;
}


HarryHSBTileset::HarryHSBTileset(stream::inout_sptr data,
	PaletteTablePtr pal)
	throw (stream::error) :
		FATTileset(data, HSB_FIRST_TILE_OFFSET),
		pal(pal)
{
	stream::pos len = this->data->size();

	this->data->seekg(0, stream::start);

	stream::pos pos = 0;
	unsigned int i = 0;
	while (pos < len) {

		uint16_t unk1, unk2;
		uint16_t width, height;
		this->data
			>> u16le(unk1)
			>> u16le(unk2)
			>> u16le(width)
			>> u16le(height)
		;
		unsigned int delta = width * height;

		FATEntry *fat = new FATEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = 0;
		fat->index = i;
		fat->offset = pos;
		fat->size = delta + HSB_HEADER_LEN;
		fat->lenHeader = 0;
		this->items.push_back(ep);

		pos += fat->size;

		this->data->seekg(delta, stream::cur);
		i++;
	}
}

HarryHSBTileset::~HarryHSBTileset()
	throw ()
{
}

int HarryHSBTileset::getCaps()
	throw ()
{
	return 0;
}

ImagePtr HarryHSBTileset::createImageInstance(const EntryPtr& id,
	stream::inout_sptr content)
	throw (stream::error)
{
	return ImagePtr(new HarryHSBImage(content, this->pal));
}

PaletteTablePtr HarryHSBTileset::getPalette()
	throw ()
{
	return this->pal;
}

void HarryHSBTileset::setPalette(PaletteTablePtr newPalette)
	throw (stream::error)
{
	// This doesn't save anything to the file as the palette is stored externally.
	this->pal = newPalette;
	return;
}

} // namespace gamegraphics
} // namespace camoto