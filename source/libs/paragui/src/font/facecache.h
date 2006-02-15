#include "paragui.h"
#include "pgdatacontainer.h"

#define FT_FLOOR(X)	((X & -64) / 64)
#define FT_CEIL(X)	(((X + 63) & -64) / 64)
#define PG_FITALIC_ANGLE	0.26f

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include FT_CACHE_H
#include FT_CACHE_IMAGE_H
#include FT_CACHE_SMALL_BITMAPS_H

#ifdef HASH_MAP_INC
#include HASH_MAP_INC
#endif
#include <map>

class PG_GlyphCacheItem : public PG_DataContainer {
public:
	PG_GlyphCacheItem(Uint32 size) : PG_DataContainer(size) {
	};

	int Glyph_Index;
	FT_Bitmap Bitmap;

	int Bitmap_left;
	int Bitmap_top;
	int Advance_x;
};

typedef std::map<int, PG_GlyphCacheItem*> PG_GlyphCache;

class PG_FontFaceCacheItem {
public:
	
	PG_FontFaceCacheItem() {
		Face = NULL;
	};

	virtual ~PG_FontFaceCacheItem() {
		for(PG_GlyphCache::iterator i = GlyphCache.begin(); i != GlyphCache.end(); i++) {
			delete (*i).second;
		}
		FT_Done_Face(Face);
	};

	FT_Face Face;
	PG_GlyphCache GlyphCache;

	//Additional font paramters - usable for style
	int Bold_Offset;
	int Underline_Height;

	int Ascent;
	int Descent;
	int Height;
	int LineSkip;

	int Use_Kerning;
	FT_F26Dot6 fontsize;
};
