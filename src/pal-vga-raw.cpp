/**
 * @file  pal-vga-raw.cpp
 * @brief Palette interface to 768-byte raw 6-bit VGA palette files.
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

#include "pal-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

ImageType_Palette_VGA::ImageType_Palette_VGA()
{
}

ImageType_Palette_VGA::~ImageType_Palette_VGA()
{
}

std::string ImageType_Palette_VGA::getCode() const
{
	return "pal-vga-raw";
}

std::string ImageType_Palette_VGA::getFriendlyName() const
{
	return "Standard VGA palette";
}

std::vector<std::string> ImageType_Palette_VGA::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pal");
	return vcExtensions;
}

std::vector<std::string> ImageType_Palette_VGA::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty ImageType_Palette_VGA::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	// Allow palettes with 256 entries, but also ones with only 16 (Duke II)
	if ((len != 16 * 3) && (len != 256 * 3)) return DefinitelyNo;

	uint8_t buf[768];
	memset(buf, 0, 768);
	psImage->seekg(0, stream::start);
	psImage->try_read(buf, 768);
	for (int i = 0; i < 768; i++) {
		// Some palettes do use 64 instead of the max value of 63
		if (buf[i] > 64) return DefinitelyNo;
	}

	// See if the first colour is black, which is even more likely to mean it's
	// a VGA palette.
	if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0)) return DefinitelyYes;

	// TESTED BY: TODO
	return PossiblyYes;
}

ImagePtr ImageType_Palette_VGA::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new Palette_VGA(psImage, 6));
}

ImagePtr ImageType_Palette_VGA::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new Palette_VGA(psImage, 6));
}

SuppFilenames ImageType_Palette_VGA::getRequiredSupps(const std::string& filenameImage) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


ImageType_VGA8Palette::ImageType_VGA8Palette()
{
}

ImageType_VGA8Palette::~ImageType_VGA8Palette()
{
}

std::string ImageType_VGA8Palette::getCode() const
{
	return "pal-vga-raw8";
}

std::string ImageType_VGA8Palette::getFriendlyName() const
{
	return "8-bit per channel (24-bit RGB) palette";
}

std::vector<std::string> ImageType_VGA8Palette::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pal");
	return vcExtensions;
}

std::vector<std::string> ImageType_VGA8Palette::getGameList() const
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty ImageType_VGA8Palette::isInstance(stream::input_sptr psImage) const
{
	stream::pos len = psImage->size();

	if (len != 768) return DefinitelyNo;

	// See if the first colour is black, which is even more likely to mean it's
	// a VGA palette.
	psImage->seekg(0, stream::start);
	uint8_t buf[3];
	psImage->read((char *)buf, 3);
	if ((buf[0] == 0) && (buf[1] == 0) && (buf[2] == 0)) return DefinitelyYes;

	// TESTED BY: TODO
	return PossiblyYes;
}

ImagePtr ImageType_VGA8Palette::create(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new Palette_VGA(psImage, 8));
}

ImagePtr ImageType_VGA8Palette::open(stream::inout_sptr psImage,
	SuppData& suppData) const
{
	return ImagePtr(new Palette_VGA(psImage, 8));
}

SuppFilenames ImageType_VGA8Palette::getRequiredSupps(const std::string& filenameImage) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}


Palette_VGA::Palette_VGA(stream::inout_sptr data, unsigned int depth)
	:	data(data),
		depth(depth)
{
	assert(data);
}

Palette_VGA::~Palette_VGA()
{
}

PaletteTablePtr Palette_VGA::getPalette()
{
	PaletteTablePtr pal(new PaletteTable());
	pal->reserve(256);

	uint8_t buf[768];
	memset(buf, 0, 768);
	this->data->seekg(0, stream::start);
	this->data->try_read(buf, 768);
	// If the palette data is cut off (short read) the rest of the entries will
	// be black.
	int i = 0;
	switch (this->depth) {
		case 6:
			while (i < 768) {
				PaletteEntry p;
				if (buf[i] >= 0x40) buf[i] = 0x3F;
				p.red   = (buf[i] << 2) | (buf[i] >> 4);
				i++;
				if (buf[i] >= 0x40) buf[i] = 0x3F;
				p.green = (buf[i] << 2) | (buf[i] >> 4);
				i++;
				if (buf[i] >= 0x40) buf[i] = 0x3F;
				p.blue  = (buf[i] << 2) | (buf[i] >> 4);
				i++;
				p.alpha = 255;
				pal->push_back(p);
			}
			break;
		case 8:
			while (i < 768) {
				PaletteEntry p;
				p.red   = buf[i++];
				p.green = buf[i++];
				p.blue  = buf[i++];
				p.alpha = 255;
				pal->push_back(p);
			}
			break;
	}

	return pal;
}

void Palette_VGA::setPalette(PaletteTablePtr newPalette)
{
	uint8_t buf[768];
	memset(buf, 0, 768);
	int i = 0;
	unsigned int shift;
	switch (this->depth) {
		default:
		case 6: shift = 2; break;
		case 8: shift = 0; break;
	}
	for (PaletteTable::const_iterator p = newPalette->begin(); p < newPalette->end(); p++) {
		buf[i++] = p->red >> shift;
		buf[i++] = p->green >> shift;
		buf[i++] = p->blue >> shift;
	}
	this->data->truncate(768);
	this->data->seekp(0, stream::start);
	this->data->write(buf, 768);
	return;
}

} // namespace gamegraphics
} // namespace camoto
