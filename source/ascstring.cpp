
#include "ASCString.h"

/*! \class ASCString ASCString.h

    \brief The ASCString class provides an abstract way to manipulate strings.

    Depending on the prepocessor definition _UNICODE, ASCString will use Unicode text or C-null 
    terminated char array.

    \warning
    Be extremely carefull if you have to modify this class. No virtual destructor is provided. 
    This may result in memory leaks if you modify this class to free dynamically allocated memory
    in its destructor.
    The same warning applies to classes deriving from ASCString ( if any ).

    \code

    ASCString* pStr = new ASCString( "My string" );

    ASCInheritedString* pInherited = ( ASCInheritedString* ) pStr;

    // pStr's destructor will not be called when deleting pInherited. 
    // If ASCString has been modified to free memory in its destructor, 
    // this memory will never be freed up.
    delete pInherited;      

    \endcode
*/

/*!
    Convert this ASCString to lowercase.

    \return returns a reference on this ASCString.
*/
ASCString& ASCString::toLower ( )
{
    auto_ptr< charT > l_autopBuf ( new charT [ length () + sizeof ( charT ) ] );

    charT* l_pBuf = l_autopBuf.get();

    ASCStringHelpers::_Strcpy ( l_pBuf, c_str () );
    ASCStringHelpers::_Strlwr ( l_pBuf );
    assign  ( l_pBuf );

    return *this;
}

/*!
    Convert this ASCString to uppercase.

    \return returns a reference on this ASCString.
*/
ASCString& ASCString::toUpper ( )
{ 
    auto_ptr< charT > l_autopBuf ( new charT [ length () + sizeof ( charT ) ] );
    
    charT* l_pBuf = l_autopBuf.get();

    ASCStringHelpers::_Strcpy ( l_pBuf, c_str () );
    ASCStringHelpers::_Strupr ( l_pBuf );
    assign  ( l_pBuf );
    
    return *this;
}

/*!
    Format this ASCString as sprintf does.

    \param pFormat a format-control string.
    \param ... Optional arguments

    \return returns a reference on this ASCString.

    See standard system documentation for more information on \e sprintf.
*/
ASCString& ASCString::format ( charT* pFormat, ... )
{
    va_list arg_ptr;
    va_start ( arg_ptr, pFormat );

    int  l_iNbChar = 10000;
    bool l_bIsDone = false;

    while ( l_bIsDone == false ) 
    {
        charT* l_pBuf = new charT [ l_iNbChar ];    

        int l_iNbCharWritten = ASCStringHelpers::_Vsnprintf ( l_pBuf, l_iNbChar, pFormat, arg_ptr );

        if ( l_iNbCharWritten != -1 )
        {
            // ok, l_pBuf was large enough to hold the whole formated string
            assign ( l_pBuf );
            l_bIsDone = true;
        }
        else
        {
            // l_pBuf is not large enough to hold the whole formated string.
            // Double the number of characters l_pBuf can hold and retry 
            // to format the string.
            l_iNbChar *= 2;
        }

        delete [] l_pBuf;
    };

    va_end ( arg_ptr );

    return *this;
}

/*!
    Print this ASCString to the standard output stream.

    \note this function is provided for convenience. It is equivalent to :

    \code
    
    ASCString strFoo ( "foo" );

    printf ( "%s", strFoo.c_str () );

    \endcode

    See standard system documentation for more information on \e printf.
*/
void ASCString::printf ( )
{
    ASCStringHelpers::_Printf ( c_str () );
}

/*!
    Duplicate and convert to lowercase.

    \param String a const reference to an ASCString object which will be duplicated and converted to lowercase.

    \return returns an ASCString object that contains a lowercased copy of \a String.

    \relates ASCString

*/
ASCString copytoLower ( const ASCString& String )
{
    ASCString l_TempString ( String );
    l_TempString.toLower ();

    return l_TempString;
}

/*!
    Duplicate and convert to uppercase.

    \param String a const reference to an ASCString object which will be duplicated and converted to uppercase.

    \return returns an ASCString object that contains an uppercased copy of \a String.

    \relates ASCString

*/
ASCString copytoUpper ( const ASCString& String )
{
    ASCString l_TempString ( String );
    l_TempString.toUpper ();

    return l_TempString;
}

