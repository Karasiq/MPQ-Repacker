#include "MPQRepacker_DynamicAPI.h"
// -------------- TYPEDEFS --------------
typedef APIFUNC (*__API_GET_STR)(char*);
typedef APIFUNC (*__API_WRITE_LOG)(const char*);
typedef APIFUNC (*__API_REG_OVERRIDE)(TOverrideInfo);
typedef APIFUNC (*__API_GET_FS)(TMPQRepackerOption);
typedef APIFUNC (*__API_SET_FS)(TMPQRepackerOption, bool);
// --------------------------------------
#define MPQRepackerHandle ((HINSTANCE)NULL)
#define __API_GET_STR(x) ((__API_GET_STR)x)
#define __API_WRITE_LOG(x) ((__API_WRITE_LOG)x)
#define __API_REG_OVERRIDE(x) ((__API_REG_OVERRIDE)x)
#define __API_GET_FS(x) ((__API_GET_FS)x)
#define __API_SET_FS(x) ((__API_SET_FS)x)
HINSTANCE __GetMPQRepackerHandle()
{
	char szBuffer[MAX_PATH] = {0};
	GetModuleFileName(NULL, &szBuffer, MAX_PATH);
	return GetModuleHandle(szBuffer);
}
APIFUNC API_GetAppDataPath(char* szBuffer)
{
	__API_GET_STR __funñ = __API_GET_STR(GetProcAddress(MPQRepackerHandle, "API_GetAppDataPath"));
	return (*__func)(szBuffer);
}
APIFUNC API_GetTempPath(char* szBuffer)
{
	__API_GET_STR __funñ = __API_GET_STR(GetProcAddress(MPQRepackerHandle, "API_GetTempPath"));
	return (*__func)(szBuffer);
}
APIFUNC API_GetDestMap(char* szBuffer)
{
	__API_GET_STR __funñ = __API_GET_STR(GetProcAddress(MPQRepackerHandle, "API_GetDestMap"));
	return (*__func)(szBuffer);
}
APIFUNC API_GetSourceMap(char* szBuffer)
{
	__API_GET_STR __funñ = __API_GET_STR(GetProcAddress(MPQRepackerHandle, "API_GetSourceMap"));
	return (*__func)(szBuffer);
}
APIFUNC API_WriteLog(const char* szBuffer)
{
	__API_WRITE_LOG __funñ = __API_WRITE_LOG(GetProcAddress(MPQRepackerHandle, "API_WriteLog"));
	return (*__func)(szBuffer);
}
APIFUNC API_RegisterOverride(TOverrideInfo Info)
{
	__API_REG_OVERRIDE __funñ = __API_REG_OVERRIDE(GetProcAddress(MPQRepackerHandle, "API_RegisterOverride"));
	return (*__func)(Info);
}
APIFUNC API_GetFlagState(TMPQRepackerOption Opt)
{
	__API_GET_FS __funñ = __API_GET_FS(GetProcAddress(MPQRepackerHandle, "API_GetFlagState"));
	return (*__func)(Opt);
}
APIFUNC API_SetFlagState(TMPQRepackerOption Opt, bool State)
{
	__API_SET_FS __funñ = __API_SET_FS(GetProcAddress(MPQRepackerHandle, "API_SetFlagState"));
	return (*__func)(Opt, State);
}