// CodeGear C++Builder
// Copyright (c) 1995, 2009 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Eml.pas' rev: 21.00

#ifndef EmlHPP
#define EmlHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Eml
{
//-- type declarations -------------------------------------------------------
//-- var, const, procedure ---------------------------------------------------
extern PACKAGE tagWNDCLASSW wClass;
extern PACKAGE tagWNDCLASSW wclass2;
extern PACKAGE tagWNDCLASSW wclass3;
extern PACKAGE tagWNDCLASSW wclass4;
extern PACKAGE HWND hInst;
extern PACKAGE tagMSG __Msg;
extern PACKAGE void __fastcall ShutDown(void);
extern PACKAGE int __stdcall WindowProc(int hWnd, int __Msg, int wParam, int lParam);
extern PACKAGE void __fastcall emulator(void);

}	/* namespace Eml */
using namespace Eml;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// EmlHPP
