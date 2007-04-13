/************************************************************************
 * $Id: ystring.h,v 1.2 2007-04-13 16:15:57 mbickel Exp $
 *
 * ------------
 * Description:
 * ------------
 * ystring.h
 *
 * A unicode string class
 *
 * (C) Copyright 2004 Arabeyes, Mohammed Yousif
 *
 * -----------------
 * Revision Details:    (Updated by Revision Control System)
 * -----------------
 *  $Date: 2007-04-13 16:15:57 $
 *  $Author: mbickel $
 *  $Revision: 1.2 $
 *  $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/ystring.h,v $
 *
 *  (www.arabeyes.org - under GPL License)
 *
 ************************************************************************/

#ifndef YSTRING_H
#define YSTRING_H

#include <stdexcept>
#include <string>
#include <vector>

#include "ychar.h"

class DECLSPEC YString {
public:

	typedef YChar value_type;
	typedef std::vector<YChar>::pointer pointer;
	typedef YChar & reference;
	typedef const YChar & const_reference;
	typedef std::vector<YChar>::size_type size_type;
	typedef std::vector<YChar>::difference_type difference_type;
	static const size_type npos = std::string::npos;
	typedef std::vector<YChar>::iterator iterator;
	typedef std::vector<YChar>::const_iterator const_iterator;
	typedef std::vector<YChar>::reverse_iterator reverse_iterator;
	typedef std::vector<YChar>::const_reverse_iterator const_reverse_iterator;

	YString();
	YString(const std::string &, size_type = 0, size_type = npos);
	YString(const char *);
	YString(const char *, size_type) ;
	YString(size_type, char);
	template <class InputIterator>
	YString(InputIterator, InputIterator);

	YString(const YString &, size_type = 0, size_type = npos) throw(std::out_of_range);
	YString(const std::vector<YChar>);
	YString(const std::vector<YChar>, size_type);
	YString(size_type, YChar);
	YString(const YChar);

	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
	reverse_iterator rbegin();
	reverse_iterator rend();
	const_reverse_iterator rbegin() const;
	const_reverse_iterator rend() const;
	size_type size() const;
	size_type max_size() const;
	size_type capacity() const;
	bool empty() const;

	reference operator[](size_type n);
	const_reference operator[](size_type n) const;
	const char * c_str() const;
	const char * data() const;

	YString & operator=(const YString &);
	YString & operator=(const std::string &);
	YString & operator=(const char *);
	YString & operator=(char);
	YString & operator=(YChar);
	YString & operator=(uint32);

	void reserve(size_type);
	void swap(YString &);

	iterator insert(iterator, const YChar &);
	template <class InputIterator>
	void insert(iterator, InputIterator, InputIterator);
	void insert(iterator, size_type, const YChar &);
	YString & insert(size_type, const YString &);
	YString & insert(size_type, const YString &, size_type, size_type);
	YString & insert(size_type, const char *);
	YString & insert(size_type, const char *, size_type);
	YString & insert(size_type, size_type, YChar);
	YString & insert(size_type, size_type, char);

	iterator erase(iterator) ;
	iterator erase(iterator, iterator);
	YString & erase(size_type = 0, size_type = npos)  throw(std::out_of_range);

	size_type find(YChar, size_type = 0) throw(std::out_of_range);

	void push_back(const YChar &);
	void pop_back();

	void clear();



	size_type length() const;
	size_type bytes() const;
	YString reverse() const;

	YChar at(YString::size_type) const throw(std::out_of_range);
	void setAt(YString::size_type, YChar) throw(std::out_of_range);

	int compareTo(const YString &) const;
	YString substr(YString::size_type, YString::size_type = std::string::npos) const throw(std::out_of_range);

	const std::string utf8() const;


	operator std::string() const;
	/*
	operator const char *() const;
	*/



	YString & operator+=(const YString &);
	YString & operator+=(const std::string &);
	YString & operator+=(const char *);
	YString & operator+=(const YChar);
	YString & operator+=(const char);
	YString & operator+=(const uint32);

	friend YString operator+(const YString &, const YString &);
	friend YString operator+(const YString &, const char *);
	friend YString operator+(const char *, const YString &);
	friend YString operator+(const YString &, const YChar);
	friend YString operator+(const YChar, const YString &);
	friend YString operator+(const YString &, const char);
	friend YString operator+(const char, const YString &);
	friend YString operator+(const YString &, const uint32);
	friend YString operator+(const uint32, const YString &);

	static YString fromUtf8(const char *) throw(std::domain_error);
	static YString fromUtf8(const std::string&) throw(std::domain_error);

private:
	std::vector<YChar> unicode;
};

bool operator==(const YString &, const YString &);
bool operator==(const YString &, const char *);
bool operator==(const char *, const YString &);

bool operator!=(const YString &, const YString &);
bool operator!=(const YString &, const char *);
bool operator!=(const char *, const YString &);

bool operator<(const YString &, const YString &);
bool operator<(const YString &, const char *);
bool operator<(const char *, const YString &);

bool operator<=(const YString &, const YString &);
bool operator<=(const YString &, const char *);
bool operator<=(const char *, const YString &);

bool operator>(const YString &, const YString &);
bool operator>(const YString &, const char *);
bool operator>(const char *, const YString &);

bool operator>=(const YString &, const YString &);
bool operator>=(const YString &, const char *);
bool operator>=(const char *, const YString &);

YString operator+(const YString &, const YString &);
YString operator+(const YString &, const char *);
YString operator+(const char *, const YString &);
YString operator+(const YString &, const YChar);
YString operator+(const YChar, const YString &);
YString operator+(const YString &, const char);
YString operator+(const char, const YString &);
YString operator+(const YString &, const uint32);
YString operator+(const uint32, const YString &);
YString operator+(const YChar, const YChar);
YString operator+(const char *, const YChar);
YString operator+(const YChar, const char *);

std::ostream & operator<<( std::ostream &, YString &);
std::istream & operator>>( std::istream &, YString &);

#endif
