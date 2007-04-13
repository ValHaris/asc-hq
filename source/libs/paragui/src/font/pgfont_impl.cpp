#include "pglog.h"
#include "facecache.h"
#include "pgfont.h"

struct PG_FontDataInternal {
	PG_Color color;
   PG_Color highlightColor;
   int alpha;
	PG_Font::Style style;

	int size;
	int index;
	std::string name;

	Uint32 dummy1;
	Uint32 dummy2;

	PG_FontFaceCacheItem* FaceCache;

};

PG_Font::PG_Font(const std::string& fontfile, int size, int index) {
	my_internaldata = new PG_FontDataInternal;
	my_internaldata->FaceCache = NULL;

	my_internaldata->name = fontfile;
	my_internaldata->size = size;
	my_internaldata->index = index;
	my_internaldata->color.r = 255;
	my_internaldata->color.g = 255;
	my_internaldata->color.b = 255;
   my_internaldata->highlightColor.r = 180;
   my_internaldata->highlightColor.g = 180;
   my_internaldata->highlightColor.b = 180;
   my_internaldata->alpha = 255;
	my_internaldata->style = NORMAL;

	my_internaldata->FaceCache = PG_FontEngine::LoadFontFace(fontfile, size, index);

	if(my_internaldata->FaceCache == NULL) {
		PG_LogERR("Unable to create font (name=\"%s\", size=\"%i\", index=\"%i\"", fontfile.c_str(), size, index);
	}
}

void PG_Font :: copy( const PG_Font& font )
{
   // my_internaldata = font.my_internaldata;
   // my_internaldata->referenceCount++;
   my_internaldata = new PG_FontDataInternal;
   *my_internaldata = *font.my_internaldata;
}

void PG_Font :: unlink()
{
   delete my_internaldata;
   my_internaldata = NULL;
}


PG_Font :: PG_Font( const PG_Font& font )
{
   copy ( font );
}
   
   

PG_Font& PG_Font::operator= ( const PG_Font& font )
{
   unlink();
   copy ( font );
   return *this;
}


PG_Font::~PG_Font()
{
   unlink();
}

int PG_Font::GetFontAscender() {
	return my_internaldata->FaceCache ?
	       FT_CEIL(FT_MulFix(my_internaldata->FaceCache->Face->ascender, my_internaldata->FaceCache->Face->size->metrics.y_scale)) : 0;
}

int PG_Font::GetFontDescender() {
	return my_internaldata->FaceCache ?
	       FT_CEIL(FT_MulFix(my_internaldata->FaceCache->Face->descender, my_internaldata->FaceCache->Face->size->metrics.y_scale)) : 0;
}

int PG_Font::GetFontHeight() {
	return my_internaldata->FaceCache ?
	       FT_CEIL(FT_MulFix(my_internaldata->FaceCache->Face->height, my_internaldata->FaceCache->Face->size->metrics.y_scale)) : 0;
}

void PG_Font::SetColor(const PG_Color& c) {
	my_internaldata->color = c;
}

void PG_Font::SetHighlightColor(const PG_Color& c) {
   my_internaldata->highlightColor = c;
}

PG_Color PG_Font::GetColor() {
   return my_internaldata->color;
}

PG_Color PG_Font::GetHighlightColor() {
	return my_internaldata->highlightColor;
}

void PG_Font::SetAlpha(int a) {
	my_internaldata->alpha = a;
}

int PG_Font::GetAlpha() {
	return my_internaldata->alpha;
}

void PG_Font::SetStyle(Style s) {
	my_internaldata->style = s;
}

PG_Font::Style PG_Font::GetStyle() {
	return my_internaldata->style;
}

bool PG_Font::SetName(const std::string& fontfile) {
	my_internaldata->name = fontfile;
	my_internaldata->FaceCache = PG_FontEngine::LoadFontFace(
	                                 fontfile,
	                                 GetSize(),
	                                 GetIndex());

	if(my_internaldata->FaceCache == NULL) {
		PG_LogERR("Unable to create font (name=\"%s\", size=\"%i\", index=\"%i\"",
		          GetName().c_str(),
		          GetSize(),
		          GetIndex());
	}

	return (my_internaldata->FaceCache != NULL);
}

const std::string& PG_Font::GetName() {
	return my_internaldata->name;
}

void PG_Font::SetSize(int s) {
	my_internaldata->size = s;
	my_internaldata->FaceCache = PG_FontEngine::LoadFontFace(
	                                 GetName(),
	                                 GetSize(),
	                                 GetIndex());


	if(my_internaldata->FaceCache == NULL) {
		PG_LogERR("Unable to create font (name=\"%s\", size=\"%i\", index=\"%i\"",
		          GetName().c_str(),
		          GetSize(),
		          GetIndex());
	}

}

int PG_Font::GetSize() {
	return my_internaldata->size;
}

PG_FontFaceCacheItem* PG_Font::GetFaceCache(int index) {
	return my_internaldata->FaceCache;
}

void PG_Font::SetIndex(int index) {
	my_internaldata->index = index;
}

int PG_Font::GetIndex() {
	return my_internaldata->index;
}

/*void PG_Font::SetFaceCache(PG_FontFaceCacheItem* cache, int index) {
	my_internaldata->FaceCache = cache;
}*/
