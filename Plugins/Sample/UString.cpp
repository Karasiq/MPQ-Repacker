//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "UString.h"

#ifndef ADVANCED_VCL_H
#define DEL_ARRAY(x) if(x != NULL) {delete[] x;x=NULL;}
#define HANDLE_VALID(x) (x != NULL && x != INVALID_HANDLE_VALUE)
#endif
//---------------------------------------------------------------------------
void TString::ResizeBuffer(unsigned n)
{
	if(n == 0)
	{
		DEL_ARRAY(szString);
		return;
    }
	char* szBuffer = new char[n + 1];
	for (int i = 0; i < n && i < nSize; i++)
	{
    	szBuffer[i] = szString[i];
	}
	szBuffer[n] = 0x0; // Конец строки
	DEL_ARRAY(szString);
	szString = szBuffer;
	nSize = n;
}
TString TString::operator=(const char* szBuffer)
{
	const unsigned n = strlen(szBuffer);
	this->ResizeBuffer(n);
	for (int i = 0; i < n; i++)
	{
		szString[i] = szBuffer[i];
	}
	return this;
}
TString TString::operator+(const char* szBuffer)
{
	TString s;
	const unsigned n = nSize + strlen(szBuffer), j = nSize;
	s->ResizeBuffer(n);
	for (int i = j; i < n; i++)
	{
		s.szString[i] = szBuffer[i];
	}
	return s;
}
void TString::CopyToWideStr(wchar_t* wStr, const unsigned n)
{
	for(int i = 0; i < n; i++) wStr[i] = szString[i];
}
void TString::CopyToAnsiStr(char* cStr, const unsigned n)
{
	for(int i = 0; i < n; i++) cStr[i] = szString[i];
}
wchar_t* TString::w_str()
{
	wchar_t* buffer = new wchar_t[nSize];
	CopyToWideStr(buffer, nSize);
	return buffer;
}
TString TString::SubString(const unsigned nPos, const unsigned nCount)
{
	TString s;
	s.ResizeBuffer(nCount);
	for(int i = 0; i < nCount && i + nPos < s.nSize && i + nPos < this->nSize; i++)
	{
		s[i + nPos] = this[i + nPos];
	}
	return s;
}
unsigned TString::PosEx(TString SubStr, const unsigned Pos = 1)
{
	unsigned j = Pos, n = 0, l = SubStr.Length();
	while(j < nSize)
	{
		TString s = this->SubString(j, l);
		if(s == SubStr)
		{
			n = j;
			break;
		}
		j += l;
	}
	return n;
}
void TString::DeleteFromString(unsigned nStart, unsigned nEnd)
{
	if(nStart < nEnd) return;
    int i = 0, j = 0;
	char* szBuffer = new char[nSize];
	while(i < nSize)
	{
		if(i < nStart || i > nEnd)
		{
			szBuffer[j] = szString[i];
			j++;
		}
		i++;
	}
	DEL_ARRAY(szString);
	szString = szBuffer;
	this->ResizeBuffer(j);
}
void TString::InsertString(unsigned nIndex, TString s)
{
	int	nStart = nIndex, nEnd = nIndex + s.Length();
    int i = 0, j = 0;
	char* szBuffer = new char[nSize + s.Length() + 1];
	while(i < nSize + s.Length())
	{
		if(i < nStart || i > nEnd)
		{
			szBuffer[i] = szString[i];
		}
		else
		{
        	szBuffer[i] = s.szString[j];
			j++;
        }
		i++;
	}
	DEL_ARRAY(szString);
	szString = szBuffer;
	this->ResizeBuffer(i);
}
void TString::SaveToFile(TString File)
{
	HANDLE hFile = CreateFileA(File, GENERIC_WRITE, 0, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(HANDLE_VALID(hFile))
	{
		WriteFile(hFile, this->szString, this->nSize, NULL, NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
    }
}
void TString::LoadFromFile(TString File)
{
	HANDLE hFile = CreateFileA(File, GENERIC_READ, 0, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(HANDLE_VALID(hFile))
	{
		DWORD a = 0, b = 0;
		GetFileSize(hFile, &a);
		ReadFile(hFile, this->szString, a, &b, NULL);
		this->ResizeBuffer(b);
		CloseHandle(hFile);
    }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
