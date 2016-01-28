#include "MPQRepacker_DynamicAPI.h"
// -------------- TYPEDEFS --------------
typedef APIFUNC (*__API_GET_STR)(char*);
typedef APIFUNC (*__API_WRITE_LOG)(const char*);
typedef APIFUNC (*__API_REG_OVERRIDE)(TOverrideInfo);
typedef APIFUNC (*__API_GET_FS)(TMPQRepackerOption);
typedef APIFUNC (*__API_SET_FS)(TMPQRepackerOption, bool);
typedef APIFUNC (*__API_GET_VS)(void);
// --------------------------------------
#define MPQRepackerHandle ((HINSTANCE)NULL)
#define __API_GET_STR(x) ((__API_GET_STR)x)
#define __API_WRITE_LOG(x) ((__API_WRITE_LOG)x)
#define __API_REG_OVERRIDE(x) ((__API_REG_OVERRIDE)x)
#define __API_GET_FS(x) ((__API_GET_FS)x)
#define __API_SET_FS(x) ((__API_SET_FS)x)
#define __API_GET_VS(x) ((__API_GET_VS)x)
APIFUNC API_GetAppDataPath(char* szBuffer)
{
	__API_GET_STR __func = __API_GET_STR(GetProcAddress(MPQRepackerHandle, "API_GetAppDataPath"));
	return (*__func)(szBuffer);
}
APIFUNC API_GetTempPath(char* szBuffer)
{
	__API_GET_STR __func = __API_GET_STR(GetProcAddress(MPQRepackerHandle, "API_GetTempPath"));
	return (*__func)(szBuffer);
}
APIFUNC API_GetDestMap(char* szBuffer)
{
	__API_GET_STR __func = __API_GET_STR(GetProcAddress(MPQRepackerHandle, "API_GetDestMap"));
	return (*__func)(szBuffer);
}
APIFUNC API_GetSourceMap(char* szBuffer)
{
	__API_GET_STR __func = __API_GET_STR(GetProcAddress(MPQRepackerHandle, "API_GetSourceMap"));
	return (*__func)(szBuffer);
}
APIFUNC API_WriteLog(const char* szBuffer)
{
	__API_WRITE_LOG __func = __API_WRITE_LOG(GetProcAddress(MPQRepackerHandle, "API_WriteLog"));
	return (*__func)(szBuffer);
}
APIFUNC API_RegisterOverride(TOverrideInfo Info)
{
	__API_REG_OVERRIDE __func = __API_REG_OVERRIDE(GetProcAddress(MPQRepackerHandle, "API_RegisterOverride"));
	return (*__func)(Info);
}
APIFUNC API_UnregisterOverride(TOverrideInfo Info)
{
	__API_REG_OVERRIDE __func = __API_REG_OVERRIDE(GetProcAddress(MPQRepackerHandle, "API_UnregisterOverride"));
	return (*__func)(Info);
}
APIFUNC API_GetFlagState(TMPQRepackerOption Opt)
{
	__API_GET_FS __func = __API_GET_FS(GetProcAddress(MPQRepackerHandle, "API_GetFlagState"));
	return (*__func)(Opt);
}
APIFUNC API_SetFlagState(TMPQRepackerOption Opt, bool State)
{
	__API_SET_FS __func = __API_SET_FS(GetProcAddress(MPQRepackerHandle, "API_SetFlagState"));
	return (*__func)(Opt, State);
}
APIFUNC API_GetVersion() // 1.2.4.97
{
	__API_GET_VS __func = __API_GET_VS(GetProcAddress(MPQRepackerHandle, "API_GetVersion"));
	if(__func != NULL && (int)__func != -1) return (*__func)();
	else return -1;
}
APIFUNC API_GetProgramStatus() // 1.2.5.98
{
	__API_GET_VS __func = __API_GET_VS(GetProcAddress(MPQRepackerHandle, "API_GetProgramStatus"));
	if(__func != NULL && (int)__func != -1) return (*__func)();
	else return -1;
}
APIFUNC API_GetCompression() // 1.2.5.98
{
	__API_GET_VS __func = __API_GET_VS(GetProcAddress(MPQRepackerHandle, "API_GetCompression"));
	if(__func != NULL && (int)__func != -1) return (*__func)();
	else return -1;
}