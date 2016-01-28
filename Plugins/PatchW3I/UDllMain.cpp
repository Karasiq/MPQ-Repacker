//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma argsused
#include "MPQRepacker_DynamicAPI.h"
#include "UBinaryFile.h"
//---------------------------------------------------------------------------
#define DLLEXPORT extern "C" __declspec(dllexport)
//---------------------------------------------------------------------------
DLLEXPORT int CALLBACK PluginLoad()
{
	API_WriteLog("PatchW3I 0.8b © ZxZ666");
	API_SetFlagState(bReplaceW3I, false);
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginUnload()
{
	return GetLastError();
}
DLLEXPORT TPluginInfo CALLBACK RegisterPlugin()
{
	TPluginInfo Result;
	Result.szName = "PatchW3I\0";		// Название плагина
	Result.szAuthor = "ZxZ666\0";							// Автор
	Result.szDescription = "Патчит файл war3map.w3i, избавляя от ошибки редактора WERandomGroupSet, но сохраняет оригинальное описание карты.\0";	// Описание
	// Настройки прерываний:
	Result.bHookFileFind = false;	// CODE_FILE_SEARCH_START и CODE_FILE_SEARCH_END
	Result.bHookFileUnpack = true;	// CODE_FILES_UNPACKED
	Result.bHookFilePack = false;	// CODE_FILES_PACKED
	return Result;
}
inline UnicodeString API_GetTempPath() // overload
{
	char szBuffer[MAX_PATH];
	API_GetTempPath(szBuffer);
	return IncludeTrailingBackslash(szBuffer);
}
inline UnicodeString API_GetAppDataPath() // overload
{
	char szBuffer[MAX_PATH];
	API_GetAppDataPath(szBuffer);
	return IncludeTrailingBackslash(szBuffer);
}
inline UnicodeString API_GetSourceMap()
{
    char szBuffer[MAX_PATH];
	API_GetSourceMap(szBuffer);
	return UnicodeString(szBuffer);
}
inline UnicodeString AddQuotes(const UnicodeString Source)
{
	return "\"" + Source + "\"";
}
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
	LOOP(j)
	{
		b->Skip(sizeof(int) * 2);
		b->ReadNullTerminatedString(NULL, INFINITE);
	}
	if(b->EndOfFile(1))
	{
        b->WriteInt(0);
	}
	else
	{
		j = b->ReadInt(); // UCOUNT
		LOOP(j)
		{
			b->Skip(sizeof(char) * 4 + sizeof(int) * 3);
			b->ReadNullTerminatedString(NULL, INFINITE);
		}
	}
	if(b->EndOfFile(1))
	{
        b->WriteInt(0);
	}
	else
	{
		j = b->ReadInt(); // TCOUNT
		LOOP(j)
		{
			b->Skip(sizeof(char) * 4 + sizeof(int));
			b->ReadNullTerminatedString(NULL, INFINITE);
		}
	}
	if(b->EndOfFile(1))
	{
		b->WriteInt(0);
		b->WriteInt(0);
		b->WriteByte(0xFF);
		b->CutThere(); // Обрезать файл
	}
//    j = b->ReadInt(); // UTCOUNT (Random Unit Tables)
//	LOOP(j)
//	{
//		b->WriteInt(0);
//	}
	b->SaveToFile(w3i); // Сохранить в war3map.w3i
	VCL_FREE(b);
	API_WriteLog("PatchW3I: Файл war3map.w3i пропатчен! (TFT)");
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
	LOOP(j)
	{
		b->Skip(sizeof(int) * 2);
		b->ReadNullTerminatedString(NULL, INFINITE);
	}
	if(b->EndOfFile(1))
	{
        b->WriteInt(0);
	}
	else
	{
		j = b->ReadInt(); // UCOUNT
		LOOP(j)
		{
			b->Skip(sizeof(char) * 4 + sizeof(int) * 3);
			b->ReadNullTerminatedString(NULL, INFINITE);
		}
	}
	if(b->EndOfFile(1))
	{
		b->WriteInt(0);
	}
	else
	{
		j = b->ReadInt(); // TCOUNT
		LOOP(j)
		{
			b->Skip(sizeof(char) * 4 + sizeof(int));
			b->ReadNullTerminatedString(NULL, INFINITE);
		}
	}
	if(b->EndOfFile(1))
	{
		b->WriteInt(0);
		b->WriteByte(0xFF);
        b->CutThere(); // Обрезать файл
	}
//    j = b->ReadInt(); // UTCOUNT (Random Unit Tables)
//	LOOP(j)
//	{
//		b->WriteInt(0);
//	}
	b->SaveToFile(w3i); // Сохранить в war3map.w3i
	VCL_FREE(b);
	API_WriteLog("PatchW3I: Файл war3map.w3i пропатчен! (RoC)");
}
DLLEXPORT int CALLBACK ReceiveCode(int Code)
{
	if(Code != CODE_FILES_UNPACKED) return ERROR_INVALID_PARAMETER;
	if(API_GetFlagState(bReplaceW3I))
	{
		API_WriteLog("PatchW3I: Включена замена W3I - пропатчивание отменено!");
		return 0;
    }
	const UnicodeString w3i = API_GetTempPath() + "war3map.w3i";
	if(!FileExists(w3i))
	{
		API_WriteLog("PatchW3I: Файл war3map.w3i не найден!");
		return ERROR_FILE_NOT_FOUND;
	}
	try
	{
		if(ExtractFileExt(API_GetSourceMap()) == ".w3m") PatchROCW3i(w3i);
		else if(ExtractFileExt(API_GetSourceMap()) == ".w3x") PatchTFTW3i(w3i);
	}
	catch(Exception &e)
	{
		API_WriteLog(BUF("PatchW3I: " + e.Message));
    }
	return GetLastError();
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
    return 1;
}
//---------------------------------------------------------------------------
