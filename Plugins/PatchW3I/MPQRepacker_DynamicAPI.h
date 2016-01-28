#ifndef MPQRepackerAPI
#define MPQRepackerAPI
// Dynamic version
#define APIFUNC int __stdcall
#include <windows.h>
enum TMPQRepackerOption {bUseXdep, bReplaceW3I, bReplaceDOO, bInsertCheats, bPause, bDeleteTemp, bUnpackUnknown};
typedef int __stdcall (*OverrideFunc)(void);
struct TPluginInfo
{
	char* szName;
	char* szDescription;
	char* szAuthor;
    bool bHookFileFind;
	bool bHookFileUnpack;
	bool bHookFilePack;
};
struct TOverrideInfo
{
	DWORD Code;
	OverrideFunc Function;
	HINSTANCE hPlugin;
};
// Коды MPQ Repacker'a:
#define CODE_FILE_SEARCH_START 0	// Начало поиска файлов
#define CODE_FILE_SEARCH_END 1		// Конец поиска файлов
#define CODE_FILES_UNPACKED 2		// Файлы распакованы во временную папку
#define CODE_FILES_PACKED 3			// Файлы запакованы в MPQ архив
// -----------------
#define OVERRIDE_SEARCH	0			// Искать файлы плагином
#define OVERRIDE_UNPACK 1 			// Распаковывать карту плагином
#define OVERRIDE_PACK 2 			// Запаковывать карту плагином
// -----------------
APIFUNC API_GetAppDataPath(char* szBuffer);
APIFUNC API_GetTempPath(char* szBuffer);
APIFUNC API_GetDestMap(char* szBuffer);
APIFUNC API_GetSourceMap(char* szBuffer);
APIFUNC API_WriteLog(const char* szBuffer);
APIFUNC API_RegisterOverride(TOverrideInfo Info);
APIFUNC API_GetFlagState(TMPQRepackerOption Opt);
APIFUNC API_SetFlagState(TMPQRepackerOption Opt, bool State);
#endif
