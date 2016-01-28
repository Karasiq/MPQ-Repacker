//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#include "UBinaryFile.h"
#include "USettingsForm.h"
#include <PerlRegEx.hpp>
//---------------------------------------------------------------------------
#pragma argsused
#include "MPQRepacker_DynamicAPI.h"
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
#define GetModuleFileName __GetModuleFileName
inline UnicodeString __GetModuleFileName(HINSTANCE h)
{
	char szBuffer[MAX_PATH];
	GetModuleFileNameA(h, szBuffer, MAX_PATH);
	return szBuffer;
}
//---------------------------------------------------------------------------
TStrings* Source = NULL, *Replace = NULL;
const UnicodeString ConfigFile = API_GetAppDataPath() + "Plugins\\" + RemoveExt(ExtractFileName(GetModuleFileName(HInstance))) + ".cfg";
//---------------------------------------------------------------------------
void LoadConfig()
{
	TBinaryFile* b = new TBinaryFile(ConfigFile);
	b->Position = 0;
	Source->Text = b->ReadAnsiString();
	Replace->Text = b->ReadAnsiString();
	VCL_FREE(b);
}
void SaveConfig()
{
	TBinaryFile* b = new TBinaryFile();
	b->WriteAnsiString(Source->Text);
	b->WriteAnsiString(Replace->Text);
	b->SaveToFile(ConfigFile);
	VCL_FREE(b);
}
DLLEXPORT int CALLBACK PluginLoad()
{
	API_WriteLog("Script Autoreplace 1.0 © ZxZ666");
	Source = new TStringList();
	Replace = new TStringList();
	LoadConfig();
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginUnload()
{
	VCL_FREE(Source);
	VCL_FREE(Replace);
	return GetLastError();
}
DLLEXPORT TPluginInfo CALLBACK RegisterPlugin()
{
	TPluginInfo Result;
	Result.szName = "Script Autoreplace\0";									// Название плагина
	Result.szAuthor = "ZxZ666\0";										// Автор
	Result.szDescription = "Плагин делает автозамену в файле war3map.j (поддерживаются регулярные выражения)\0";	// Описание
    // Настройки прерываний:
	Result.bHookFileFind = false;	// CODE_FILE_SEARCH_START и CODE_FILE_SEARCH_END
	Result.bHookFileUnpack = true;	// CODE_FILES_UNPACKED
	Result.bHookFilePack = false;	// CODE_FILES_PACKED
	return Result;
}
inline UnicodeString SpecChars(UnicodeString Source)
{
	//Source = ReplaceStr(Source, "\\\\", "[__SLASH__]");
	Source = ReplaceStr(Source, "\\r", "\r");
	Source = ReplaceStr(Source, "\\n", "\n");
	Source = ReplaceStr(Source, "\\t", "\t");
	//Source = ReplaceStr(Source, "[__SLASH__]", "\\");
	return Source;
}

DLLEXPORT int CALLBACK ReceiveCode(int Code)
{
	if(Code != CODE_FILES_UNPACKED) return ERROR_INVALID_PARAMETER;
	const UnicodeString Dir = API_GetTempPath();
	const UnicodeString JFile = Dir + "war3map.j";
	if(FileExists(Dir + "Scripts\\war3map.j"))
	{
		if(FileExists(Dir + "war3map.j")) DeleteFile(Dir + "war3map.j");
		RenameFile(Dir + "Scripts\\war3map.j", Dir + "war3map.j");
	}
	TPerlRegEx* re = new TPerlRegEx();
	re->Subject = UTF8ReadFile(JFile);
	LOOP(Source->Count)
	{
		if(!Source->Strings[i].Length()) continue;
		re->RegEx = Source->Strings[i];
		re->Replacement = SpecChars(Replace->Strings[i]);
		re->ReplaceAll();
	}
	UTF8WriteFile(JFile, re->Subject);
	VCL_FREE(re);
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginSettings() // Настройки
{
    frmReplacesEdit = new TfrmReplacesEdit(NULL);
	frmReplacesEdit->mmoSource->Text = Source->Text;
	frmReplacesEdit->mmoReplace->Text = Replace->Text;
	frmReplacesEdit->ShowModal();
	if(frmReplacesEdit->ModalResult == mrOk)
	{
		Source->Text = frmReplacesEdit->mmoSource->Text;
		Replace->Text = frmReplacesEdit->mmoReplace->Text;
		SaveConfig();
	}
	VCL_FREE(frmReplacesEdit);
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
