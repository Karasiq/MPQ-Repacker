//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
#pragma argsused
#include "MPQRepacker_DynamicAPI.h"
#include "vcl.h"
#include "StormLib.h"
//---------------------------------------------------------------------------
typedef HANDLE MPQHANDLE;
typedef MPQHANDLE (WINAPI *P_MpqOpenArchiveForUpdate)(LPCSTR lpFileName, DWORD dwFlags, DWORD dwMaximumFilesInArchive);
typedef DWORD (WINAPI *P_MpqCloseUpdatedArchive)(MPQHANDLE hMPQ, DWORD dwUnknown2);
typedef BOOL (WINAPI *P_MpqAddFileToArchiveEx)(MPQHANDLE hMPQ, LPCSTR lpSourceFileName, LPCSTR lpDestFileName, DWORD dwFlags, DWORD dwCompressionType, DWORD dwCompressLevel);
typedef BOOL (WINAPI *P_MpqDeleteFile)(MPQHANDLE hMPQ, LPCSTR lpFileName);
// MpqOpenArchiveForUpdate flags
#define MOAU_CREATE_NEW        0x00
#define MOAU_CREATE_ALWAYS     0x08 //Was wrongly named MOAU_CREATE_NEW
#define MOAU_OPEN_EXISTING     0x04
#define MOAU_OPEN_ALWAYS       0x20
#define MOAU_READ_ONLY         0x10 //Must be used with MOAU_OPEN_EXISTING
#define MOAU_MAINTAIN_LISTFILE 0x01

// MpqAddFileToArchive flags
#define MAFA_EXISTS           0x80000000 //Will be added if not present
#define MAFA_UNKNOWN40000000  0x40000000
#define MAFA_MODCRYPTKEY      0x00020000
#define MAFA_ENCRYPT          0x00010000
#define MAFA_COMPRESS         0x00000200
#define MAFA_COMPRESS2        0x00000100
#define MAFA_REPLACE_EXISTING 0x00000001

// MpqAddFileToArchiveEx compression flags
#define MAFA_COMPRESS_STANDARD 0x08 //Standard PKWare DCL compression
#define MAFA_COMPRESS_DEFLATE  0x02 //ZLib's deflate compression
#define MAFA_COMPRESS_WAVE     0x81 //Standard wave compression
#define MAFA_COMPRESS_WAVE2    0x41 //Unused wave compression

// Flags for individual compression types used for wave compression
#define MAFA_COMPRESS_WAVECOMP1 0x80 //Main compressor for standard wave compression
#define	MAFA_COMPRESS_WAVECOMP2 0x40 //Main compressor for unused wave compression
#define MAFA_COMPRESS_WAVECOMP3 0x01 //Secondary compressor for wave compression

// ZLib deflate compression level constants (used with MpqAddFileToArchiveEx and MpqAddFileFromBufferEx)
#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
//---------------------------------------------------------------------------
#define DLLEXPORT extern "C" __declspec(dllexport)
//---------------------------------------------------------------------------
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
inline UnicodeString API_GetDestMap()
{
    char szBuffer[MAX_PATH];
	API_GetDestMap(szBuffer);
	return UnicodeString(szBuffer);
}
DLLEXPORT int CALLBACK DummySearch()
{
    /* if(!API_GetFlagState(bInsertCheats) && !API_GetFlagState(bPause))
	{
		throw Exception("Ошибка: включите опцию вставки чит-пака или редактирования вручную!");
		return ERROR_INVALID_PARAMETER;
    } */
	TStrings* s = new TStringList();
	s->Add("Scripts\\war3map.j");
	s->Add("war3map.j");
	s->SaveToFile(API_GetAppDataPath() + "FILELIST_CACHE.txt");
	return GetLastError();
}
DLLEXPORT int CALLBACK DummyUnpack()
{
	return GetLastError();
}
unsigned int GetFileCount(HANDLE hMpq)
{
	int    nError = ERROR_SUCCESS; // Error value
	unsigned int nFileCount = 0; // Result

	if(nError == ERROR_SUCCESS)
	{
		DWORD pcbLengthNeeded = sizeof(unsigned int);
		if(!SFileGetFileInfo(hMpq, SFILE_INFO_NUM_FILES, &nFileCount, sizeof(unsigned int), &pcbLengthNeeded))
        	nError = GetLastError();
    }

	return nFileCount;
}
unsigned int GetFileCount(const UnicodeString Archive)
{
	int    nError = ERROR_SUCCESS; // Error value
	unsigned int nFileCount = 0; // Result
	HANDLE hMpq = NULL;
	SFileOpenArchive(BUF(Archive), 0, MPQ_OPEN_FORCE_MPQ_V1, &hMpq);

	if(nError == ERROR_SUCCESS)
	{
		DWORD pcbLengthNeeded = sizeof(unsigned int);
		if(!SFileGetFileInfo(hMpq, SFILE_INFO_NUM_FILES, &nFileCount, sizeof(unsigned int), &pcbLengthNeeded))
        	nError = GetLastError();
    }
	if(HANDLE_VALID(hMpq)) SFileCloseArchive(hMpq);
	return nFileCount;
}
bool MapSpazzled(const UnicodeString Map)
{
	if(!FileExists(Map)) return false;
	Classes::TFileStream* fs = new Classes::TFileStream(Map, fmOpenRead);
	/* fs->Position = 0x604;
	char szBuffer[8] = {0};
	fs->Read(szBuffer, 8);
	bool bResult = (strncmp(szBuffer, "SPAZZLER", 8) == 0); */
	fs->Position = 0x200;
	DWORD d1 = 0, d2 = 0;
	fs->Read(&d1, 4);
	fs->Position = 0x600;
	fs->Read(&d2, 4);
	bool bResult = (d1 == 0x1B51504D || d2 == 0x1A51504D);

	VCL_FREE(fs);
	return bResult;
}
int RebuildBuffer(const char* szSource, char* szDest, int nStart, int nEnd, int nSize)
{
	int i = 0, j = 0;


	while(i < nSize)
	{
		if(i < nStart || i > nEnd)
		{
			szDest[j] = szSource[i];
			j++;
		}
		i++;
	}
	return j;
}
void DeSpazzle(const UnicodeString Map)
{
    TMPQArchive* ha = NULL;

	SFileOpenArchive(BUF(Map), 0, MPQ_OPEN_FORCE_MPQ_V1, &(HANDLE)ha);
	const unsigned nFileCount = GetFileCount(ha);
	if(HANDLE_VALID(ha))
	{
		DWORD mpqpos = 512;
		DWORD offset = ha->MpqPos - mpqpos;
		DWORD htpos = mpqpos + ha->pHeader->dwHashTablePos, btpos = mpqpos + ha->pHeader->dwBlockTablePos;
		ha->pHeader->wFormatVersion = MPQ_FORMAT_VERSION_1;
		ha->pHeader->dwHeaderSize = MPQ_HEADER_SIZE_V1;
		TMPQHeader h = *ha->pHeader;
		if(HANDLE_VALID(ha)) SFileCloseArchive(ha);
		Classes::TFileStream* fs = new Classes::TFileStream(Map, fmOpenReadWrite);
		fs->Position = mpqpos; // offset;
		fs->Write(&h, 32);
		fs->Position = 0;
		char* szBuffer = new char[fs->Size], *szNewBuffer = new char[(fs->Size - (0x7FF - 0x400))];
		fs->Read(szBuffer, fs->Size);
		int nNewSize = RebuildBuffer(szBuffer, szNewBuffer, 0x400, 0x7FF, fs->Size);
		fs->Size = 0;
		fs->Write(szNewBuffer, nNewSize);

		DEL_ARRAY(szBuffer);
		DEL_ARRAY(szNewBuffer);
		VCL_FREE(fs);
	}
}
bool PackWithSFMPQ()
{
    const UnicodeString sfmpq_dll = GetTempDir() + "sfmpq_api.dll";
	if(!ResUnpack("SFMPQ_dll_internal", sfmpq_dll, 0)) return -1;
	LoadDLL(hDll, sfmpq_dll);
	BOOL bResult = FALSE;
	try
	{
		P_MpqOpenArchiveForUpdate __open = (P_MpqOpenArchiveForUpdate)GetProcAddress(hDll, "MpqOpenArchiveForUpdate");
		P_MpqCloseUpdatedArchive __close = (P_MpqCloseUpdatedArchive)GetProcAddress(hDll, "MpqCloseUpdatedArchive");
		P_MpqAddFileToArchiveEx __add = (P_MpqAddFileToArchiveEx)GetProcAddress(hDll, "MpqAddFileToArchiveEx");
		P_MpqDeleteFile __del = (P_MpqDeleteFile)GetProcAddress(hDll, "MpqDeleteFile");
		MPQHANDLE hMpq = (*__open)(BUF(API_GetDestMap()), MOAU_OPEN_EXISTING, GetFileCount(API_GetDestMap()));
		(*__del)(hMpq, "Scripts\\war3map.j");
		(*__del)(hMpq, "war3map.j");
		(*__del)(hMpq, "(attributes)");

		bResult = (*__add)(hMpq, BUF(API_GetTempPath() + "war3map.j"), "war3map.j", MAFA_COMPRESS + MAFA_REPLACE_EXISTING, MAFA_COMPRESS_DEFLATE, Z_BEST_COMPRESSION);
		if(HANDLE_VALID(hMpq)) (*__close)(hMpq, 0);
		hMpq = NULL;

		if(bResult)
			API_WriteLog("SFMPQ: war3map.j успешно упакован!");
		else
			API_WriteLog("SFMPQ: Ошибка упаковки war3map.j!");
	}
	catch(Exception &e)
	{
		API_WriteLog(BUF("Ошибка: " + e.Message));
		bResult = false;
	}
	FreeDLL(hDll);
	return bResult;
}
bool PackWithStormLib()
{
	HANDLE hMpq = NULL;
	bool bResult = SFileOpenArchive(BUF(API_GetDestMap()), 0, 0, &hMpq);
	if(!HANDLE_VALID(hMpq))
	{
		int nError = GetLastError();
		API_WriteLog(BUF("Ошибка: #" + IntToStr(nError)));
		return false;
	}
	SFileRemoveFile(hMpq, "war3map.j", 0);
	SFileRemoveFile(hMpq, "Scripts\\war3map.j", 0);
	bResult = SFileAddFileEx(hMpq, BUF(API_GetTempPath() + "war3map.j"), "war3map.j", MPQ_FILE_REPLACEEXISTING + MPQ_FILE_COMPRESS + MPQ_FILE_ENCRYPTED, MPQ_COMPRESSION_ZLIB);
	if(bResult)
		API_WriteLog("StormLib: war3map.j успешно упакован!");
	else
		API_WriteLog("StormLib: Ошибка упаковки war3map.j!");
	SFileCloseArchive(hMpq);
	return bResult;
}
DLLEXPORT int CALLBACK DummyPack()
{
	char* szSource = BUF(API_GetSourceMap()), *szDest = BUF(API_GetDestMap());
	CopyFile(API_GetSourceMap(), API_GetDestMap());
	if(MapSpazzled(API_GetDestMap())) DeSpazzle(API_GetDestMap());
	if(!PackWithStormLib()) PackWithSFMPQ();
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginLoad()
{
	API_WriteLog("Cheat Injector 1.10 © ZxZ666");
	API_SetFlagState(bInsertCheats, true);
	// Оверрайды - замены функций
	// Код обработки файлов в ReceiveCode помогает программе работать,
	// А в функциях-оверрайдах он заменяет соответствующие этапы работы программы
	// Т.е. поиск, распаковку, и упаковку
	TOverrideInfo inf[3];
	inf[0].Code = OVERRIDE_SEARCH;
	inf[0].Function = DummySearch;
	inf[0].hPlugin = HInstance;
	//////////////////////////////
	inf[1].Code = OVERRIDE_UNPACK;
	inf[1].Function = DummyUnpack;
	inf[1].hPlugin = HInstance;
	//////////////////////////////
	inf[2].Code = OVERRIDE_PACK;
	inf[2].Function = DummyPack;
	inf[2].hPlugin = HInstance;
	//////////////////////////////
	API_RegisterOverride(inf[0]);
	//API_RegisterOverride(inf[1]);
	API_RegisterOverride(inf[2]);
	// Снимать оверрайды не надо, это сделает сама программа если вы
	// Правильно указали hPlugin, а если нет - то оверрайды просто
	// Не зарегистрируются - т.к. программа проверяет правильнось hPlugin
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginUnload()
{
	API_WriteLog("[ CheatInjector unloaded ]");
	return GetLastError();
}
DLLEXPORT TPluginInfo CALLBACK RegisterPlugin()
{
	TPluginInfo Result;
	Result.szName = "Cheat Injector\0";									// Название плагина
	Result.szAuthor = "ZxZ666\0";										// Автор
	Result.szDescription = "Плагин, преобразовывающий MPQ RePacker в обычный чит-инжектор.\nВнимание: может не работать с включенной опцией \"Давать карте название из war3map.w3i/wts\"!\0";	// Описание
    // Настройки прерываний:
	Result.bHookFileFind = false;	// CODE_FILE_SEARCH_START и CODE_FILE_SEARCH_END
	Result.bHookFileUnpack = false;	// CODE_FILES_UNPACKED
	Result.bHookFilePack = false;	// CODE_FILES_PACKED
	return Result;
}
DLLEXPORT int CALLBACK ReceiveCode(int Code)
{
	return GetLastError();
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	// Лучше не писать здесь ничего, для этого есть функции PluginLoad() и PluginUnload().
	// DllMain(DLL_PROCESS_ATTACH) вызывается не только при включении плагина,
	// но и при загрузке для получения информации о нём (RegisterPlugin),
	// а PluginLoad/PluginUnload вызываются соответственно только при включении/выключении
    return 1;
}
//---------------------------------------------------------------------------
