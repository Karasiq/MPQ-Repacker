//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma argsused
#include "MPQRepacker_DynamicAPI.h"
#include "USettings.h"
#include "UBinaryFile.h"
//---------------------------------------------------------------------------
#define DLLEXPORT extern "C" __declspec(dllexport)
bool bReplaceActivator = true;
void LoadCfg(bool &bReplaceActivator, TStrings* s);
void SaveCfg(bool bReplaceActivator, TStrings* s);
//---------------------------------------------------------------------------
DLLEXPORT int CALLBACK PluginLoad()
{
	API_WriteLog("Extendable Cheatpack Detector 1.1 © ZxZ666");
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginUnload()
{
	return GetLastError();
}
DLLEXPORT TPluginInfo CALLBACK RegisterPlugin()
{
	TPluginInfo Result;
	Result.szName = "Extendable Cheatpack Detector\0";		// Название плагина
	Result.szAuthor = "ZxZ666\0";							// Автор
	Result.szDescription = "Ищет чит-паки в картах, заменяет активатор.\0";	// Описание
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
const UnicodeString CpSearch = API_GetAppDataPath() + "Plugins\\" + DLLNAME_WITHOUT_EXT + ".cfg";
void LoadCfg(bool &bReplaceActivator, TStrings* s)
{
	if(!FileExists(CpSearch))
	{
		// API_WriteLog("Файл " + ExtractFileName(CpSearch) + " не найден!");
		bReplaceActivator = true;
		s->Text = "string Activator=\"\n"
				"if SubString(GetEventPlayerChatString(),0,23)==\"\n"
				"call TriggerRegisterPlayerChatEvent( gg_trg_ACTIVATOR, Player(0), \"\n"
				"set vfai_col7[12]=\"\n"
				"string FSs=\"\n"
				"if SubString(GetEventPlayerChatString(),0,100)==\"\n"
				"call TriggerRegisterPlayerChatEvent(Fai_Password,Player(0),\"\n"
				"call TriggerRegisterPlayerChatEvent( gg_trg_Activator, Player(1), \"\n"
				"string ACTIVATOR_OLOLO=\"\n"
				"SubStringBJ(GetEventPlayerChatString(),1,8)==\"\n"
				"string Spack=\"";
		SaveCfg(bReplaceActivator, s);
		return;
	}
	TBinaryFile* b = new TBinaryFile(CpSearch);
	b->Position = 0;
	s->Text = b->ReadAnsiString();
	bReplaceActivator = b->ReadByte();
	VCL_FREE(b);
}
void SaveCfg(bool bReplaceActivator, TStrings* s)
{
	TBinaryFile* b = new TBinaryFile(CpSearch);
	b->Position = 0;
	b->WriteAnsiString(s->Text);
	b->WriteByte(bReplaceActivator);
	b->SaveToFile(CpSearch);
	VCL_FREE(b);
}
UnicodeString FindActivator(const UnicodeString Source, const UnicodeString SearchStr, const char EndChar = '"')
{
	int Pos = Source.Pos(SearchStr) + SearchStr.Length();
	if (Pos == SearchStr.Length()) return ""; // Строка не найдена
	int Pos2 = Pos;
	while (Source[Pos2] != EndChar) Pos2++;
	const unsigned int InfoLength = Pos2 - Pos;
	const UnicodeString Result = Source.SubString(Pos, InfoLength);
	return Result;
}
inline UnicodeString AddQuotes(const UnicodeString Source)
{
	return "\"" + Source + "\"";
}
bool SearchForCheatpack(UnicodeString &Activator)
{
	bool bResult = false;
	Activator = "";
	const UnicodeString Dir = API_GetTempPath();
	const UnicodeString JFile = Dir + "war3map.j";

	if(FileExists(Dir + "Scripts\\war3map.j"))
	{
		if(FileExists(Dir + "war3map.j")) DeleteFile(Dir + "war3map.j");
		RenameFile(Dir + "Scripts\\war3map.j", Dir + "war3map.j");
	}

    if(!FileExists(JFile))
	{
		API_WriteLog("Файл war3map.j не найден!");
		return false;
	}

	if(API_GetFlagState(bInsertCheats)) API_WriteLog("[CheatPack Detector] Осторожно! Плагин не сможет предотвратить вставку чит-пака в карту!");
	TStrings* j = new TStringList();
	TStrings* s = new TStringList();
	// s->LoadFromFile(CpSearch);
	LoadCfg(bReplaceActivator, s);
	// j->LoadFromFile(JFile);
	j->Text = UTF8ReadFile(JFile);
	for(int i = 0; i < s->Count; i++)
	{
		const UnicodeString act = FindActivator(j->Text, s->Strings[i]);
		if(act.Length())
		{
			Activator = act;
			bResult = true;
			break;
        }
	}
	s->Free();
	j->Free();
	return bResult;
}
void ReplaceActivator(const UnicodeString Old, const UnicodeString New)
{
	const UnicodeString Dir = API_GetTempPath();
	const UnicodeString JFile = Dir + "war3map.j";
	
	if(FileExists(Dir + "Scripts\\war3map.j"))
	{
		if(FileExists(Dir + "war3map.j")) DeleteFile(Dir + "war3map.j");
		RenameFile(Dir + "Scripts\\war3map.j", Dir + "war3map.j");
	}
	TStrings* j = new TStringList();
	// j->LoadFromFile(JFile);
	j->Text = UTF8ReadFile(JFile);
	j->Text = ReplaceStr(j->Text, AddQuotes(Old), AddQuotes(New));
	// j->SaveToFile(JFile);
	UTF8WriteFile(JFile, j->Text);
	j->Free();
}
DLLEXPORT int CALLBACK ReceiveCode(int Code)
{
	if(Code == CODE_FILES_UNPACKED)
	{
		UnicodeString Activator = "";
		if(SearchForCheatpack(Activator))
		{
			const UnicodeString msgtext = "Чит-пак найден! \nАктиватор: " + AddQuotes(Activator);
			API_WriteLog(BUF(msgtext));
			Msg(msgtext);
			if(bReplaceActivator)
			{
				const UnicodeString NewActivator = InputBox("Extendable Cheatpack Detector", "Новый активатор:", "");
				if(NewActivator.Length()) ReplaceActivator(Activator, NewActivator);
			}
		}
	}
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginSettings() // Настройки
{
	frmSettings = new TfrmSettings(NULL);
	// frmSettings->chkReplaceActivator->Checked = bReplaceActivator;
	// frmSettings->mmoCheatStrings->Lines->LoadFromFile(CpSearch);
	LoadCfg(bReplaceActivator, frmSettings->mmoCheatStrings->Lines);
	frmSettings->chkReplaceActivator->Checked = bReplaceActivator;
	frmSettings->ShowModal();
	if(frmSettings->ModalResult == mrOk)
	{
		// bReplaceActivator = frmSettings->chkReplaceActivator->Checked; //Ask("Заменять активатор?");
		// frmSettings->mmoCheatStrings->Lines->SaveToFile(CpSearch);
		SaveCfg(frmSettings->chkReplaceActivator->Checked, frmSettings->mmoCheatStrings->Lines);
	}
	VCL_FREE(frmSettings);
	return GetLastError();
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
    return 1;
}
//---------------------------------------------------------------------------
