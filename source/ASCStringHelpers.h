#ifndef __ASC_STRING_HELPERS_H_INCLUDED__
#define __ASC_STRING_HELPERS_H_INCLUDED__

#include <cstdlib>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <string>
#include <wchar.h>
#include <stdio.h>
#include <ctype.h>

using std::auto_ptr;
using std::string;

#ifndef _UNICODE_BROKEN_

    using std::wstring;

#else

    #undef _UNICODE

#endif // _UNICODE_BROKEN_

/*! \class ASCCharTString ASCStringHelpers.h

    \brief \e ASCCharTString represents the STL parent's string class of ASCString.

    \li If the preprocessor definition _UNICODE is defined, \e ASCCharTString is an
    STL's \e std::wstring to use \e wchar_t type.
    \li If _UNICODE is not defined, \e ASCCharTString is an STL's \e std::string
    to use \e char type.

    \par
    That way, \e ASCCharTString will addapt itself to the right character type needed for its internal
    string representation depending on the _UNICODE preprocessor definition.


    \e ASCCharTString is defined through a typedef :

    \code

    #ifdef _UNICODE

        typedef wstring      ASCCharTString;

    #else

        typedef string       ASCCharTString;

    #endif

    \endcode

    \par
    See STL's documentation for more information on \e std::string and \e std::wstring.

    \sa ASCAdaptatorString

*/

/*! \class ASCAdaptatorString ASCStringHelpers.h

    \brief \e ASCAdaptatorString represents the complementary class to ASCCharTString.

    Complementary to ASCCharTString means that if ASCCharTString is using \e char types
    for its internal string representation, \e ASCAdaptatorString will use \e wchar_t types.
    On the other hand, if ASCCharTString is using \e wchar_t, \e ASCAdaptatorString will use \e char.

    \li If the preprocessor definition _UNICODE is defined, \e ASCAdaptatorString is an
    STL's \e std::string to use \e char type.
    \li If _UNICODE is not defined, \e ASCAdaptatorString is an STL's \e std::wstring to use \e wchar_t type.

    \e ASCAdaptatorString is defined through a typedef :

    \code

    #ifdef _UNICODE

        typedef string       ASCAdaptatorString;

    #else

        typedef wstring      ASCAdaptatorString;

    #endif

    \endcode

    \par
    See STL's documentation for more information on \e std::string and \e std::wstring.

    \sa ASCCharTString
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    #ifdef ASC_UNICODE

        typedef wstring     ASCCharTString;
        typedef string      ASCAdaptatorString;

        #ifndef _T
            #define  _T( text )   L ## text
        #else
            #error "_T macro already defined."
        #endif

    #else // ASC_UNICODE

        typedef string       ASCCharTString;

        #ifndef _UNICODE_BROKEN_

            typedef wstring      ASCAdaptatorString;

        #endif // _UNICODE_BROKEN_

        #ifndef _T
            #define  _T( text )   text
        #else
            // #error "_T macro already defined."
        #endif

    #endif // _UNICODE

#endif // DOXYGEN_SHOULD_SKIP_THIS

/*!
    \class ASCStringHelpers ASCStringHelpers.h

    \brief The \e ASCStringHelpers class provides helper's functions used by ASCString.

    \par
    \e ASCStringHelpers provides standard interfaces to utility functions across platforms and char types
    ( char or wchar_t ).

    \sa ASCCharTString, ASCAdaptatorString

*/
class ASCStringHelpers
{
public:

    /*!
        charT is an alias to ASCCharTString::value_type, the character type used to represent strings
        internally.

        \li If _UNICODE is defined, charT is a \e wchar_t.
        \li If _UNICODE is not defined, charT is a \e char.

        \sa ASCCharTString
    */
    typedef ASCCharTString::value_type  charT;

#ifndef _UNICODE_BROKEN_

    /*!
        NoncharT is an alias to ASCAdaptatorString::value_type, the character type used to represent
        complementary strings internally.

        \li If _UNICODE is defined, NoncharT is a \e char.
        \li If _UNICODE is not defined, NoncharT is a \e wchar_t.

        \sa ASCAdaptatorString
    */
    typedef ASCAdaptatorString::value_type  NoncharT;

#endif // _UNICODE_BROKEN_

    static size_t  _Strlen    ( const charT* pS );
    static charT*  _Strcpy    ( charT* pDest, const charT* pSrc );
    static charT*  _Strlwr    ( charT* pS );
    static charT*  _Strupr    ( charT* pS );
    static int     _Stricmp   ( const charT* pS1, const charT* pS2 );
    static int     _Vsnprintf ( charT* buffer, size_t count, const charT* format, std::va_list argptr );
    static int     _Printf    ( const charT* format, ... );

#ifndef _UNICODE_BROKEN_

    static size_t  _ConvertToCharT ( charT* pDest, const NoncharT* pSrc, size_t count );

#endif // _UNICODE_BROKEN_

};


/*!
    \fn size_t ASCStringHelpers::_Strlen ( const charT* pS )

    Get the length of a string.

    \param pS a pointer to a NULL-terminated string.

    \return returns the number of characters in string, excluding the terminal NULL.
    No return value is reserved to indicate an error.

    \remarks
    See standard system documentation for more information about \e strlen.
*/
inline size_t ASCStringHelpers::_Strlen ( const charT* pS )
{
    assert ( pS != NULL );

    #ifdef ASC_UNICODE
        return ::wcslen ( pS );
    #else
        return ::strlen ( pS );
    #endif
}

/*!
    \fn ASCStringHelpers::charT* ASCStringHelpers::_Strcpy ( charT* pDest, const charT* pSrc )

    Copy a string

    \param pDest a pointer to a NULL-terminated string which will receive a copy of \a pSrc.

    \param pSrc a pointer to a NULL-terminated string which will be copied.

    \return returns a pointer to the destination string. No return value is reserved to indicate an error.

    \remarks
    See standard system documentation for more information about \e strcpy.
*/
inline ASCStringHelpers::charT* ASCStringHelpers::_Strcpy ( charT* pDest, const charT* pSrc )
{
    assert ( pSrc  != NULL );
    assert ( pDest != NULL );

    #ifdef ASC_UNICODE
        return ::wcscpy ( pDest, pSrc );
    #else
        return ::strcpy ( pDest, pSrc );
    #endif
}

/*!
    \fn ASCStringHelpers::charT* ASCStringHelpers::_Strlwr ( charT* pS )

    Convert to lowercase.

    \param pS a pointer to a NULL-terminated string.

    \return returns a pointer to the destination string. No return value is reserved to indicate an error.

    \remarks
    See standard system documentation for more information about \e strlwr.
*/
inline ASCStringHelpers::charT* ASCStringHelpers::_Strlwr ( charT* pS )
{
    assert ( pS != NULL );

#ifndef _UNIX_

    #ifdef ASC_UNICODE
        return ::_wcslwr ( pS );
    #else
        return :: strlwr ( pS );
    #endif

#else // _UNIX_

    charT* pTemp = pS;

    while ( *pTemp != 0 )
    {
        #ifdef ASC_UNICODE
            *pTemp = towlower ( *pTemp );
        #else
            *pTemp = tolower  ( *pTemp );
        #endif

        ++pTemp;
    };
    return pS;

#endif // _UNIX_

}

/*!
    \fn ASCStringHelpers::charT* ASCStringHelpers::_Strupr ( charT* pS )

    Convert to uppercase.

    \param pS a pointer to a NULL-terminated string.

    \return returns a pointer to the destination string. No return value is reserved to indicate an error.

    \remarks
    See standard system documentation for more information about \e strupr.
*/
inline ASCStringHelpers::charT* ASCStringHelpers::_Strupr ( charT* pS )
{
    assert ( pS != NULL );

#ifndef _UNIX_

    #ifdef ASC_UNICODE
        return ::_wcsupr ( pS );
    #else
        return :: strupr ( pS );
    #endif

#else // _UNIX_

    charT* pTemp = pS;

    while ( *pTemp != 0 )
    {
        #ifdef ASC_UNICODE
            *pTemp = towupper ( *pTemp );
        #else
            *pTemp = toupper  ( *pTemp );
        #endif

        ++pTemp;
    };
    return pS;

#endif // _UNIX_

}

/*!
    \fn int ASCStringHelpers::_Stricmp ( const charT* pS1, const charT* pS2 )

    Perform a lowercase comparison of strings.

    \param pS1 a pointer to a NULL-terminated string.

    \param pS2 a pointer to a NULL-terminated string.

    \return returns a value indicating the relationship between the compared strings :

    < 0 pS1 less than pS2
      0 pS1 identical to pS2
    > 0 pS1 greater than pS2

    \remarks
    See standard system documentation for more information about \e stricmp.

*/
inline int ASCStringHelpers::_Stricmp ( const charT* pS1, const charT* pS2 )
{
    assert ( pS1 != NULL );
    assert ( pS2 != NULL );

#ifndef _UNIX_

    #ifdef ASC_UNICODE
        return ::wcsicmp ( pS1, pS2 );
    #else
        return ::stricmp ( pS1, pS2 );
    #endif

#else // _UNIX_

    #ifdef ASC_UNICODE
        // auto_ptr will release the memorey if an exception is raised
        auto_ptr< charT > l_autopS1 ( new charT [ _Strlen ( pS1 ) + sizeof ( charT ) ] );
        auto_ptr< charT > l_autopS2 ( new charT [ _Strlen ( pS2 ) + sizeof ( charT ) ] );

        charT* l_pS1 = l_autopS1.get();
        charT* l_pS2 = l_autopS2.get();

        _Strcpy ( l_pS1, pS1 );
        _Strcpy ( l_pS2, pS2 );

        _Strlwr ( l_pS1 );
        _Strlwr ( l_pS2 );

        return ::wcscmp ( l_pS1, l_pS2 );
    #else
        return ::strcasecmp ( pS1, pS2 );
    #endif

#endif // _UNIX_

}

/*!
    \fn int ASCStringHelpers::_Vsnprintf ( charT* buffer, size_t count, const charT* format, va_list argptr )

    Write formated output.

    \param buffer the storage location for output.

    \param count the maximum number of characters to write.

    \param format the format string.

    \param argptr pointer to a list of arguments.

    \return returns the number of characters written.

    \remarks
    See standard system documentation for more information about \e vsnprintf.
*/
inline int ASCStringHelpers::_Vsnprintf ( charT* buffer, size_t count, const charT* format, std::va_list argptr )
{
    assert ( buffer != NULL );
    assert ( format != NULL );

#ifdef _MSC_VER
    #define vsnwprintf  _vsnwprintf
    #define vsnprintf   _vsnprintf
#endif

    #ifdef ASC_UNICODE
        return ::vsnwprintf ( buffer, count, format, argptr );
    #else
        return ::vsnprintf ( buffer, count, format, argptr );
    #endif
}

/*!
    \fn int ASCStringHelpers::_Printf ( const charT* format, ... )

    Print formated string to standard output.

    \param format the format string.

    \param ... optional arguments.

    \return returns the number of characters printed.

    \remarks
    See standard system documentation for more information about \e printf.
*/
inline int ASCStringHelpers::_Printf ( const charT* format, ... )
{
    assert ( format != NULL );

    std::va_list argptr;
    va_start ( argptr, format );

    int nRes = 0;

    #ifdef ASC_UNICODE
        nRes = ::vwprintf ( format, argptr );
    #else
        nRes = ::vprintf ( format, argptr );
    #endif

    va_end ( argptr );
    return nRes;
}

#ifndef _UNICODE_BROKEN_

/*!
    \fn size_t ASCStringHelpers::_ConvertToCharT ( charT* pDest, const NoncharT* pSrc, size_t count )

    Convert a non-native string to a native one.

    \param pDest a pointer to a NULL-terminated string which will receive a converted \a pSrc.

    \param pSrc a pointer to a NULL-terminated string which will be converted.

    \param count the size of the destination buffer

    \return returns the number of characters written to \a pDest.

*/
inline size_t ASCStringHelpers::_ConvertToCharT ( charT* pDest, const NoncharT* pSrc, size_t count )
{
    assert ( pDest != NULL );
    assert ( pSrc  != NULL );

    #ifdef ASC_UNICODE
        return ::mbstowcs( pDest, pSrc, count );
    #else
        return std::wcstombs( pDest, pSrc, count );
    #endif
}

#endif // _UNICODE_BROKEN_

#endif // __ASC_STRING_HELPERS_H_INCLUDED__

