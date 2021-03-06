/**
 * @file  tls-ccaves-main.hpp
 * @brief Crystal Caves top-level tileset handler (containing sub tilesets)
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

#ifndef _CAMOTO_TLS_CCAVES_MAIN_HPP_
#define _CAMOTO_TLS_CCAVES_MAIN_HPP_

#include <camoto/gamegraphics/tilesettype.hpp>
#include "tileset-fat.hpp"

namespace camoto {
namespace gamegraphics {

class TilesetType_CCavesMain: virtual public TilesetType
{
	public:
		TilesetType_CCavesMain();
		virtual ~TilesetType_CCavesMain();

		virtual std::string getCode() const;
		virtual std::string getFriendlyName() const;
		virtual std::vector<std::string> getFileExtensions() const;
		virtual std::vector<std::string> getGameList() const;
		virtual Certainty isInstance(stream::input_sptr fsGraphics) const;
		virtual TilesetPtr create(stream::inout_sptr psGraphics,
			SuppData& suppData) const;
		virtual TilesetPtr open(stream::inout_sptr fsGraphics,
			SuppData& suppData) const;
		virtual SuppFilenames getRequiredSupps(const std::string& filenameGraphics)
			const;

	protected:
		/// How much to pad each tileset (in bytes)
		stream::len pad;
};

class Tileset_CCavesMain: virtual public Tileset_FAT
{
	public:
		Tileset_CCavesMain(stream::inout_sptr data,
			unsigned int numPlanes, stream::len pad);
		virtual ~Tileset_CCavesMain();

		virtual int getCaps();

		// Tileset_FAT
		virtual TilesetPtr createTilesetInstance(const EntryPtr& id,
			stream::inout_sptr content);

	protected:
		unsigned int numPlanes;
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_CCAVES_MAIN_HPP_
