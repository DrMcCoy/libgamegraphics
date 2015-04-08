/**
 * @file  test-img-ega-planar.cpp
 * @brief Test code for conversion to and from planar EGA data.
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

#include "../src/img-ega-planar.hpp"
#include "test-image.hpp"

class test_img_ega_planar: public test_image
{
	public:
		test_img_ega_planar()
		{
			this->type = "img-ega-planar";
			this->hasMask = true;
			this->hasHitmask = true;
		}

		void addTests()
		{
			this->test_image::addTests();

			this->sizedContent({8, 8}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\x81\x81\x81\x81\x81\x81\xFF"
				"\xFF\x00\x00\x00\x00\x00\x00\x7E"
				"\xFF\x01\x01\x01\x01\x01\x01\x01"
				"\xFF\x80\x80\x80\x80\x80\x80\x81"
				"\xFF\x81\x81\x81\x81\x81\x81\xFF"
				"\x00\xFF\x00\x00\x00\x00\x00\x00"
			));

			this->sizedContent({16, 16}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01" "\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFF"
				"\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\xFE"
				"\xFF\xFF\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01" "\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01\x00\x01"
				"\xFF\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00" "\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x01"
				"\xFF\xFF\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01" "\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\x80\x01\xFF\xFF"
				"\x00\x00\xFF\xFF\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
			));

			this->sizedContent({9, 9}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\xFF\x80"
				"\xFF\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x7F\x00"
				"\xFF\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80"
				"\xFF\x80\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x00\x80\x80"
				"\xFF\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\xFF\x80"
				"\x00\x00\xFF\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
			));

			this->sizedContent({8, 4}, ImageType::DefinitelyYes, STRING_WITH_NULLS(
				"\xFF\x81\x81\xFF"
				"\xFF\x00\x00\x7E"
				"\xFF\x01\x01\x01"
				"\xFF\x80\x80\x81"
				"\xFF\x81\x81\xFF"
				"\x00\xFF\x00\x00"
			));
		}

		virtual std::string initialstate() const
		{
			// No instance-related tests for this format.
			return {};
		}

		virtual std::unique_ptr<Image> openImage(const Point& dims,
			std::unique_ptr<stream::inout> content, ImageType::Certainty result,
			bool create)
		{
			EGAPlaneLayout planes = {
				PlanePurpose::Opaque1, // swaps
				PlanePurpose::Blue1,
				PlanePurpose::Green1,
				PlanePurpose::Red1,
				PlanePurpose::Intensity1,
				PlanePurpose::Hit1
			};

			return std::make_unique<Image_EGA_Planar>(
				std::move(content), 0, dims, planes, nullptr
			);
		}
};

IMPLEMENT_TESTS(img_ega_planar);
