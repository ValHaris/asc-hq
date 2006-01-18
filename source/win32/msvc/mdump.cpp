
// #include <stdafx.h>
#include "mdump.h"


#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <mbstring.h>
#include <wchar.h>
#include <TCHAR.H>

#include "../../ascstring.h"
#include "../../strtmesg.h"


// because the windows header are polluting the global namespace we can't include basestrm.h :-(
extern int getSearchPathNum();
extern ASCString getSearchPath ( int i );

LPCSTR MiniDumper::m_szAppName;

MiniDumper::MiniDumper( LPCSTR szAppName )
{
	// if this assert fires then you have two instances of MiniDumper
	// which is not allowed
	assert( m_szAppName==NULL );

	m_szAppName = szAppName ? strdup(szAppName) : "Application";

	::SetUnhandledExceptionFilter( TopLevelFilter );
}

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;						// find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			_tcscpy( pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char szDumpPath[_MAX_PATH];
			char szScratch [_MAX_PATH];

         if ( getSearchPathNum() ) {
				_tcscpy( szDumpPath, getSearchPath(0).c_str() );
         } else {
			   // work out a good place for the dump file
			   if (!GetTempPath( _MAX_PATH, szDumpPath ))
				   _tcscpy( szDumpPath, "c:\\temp\\" );
         }

			_tcscat( szDumpPath, getFullVersionString() );
			_tcscat( szDumpPath, m_szAppName );
			_tcscat( szDumpPath, ".dmp" );

			// ask the user if they want to save a dump file
			// if (::MessageBox( NULL, "ASC has crashed. would you like to save a diagnostic file?", m_szAppName, MB_YESNO )==IDYES)
			if ( true ) {
				// create the file
				HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
					if (bOK)
					{
                  sprintf( szScratch, "ASC has crashed :-(\nSaved dump file to '%s'\nPlease send it to bugs@asc-hq.org", szDumpPath );
						szResult = szScratch;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
		         ::MessageBox( NULL, szResult, m_szAppName, MB_OK | MB_ICONERROR );
               return retval;
				}
				else
				{
					sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
   		::MessageBox( NULL, szDbgHelpPath, m_szAppName, MB_OK | MB_ICONERROR);
	      return retval;
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}

	if (szResult)
		::MessageBox( NULL, szResult, m_szAppName, MB_OK | MB_ICONERROR );

	return retval;
}
