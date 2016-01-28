#include "StdAfx.h"

#include "IsValidFileName.h"

FileName::RES_CODE FileName::IsStdDeviceName( const char* pFileName, int len )
{
    // Since the device names aren't numerous, this method of checking is the
    // fastest. Note that each character is checked with both cases.
    if( len == 3 )
    {
        if( 0 == strnicmp( pFileName, "aux", 3 ) ) return FILENAME_AUX;
        if( 0 == strnicmp( pFileName, "con", 3 ) ) return FILENAME_CON;
        if( 0 == strnicmp( pFileName, "nul", 3 ) ) return FILENAME_NUL;
        if( 0 == strnicmp( pFileName, "prn", 3 ) ) return FILENAME_PRN;
    }
    else if( len == 4 )
    {
        char lastLetter = pFileName[3];

        if( 0 == strnicmp( pFileName, "com", 3 ) )
        {
            if( lastLetter >= '1' && lastLetter <= '9' ) return FILENAME_AUX;
            return OK; // уж точно это не будет "lpt*"
        }

        if( 0 == strnicmp( pFileName, "lpt", 3 ) )
        {
            if( lastLetter >= '1' && lastLetter <= '9' ) return FILENAME_AUX;
        }
    }
    else if( len == 6 )
    {
        if( 0 == strnicmp( pFileName, "clock$", 6 ) ) return FILENAME_CLOCK;
    }

    return OK;
}

FileName::RES_CODE FileName::IsStdDeviceName( const wchar_t* pFileName, int len )
{
    // Since the device names aren't numerous, this method of checking is the
    // fastest. Note that each character is checked with both cases.
    if( len == 3 )
    {
        if( 0 == wcsnicmp( pFileName, L"aux", 3 ) ) return FILENAME_AUX;
        if( 0 == wcsnicmp( pFileName, L"con", 3 ) ) return FILENAME_CON;
        if( 0 == wcsnicmp( pFileName, L"nul", 3 ) ) return FILENAME_NUL;
        if( 0 == wcsnicmp( pFileName, L"prn", 3 ) ) return FILENAME_PRN;
    }
    else if( len == 4 )
    {
        wchar_t lastLetter = pFileName[3];

        if( 0 == wcsnicmp( pFileName, L"com", 3 ) )
        {
            if( lastLetter >= L'1' && lastLetter <= L'9' ) return FILENAME_AUX;
            return OK; // уж точно это не будет "lpt*"
        }

        if( 0 == wcsnicmp( pFileName, L"lpt", 3 ) )
        {
            if( lastLetter >= L'1' && lastLetter <= L'9' ) return FILENAME_AUX;
        }
    }
    else if( len == 6 )
    {
        if( 0 == wcsnicmp( pFileName, L"clock$", 6 ) ) return FILENAME_CLOCK;
    }

    return OK;
}