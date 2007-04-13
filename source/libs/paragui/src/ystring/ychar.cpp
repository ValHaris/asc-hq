/************************************************************************
 * $Id: ychar.cpp,v 1.2 2007-04-13 16:16:04 mbickel Exp $
 *
 * ------------
 * Description:
 * ------------
 * ychar.cpp
 *
 * A lightweight unicode character class
 *
 * (C) Copyright 2004 Arabeyes, Mohammed Yousif
 *
 * -----------------
 * Revision Details:    (Updated by Revision Control System)
 * -----------------
 *  $Date: 2007-04-13 16:16:04 $
 *  $Author: mbickel $
 *  $Revision: 1.2 $
 *  $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/ystring/ychar.cpp,v $
 *
 *  (www.arabeyes.org - under GPL License)
 *
 ************************************************************************/

#include <stdexcept>
#include <string>

#include "paragui.h"

#ifdef ENABLE_UNICODE

#include "ychar.h"

YChar::YChar() {
	ucs4 = 0x0000;
}

YChar::YChar(const int c) : ucs4(c) {}

YChar::YChar(const uint32 c) : ucs4(c) {}

YChar::YChar(const char c) : ucs4(c) {}

YChar::YChar(const char * c) throw(std::domain_error) {
	*this = fromUtf8(c);
}

YChar::YChar(const std::string c) throw(std::domain_error) {
	*this = fromUtf8(c);
}

int YChar::getNumberOfContinuingOctents(byte firstOctet) throw(std::domain_error) {
	if (firstOctet < 0x80) {
		/*
		 Characters in the range:
		   00000000 to 01111111 (ASCII Range)
		 are stored in one octet:
		   0xxxxxxx (The same as its ASCII representation)
		*/
		return 0;
	} else if ((firstOctet & 0xE0) // get the most 3 significant bits by AND'ing with 11100000
	           == 0xC0 ) {  // see if those 3 bits are 110. If so, the char is in this range
		/*
		 Characters in the range:
		   00000000 10000000 to 00000111 11111111
		 are stored in two octets:
		   110xxxxx 10xxxxxx
		*/
		return 1;
	} else if ((firstOctet & 0xF0) // get the most 4 significant bits by AND'ing with 11110000
	           == 0xE0) {  // see if those 4 bits are 1110. If so, the char is in this range
		/*
		 Characters in the range:
		   00001000 00000000 to 11111111 11111111
		 are stored in three octets:
		   1110xxxx 10xxxxxx 10xxxxxx
		*/
		return 2;
	} else if ((firstOctet & 0xF8) // get the most 5 significant bits by AND'ing with 11111000
	           == 0xF0) {  // see if those 5 bits are 11110. If so, the char is in this range
		/*
		 Characters in the range:
		   00000001 00000000 00000000 to 00011111 11111111 11111111
		are stored in four octets:
		   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		*/
		return 3;
	} else if ((firstOctet & 0xFC) // get the most 6 significant bits by AND'ing with 11111100
	           == 0xF8) { // see if those 6 bits are 111110. If so, the char is in this range
		/*
		 Characters in the range:
		   00000000 00100000 00000000 00000000 to
		   00000011 11111111 11111111 11111111
		 are stored in five octets:
		   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*/
		return 4;
	} else if ((firstOctet & 0xFE) // get the most 7 significant bits by AND'ing with 11111110
	           == 0xFC) { // see if those 7 bits are 1111110. If so, the char is in this range
		/*
		  Characters in the range:
		    00000100 00000000 00000000 00000000 to
		    01111111 11111111 11111111 11111111
		  are stored in six octets:
		    1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*/
		return 5;
	} else {
		PG_THROW(std::domain_error("The given octet is not a valid UTF-8 encoded character first octet"));
		return -1;
	}
}

YChar YChar::fromUtf8(const char * raw) throw(std::domain_error) {
	return fromUtf8(std::string(raw));
}

YChar YChar::fromUtf8(const std::string raw) throw(std::domain_error) {
	YChar c;
	int noOctets = 0;
	int firstOctetMask = 0;
	byte firstOctet = raw[0];
	if (firstOctet < 0x80) {
		/*
		 Characters in the range:
		   00000000 to 01111111 (ASCII Range)
		 are stored in one octet:
		   0xxxxxxx (The same as its ASCII representation)
		 The least 6 significant bits of the first octet is the most 6 significant nonzero bits
		 of the UCS4 representation.
		*/
		noOctets = 1;
		firstOctetMask = 0x7F;  // 0(1111111) - The most significant bit is ignored
	} else if ((firstOctet & 0xE0) // get the most 3 significant bits by AND'ing with 11100000
	           == 0xC0 ) {  // see if those 3 bits are 110. If so, the char is in this range
		/*
		 Characters in the range:
		   00000000 10000000 to 00000111 11111111
		 are stored in two octets:
		   110xxxxx 10xxxxxx
		 The least 5 significant bits of the first octet is the most 5 significant nonzero bits
		 of the UCS4 representation.
		*/
		noOctets = 2;
		firstOctetMask = 0x1F;  // 000(11111) - The most 3 significant bits are ignored
	} else if ((firstOctet & 0xF0) // get the most 4 significant bits by AND'ing with 11110000
	           == 0xE0) {  // see if those 4 bits are 1110. If so, the char is in this range
		/*
		 Characters in the range:
		   00001000 00000000 to 11111111 11111111
		 are stored in three octets:
		   1110xxxx 10xxxxxx 10xxxxxx
		 The least 4 significant bits of the first octet is the most 4 significant nonzero bits
		 of the UCS4 representation.
		*/
		noOctets = 3;
		firstOctetMask = 0x0F; // 0000(1111) - The most 4 significant bits are ignored
	} else if ((firstOctet & 0xF8) // get the most 5 significant bits by AND'ing with 11111000
	           == 0xF0) {  // see if those 5 bits are 11110. If so, the char is in this range
		/*
		 Characters in the range:
		   00000001 00000000 00000000 to 00011111 11111111 11111111
		are stored in four octets:
		   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		 The least 3 significant bits of the first octet is the most 3 significant nonzero bits
		 of the UCS4 representation.
		*/
		noOctets = 4;
		firstOctetMask = 0x07; // 11110(111) - The most 5 significant bits are ignored
	} else if ((firstOctet & 0xFC) // get the most 6 significant bits by AND'ing with 11111100
	           == 0xF8) { // see if those 6 bits are 111110. If so, the char is in this range
		/*
		 Characters in the range:
		   00000000 00100000 00000000 00000000 to
		   00000011 11111111 11111111 11111111
		 are stored in five octets:
		   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		 The least 2 significant bits of the first octet is the most 2 significant nonzero bits
		 of the UCS4 representation.
		*/
		noOctets = 5;
		firstOctetMask = 0x03; // 111110(11) - The most 6 significant bits are ignored
	} else if ((firstOctet & 0xFE) // get the most 7 significant bits by AND'ing with 11111110
	           == 0xFC) { // see if those 7 bits are 1111110. If so, the char is in this range
		/*
		  Characters in the range:
		    00000100 00000000 00000000 00000000 to
		    01111111 11111111 11111111 11111111
		  are stored in six octets:
		    1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		  The least significant bit of the first octet is the most significant nonzero bit
		  of the UCS4 representation.
		*/
		noOctets = 6;
		firstOctetMask = 0x01; // 1111110(1) - The most 7 significant bits are ignored
	} else {
		PG_THROW(std::domain_error("The given chunk is not a valid UTF-8 encoded Unicode character"));
		return '\0';
	}

	/*
	   The least noOctets significant bits of the first octet is the most 2 significant nonzero bits
	   of the UCS4 representation.
	   The first 6 bits of the UCS4 representation is the least 8-noOctets-1 significant bits of
	   firstOctet if the character is not ASCII. If so, it's the least 7 significant bits of firstOctet.
	   This done by AND'ing firstOctet with its mask to trim the bits used for identifying the
	   number of continuing octets (if any) and leave only the free bits (the x's)
	   Sample:
	     1-octet:    0xxxxxxx  &  01111111 = 0xxxxxxx
	     2-octets:  110xxxxx  &  00011111 = 000xxxxx
	*/
	c.ucs4 = firstOctet & firstOctetMask;

	// Now, start filling c.ucs4 with the bits from the continuing octets from raw.
	for (int i = 1; i < noOctets; i++) {
		// A valid continuing octet is of the form 10xxxxxx
		if ((raw[i] & 0xC0) // get the most 2 significant bits by AND'ing with 11000000
		        != 0x80) { // see if those 2 bits are 10. If not, the is a malformed sequence.
			PG_THROW(std::domain_error("The given chunk is a partial sequence at the end of a string that could begin a valid character"));
			return '\0';
		}

		// Make room for the next 6-bits
		c.ucs4 <<= 6;

		/*
		   Take only the least 6 significance bits of the current octet (raw[i]) and fill the created room
		   of c.ucs4 with them.
		   This done by AND'ing raw[i] with 00111111 and the OR'ing the result with c.ucs4.
		   
		*/
		c.ucs4 |= raw[i] & 0x3F;
	}

	return c;
}

bool YChar::isNull() const {
	return ucs4 == 0x0000;
}

bool YChar::isAsciiLetter() const {
	return ( (ucs4 >= 0x0041 && ucs4 <= 0x005A) ||
	         (ucs4 >= 0x0061 && ucs4 <= 0x007A)
	       );
}

int YChar::bytes() const {
	std::string b = utf8();
	return b.length();
}

std::string YChar::utf8() const {
	byte firstOctetMask = 0;
	int noOctets = 0;
	uint32 c = ucs4;
	if (c < 0x0080) {
		/*
		 Characters in the range:
		   00000000 to 01111111 (ASCII Range)
		 Will be stored in one octet:
		   0xxxxxxx (The same as its ASCII representation)
		*/
		firstOctetMask = 0x00; // 0(0000000)
		noOctets = 1;
	} else if (c < 0x0800) {
		/*
		 Characters in the range:
		   00000000 10000000 to 00000111 11111111
		 Will be stored in two octets:
		   110xxxxx 10xxxxxx
		*/
		firstOctetMask = 0xC0; // 110(00000)
		noOctets = 2;
	} else if (c < 0x10000) {
		/*
		 Characters in the range:
		   00001000 00000000 to 11111111 11111111
		 Will be stored in three octets:
		   1110xxxx 10xxxxxx 10xxxxxx
		*/
		firstOctetMask = 0xE0;  // 1110(0000)
		noOctets = 3;
	} else if (c < 0x200000) {
		/*
		 Characters in the range:
		   00000001 00000000 00000000 to 00011111 11111111 11111111
		 Will be stored in four octets:
		   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		*/
		firstOctetMask = 0xF0;
		noOctets = 4;  // 11110(000)
	} else if (c < 0x4000000) {
		/*
		 Characters in the range:
		   00000000 00100000 00000000 00000000 to
		   00000011 11111111 11111111 11111111
		 Will be stored in five octets:
		   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*/
		firstOctetMask = 0xF8;  // 111110(00)
		noOctets = 5;
	} else { // c < 0x80000000
		/*
		 Characters in the range:
		   00000100 00000000 00000000 00000000 to
		   01111111 11111111 11111111 11111111
		 Will be stored in six octets:
		   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
		*/
		firstOctetMask = 0xFC;  // 1111110(0)
		noOctets = 6;
	}

	/* The octets will be stored in b with the first octet in b[0], the second in b[1] ...etc
	    For a 6-octet character:
	      1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	         b[0]         b[1]        b[2]         b[3]       b[4]         b[5]    */
	std::string b(noOctets, ' ');
	for (int i = noOctets - 1; i > 0; i--) {
		b[i] = (c & 0x3F)  /*  Each octet except the first one (which will be stored at b[0]) has only
		       		       		                                        6 free bits (x's) while the two leading bits are 1 and 0 (10xxxxxx).
		       		       		                                        So, we Rip all the ones after the 6th bit starting from the least significant
		       		       		                                        bit by AND'ing it with 111111 (0x3F) to get the first 6 bits starting from
		       		       		                                        the least significant bit (1101 00010111 & 111111 = 010111) */

		       |  0x80;     /*  Now, we put 10 to the left of the resulting 6 bits to get the octet and
						                                 store it in b[i].
						                                 This is done by OR'ing the resulting 6-bits with 10000000 (0x80) 
						                                 to get the needed 8-bits octet (010111 | 10000000 = 10010111) */

		/* We stored the first 6 least significant bits, so we "throw" them away and put in their place
		    the next significant 6-bits to be stored the next iteration in b[i-1]  (1101 00010111 >> 6 = 1101) */
		c >>= 6;
	}

	/* The first octet is the mask with the x's (which are reperesented by zeros in the mask)
	   replaced by the remaining bits of the 4-bytes ucs4 character and if there are still some
	   free bits (x's), they are left at zero (because 0 | 0 = 0) */
	b[0] = c | firstOctetMask;
	return b;
}

/*
YChar::operator char()
{
  return static_cast<char>(ucs4);
}
*/

YChar::operator uint32() const {
	return ucs4;
}

bool operator==(const YChar & rhs, const YChar & lhs) {
	return rhs.ucs4 == lhs.ucs4;
}

bool operator==(const YChar & rhs, const char & lhs) {
	return rhs == YChar(lhs);
}
bool operator==(const char & rhs, const YChar & lhs) {
	return YChar(rhs) == lhs;
}

bool operator==(const YChar & rhs, const int & lhs) {
	return rhs == YChar(lhs);
}
bool operator==(const int & rhs, const YChar & lhs) {
	return YChar(rhs) == lhs;
}

std::ostream & operator<<( std::ostream & out, YChar c) {
	out << c.utf8();
	return out;
}

std::istream & operator>>( std::istream & in, YChar & c) {
	std::string b;
	in >> b;
	YChar new_char(b);
	c = new_char;
	return in;
}

#endif // ENABLE_UNICODE
