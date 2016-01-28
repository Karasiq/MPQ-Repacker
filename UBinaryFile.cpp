//---------------------------------------------------------------------------
#include "UBinaryFile.h"
//---------------------------------------------------------------------------
__fastcall TBinaryFile::TBinaryFile(UnicodeString File) : TObject()
{
	this->fs = new TMemoryStream();
	this->LoadFromFile(File);
}
__fastcall TBinaryFile::TBinaryFile(TStream* S) : TObject()
{
	this->fs = new TMemoryStream();
	this->LoadFromStream(S);
}
__fastcall TBinaryFile::TBinaryFile() : TObject()
{
	this->fs = new TMemoryStream();
}
__fastcall TBinaryFile::TBinaryFile(const void* Buffer, const unsigned __int64 Size) : TObject()
{
	this->fs = new TMemoryStream();
	fs->Write(Buffer, Size);
}
__fastcall TBinaryFile::~TBinaryFile()
{
    VCL_FREE(fs);
}
BYTE TBinaryFile::ReadByte()
{
	BYTE Result = 0;
	fs->Read(&Result, 1);
	return Result;
}
short TBinaryFile::ReadShort()
{
    short Result = 0;
	fs->Read(&Result, 2);
	return Result;
}
float TBinaryFile::ReadFloat()
{
	float Result = 0;
	fs->Read(&Result, 4);
	return Result;
}
int TBinaryFile::ReadInt()
{
	int Result = 0;
	fs->Read(&Result, 4);
	return Result;
}
double TBinaryFile::ReadDouble()
{
	double Result = 0;
	fs->Read(&Result, 8);
	return Result;
}
__int64 TBinaryFile::ReadInt64()
{
    __int64 Result = 0;
	fs->Read(&Result, 8);
	return Result;
}
UnicodeString TBinaryFile::ReadString()
{
	unsigned __int64 size = 0;
	fs->Read(&size, 8);
	wchar_t* buffer = new wchar_t[size];
	fs->Read(buffer, size);
	const UnicodeString Result = UnicodeString(buffer).SubString(1, size);
	DEL_ARRAY(buffer);
	return Result;
}
AnsiString TBinaryFile::ReadAnsiString()
{
	unsigned int size = 0;
	fs->Read(&size, 4);
	char* buffer = new char[size];
	fs->Read(buffer, size);
	const AnsiString Result = AnsiString(buffer).SubString(1, size);
	DEL_ARRAY(buffer);
	return Result;
}
void TBinaryFile::ReadStream(TStream* Dest)
{
	unsigned __int64 size = 0;
	fs->Read(&size, 8);
	BYTE* buffer = new BYTE[size];
	fs->Read(buffer, size);
	Dest->Size = 0;
	Dest->Write(buffer, size);
	DEL_ARRAY(buffer);
}
void TBinaryFile::ReadDiskFile(const UnicodeString File)
{
	if(FileExists(File)) DeleteFile(File);
	TFileStream* f = new TFileStream(File, fmCreate);
	this->ReadStream(f);
	VCL_FREE(f);
}
void TBinaryFile::ReadNullTerminatedString(char* szDest, int nMaxRead)
{
	char c = 1;
	for(int i = 0; i != nMaxRead && c != 0; i++)
	{
		fs->Read(&c, 1);
		if(szDest != NULL) szDest[i] = c;
	}
}
void TBinaryFile::ReadBuffer(const unsigned nSize, PVOID pBuffer)
{
    fs->Read(pBuffer, nSize);
}
void TBinaryFile::WriteByte(const BYTE Value)
{
	fs->Write(&Value, 1);
}
void TBinaryFile::WriteShort(const short Value)
{
    fs->Write(&Value, 2);
}
void TBinaryFile::WriteFloat(const float Value)
{
    fs->Write(&Value, 4);
}
void TBinaryFile::WriteInt(const int Value)
{
    fs->Write(&Value, 4);
}
void TBinaryFile::WriteDouble(const double Value)
{
    fs->Write(&Value, 8);
}
void TBinaryFile::WriteInt64(const __int64 Value)
{
    fs->Write(&Value, 8);
}
void TBinaryFile::WriteString(const UnicodeString Value)
{
	const unsigned __int64 size = Value.Length() * sizeof(wchar_t);
	wchar_t* buffer = WBUF(Value);
	fs->Write(&size, 8);
	fs->Write(buffer, size);
}
void TBinaryFile::WriteAnsiString(const AnsiString Value)
{
	const unsigned int size = Value.Length();
	char* buffer = BUF(Value);
	fs->Write(&size, 4);
	fs->Write(buffer, size);
}
void TBinaryFile::WriteStream(TStream* Value)
{
	const unsigned __int64 size = Value->Size;
	BYTE* buffer = new BYTE[size];
	Value->Position = 0;
	Value->Read(buffer, size);
	fs->Write(&size, 8);
	fs->Write(buffer, size);
}
void TBinaryFile::WriteDiskFile(const UnicodeString File)
{
	short mode = fmOpenRead;
	if(!FileExists(File)) mode = fmCreate;
	TFileStream* fstream = new TFileStream(File, mode);
	this->WriteStream(fstream);
	VCL_FREE(fstream);
}
void TBinaryFile::WriteBuffer(const unsigned nSize, const PVOID pBuffer)
{
    fs->Write(pBuffer, nSize);
}
void __fastcall TBinaryFile::SetPos(const unsigned __int64 __pos)
{
    fs->Position = __pos;
}
unsigned __int64 __fastcall TBinaryFile::GetPos()
{
    return fs->Position;
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
