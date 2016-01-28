//---------------------------------------------------------------------------
#ifndef UStringH
#define UStringH
//---------------------------------------------------------------------------
#include "windows.h"
//---------------------------------------------------------------------------
namespace ZxZ666
{
	class TString
	{
		private:
			void ResizeBuffer(unsigned n);
		protected:
			char* szString;
			unsigned nSize;
		public:
			TString() {szString = NULL; nSize = 0;}
			~TString() {ResizeBuffer(0);}
			unsigned Length() {return nSize;}
			bool operator==(TString s) {return (strcmp(this->szString, s.szString) == 0);}
			TString operator=(const char* szBuffer);
			TString operator+(const char* szBuffer);
			char* c_str() {return s.szString;}
			void CopyToWideStr(wchar_t* wStr, const unsigned n);
			void CopyToAnsiStr(char* cStr, const unsigned n);
			wchar_t* w_str();
			char* operator=(TString s) {return this->c_str();}
			wchar_t* operator=(TString s) {return this->c_str();}
			char& operator[](const unsigned i) {return &(szString[i - 1]);}
			void DeleteFromString(unsigned nStart, unsigned nEnd);
			void InsertString(unsigned nIndex, TString s);
			TString SubString(const unsigned nPos, const unsigned nCount);
			unsigned PosEx(TString SubStr, const unsigned Pos = 1);
			void SaveToFile(TString File);
			void LoadFromFile(TString File);
	};
};
using namespace ZxZ666;
//---------------------------------------------------------------------------
#endif
