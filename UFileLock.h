//---------------------------------------------------------------------------
#ifndef UFileLockH
#define UFileLockH
//---------------------------------------------------------------------------
#include <System.hpp>
#include <windows.h>
#include "UFileLock.h"
//---------------------------------------------------------------------------
class TLockedFile : public TObject
{
	private:
		UnicodeString File;
		bool Locked;
		HANDLE hFile;
	public:
		__fastcall TLockedFile(const UnicodeString FileName);
		__fastcall ~TLockedFile();
		void Lock();
		void Unlock();
};
//---------------------------------------------------------------------------
#endif
