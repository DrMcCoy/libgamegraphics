/**
 * @file   subimage.hpp
 * @brief  Image specialisation images within other images.
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

#ifndef _CAMOTO_SUBIMAGE_HPP_
#define _CAMOTO_SUBIMAGE_HPP_

#include <camoto/gamegraphics/imagetype.hpp>

namespace camoto {
namespace gamegraphics {

/// Image stored within another Image.
class SubImage: virtual public Image {

	public:
		/// Constructor
		/**
		 * Create an image from the supplied stream.
		 *
		 * @param img
		 *   Underlying image.
		 *
		 * @param xOffset
		 *   X-coordinate of pixel in underlying image that will appear at (0,0) in
		 *   this image.  Must be contained within the underlying image.
		 *
		 * @param yOffset
		 *   Y-coordinate of pixel in underlying image that will appear at (0,0) in
		 *   this image.  Must be contained within the underlying image.
		 *
		 * @param width
		 *   Width of this image.  x + width must not exceed the width of the
		 *   underlying image.
		 *
		 * @param y
		 *   Height of this image.  y + height must not exceed the width of the
		 *   underlying image.
		 */
		SubImage(ImagePtr img, int xOffset, int yOffset, int width, int height)
			throw (std::ios::failure);

		virtual ~SubImage()
			throw ();

		virtual int getCaps()
			throw ();

		virtual void getDimensions(unsigned int *width, unsigned int *height)
			throw ();

		virtual void setDimensions(unsigned int width, unsigned int height)
			throw (std::ios::failure);

		virtual StdImageDataPtr toStandard()
			throw (std::ios::failure);

		virtual StdImageDataPtr toStandardMask()
			throw ();

		virtual void fromStandard(StdImageDataPtr newContent,
			StdImageDataPtr newMask)
			throw (std::ios::failure);

	protected:
		/// Common code between toStandard() and toStandardMask()
		StdImageDataPtr extractPortion(const StdImageDataPtr& source)
			throw ();

		ImagePtr img;               ///< Underlying image
		StdImageDataPtr parent;     ///< Pixel data cache
		StdImageDataPtr parentMask; ///< Pixel data cache for mask
		int xOffset;
		int yOffset;
		int width;
		int height;

};

} // namespace gamegraphics
} // namespace camoto

#endif // _CAMOTO_SUBIMAGE_HPP_