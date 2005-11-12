

#ifndef ascstringH
#define ascstringH

#include "ASCStringHelpers.h"
#include <stdexcept>      // for range_error
#include <stdarg.h>

using std::range_error;


class ASCString : public ASCCharTString
{

public:

    /*!
        charT is an alias to ASCStringHelpers::charT, the character type used to represent strings internally.
    */
    typedef ASCStringHelpers::charT     charT;

#ifndef _UNICODE_BROKEN_

    /*!
        NoncharT is an alias to ASCStringHelpers::NoncharT, the complementary character type used to convert strings
        not using charT types.
    */
    typedef ASCStringHelpers::NoncharT        NoncharT;

#endif // _UNICODE_BROKEN_

    // Constructors
    ASCString ( );
    ASCString ( const charT* pS );
    ASCString ( const charT* pS, size_type n );
    ASCString ( size_type n, charT c );
    ASCString ( const_iterator first, const_iterator last );

    // Constructors for ASCCharTString type
    ASCString ( const ASCCharTString& s );
    ASCString ( const ASCCharTString& s, size_type pos, size_type n );

#ifndef _UNICODE_BROKEN_

    // Constructors for ASCAdaptatorString type
    explicit ASCString ( const ASCAdaptatorString& s );
    explicit ASCString ( const ASCAdaptatorString& s, size_type pos, size_type n );

    // Copy Operators for ASCAdaptatorString type
    ASCString& operator=( const ASCAdaptatorString& s );

#endif // _UNICODE_BROKEN_

    // Copy Operators for ASCCharTString type
    ASCString& operator= ( const ASCCharTString& s );

    // Copy Operators for charT*  type
    ASCString& operator= ( const charT* pS );

    // Case-insensitive comparisons
    int compare_ci ( const ASCCharTString& s ) const;
    int compare_ci ( size_type p0, size_type n0, const ASCCharTString& s );
    int compare_ci ( size_type p0, size_type n0, const ASCCharTString& s, size_type pos, size_type n );
    int compare_ci ( const charT* pS ) const;
    int compare_ci ( size_type p0, size_type n0, const charT* pS, size_type pos ) const;

    //! converts the parameter to a String
    static ASCString toString( int i );
    
   #ifdef SIZE_T_not_identical_to_INT
    static ASCString toString( size_t i );
   #endif 
    
    //! converts the parameter to a String with base radix
    // static ASCString toString(int i, int radix );
    //! converts the parameter to a String
    static ASCString toString( double d );

    // Case-manipulation helpers
    ASCString&  toLower    ();
    ASCString&  toUpper    ();

    // Printing and formating helpers.
    ASCString&  format     ( const charT* pFormat, ... );
    ASCString&  vaformat     ( const charT* pFormat, va_list ap );
    void        printf     ();
    
    // convenience functions
    bool endswith( const ASCString& s ) const;
};

// CONSTRUCTORS
/*!
    Construct an ASCString object.
*/
inline ASCString::ASCString ( )
: ASCCharTString ( )
{
}

/*!
    \overload ASCString::ASCString ( const charT* pS )

    \param pS a pointer to a NULL-terminated array of \a charT element types.
*/
inline ASCString::ASCString ( const charT* pS )
: ASCCharTString ( pS )
{
}

/*!
    \overload ASCString::ASCString ( const charT* pS, size_type n )

    \param pS a pointer to a NULL-terminated array of \a charT element types.

    \param n the number of elements from \a pS to use to initialise the ASCString object.
*/
inline ASCString::ASCString ( const charT* pS, size_type n )
: ASCCharTString ( pS, n )
{
}

/*!
    \overload ASCString::ASCString ( size_type n, charT c )

    \param n the repeat count of \a c.

    \param c a single \a charT element type to be repeated \a n times.
*/
inline ASCString::ASCString ( size_type n, charT c )
: ASCCharTString ( n, c )
{
}

/*!
    \overload ASCString::ASCString ( const_iterator first, const_iterator last )

    \param first a const iterator delimiting the begining of a range of \a charT element types to be used
    to construct this ASCString.

    \param last a const iterator delimiting the ending of a range of \a charT element types to be used
    to construct this ASCString.
*/
inline ASCString::ASCString ( const_iterator first, const_iterator last )
: ASCCharTString ( first, last )
{
}

// Constructors for ASCCharTString type
/*!
    \overload ASCString::ASCString ( const ASCCharTString& s )

    \param s a const reference to an ASCCharTString object to be used to construct this ASCString.
*/
inline ASCString::ASCString ( const ASCCharTString& s )
: ASCCharTString ( s )
{
}

/*!
    \overload ASCString::ASCString ( const ASCCharTString& s, size_type pos, size_type n )

    \param s a const reference to an ASCCharTString object to be used to construct this ASCString.

    \param pos the position of the first character in \a s to be used to construct this ASCString.

    \param n the count of characters used to construct this ASCString begining at position \a pos.
*/
inline ASCString::ASCString ( const ASCCharTString& s, size_type pos, size_type n )
: ASCCharTString ( s, pos, n )
{
}


#ifndef _UNICODE_BROKEN_

// Constructors for ASCAdaptatorString type

/*!
    \overload ASCString::ASCString ( const ASCAdaptatorString& s )

    \param s a const reference to an ASCAdaptatorString object to be used to construct this ASCString.

    \exception range_error
    The range_error exception is raised when not all the characters of ASCAdaptatorString can be converted
    to ASCCharTString's characters.
*/
inline ASCString::ASCString ( const ASCAdaptatorString& s )
: ASCCharTString ()
{
    // auto_ptr will release memory if an exception is raised
    auto_ptr< charT > pE ( new charT [ s.length () + sizeof ( charT ) ] );

    size_t stNumCharConverted = ASCStringHelpers::_ConvertToCharT ( pE.get (), s.c_str(), s.length() );
    if ( stNumCharConverted != s.length() )
    {
        throw range_error ( "ASCString::ASCString( const ASCAdaptatorString& s ) ==> unable to convert all characters from ASCAdaptatorString to ASCCharTString type." );
    }

    assign ( pE.get(), stNumCharConverted );
}

/*!
    \overload ASCString::ASCString ( const ASCAdaptatorString& s, size_type pos, size_type n )

    \param s a const reference to an ASCAdaptatorString object to be used to construct this ASCString.

    \param pos the position of the first character in \a s to be used to construct this ASCString.

    \param n the count of characters used to construct this ASCString begining at position \a pos.

    \exception range_error
    The range_error exception is raised when not all the characters of ASCAdaptatorString can be converted
    to ASCCharTString's characters.
*/
inline ASCString::ASCString ( const ASCAdaptatorString& s, size_type pos, size_type n )
: ASCCharTString ()
{
    size_t stLen = n - pos;

    // auto_ptr will release memory if an exception is raised
    auto_ptr< charT > pE ( new charT [ stLen + sizeof ( charT ) ] );

    const NoncharT* pCE = s.c_str ();
    size_t stNumCharConverted = ASCStringHelpers::_ConvertToCharT ( pE.get (), &pCE[ pos ], stLen );
    if ( stNumCharConverted != stLen )
    {
        throw range_error ( "ASCString::ASCString( const ASCAdaptatorString& s, size_type pos, size_type n ) ==> unable to convert all characters from ASCAdaptatorString to ASCCharTString type." );
    }

    assign ( pE.get(), stLen );
}

// Copy Operators for ASCAdaptatorString type

/*!
    \overload ASCString& ASCString::operator=( const ASCAdaptatorString& s )

    \param s a const reference to an ASCAdaptatorString object to be assigned to this ASCString.

    \return returns a reference to this ASCString.

    \exception range_error
    The range_error exception is raised when not all the characters of ASCAdaptatorString can be converted
    to ASCCharTString's characters.
*/
inline ASCString& ASCString::operator= ( const ASCAdaptatorString& s )
{
    ASCString str ( s );
    assign ( str );
    return *this;
}

#endif // _UNICODE_BROKEN_


// Copy Operators

/*!
    Assign a new value to an ASCString object.

    \param s a const reference to an ASCCharTString object to be assigned to this ASCString.

    \return returns a reference to this ASCString.
*/
inline ASCString& ASCString::operator= ( const ASCCharTString& s )
{
    assign ( s );
    return *this;
}


inline ASCString& ASCString::operator= ( const charT* pS )
{
    ASCString str ( pS );
    assign ( str );
    return *this;
}

// COMPARE_CI

/*!
    Compare two strings, case insensitive.

    \param s a const reference to an ASCCharTString object to be used to construct this ASCString.
*/
inline int ASCString::compare_ci ( const ASCCharTString& s ) const
{
    return ASCStringHelpers::_Stricmp ( c_str (), s.c_str () );
}

/*!
    \overload int ASCString::compare_ci ( size_type p0, size_type n0, const ASCCharTString& s )

    Compare the range [ p0 , n0 ] of this ASCString with the whole ASCCharTString.
*/
inline int ASCString::compare_ci ( size_type p0, size_type n0, const ASCCharTString& s )
{
    ASCString l_TempString ( *this, p0, n0 );
    return ASCStringHelpers::_Stricmp ( l_TempString.c_str () , s.c_str () );
}

/*!
    \overload int ASCString::compare_ci ( size_type p0, size_type n0,const ASCCharTString& s, size_type pos, size_type n )

    Compare the range [ p0 , n0 ] of this ASCString with the range [ pos, n ] of ASCCharTString.
*/
inline int ASCString::compare_ci ( size_type p0, size_type n0,const ASCCharTString& s, size_type pos, size_type n )
{
    ASCString l_TempStr1 ( *this, p0, n0 );
    ASCString l_TempStr2 ( s, pos, n );
    return ASCStringHelpers::_Stricmp ( l_TempStr1.c_str (), l_TempStr2.c_str () );
}

/*!
    \overload int ASCString::compare_ci ( const charT* pS ) const
*/
inline int ASCString::compare_ci ( const charT* pS ) const
{
    return ASCStringHelpers::_Stricmp ( c_str (), pS );
}

/*!
    \overload int ASCString::compare_ci ( size_type p0, size_type n0, const charT* pS, size_type pos ) const

    Compare the range [ p0 , n0 ] of this ASCString with up to [ 0, pos ] elements of \a pS.
*/
inline int ASCString::compare_ci ( size_type p0, size_type n0, const charT* pS, size_type pos ) const
{
    ASCString l_TempStr1 ( *this, p0, n0 );
    ASCString l_TempStr2 ( pS, pos  );
    return ASCStringHelpers::_Stricmp ( l_TempStr1.c_str (), l_TempStr2.c_str () );
}

// GLOBALS
ASCString copytoLower ( const ASCString& s );
ASCString copytoUpper ( const ASCString& s );

// extern const ASCString operator+ ( const ASCString& s1, const ASCString& s2 );
extern const ASCString operator+ ( const char* s1, const ASCString& s2 );

#endif // _ASC_STRING_H_INCLUDED_
