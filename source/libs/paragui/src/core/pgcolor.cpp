//
// C++ Implementation: pgcolor
//
// Description:
//
//
// Author: Alexander Pipelka <pipelka@pipelka.net>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "pgcolor.h"

PG_Color::PG_Color() {
	r = 0;
	g = 0;
	b = 0;
}

PG_Color::PG_Color(const SDL_Color& c) {
	*this = c;
}

PG_Color::PG_Color(Uint32 c) {
	*this = c;
}

PG_Color::PG_Color(Uint8 r, Uint8 g, Uint8 b) {
	*this = (Uint32)((r << 16) | (g << 8) | b);
}

PG_Color& PG_Color::operator=(const SDL_Color& c) {
	r = c.r;
	g = c.g;
	b = c.b;

	return *this;
}

PG_Color& PG_Color::operator=(Uint32 c) {
	r = (c >> 16) & 0xFF;
	g = (c >> 8) & 0xFF;
	b = c & 0xFF;

	return *this;
}

PG_Color::operator Uint32() const {
	return (r << 16) | (g << 8) | b;
}
