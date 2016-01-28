//---------------------------------------------------------------------------
#ifndef UBinaryFileH
#define UBinaryFileH
//---------------------------------------------------------------------------
#include <windows.h>
#include <System.hpp>
#include <Classes.hpp>
#include <vcl.h>
//---------------------------------------------------------------------------
class TBinaryFile : public TObject
{
	__published:
		__property const unsigned __int64 Position = {read=GetPos,write=SetPos,default=0};
	protected:
		TMemoryStream* fs;
	public:
		__fastcall SetPos(const unsigned __int64 __pos);
		unsigned __int64 __fastcall GetPos();
		BYTE* operator=(TBinaryFile* Source)
		{
			const unsigned __int64 size = Source->fs->Size;
			BYTE* buffer = new BYTE[size];
			Source->fs->Read(buffer, size);
			return buffer;
		}
		void Clear() {fs->Clear();}
		__fastcall TBinaryFile();
		__fastcall TBinaryFile(UnicodeString File);
		__fastcall TBinaryFile(TStream* S);
		__fastcall TBinaryFile(const void* Buffer, const unsigned __int64 Size);
		__fastcall ~TBinaryFile();
		BYTE ReadByte();
		short ReadShort();
		float ReadFloat();
		int ReadInt();
		double ReadDouble();
		__int64 ReadInt64();
		UnicodeString ReadString();
		AnsiString ReadAnsiString();
		void ReadStream(TStream* Dest);
		void ReadDiskFile(const UnicodeString File);
		void ReadBuffer(const unsigned nSize, PVOID pBuffer);
		void WriteByte(const BYTE Value);
		void WriteShort(const short Value);
		void WriteFloat(const float Value);
		void WriteInt(const int Value);
		void WriteDouble(const double Value);
		void WriteInt64(const __int64 Value);
		void WriteString(const UnicodeString Value);
		void WriteAnsiString(const AnsiString Value);
		void WriteStream(TStream* Value);
		void WriteDiskFile(const UnicodeString File);
		void WriteBuffer(const unsigned nSize, const PVOID pBuffer);

		virtual void SaveToFile(const UnicodeString File)
		{
            fs->SaveToFile(File);
        }
		virtual void LoadFromFile(const UnicodeString File)
		{
            if(FileExists(File)) fs->LoadFromFile(File);
        }
		virtual void SaveToStream(TStream* S)
		{
			fs->SaveToStream(S);
        }
		virtual void LoadFromStream(TStream* S)
		{
            if(HANDLE_VALID(S)) fs->LoadFromStream(S);
        }
};
//---------------------------------------------------------------------------
#endif
