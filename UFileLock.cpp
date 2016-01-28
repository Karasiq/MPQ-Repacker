//---------------------------------------------------------------------------
#include "UFileLock.h"
#pragma hdrstop
//---------------------------------------------------------------------------
__fastcall TLockedFile::TLockedFile(const UnicodeString FileName)
{
	this->File = FileName;
	this->Locked = false;
	this->hFile = NULL;
}
__fastcall TLockedFile::~TLockedFile()
{
	Unlock();
}
void TLockedFile::Lock()
{
	if(Locked) return;
	hFile = CreateFile(File.t_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	Locked = true;
}
void TLockedFile::Unlock()
{
	if(!Locked) return;
	CloseHandle(hFile);
	hFile = NULL;
	Locked = false;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
