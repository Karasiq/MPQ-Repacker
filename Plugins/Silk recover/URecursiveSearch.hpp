// CodeGear C++Builder
// Copyright (c) 1995, 2009 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Urecursivesearch.pas' rev: 21.00

#ifndef UrecursivesearchHPP
#define UrecursivesearchHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Urecursivesearch
{
//-- type declarations -------------------------------------------------------
//-- var, const, procedure ---------------------------------------------------
extern PACKAGE bool __fastcall FullRemoveDir(System::UnicodeString Dir, bool DeleteAllFilesAndFolders, bool StopIfNotAllDeleted, bool RemoveRoot);
extern PACKAGE void __fastcall FileFinder(System::UnicodeString Path, System::UnicodeString maska, bool recur, Classes::TStringList* sl);

}	/* namespace Urecursivesearch */
using namespace Urecursivesearch;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// UrecursivesearchHPP
