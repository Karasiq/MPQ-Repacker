//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "UBinaryFile.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
void PatchTFTW3i(const UnicodeString w3i)
{
	TBinaryFile* b = new TBinaryFile(w3i);
	b->Skip(sizeof(int) * 3);
	LOOP(4) b->ReadNullTerminatedString(NULL, INFINITE);
	b->Skip(sizeof(float) * 8);
	b->Skip(sizeof(int) * 7);
	b->Skip(sizeof(char) + sizeof(int) * 2);
	LOOP(4) b->ReadNullTerminatedString(NULL, INFINITE);
	b->Skip(sizeof(int));
	LOOP(4) b->ReadNullTerminatedString(NULL, INFINITE);
	b->Skip(sizeof(int) * 2 + sizeof(float) * 3 + sizeof(BYTE) * 4);
	b->ReadNullTerminatedString(NULL, INFINITE);
	b->Skip(sizeof(char) * 5);
	int j = b->ReadInt(); // MAXPL
	LOOP(j)
	{
		b->Skip(sizeof(int) * 4);
		b->ReadNullTerminatedString(NULL, INFINITE);
        b->Skip(sizeof(float) * 2 + sizeof(int) * 2);
	}
	j = b->ReadInt(); // MAXFC
	if(j == 255)
	{
		j = 0;
		b->Position -= 4;
		b->WriteInt(0);
    }
	LOOP(j)
	{
		b->Skip(sizeof(int) * 2);
		b->ReadNullTerminatedString(NULL, INFINITE);
	}
	j = b->ReadInt(); // UCOUNT
	if(j == 255)
	{
		j = 0;
		b->Position -= 4;
		b->WriteInt(0);
    }
	LOOP(j)
	{
		b->Skip(sizeof(char) * 4 + sizeof(int) * 3);
		b->ReadNullTerminatedString(NULL, INFINITE);
	}
	j = b->ReadInt(); // TCOUNT
	if(j == 255)
	{
		j = 0;
		b->Position -= 4;
		b->WriteInt(0);
    }
	LOOP(j)
	{
		b->Skip(sizeof(char) * 4 + sizeof(int));
		b->ReadNullTerminatedString(NULL, INFINITE);
	}
	b->WriteInt(0);
	b->WriteInt(0);
	b->WriteByte(0xFF);
//    j = b->ReadInt(); // UTCOUNT (Random Unit Tables)
//	LOOP(j)
//	{
//		b->WriteInt(0);
//	}
	b->CutThere(); // Обрезать файл
	b->SaveToFile(w3i); // Сохранить в war3map.w3i
	VCL_FREE(b);
}
void PatchROCW3i(const UnicodeString w3i)
{
	TBinaryFile* b = new TBinaryFile(w3i);
	b->Skip(sizeof(int) * 3);
	LOOP(4) b->ReadNullTerminatedString(NULL, INFINITE);
	b->Skip(sizeof(float) * 8);
	b->Skip(sizeof(int) * 7);
	b->Skip(sizeof(char) + sizeof(int));
	LOOP(3) b->ReadNullTerminatedString(NULL, INFINITE);
	b->Skip(sizeof(int));
	LOOP(3) b->ReadNullTerminatedString(NULL, INFINITE);
	int j = b->ReadInt(); // MAXPL
	LOOP(j)
	{
		b->Skip(sizeof(int) * 4);
		b->ReadNullTerminatedString(NULL, INFINITE);
        b->Skip(sizeof(float) * 2 + sizeof(int) * 2);
	}
	j = b->ReadInt(); // MAXFC
	if(j == 255)
	{
		j = 0;
		b->Position -= 4;
		b->WriteInt(0);
    }
	LOOP(j)
	{
		b->Skip(sizeof(int) * 2);
		b->ReadNullTerminatedString(NULL, INFINITE);
	}
	j = b->ReadInt(); // UCOUNT
	if(j == 255)
	{
		j = 0;
		b->Position -= 4;
		b->WriteInt(0);
    }
	LOOP(j)
	{
		b->Skip(sizeof(char) * 4 + sizeof(int) * 3);
		b->ReadNullTerminatedString(NULL, INFINITE);
	}
	j = b->ReadInt(); // TCOUNT
	if(j == 255)
	{
		j = 0;
		b->Position -= 4;
		b->WriteInt(0);
    }
	LOOP(j)
	{
		b->Skip(sizeof(char) * 4 + sizeof(int));
		b->ReadNullTerminatedString(NULL, INFINITE);
	}
	b->WriteInt(0);
	b->WriteInt(0);
	b->WriteByte(0xFF);
//    j = b->ReadInt(); // UTCOUNT (Random Unit Tables)
//	LOOP(j)
//	{
//		b->WriteInt(0);
//	}
	b->CutThere(); // Обрезать файл
	b->SaveToFile(w3i); // Сохранить в war3map.w3i
	VCL_FREE(b);
}
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClick(TObject *Sender)
{
//	PatchTFTW3i("C:\\$LDROOT\\war3map.w3i");
}
//---------------------------------------------------------------------------
