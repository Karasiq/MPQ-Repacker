//---------------------------------------------------------------------------
#include <windows.h>
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma argsused
#include "MPQRepacker_DynamicAPI.h"
#include "UApiHook.h"
#include "StormLib.h"
#define TFileStream Classes::TFileStream
//---------------------------------------------------------------------------
OldCode SFOA_OC;
SFILEOPENARCHIVE __sfoac = (SFILEOPENARCHIVE)GetProcAddress(GetModuleHandle("StormLib.dll"), "SFileOpenArchive");
bool   WINAPI Hook_SFileOpenArchive(const TCHAR * szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE * phMpq);
//---------------------------------------------------------------------------
#define DLLEXPORT extern "C" __declspec(dllexport)
//---------------------------------------------------------------------------
inline UnicodeString API_GetSourceMap() // overload
{
	char szBuffer[MAX_PATH];
	API_GetSourceMap(szBuffer);
	return UnicodeString(szBuffer);
}
bool WINAPI True_SFileOpenArchive(const TCHAR * szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE * phMpq)
{
	UnhookAPI(SFOA_OC, __sfoac);
	bool bResult = __sfoac(szMpqName, dwPriority, dwFlags, phMpq);
	SFOA_OC = HookAPI(__sfoac, Hook_SFileOpenArchive);
	return bResult;
}
void FixMap(UnicodeString Map)
{
	HANDLE hMpq = NULL;
	True_SFileOpenArchive(Map.t_str(), 0, MPQ_OPEN_FORCE_MPQ_V1, &hMpq);
	SFileCompactArchive(hMpq);
	SFileCloseArchive(hMpq);
}
bool WINAPI Hook_SFileOpenArchive(const TCHAR * szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE * phMpq)
{
	const UnicodeString SourceMap = API_GetSourceMap();
	if(LowerCase(szMpqName) == SourceMap.LowerCase())
	{
		return True_SFileOpenArchive(UnicodeString(SourceMap).t_str(), dwPriority, MPQ_OPEN_FORCE_MPQ_V1, phMpq);
	}
	else return True_SFileOpenArchive(szMpqName, dwPriority, dwFlags, phMpq);
}
DLLEXPORT int CALLBACK PluginLoad()
{
	API_WriteLog("UnSpazzler 1.0 © ZxZ666");
	SFOA_OC = HookAPI(__sfoac, Hook_SFileOpenArchive);
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginUnload()
{
	UnhookAPI(SFOA_OC, __sfoac);
	API_WriteLog("SFileOpenArchive unhooked.");
	return GetLastError();
}
DLLEXPORT TPluginInfo CALLBACK RegisterPlugin()
{
	TPluginInfo Result;
	Result.szName = "UnSpazzler\0";									// Название плагина
	Result.szAuthor = "ZxZ666\0";										// Автор
	Result.szDescription = "Плагин, позволяющий вскрывать карты, защищённые Ken Spazzler'ом\0";	// Описание
	// Настройки прерываний:
	Result.bHookFileFind = false;	// CODE_FILE_SEARCH_START и CODE_FILE_SEARCH_END
	Result.bHookFilePack = false;	// CODE_FILES_UNPACKED
	Result.bHookFileUnpack = false;	// CODE_FILES_PACKED
	return Result;
}
DLLEXPORT int CALLBACK ReceiveCode(int Code)
{
	return GetLastError();
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
    return 1;
}
//---------------------------------------------------------------------------
