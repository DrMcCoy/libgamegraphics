/**
 * @file  img-palette.hpp
 * @brief Image implementation of a Palette file.  This will be inherited by
 *        classes implementing a specific palette file formats.
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

#ifndef _CAMOTO_IMG_PALETTE_HPP_
#define _CAMOTO_IMG_PALETTE_HPP_

#include "baseimage.hpp"

namespace camoto {
namespace gamegraphics {

/// Image specialisation for palette files.
class Palette: virtual public Image_Base
{
	public:
		Palette();
		virtual ~Palette();

		virtual int getCaps();
		virtual void getDimensions(unsigned int *width, unsigned int *height);
		virtual StdImageDataPtr toStandard();
		virtual StdImageDataPtr toStandardMask();
		virtual PaletteTablePtr getPalette() = 0;
		virtual void setPalette(PaletteTablePtr newPalette) = 0;
		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask);
};

} // namespace gamearchive
} // namespace camoto

#endif // _CAMOTO_IMG_PALETTE_HPP_
