/**
 * @file   img-vga-raw.cpp
 * @brief  VGAImage specialisation for fixed-size headerless images.
 *
 * Copyright (C) 2010 Adam Nielsen <malvineous@shikadi.net>
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

#include <boost/iostreams/stream.hpp>
#include "pal-vga-raw.hpp"
#include "img-vga-raw.hpp"

namespace camoto {
namespace gamegraphics {

namespace io = boost::iostreams;

VGARawImageType::VGARawImageType()
	throw ()
{
}

VGARawImageType::~VGARawImageType()
	throw ()
{
}

std::string VGARawImageType::getCode() const
	throw ()
{
	return "img-vga-raw-fullscreen";
}

std::string VGARawImageType::getFriendlyName() const
	throw ()
{
	return "Raw VGA fullscreen image";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> VGARawImageType::getFileExtensions() const
	throw ()
{
	std::vector<std::string> vcExtensions;
	return vcExtensions;
}

std::vector<std::string> VGARawImageType::getGameList() const
	throw ()
{
	std::vector<std::string> vcGames;
	return vcGames;
}

ImageType::Certainty VGARawImageType::isInstance(iostream_sptr psImage) const
	throw (std::ios::failure)
{
	psImage->seekg(0, std::ios::end);
	io::stream_offset len = psImage->tellg();

	// TESTED BY: TODO
	if (len == 64000) return PossiblyYes;

	// TESTED BY: TODO
	return DefinitelyNo;
}

ImagePtr VGARawImageType::create(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	fnTruncate(64000);
	char buf[64];
	memset(buf, 0, 64);
	for (int i = 0; i < 1000; i++) psImage->write(buf, 64);

	ImagePtr palFile(new VGAPalette(
		suppData[SuppItem::Palette].stream,
		suppData[SuppItem::Palette].fnTruncate
	));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new VGARawImage(psImage, 320, 200, pal));
}

ImagePtr VGARawImageType::open(iostream_sptr psImage,
	FN_TRUNCATE fnTruncate, MP_SUPPDATA& suppData) const
	throw (std::ios::failure)
{
	ImagePtr palFile(new VGAPalette(
		suppData[SuppItem::Palette].stream,
		suppData[SuppItem::Palette].fnTruncate
	));
	PaletteTablePtr pal = palFile->getPalette();
	return ImagePtr(new VGARawImage(psImage, 320, 200, pal));
}

MP_SUPPLIST VGARawImageType::getRequiredSupps(const std::string& filenameImage) const
	throw ()
{
	MP_SUPPLIST supps;
	std::string filenameBase =
		filenameImage.substr(0, filenameImage.find_last_of('.'));
	supps[SuppItem::Palette] = filenameBase + ".pal"; // TODO: case sensitivity?
	return supps;
}


VGARawImage::VGARawImage(iostream_sptr data, int width, int height,
	PaletteTablePtr pal)
	throw () :
		VGAImage(data, NULL, 0),
		width(width),
		height(height),
		pal(pal)
{
}

VGARawImage::~VGARawImage()
	throw ()
{
}

int VGARawImage::getCaps()
	throw ()
{
	return ColourDepthVGA | HasPalette;
}

void VGARawImage::getDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->width;
	*height = this->height;
	return;
}

void VGARawImage::setDimensions(unsigned int width, unsigned int height)
	throw (std::ios::failure)
{
	throw std::ios::failure("this image is a fixed size");
}

PaletteTablePtr VGARawImage::getPalette()
	throw (std::ios::failure)
{
	return this->pal;
}

} // namespace gamegraphics
} // namespace camoto