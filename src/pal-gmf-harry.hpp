/**
 * @file  pal-gmf-harry.hpp
 * @brief Interface to the palette stored inside Halloween Harry levels.
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

#ifndef _CAMOTO_GAMEGRAPHICS_PAL_GMF_HARRY_HPP_
#define _CAMOTO_GAMEGRAPHICS_PAL_GMF_HARRY_HPP_

#include <camoto/gamegraphics/imagetype.hpp>
#include "img-palette.hpp"

namespace camoto {
namespace gamegraphics {

/// Filetype handler for Halloween Harry palette files.
class ImageType_Palette_HarryGMF: virtual public ImageType {

	public:

		ImageType_Palette_HarryGMF();

		virtual ~ImageType_Palette_HarryGMF();

		virtual std::string getCode() const;

		virtual std::string getFriendlyName() const;

		virtual std::vector<std::string> getFileExtensions() const;

		virtual std::vector<std::string> getGameList() const;

		virtual Certainty isInstance(stream::input_sptr fsImage) const;

		virtual ImagePtr create(stream::inout_sptr psImage,
			SuppData& suppData) const;

		virtual ImagePtr open(stream::inout_sptr fsImage,
			SuppData& suppData) const;

		virtual SuppFilenames getRequiredSupps(const std::string& filenameImage) const;

};

/// Palette interface to Halloween Harry GMF files.
class Palette_HarryGMF: virtual public Palette {

	private:
		stream::inout_sptr data;

	public:
		Palette_HarryGMF(stream::inout_sptr data);

		virtual ~Palette_HarryGMF();

		virtual PaletteTablePtr getPalette();

		virtual void setPalette(PaletteTablePtr newPalette);
};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_GAMEGRAPHICS_PAL_GMF_HARRY_HPP_
