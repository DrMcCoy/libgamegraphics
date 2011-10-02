/**
 * @file   tls-img.cpp
 * @brief  Tiles stored as a grid in an image file.
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

#include "tls-img.hpp"
#include "subimage.hpp"

namespace camoto {
namespace gamegraphics {

TilesetFromImage::TilesetFromImage(ImagePtr img,
	int tileWidth, int tileHeight, int tilesWide, int tilesHigh)
	throw (std::ios::failure) :
		img(img),
		tileWidth(tileWidth),
		tileHeight(tileHeight),
		tilesWide(tilesWide),
		tilesHigh(tilesHigh)
{
	int numImages = this->tilesWide * this->tilesHigh;
	this->items.reserve(numImages);
	for (int i = 0; i < numImages; i++) {
		ImageEntry *fat = new ImageEntry();
		EntryPtr ep(fat);
		fat->isValid = true;
		fat->attr = None;
		fat->index = i;
		this->items.push_back(ep);
	}
}

TilesetFromImage::~TilesetFromImage()
	throw ()
{
}

int TilesetFromImage::getCaps()
	throw ()
{
	int caps = 0;
	int imgCaps = this->img->getCaps();
	if (imgCaps & Image::HasPalette) caps |= Tileset::HasPalette;
	return caps;
}

const Tileset::VC_ENTRYPTR& TilesetFromImage::getItems() const
	throw ()
{
	return this->items;
}

ImagePtr TilesetFromImage::openImage(const EntryPtr& id)
	throw (std::ios::failure)
{
	ImageEntry *fat = dynamic_cast<ImageEntry *>(id.get());
	assert(fat);
	int x = (fat->index % this->tilesWide) * this->tileWidth;
	int y = (fat->index / this->tilesWide) * this->tileHeight;

#ifdef DEBUG
	unsigned int imgWidth, imgHeight;
	this->img->getDimensions(&imgWidth, &imgHeight);
	assert(x + this->tileWidth <= imgWidth);
	assert(y + this->tileHeight <= imgHeight);
#endif

	ImagePtr subimg(new SubImage(this->img, x, y, this->tileWidth, this->tileHeight));
	return subimg;
}

Tileset::EntryPtr TilesetFromImage::insert(const EntryPtr& idBeforeThis, int attr)
	throw (std::ios::failure)
{
	throw std::ios::failure("tiles in this tileset cannot be rearranged (yet?)");
}

void TilesetFromImage::remove(EntryPtr& id)
	throw (std::ios::failure)
{
	throw std::ios::failure("tiles in this tileset cannot be rearranged (yet?)");
}

void TilesetFromImage::flush()
	throw (std::ios::failure)
{
	return;
}

void TilesetFromImage::resize(EntryPtr& id, size_t newSize)
	throw (std::ios::failure)
{
	if (newSize != this->tileWidth * this->tileHeight) {
		throw std::ios::failure("tiles in this tileset are a fixed size");
	}
	return;
}

void TilesetFromImage::getTilesetDimensions(unsigned int *width, unsigned int *height)
	throw ()
{
	*width = this->tileWidth;
	*height = this->tileHeight;
	return;
}

unsigned int TilesetFromImage::getLayoutWidth()
	throw ()
{
	return this->tilesWide;
}

PaletteTablePtr TilesetFromImage::getPalette()
	throw ()
{
	return this->img->getPalette();
}

void TilesetFromImage::setPalette(PaletteTablePtr newPalette)
	throw (std::ios::failure)
{
	this->img->setPalette(newPalette);
	return;
}

} // namespace gamegraphics
} // namespace camoto