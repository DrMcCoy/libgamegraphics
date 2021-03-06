/**
 * @file  gametls.cpp
 * @brief Command-line interface to libgamegraphics.
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

#include <boost/algorithm/string.hpp> // for case-insensitive string compare
#include <boost/program_options.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/bind.hpp>
#include <camoto/gamegraphics.hpp>
#include <camoto/util.hpp>
#include <camoto/stream_file.hpp>
#include <iostream>
#include "common.hpp"

namespace po = boost::program_options;

#define PROGNAME "gametls"

/*** Return values ***/
// All is good
#define RET_OK                 0
// Bad arguments (missing/invalid parameters)
#define RET_BADARGS            1
// Major error (couldn't open tileset file, etc.)
#define RET_SHOWSTOPPER        2
// More info needed (-t auto didn't work, specify a type)
#define RET_BE_MORE_SPECIFIC   3
// One or more files failed, probably user error (file not found, etc.)
#define RET_NONCRITICAL_FAILURE 4
// Some files failed, but not in a common way (cut off write, disk full, etc.)
#define RET_UNCOMMON_FAILURE   5

// Split a string in two at a delimiter, e.g. "one=two" becomes "one" and "two"
// and true is returned.  If there is no delimiter both output strings will be
// the same as the input string and false will be returned.
//
// If delim == '=' then:
// in        -> ret, out1, out2
// "one=two" -> true, "one", "two"
// "one=two=three" -> true, "one=two", "three"
// "four" -> false, "four", "four"
// If delim == '@' then:
// "one@two" -> true, "one", "two"
// "@1=myfile@@4"
// "test.txt@here.txt"
// "@2=test.txt"
// "e1m1.mid=mysong.mid:@4"
// "e1m1.mid=mysong.mid:e1m2.mid"
bool split(const std::string& in, char delim, std::string *out1, std::string *out2)
{
	std::string::size_type iEqualPos = in.find_last_of(delim);
	*out1 = in.substr(0, iEqualPos);
	// Does the destination have a different filename?
	bool bAltDest = iEqualPos != std::string::npos;
	*out2 = bAltDest ? in.substr(iEqualPos + 1) : *out1;
	return bAltDest;
}

/// Dump a list of tilesets and images
/**
 * This is a recursive function to display a list of tilesets and images when
 * the -l option is used.
 *
 * @return true to continue, false on error (e.g. corrupt file suspected)
 */
void printTilesetList(std::string prefix, gg::TilesetPtr pTileset,
	bool bScript
) {
	const gg::Tileset::VC_ENTRYPTR& tiles = pTileset->getItems();

	if (bScript) {
		std::cout << "id=" << prefix << ";type=tileset";
	} else {
		std::cout << prefix << ": Tileset";
	}

	if (pTileset->getCaps() & gg::Tileset::ChangeDimensions) {
		// This tileset has dimensions
		unsigned int width, height;
		pTileset->getTilesetDimensions(&width, &height);
		if (bScript) {
			std::cout << ";width=" << width << ";height=" << height;
		} else {
			std::cout << " (" << width << 'x' << height << ")";
		}
	}
	if (bScript) {
		std::cout << ";numitems=" << tiles.size() << '\n';
	} else {
		std::cout << ", ";
		if (tiles.size()) {
			std::cout << tiles.size() << " items\n";
		} else {
			std::cout << "no images" << '\n';
		}
	}

	int j = 0;
	for (gg::Tileset::VC_ENTRYPTR::const_iterator i = tiles.begin();
		i != tiles.end();
		i++
	) {
		// If this is an empty slot skip it, because we can't open it of course
		if ((*i)->getAttr() & gg::Tileset::EmptySlot) {
			if (bScript) {
				std::cout << "id=" << prefix << '.' << j
					<< ";type=empty\n";
			} else {
				std::cout << prefix << '.' << j << ": Empty slot\n";
			}
		// If this entry is a tileset, open it and dump its contents recursively
		} else if ((*i)->getAttr() & gg::Tileset::SubTileset) {
			gg::TilesetPtr tileset = pTileset->openTileset(*i);
			std::ostringstream ss;
			ss << prefix << '.' << j;
			printTilesetList(ss.str(), tileset, bScript);

		// Otherwise if it's an image just dump the specs
		} else {
			gg::ImagePtr img = pTileset->openImage(*i);
			unsigned int iwidth, iheight;
			img->getDimensions(&iwidth, &iheight);
			if (bScript) {
				std::cout << "id=" << prefix << '.' << j
					<< ";type=image;width=" << iwidth
					<< ";height=" << iheight << '\n';
			} else {
				std::cout << prefix << '.' << j << ": Image (" << iwidth << "x"
					<< iheight << ")\n";
			}
		}
		j++;
	}

	return;
}


/// Explode an ID into a list of ints
/**
 * Take an ID like 0.3.1/15 and convert it into a vector like [0, 3, 1, -16].
 *
 * @note Because -0 is not possible, the (optional) final image value will
 *       start at -1.  This value will need to be adjusted before it can be
 *       passed to the openImage() function.
 *
 * @param  id  Pointer to a vector where each element should be appended.
 *
 * @param  name  ID to be examined.
 *
 * @return true on success, false on an invalid ID
 */
bool explodeId(std::vector<unsigned int> *id, const std::string& name)
{
	int next = 0;
	bool img = false; // is the next number an image? (if not, it's a tileset)
	for (unsigned int i = 0; i < name.length(); i++) {
		if ((name[i] == '.') || (name[i] == '+')) {
			// Separator
			id->push_back(next);
			next = 0;
			img = (name[i] == '+'); // is next num an image?
		} else if ((name[i] >= '0') || (name[i] <= '9')) {
			next *= 10;
			next += name[i] - '0';
		} else {
			// Invalid char
			return false;
		}
	}
	// If the last separator was a slash, this is an image number so make it neg.
	if (img) {
		next++; // can't have -0 so images start at -1
		next *= -1;
	}
	id->push_back(next);
	return true;
}

/// Export a whole tileset to a single .png file
/**
 * Convert the given tileset into a PNG file on disk, by arranging each image
 * in the tileset as a grid of the given width.
 *
 * @param tileset
 *   The tileset to export.
 *
 * @param widthTiles
 *   The width of the exported .png image, in tiles.  0 means as wide as
 *   possible (all tiles on a single row.)
 *
 * @param destFile
 *   Filename of destination (including ".png")
 */
void tilesetToPng(gg::TilesetPtr tileset, unsigned int widthTiles,
	const std::string& destFile)
{
	if (widthTiles == 0) {
		widthTiles = tileset->getLayoutWidth();
	}
	if (widthTiles == 0) widthTiles = 16;

	const gg::Tileset::VC_ENTRYPTR& tiles = tileset->getItems();
	unsigned int numTiles = tiles.size();
	if (widthTiles > numTiles) widthTiles = numTiles;
	unsigned int heightTiles = (numTiles + widthTiles - 1) / widthTiles;

	unsigned int width, height;
	tileset->getTilesetDimensions(&width, &height);
	if ((width == 0) || (height == 0)) {
		throw stream::error("this only works with tilesets where all tiles "
			"are the same size");
	}

	png::image<png::index_pixel> png(width * widthTiles, height * heightTiles);

	bool useMask;
	gg::PaletteTablePtr srcPal;
	if (tileset->getCaps() & gg::Tileset::HasPalette) {
		srcPal = tileset->getPalette();
	} else {
		// Need to use the default palette
		switch (tileset->getCaps() & gg::Tileset::ColourDepthMask) {
			case gg::Tileset::ColourDepthVGA:
				srcPal = gg::createPalette_DefaultVGA();
				srcPal->pop_back(); // remove entry 255 to make room for transparency
				break;
			case gg::Tileset::ColourDepthEGA:
				srcPal = gg::createPalette_DefaultEGA();
				break;
			case gg::Tileset::ColourDepthCGA:
				srcPal = gg::createPalette_CGA(gg::CGAPal_CyanMagenta);
				break;
			case gg::Tileset::ColourDepthMono:
				srcPal = gg::createPalette_DefaultMono();
				break;
		}
	}

	unsigned int palSize = srcPal->size();
	int j = 0;

	// Figure out whether there's enough room in the palette to use the image
	// mask for transparency, or whether we have to fall back to palette index
	// transparency only.
	png::tRNS transparency;
	if (palSize < 256) {
		transparency.push_back(j);
		j++;
		palSize++;
		useMask = true;
	} else {
		// Not enough room in the palette for masked transparent entry
		useMask = false;
	}
	png::palette pal(palSize);

	// Set a colour for the transparent palette entry, for apps which can't
	// display transparent pixels (we couldn't set this above.)
	if (useMask) pal[0] = png::color(0xFF, 0x00, 0xFF);

	for (gg::PaletteTable::iterator
		i = srcPal->begin(); i != srcPal->end(); i++, j++
	) {
		pal[j] = png::color(i->red, i->green, i->blue);
		if (i->alpha == 0x00) transparency.push_back(j);
	}
	png.set_palette(pal);
	if (transparency.size()) png.set_tRNS(transparency);

	int t = 0;
	for (gg::Tileset::VC_ENTRYPTR::const_iterator i = tiles.begin();
		i != tiles.end();
		i++, t++
	) {
		if ((*i)->getAttr() & gg::Tileset::SubTileset) continue; // aah! tileset! bad!

		gg::ImagePtr img = tileset->openImage(*i);
		gg::StdImageDataPtr data = img->toStandard();
		gg::StdImageDataPtr mask = img->toStandardMask();

		unsigned int offX = (t % widthTiles) * width;
		unsigned int offY = (t / widthTiles) * height;

		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int x = 0; x < width; x++) {
				if (useMask) {
					if (mask[y*width+x] & 0x01) {
						png[offY+y][offX+x] = png::index_pixel(0);
					} else {
						png[offY+y][offX+x] =
							// +1 to the colour to skip over transparent (#0)
							png::index_pixel(data[y*width+x] + 1);
					}
				} else {
					png[offY+y][offX+x] = png::index_pixel(data[y*width+x]);
				}
			}
		}
	}

	png.write(destFile);
	return;
}

/// Replace a tileset with the contents of a .png file
/**
 * Load the given PNG file and use it to replace the given tileset.  The .png
 * image must be an even multiple of the tile width.
 *
 * @param  tileset  Tileset to overwrite
 *
 * @param  srcFile  Filename of source (including ".png")
 */
void pngToTileset(gg::TilesetPtr tileset, const std::string& srcFile)
{
	png::image<png::index_pixel> png(srcFile);

	unsigned int width, height;
	tileset->getTilesetDimensions(&width, &height);
	if ((width == 0) || (height == 0)) {
		throw stream::error("this only works with tilesets where all tiles "
			"are the same size");
	}

	if ((png.get_width() % width) != 0) {
		throw stream::error("image width must be a multiple of the tile width");
	}
	if ((png.get_height() % height) != 0) {
		throw stream::error("image height must be a multiple of the tile height");
	}

	gg::PaletteTablePtr tilesetPal;
	if (tileset->getCaps() & gg::Tileset::HasPalette) {
		tilesetPal = tileset->getPalette();
	} else {
		// Need to use the default palette
		switch (tileset->getCaps() & gg::Tileset::ColourDepthMask) {
			case gg::Tileset::ColourDepthVGA:
				tilesetPal = gg::createPalette_DefaultVGA();
				break;
			case gg::Tileset::ColourDepthEGA:
				tilesetPal = gg::createPalette_DefaultEGA();
				break;
			case gg::Tileset::ColourDepthCGA:
				tilesetPal = gg::createPalette_CGA(gg::CGAPal_CyanMagenta);
				break;
			case gg::Tileset::ColourDepthMono:
				tilesetPal = gg::createPalette_DefaultMono();
				break;
		}
	}

	signed int paletteMap[256]; ///< -1 means that colour is transparent, 0 == EGA/VGA black, etc.
	// Create a palette map in case the .png colours aren't in the same
	// order as the original palette.  This is needed because some image
	// editors (e.g. GIMP) omit colours from the palette if they are
	// unused, messing up the index values.
	memset(paletteMap, 0, sizeof(paletteMap));
	const png::palette& pngPal = png.get_palette();
	int i_index = 0;
	for (png::palette::const_iterator
		i = pngPal.begin(); i != pngPal.end(); i++, i_index++
	) {
		int j_index = 0;
		for (gg::PaletteTable::iterator
			j = tilesetPal->begin(); j != tilesetPal->end(); j++, j_index++
		) {
			if (
				(i->red == j->red) &&
				(i->green == j->green) &&
				(i->blue == j->blue)
			) {
				paletteMap[i_index] = j_index;
			}
		}
	}
	png::tRNS transparency = png.get_tRNS();
	for (png::tRNS::const_iterator
		tx = transparency.begin(); tx != transparency.end(); tx++
	) {
		// This colour index is transparent
		paletteMap[*tx] = -1;
	}

	unsigned int tilesX = png.get_width() / width;
	unsigned int tilesY = png.get_height() / height;
	const gg::Tileset::VC_ENTRYPTR& tiles = tileset->getItems();
	unsigned int numTiles = tiles.size();
	if (numTiles > tilesX * tilesY) numTiles = tilesX * tilesY;

	int imgSizeBytes = width * height;
	uint8_t *imgData = new uint8_t[imgSizeBytes];
	uint8_t *maskData = new uint8_t[imgSizeBytes];
	gg::StdImageDataPtr stdimg(imgData);
	gg::StdImageDataPtr stdmask(maskData);

	int t = 0;
	for (gg::Tileset::VC_ENTRYPTR::const_iterator i = tiles.begin();
		i != tiles.end();
		i++, t++
	) {
		if ((*i)->getAttr() & gg::Tileset::SubTileset) continue; // aah! tileset! bad!

		gg::ImagePtr img = tileset->openImage(*i);

		unsigned int offX = (t % tilesX) * width;
		unsigned int offY = (t / tilesX) * height;

		for (unsigned int y = 0; y < height; y++) {
			for (unsigned int x = 0; x < width; x++) {
				signed int pixel = paletteMap[png[offY + y][offX + x]];
				if (pixel == -1) { // Palette #0 must be transparent
					maskData[y * width + x] = 0x01; // transparent
					imgData[y * width + x] = 0x00; // use black in case someone else ignores transparency
				} else {
					maskData[y * width + x] = 0x00; // opaque
					imgData[y * width + x] = pixel;
				}
			}
		}

		if (img->getCaps() & gg::Image::HasPalette) {
			// TODO: This format supports custom palettes, so update it from the
			// PNG image.
			//img->setPalette(...);
		}
		img->fromStandard(stdimg, stdmask);
	}

	return;
}

/// Export all images in the graphics file as either individual images or
/// tilesets.
/**
 * Extract all images or tilesets and convert them into PNG files.
 *
 * @param prefix
 *   ID prefix (used when recurring, use "0" on first call)
 *
 * @param tilesetAsSingleImage
 *   false to extract each image as a separate .png file, true to combine
 *   images into one .png per tileset
 *
 * @param widthTiles
 *   The width of the exported .png image, in tiles.  0 means as wide as
 *   possible (all tiles on a single row)
 *
 * @param tiles
 *   Tileset file to export
 *
 * @param bScript
 *   true if -s option given (produces easily parseable output)
 */
void extractAllImages(std::string prefix, bool tilesetAsSingleImage,
	int widthTiles, gg::TilesetPtr tileset, bool bScript
) {
	const gg::Tileset::VC_ENTRYPTR& tiles = tileset->getItems();

	int j = 0;
	for (gg::Tileset::VC_ENTRYPTR::const_iterator i = tiles.begin();
		i != tiles.end();
		i++, j++
	) {
		try {
			if ((*i)->getAttr() & gg::Tileset::SubTileset) {
				if (tilesetAsSingleImage) {
					std::ostringstream ssFilename;
					ssFilename << prefix << '.' << ".png";
					if (bScript) {
						std::cout << "id=" << prefix << '.' << j
							<< ";filename=" << ssFilename.str()
							<< ";status=";
					} else {
						std::cout << " extracting: " << ssFilename.str() << std::endl;
					}
					try {
						tilesetToPng(tileset, widthTiles, ssFilename.str());
						if (bScript) {
							std::cout << "ok" << std::endl;
						}
					} catch (std::exception& e) {
						if (bScript) {
							std::cout << "fail" << std::endl;
						} else {
							std::cout << " [failed; " << e.what() << "]" << std::endl;
						}
						//iRet = RET_NONCRITICAL_FAILURE; // one or more files failed
					}
				} else {
					std::ostringstream ss;
					ss << prefix << '.' << j;
					gg::TilesetPtr sub = tileset->openTileset(*i);
					assert(sub); // must throw exception on failure
					extractAllImages(ss.str(), tilesetAsSingleImage, widthTiles,
						sub, bScript);
				}

			} else { // single image
				std::ostringstream ssFilename;
				ssFilename << prefix << '.' << j << ".png";
				if (bScript) {
					std::cout << "id=" << prefix << '.' << j
						<< ";filename=" << ssFilename.str()
						<< ";status=";
				} else {
					std::cout << " extracting: " << ssFilename.str() << std::endl;
				}
				gg::ImagePtr img = tileset->openImage(*i);
				imageToPng(img, ssFilename.str());
				if (bScript) {
					std::cout << "ok" << std::endl;
				}
			}
		} catch (std::exception& e) {
			if (bScript) {
				std::cout << "fail" << std::endl;
			} else {
				std::cout << " [failed; " << e.what() << "]" << std::endl;
			}
			//iRet = RET_NONCRITICAL_FAILURE; // one or more files failed
		}
	}

	return;
}

/// Find the EntryPtr for the given (user-supplied) ID.
/**
 * Take the given ID and try to open it.
 *
 * @param idText
 *   The ID to open.
 *
 * @param ep
 *   On return, set to an image or null if the ID is for a tileset.
 *
 * @param tileset
 *   Root tileset to search under.  On return, set to the tileset containing ep
 *   or NULL on invalid ID.
 *
 * @return EntryPtr to file represented by id, or NULL if the ID specified a
 *   tileset (which is supplied in tileset, which could also be NULL if the ID
 *   was invalid.)
 */
gg::Tileset::EntryPtr idToEntryPtr(const std::string& idText,
	gg::TilesetPtr& tileset
) {
	gg::Tileset::EntryPtr ep;
	std::vector<unsigned int> id;
	if (explodeId(&id, idText)) {
		// TODO: Make sure the next line's +1 condition works when it == id.end(),
		//       i.e. there's only images here, no subtilesets
		for (std::vector<unsigned int>::iterator i = id.begin() + 1; i != id.end(); i++) {

			if (ep) {
				// An image has already been found but there are more ID components,
				// so technically the ID is invalid.
				std::cout << " [failed; invalid ID]" << std::endl;
				tileset.reset();
				ep.reset();
				break;
			}

			const gg::Tileset::VC_ENTRYPTR& tiles = tileset->getItems();
			if (*i >= tiles.size()) {
				std::cout << " [failed; invalid ID]" << std::endl;
				tileset.reset();
				break;
			} else {
				if (tiles[*i]->getAttr() & gg::Tileset::SubTileset) {
					tileset = tileset->openTileset(tiles[*i]);
				} else {
					ep = tiles[*i];
				}
			}

		}
	} else {
		// Invalid ID
		tileset.reset();
	}
	return ep;
}

int main(int iArgC, char *cArgV[])
{
#ifdef __GLIBCXX__
	// Set a better exception handler
	std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
#endif

	// Disable stdin/printf/etc. sync for a speed boost
	std::ios_base::sync_with_stdio(false);

	// Declare the supported options.
	po::options_description poActions("Actions");
	poActions.add_options()
		("list,l",
			"list contents of the graphics file")

		("extract-all-images,X",
			"extract all images as separate files")

		("extract-all-tilesets,T",
			"extract all images, combining into one image file per tileset")

		("extract,x", po::value<std::string>(),
			"extract a specific image")

		("print,p", po::value<std::string>(),
			"print a specific image in ANSI text")

		("overwrite,o", po::value<std::string>(),
			"replace an image with the given file")

		("insert-image,i", po::value<std::string>(),
			"insert an image at the given ID")

		("insert-tileset,n", po::value<std::string>(),
			"insert an empty tileset at the given ID")

		("set-size,z", po::value<std::string>(),
			"set the tileset to the given image size in pixels (e.g. 16x16)")

	;

	po::options_description poOptions("Options");
	poOptions.add_options()
		("type,t", po::value<std::string>(),
			"specify the file format (default is autodetect)")
		("script,s",
			"format output suitable for script parsing")
		("force,f",
			"force open even if the file is not in the given format")
		("width,w", po::value<int>(),
			"width (in tiles) when exporting whole tileset")
		("list-types",
			"list available types that can be passed to --type")
	;

	po::options_description poHidden("Hidden parameters");
	poHidden.add_options()
		("file", "file to manipulate")
		("help", "produce help message")
	;

	po::options_description poVisible("");
	poVisible.add(poActions).add(poOptions);

	po::options_description poComplete("Parameters");
	poComplete.add(poActions).add(poOptions).add(poHidden);
	po::variables_map mpArgs;

	std::string strFilename;
	std::string strType;

	gg::ManagerPtr pManager(gg::getManager());

	int iRet = RET_OK;
	bool bScript = false; // show output suitable for script parsing?
	bool bForceOpen = false; // open anyway even if tileset not in given format?
	int iTilesetExportWidth = 0;  // Width when exporting whole tileset as single file (0 == entire tileset on one line)
	try {
		po::parsed_options pa = po::parse_command_line(iArgC, cArgV, poComplete);

		// Parse the global command line options
		for (std::vector<po::option>::iterator i = pa.options.begin(); i != pa.options.end(); i++) {
			if (i->string_key.empty()) {
				// If we've already got an tileset filename, complain that a second one
				// was given (probably a typo.)
				if (!strFilename.empty()) {
					std::cerr << "Error: unexpected extra parameter (multiple tileset "
						"filenames given?!)" << std::endl;
					return 1;
				}
				assert(i->value.size() > 0);  // can't have no values with no name!
				strFilename = i->value[0];
			} else if (i->string_key.compare("help") == 0) {
				std::cout <<
					"Copyright (C) 2010-2015 Adam Nielsen <malvineous@shikadi.net>\n"
					"This program comes with ABSOLUTELY NO WARRANTY.  This is free software,\n"
					"and you are welcome to change and redistribute it under certain conditions;\n"
					"see <http://www.gnu.org/licenses/> for details.\n"
					"\n"
					"Utility to edit tileset files used by games to store collections of images.\n"
					"Build date " __DATE__ " " __TIME__ << "\n"
					"\n"
					"Usage: " PROGNAME " <tileset> <action> [action...]\n" << poVisible
					<< "\n" << std::endl;
				return RET_OK;
			} else if (
				(i->string_key.compare("list-types") == 0)
			) {
				gg::TilesetTypePtr nextType;
				int i = 0;
				while ((nextType = pManager->getTilesetType(i++))) {
					std::string code = nextType->getCode();
					std::cout << code;
					int len = code.length();
					if (len < 20) std::cout << std::string(20-code.length(), ' ');
					std::cout << ' ' << nextType->getFriendlyName() << '\n';
				}
				return RET_OK;
			} else if (
				(i->string_key.compare("t") == 0) ||
				(i->string_key.compare("type") == 0)
			) {
				if (i->value.size() == 0) {
					std::cerr << PROGNAME ": --type (-t) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				strType = i->value[0];
			} else if (
				(i->string_key.compare("s") == 0) ||
				(i->string_key.compare("script") == 0)
			) {
				bScript = true;
			} else if (
				(i->string_key.compare("f") == 0) ||
				(i->string_key.compare("force") == 0)
			) {
				bForceOpen = true;
			} else if (
				(i->string_key.compare("w") == 0) ||
				(i->string_key.compare("width") == 0)
			) {
				if (i->value.size() == 0) {
					std::cerr << PROGNAME ": --width (-w) requires a parameter."
						<< std::endl;
					return RET_BADARGS;
				}
				iTilesetExportWidth = strtod(i->value[0].c_str(), NULL);
				if (iTilesetExportWidth < 1) {
					std::cerr << PROGNAME ": --width (-w) must be greater than zero."
						<< std::endl;
					return RET_BADARGS;
				}
			}
		}

		if (strFilename.empty()) {
			std::cerr << "Error: no graphics filename given" << std::endl;
			return RET_BADARGS;
		}
		std::cout << "Opening " << strFilename << " as type "
			<< (strType.empty() ? "<autodetect>" : strType) << std::endl;

		stream::file_sptr psTileset(new stream::file());
		try {
			psTileset->open(strFilename.c_str());
		} catch (const stream::open_error& e) {
			std::cerr << "Error opening " << strFilename << ": " << e.what()
				<< std::endl;
			return RET_SHOWSTOPPER;
		}

		gg::TilesetTypePtr pGfxType;
		if (strType.empty()) {
			// Need to autodetect the file format.
			gg::TilesetTypePtr pTestType;
			int i = 0;
			while ((pTestType = pManager->getTilesetType(i++))) {
				try {
					gg::TilesetType::Certainty cert = pTestType->isInstance(psTileset);
					switch (cert) {
						case gg::TilesetType::DefinitelyNo:
							// Don't print anything (TODO: Maybe unless verbose?)
							break;
						case gg::TilesetType::Unsure:
							std::cout << "File could be a " << pTestType->getFriendlyName()
								<< " [" << pTestType->getCode() << "]" << std::endl;
							// If we haven't found a match already, use this one
							if (!pGfxType) pGfxType = pTestType;
							break;
						case gg::TilesetType::PossiblyYes:
							std::cout << "File is likely to be a " << pTestType->getFriendlyName()
								<< " [" << pTestType->getCode() << "]" << std::endl;
							// Take this one as it's better than an uncertain match
							pGfxType = pTestType;
							break;
						case gg::TilesetType::DefinitelyYes:
							std::cout << "File is definitely a " << pTestType->getFriendlyName()
								<< " [" << pTestType->getCode() << "]" << std::endl;
							pGfxType = pTestType;
							// Don't bother checking any other formats if we got a 100% match
							goto finishTesting;
					}
				} catch (const stream::error& e) {
					std::cout << "Ignoring handler for " << pTestType->getFriendlyName()
						<< " due to error: " << e.what() << std::endl;
				}
			}
finishTesting:
			if (!pGfxType) {
				std::cerr << "Unable to automatically determine the file type.  Use "
					"the --type option to manually specify the file format." << std::endl;
				return RET_BE_MORE_SPECIFIC;
			}
		} else {
			gg::TilesetTypePtr pTestType(pManager->getTilesetTypeByCode(strType));
			if (!pTestType) {
				std::cerr << "Unknown file type given to -t/--type: " << strType
					<< std::endl;
				return RET_BADARGS;
			}
			pGfxType = pTestType;
		}

		assert(pGfxType != NULL);

		// Check to see if the file is actually in this format
		if (!pGfxType->isInstance(psTileset)) {
			if (bForceOpen) {
				std::cerr << "Warning: " << strFilename << " is not a "
					<< pGfxType->getFriendlyName() << ", open forced." << std::endl;
			} else {
				std::cerr << "Invalid format: " << strFilename << " is not a "
					<< pGfxType->getFriendlyName() << "\n"
					<< "Use the -f option to try anyway." << std::endl;
				return 3;
			}
		}

		// See if the format requires any supplemental files
		camoto::SuppFilenames suppList = pGfxType->getRequiredSupps(strFilename);
		camoto::SuppData suppData;
		if (suppList.size() > 0) {
			for (camoto::SuppFilenames::iterator i = suppList.begin(); i != suppList.end(); i++) {
				try {
					stream::file_sptr suppStream(new stream::file());
					std::cout << "Opening supplemental file " << i->second << std::endl;
					suppStream->open(i->second);
					suppData[i->first] = suppStream;
				} catch (const stream::open_error& e) {
					std::cerr << "Error opening supplemental file " << i->second
						<< ": " << e.what() << std::endl;
					return RET_SHOWSTOPPER;
				}
			}
		}

		// Open the graphics file
		gg::TilesetPtr pTileset(pGfxType->open(psTileset, suppData));
		assert(pTileset);

		bool modified = false; // have we changed the tileset file?

		// Run through the actions on the command line
		for (std::vector<po::option>::iterator i = pa.options.begin(); i != pa.options.end(); i++) {
			if (i->string_key.compare("list") == 0) {
				printTilesetList("0", pTileset, bScript);

			} else if (i->string_key.compare("extract-all-images") == 0) {
				extractAllImages("0", false, iTilesetExportWidth, pTileset, bScript);

			} else if (i->string_key.compare("extract-all-tilesets") == 0) {
				extractAllImages("0", true, iTilesetExportWidth, pTileset, bScript);

			} else if (i->string_key.compare("extract") == 0) {
				std::string id, strLocalFile;
				bool bAltDest = split(i->value[0], '=', &id, &strLocalFile);
				if (!bAltDest) strLocalFile += ".png";

				if (bScript) {
					std::cout << "extracting=" << id
						<< ";wrote=" << strLocalFile
						<< ";status=";
				} else {
					std::cout << " extracting: " << id;
					if (bAltDest) std::cout << " (into " << strLocalFile << ")";
					std::cout << std::flush;
				}

				gg::TilesetPtr nextTileset = pTileset;
				gg::Tileset::EntryPtr ep = idToEntryPtr(id, nextTileset);
				if (!ep) {
					if (nextTileset) {
						// We have an entire tileset to extract
						tilesetToPng(nextTileset, iTilesetExportWidth, strLocalFile);
						if (bScript) std::cout << "ok";
					} else {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; invalid ID]";
						iRet = RET_NONCRITICAL_FAILURE;
					}
				} else {
					// We have a single image to extract
					gg::ImagePtr img = nextTileset->openImage(ep);
					if (!img) {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; unable to open image]";
						iRet = RET_NONCRITICAL_FAILURE;
					} else {
						imageToPng(img, strLocalFile);
						if (bScript) std::cout << "ok";
					}
				}
				std::cout << std::endl;

			} else if (i->string_key.compare("print") == 0) {
				gg::TilesetPtr nextTileset = pTileset;
				const std::string& id = i->value[0];
				gg::Tileset::EntryPtr ep = idToEntryPtr(id, nextTileset);
				if (!ep) {
					if (nextTileset) {
						std::cerr << "-p/--print parameter must be an image ID (not a tileset ID.)" << std::endl;
						return RET_BADARGS;
					} else {
						std::cerr << "ERROR: Invalid ID \"" << id << "\"" << std::endl;
						return RET_BADARGS;
					}
				} else {
					// We have a single image to extract
					gg::ImagePtr img = nextTileset->openImage(ep);
					if (!img) {
						std::cout << " [failed; unable to open image]";
						iRet = RET_NONCRITICAL_FAILURE;
					} else {
						imageToANSI(img);
					}
				}

			} else if (i->string_key.compare("overwrite") == 0) {
				std::string id, strLocalFile;
				bool bAltDest = split(i->value[0], '=', &id, &strLocalFile);
				if (!bAltDest) strLocalFile += ".png";

				if (bScript) {
					std::cout << "overwriting=" << id
						<< ";read=" << strLocalFile
						<< ";status=";
				} else {
					std::cout << "overwriting: " << id;
					if (bAltDest) std::cout << " (from " << strLocalFile << ")";
					std::cout << std::flush;
				}

				gg::TilesetPtr nextTileset = pTileset;
				gg::Tileset::EntryPtr ep = idToEntryPtr(id, nextTileset);
				if (!ep) {
					if (nextTileset) {
						// We have an entire tileset to extract
						pngToTileset(nextTileset, strLocalFile);
						if (bScript) std::cout << "ok";
					} else {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; invalid ID]";
						iRet = RET_NONCRITICAL_FAILURE;
					}
				} else {
					// We have a single image to overwrite
					gg::ImagePtr img = nextTileset->openImage(ep);
					if (!img) {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; unable to open image]";
						iRet = RET_NONCRITICAL_FAILURE;
					} else {
						pngToImage(img, strLocalFile);
						modified = true;
						if (bScript) std::cout << "ok";
					}
				}
				if (nextTileset) nextTileset->flush();
				std::cout << std::endl;

			} else if (i->string_key.compare("insert-image") == 0) {
				std::string id, strLocalFile;
				bool bAltDest = split(i->value[0], '=', &id, &strLocalFile);
				if (!bAltDest) strLocalFile += ".png";

				if (bScript) {
					std::cout << "inserting=" << id
						<< ";read=" << strLocalFile
						<< ";status=";
				} else {
					std::cout << "  inserting: " << id;
					if (bAltDest) std::cout << " (from " << strLocalFile << ")";
					std::cout << std::flush;
				}

				gg::TilesetPtr nextTileset = pTileset;
				gg::Tileset::EntryPtr epBefore = idToEntryPtr(id, nextTileset);
				if (!epBefore) {
					if (bScript) std::cout << "fail";
					else std::cout << " [failed; invalid ID]";
					iRet = RET_NONCRITICAL_FAILURE;
				}
				try {
					gg::Tileset::EntryPtr ep = nextTileset->insert(epBefore, 0);
					modified = true;

					gg::ImagePtr img = nextTileset->openImage(ep);
					if (!img) {
						if (bScript) std::cout << "fail";
						else std::cout << " [failed; unable to open newly inserted image]";
						iRet = RET_NONCRITICAL_FAILURE;
					} else {
						pngToImage(img, strLocalFile);
						if (bScript) std::cout << "ok";
					}
					nextTileset->flush();
				} catch (std::exception& e) {
					if (bScript) std::cout << "fail";
					else std::cout << " [failed; " << e.what() << ']' << std::endl;
				}
				std::cout << std::endl;

			} else if (i->string_key.compare("insert-tileset") == 0) {
				std::string id = i->value[0];

				if (bScript) {
					std::cout << "addtileset=" << id
						<< ";status=";
				} else {
					std::cout << "add tileset: " << id << std::flush;
				}

				gg::TilesetPtr nextTileset = pTileset;
				gg::Tileset::EntryPtr epBefore = idToEntryPtr(id, nextTileset);
				if (!epBefore) {
					if (bScript) std::cout << "fail";
					else std::cout << " [failed; invalid ID]";
					iRet = RET_NONCRITICAL_FAILURE;
				}
				try {
					gg::Tileset::EntryPtr ep = nextTileset->insert(epBefore,
						gg::Tileset::SubTileset);
					modified = true;
					nextTileset->flush();
					if (bScript) std::cout << "ok";
				} catch (std::exception& e) {
					if (bScript) std::cout << "fail";
					else std::cout << " [failed; " << e.what() << ']' << std::endl;
				}
				std::cout << std::endl;

			} else if (i->string_key.compare("set-size") == 0) {
				std::string strArchFile, strNewSize;
				bool bHasSize = split(i->value[0], '=', &strArchFile, &strNewSize);
				if (bHasSize) {
					std::string strWidth, strHeight;
					bool bSizeValue = split(strNewSize, 'x', &strWidth, &strHeight);
					std::cout << "   set size: " << strArchFile;
					if (bSizeValue) {
						int newWidth = strtod(strWidth.c_str(), NULL);
						int newHeight = strtod(strHeight.c_str(), NULL);
						std::cout << " -> " << newWidth << 'x' << newHeight;

						gg::TilesetPtr nextTileset = pTileset;
						gg::Tileset::EntryPtr ep = idToEntryPtr(strArchFile, nextTileset);
						if (!ep) {
							if (nextTileset) {
								// ID was for a tileset
								if (nextTileset->getCaps() & gg::Tileset::ChangeDimensions) {
									try {
										nextTileset->setTilesetDimensions(newWidth, newHeight);
										modified = true;
										std::cout << std::endl;
									} catch (std::exception& e) {
										std::cout << " [failed; " << e.what()
											<< ']' << std::endl;
									}
								} else {
									std::cout << " [failed; this file format has a fixed tile size]"
										<< std::endl;
									// Abort in this case, because any subsequent
									// commands are likely to break if the tile size
									// wasn't changed as expected.
									return RET_BADARGS;
								}
							} else {
								std::cout << " [failed; invalid ID " << strArchFile << ']'
									<< std::endl;
								iRet = RET_NONCRITICAL_FAILURE;
							}

						} else {
							// ID was for an image
							gg::ImagePtr img = nextTileset->openImage(ep);
							if (img->getCaps() & gg::Image::CanSetDimensions) {
								img->setDimensions(newWidth, newHeight);
								modified = true;
								std::cout << std::endl;
							} else {
								std::cout << " [failed; this image's size is fixed]"
									<< std::endl;
								iRet = RET_NONCRITICAL_FAILURE;
							}
						}
						if (nextTileset) nextTileset->flush();
					} else {
						std::cout << " [failed; invalid size]" << std::endl;
						iRet = RET_NONCRITICAL_FAILURE;
					}
				} else {
					std::cerr << "-z/--set-size requires an ID and a size." << std::endl;
					return RET_BADARGS;
				}


			// Ignore --type/-t
			} else if (i->string_key.compare("type") == 0) {
			} else if (i->string_key.compare("t") == 0) {
			// Ignore --script/-s
			} else if (i->string_key.compare("script") == 0) {
			} else if (i->string_key.compare("s") == 0) {
			// Ignore --force/-f
			} else if (i->string_key.compare("force") == 0) {
			} else if (i->string_key.compare("f") == 0) {

			} // else it's the tileset filename, but we already have that

		} // for (all command line elements)

		if (modified) {
			// Save changes
			pTileset->flush();
		}

	} catch (const stream::error& e) {
		std::cerr << PROGNAME ": I/O error - " << e.what() << std::endl;
		return RET_SHOWSTOPPER;

	} catch (po::unknown_option& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return RET_BADARGS;

	} catch (po::invalid_command_line_syntax& e) {
		std::cerr << PROGNAME ": " << e.what()
			<< ".  Use --help for help." << std::endl;
		return RET_BADARGS;
	}

	return iRet;
}
