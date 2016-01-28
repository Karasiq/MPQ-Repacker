// CodeGear C++Builder
// Copyright (c) 1995, 2009 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Perlregex.pas' rev: 21.00

#ifndef PerlregexHPP
#define PerlregexHPP


#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Pcre.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Perlregex
{
//-- type declarations -------------------------------------------------------
#pragma option push -b-
enum Perlregex__1 { preCaseLess, preMultiLine, preSingleLine, preExtended, preAnchored, preUnGreedy, preNoAutoCapture };
#pragma option pop

typedef Set<Perlregex__1, preCaseLess, preNoAutoCapture>  TPerlRegExOptions;

#pragma option push -b-
enum Perlregex__2 { preNotBOL, preNotEOL, preNotEmpty };
#pragma option pop

typedef Set<Perlregex__2, preNotBOL, preNotEmpty>  TPerlRegExState;

typedef System::UTF8String PCREString;

typedef void __fastcall (__closure *TPerlRegExReplaceEvent)(System::TObject* Sender, System::UTF8String &ReplaceWith);

class DELPHICLASS TPerlRegEx;
class PASCALIMPLEMENTATION TPerlRegEx : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	typedef DynamicArray<System::UTF8String> _TPerlRegEx__1;
	
	
private:
	bool FCompiled;
	bool FStudied;
	TPerlRegExOptions FOptions;
	TPerlRegExState FState;
	System::UTF8String FRegEx;
	System::UTF8String FReplacement;
	System::UTF8String FSubject;
	int FStart;
	int FStop;
	Classes::TNotifyEvent FOnMatch;
	TPerlRegExReplaceEvent FOnReplace;
	System::UTF8String __fastcall GetMatchedText(void);
	int __fastcall GetMatchedLength(void);
	int __fastcall GetMatchedOffset(void);
	void __fastcall SetOptions(TPerlRegExOptions Value);
	void __fastcall SetRegEx(const System::UTF8String Value);
	int __fastcall GetGroupCount(void);
	System::UTF8String __fastcall GetGroups(int Index);
	int __fastcall GetGroupLengths(int Index);
	int __fastcall GetGroupOffsets(int Index);
	void __fastcall SetSubject(const System::UTF8String Value);
	void __fastcall SetStart(const int Value);
	void __fastcall SetStop(const int Value);
	bool __fastcall GetFoundMatch(void);
	StaticArray<int, 301> Offsets;
	int OffsetCount;
	int pcreOptions;
	void *pattern;
	void *hints;
	void *chartable;
	char *FSubjectPChar;
	bool FHasStoredGroups;
	_TPerlRegEx__1 FStoredGroups;
	System::UTF8String __fastcall GetSubjectLeft(void);
	System::UTF8String __fastcall GetSubjectRight(void);
	
protected:
	void __fastcall CleanUp(void);
	void __fastcall ClearStoredGroups(void);
	
public:
	__fastcall TPerlRegEx(void);
	__fastcall virtual ~TPerlRegEx(void);
	__classmethod System::UnicodeString __fastcall EscapeRegExChars(const System::UnicodeString S);
	void __fastcall Compile(void);
	void __fastcall Study(void);
	bool __fastcall Match(void);
	bool __fastcall MatchAgain(void);
	System::UTF8String __fastcall Replace(void);
	bool __fastcall ReplaceAll(void);
	System::UTF8String __fastcall ComputeReplacement(void);
	void __fastcall StoreGroups(void);
	int __fastcall NamedGroup(const System::UTF8String Name);
	void __fastcall Split(Classes::TStrings* Strings, int Limit);
	void __fastcall SplitCapture(Classes::TStrings* Strings, int Limit)/* overload */;
	void __fastcall SplitCapture(Classes::TStrings* Strings, int Limit, int Offset)/* overload */;
	__property bool Compiled = {read=FCompiled, nodefault};
	__property bool FoundMatch = {read=GetFoundMatch, nodefault};
	__property bool Studied = {read=FStudied, nodefault};
	__property System::UTF8String MatchedText = {read=GetMatchedText};
	__property int MatchedLength = {read=GetMatchedLength, nodefault};
	__property int MatchedOffset = {read=GetMatchedOffset, nodefault};
	__property int Start = {read=FStart, write=SetStart, nodefault};
	__property int Stop = {read=FStop, write=SetStop, nodefault};
	__property TPerlRegExState State = {read=FState, write=FState, nodefault};
	__property int GroupCount = {read=GetGroupCount, nodefault};
	__property System::UTF8String Groups[int Index] = {read=GetGroups};
	__property int GroupLengths[int Index] = {read=GetGroupLengths};
	__property int GroupOffsets[int Index] = {read=GetGroupOffsets};
	__property System::UTF8String Subject = {read=FSubject, write=SetSubject};
	__property System::UTF8String SubjectLeft = {read=GetSubjectLeft};
	__property System::UTF8String SubjectRight = {read=GetSubjectRight};
	__property TPerlRegExOptions Options = {read=FOptions, write=SetOptions, nodefault};
	__property System::UTF8String RegEx = {read=FRegEx, write=SetRegEx};
	__property System::UTF8String Replacement = {read=FReplacement, write=FReplacement};
	__property Classes::TNotifyEvent OnMatch = {read=FOnMatch, write=FOnMatch};
	__property TPerlRegExReplaceEvent OnReplace = {read=FOnReplace, write=FOnReplace};
};


class DELPHICLASS TPerlRegExList;
class PASCALIMPLEMENTATION TPerlRegExList : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	Classes::TList* FList;
	System::UTF8String FSubject;
	TPerlRegEx* FMatchedRegEx;
	int FStart;
	int FStop;
	TPerlRegEx* __fastcall GetRegEx(int Index);
	void __fastcall SetRegEx(int Index, TPerlRegEx* Value);
	void __fastcall SetSubject(const System::UTF8String Value);
	void __fastcall SetStart(const int Value);
	void __fastcall SetStop(const int Value);
	int __fastcall GetCount(void);
	
protected:
	void __fastcall UpdateRegEx(TPerlRegEx* ARegEx);
	
public:
	__fastcall TPerlRegExList(void);
	__fastcall virtual ~TPerlRegExList(void);
	int __fastcall Add(TPerlRegEx* ARegEx);
	void __fastcall Clear(void);
	void __fastcall Delete(int Index);
	int __fastcall IndexOf(TPerlRegEx* ARegEx);
	void __fastcall Insert(int Index, TPerlRegEx* ARegEx);
	bool __fastcall Match(void);
	bool __fastcall MatchAgain(void);
	__property TPerlRegEx* RegEx[int Index] = {read=GetRegEx, write=SetRegEx};
	__property int Count = {read=GetCount, nodefault};
	__property System::UTF8String Subject = {read=FSubject, write=SetSubject};
	__property int Start = {read=FStart, write=SetStart, nodefault};
	__property int Stop = {read=FStop, write=SetStop, nodefault};
	__property TPerlRegEx* MatchedRegEx = {read=FMatchedRegEx};
};


//-- var, const, procedure ---------------------------------------------------
static const ShortInt MAX_SUBEXPRESSIONS = 0x63;

}	/* namespace Perlregex */
using namespace Perlregex;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// PerlregexHPP
