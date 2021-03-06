#ifndef MPQRepackerAPI
#define MPQRepackerAPI
// Dynamic version
#define APIFUNC int __stdcall
#include <windows.h>
enum TMPQRepackerStatus {mrsIdle = 0, mrsRePacking = 1, mrsBatchRePacking = 2, mrsClosing = 3};
enum TMPQCompression {mcHUFFMAN = 0, mcZLIB = 1, mcPKWARE = 2, mcLZMA = 3, mcBZIP2 = 4, mcSPARSE = 5};
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
// ���� MPQ Repacker'a:
#define CODE_FILE_SEARCH_START 0	// ������ ������ ������
#define CODE_FILE_SEARCH_END 1		// ����� ������ ������
#define CODE_FILES_UNPACKED 2		// ����� ����������� �� ��������� �����
#define CODE_FILES_PACKED 3			// ����� ���������� � MPQ �����
// -----------------
#define OVERRIDE_SEARCH	0			// ������ ����� ��������
#define OVERRIDE_UNPACK 1 			// ������������� ����� ��������
#define OVERRIDE_PACK 2 			// ������������ ����� ��������
// -----------------
APIFUNC API_GetAppDataPath(char* szBuffer);
APIFUNC API_GetTempPath(char* szBuffer);
APIFUNC API_GetDestMap(char* szBuffer);
APIFUNC API_GetSourceMap(char* szBuffer);
APIFUNC API_WriteLog(const char* szBuffer);
APIFUNC API_RegisterOverride(TOverrideInfo Info);
APIFUNC API_UnregisterOverride(TOverrideInfo Info);
APIFUNC API_GetFlagState(TMPQRepackerOption Opt);
APIFUNC API_SetFlagState(TMPQRepackerOption Opt, bool State);
APIFUNC API_GetProgramStatus();
APIFUNC API_GetVersion();
APIFUNC API_GetCompression();
#endif
