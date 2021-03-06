/**
 * @file  tls-wacky.cpp
 * @brief Wacky Wheels tileset
 *
 * This file format is fully documented on the ModdingWiki:
 *   http://www.shikadi.net/moddingwiki/Wacky_Wheels
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

#include "tls-wacky.hpp"
#include "tls-img.hpp"
#include "img-pcx.hpp"

namespace camoto {
namespace gamegraphics {

#define WW_TILE_WIDTH  32 ///< Width of each tile
#define WW_TILE_HEIGHT 32 ///< Height of each tile

#define WW_TILES_X (320 / WW_TILE_WIDTH)  ///< Number of tiles in a row
#define WW_TILES_Y (200 / WW_TILE_HEIGHT) ///< Number of tile rows

TilesetType_Wacky::TilesetType_Wacky()
{
}

TilesetType_Wacky::~TilesetType_Wacky()
{
}

std::string TilesetType_Wacky::getCode() const
{
	return "tls-wacky";
}

std::string TilesetType_Wacky::getFriendlyName() const
{
	return "Wacky Wheels Tileset";
}

// Get a list of the known file extensions for this format.
std::vector<std::string> TilesetType_Wacky::getFileExtensions() const
{
	std::vector<std::string> vcExtensions;
	vcExtensions.push_back("pcx");
	return vcExtensions;
}

std::vector<std::string> TilesetType_Wacky::getGameList() const
{
	std::vector<std::string> vcGames;
	vcGames.push_back("Wacky Wheels");
	return vcGames;
}

TilesetType_Wacky::Certainty TilesetType_Wacky::isInstance(stream::input_sptr psGraphics) const
{
	ImageType_PCX_LinearVGA pcx;
	if (!pcx.isInstance(psGraphics)) return DefinitelyNo;

/// @todo Implement this when TilesetType::open() can take a read-only stream
/*
	SuppData sd;
	ImagePtr img = pcx.open(psGraphics, sd);

	unsigned int width, height;
	img->getDimensions(&width, &height);
	if ((width != 320) || (height != 200)) return DefinitelyNo;
*/
	return PossiblyYes; // best we can hope for
}

TilesetPtr TilesetType_Wacky::create(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	// TODO
	throw stream::error("Not yet implemented");
}

TilesetPtr TilesetType_Wacky::open(stream::inout_sptr psGraphics,
	SuppData& suppData) const
{
	ImagePtr img(new Image_PCX(psGraphics, 8, 1));
	return TilesetPtr(new Image_TilesetFrom(img,
		WW_TILE_WIDTH, WW_TILE_HEIGHT, WW_TILES_X, WW_TILES_Y));
}

SuppFilenames TilesetType_Wacky::getRequiredSupps(const std::string& filenameGraphics) const
{
	// No supplemental types/empty list
	return SuppFilenames();
}

} // namespace gamegraphics
} // namespace camoto
