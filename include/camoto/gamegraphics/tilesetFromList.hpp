/**
 * @file   tilesetFromList.hpp
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

#ifndef _CAMOTO_TLS_IMG_LIST_HPP_
#define _CAMOTO_TLS_IMG_LIST_HPP_

#include <camoto/gamegraphics/tileset.hpp>

namespace camoto {
namespace gamegraphics {

class TilesetFromList: virtual public Tileset
{
	public:
		struct Tile {
			ImagePtr img;
			unsigned int xOffset;
			unsigned int yOffset;
			unsigned int width;
			unsigned int height;
		};
		typedef std::vector<Tile> TileList;

		struct ImageEntry: public Entry {
			unsigned int index; ///< Zero-based index of tile
		};

		TilesetFromList(const TileList& tileList);

		virtual ~TilesetFromList();

		virtual int getCaps();

		virtual const VC_ENTRYPTR& getItems() const;

		virtual ImagePtr openImage(const EntryPtr& id);

		virtual EntryPtr insert(const EntryPtr& idBeforeThis, int attr);

		virtual void remove(EntryPtr& id);

		void resize(EntryPtr& id, stream::len newSize);

		virtual void flush();

		virtual PaletteTablePtr getPalette();

		void setPalette(PaletteTablePtr newPalette);

	protected:
		TileList tileList;   ///< List of underlying images
		VC_ENTRYPTR items;   ///< List of tiles

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_TLS_IMG_LIST_HPP_
