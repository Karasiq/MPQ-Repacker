//---------------------------------------------------------------------------


#pragma hdrstop

#include "UPluginFramework.h"
#include "UMainForm.h"
#include "URecursiveSearch.hpp"
//---------------------------------------------------------------------------
typedef TPluginInfo __stdcall (*P_RegisterPlg)(void);
typedef int __stdcall (*P_PluginLoad)(void);
typedef int __stdcall (*P_ReceiveCode)(int Code);
typedef int __stdcall (*P_Settings)(void);
//---------------------------------------------------------------------------
TOverride InitOverride()
{
	TOverride Result;
	Result.Enabled = false;
	Result.PluginIndex = -1;
	Result.Function = NULL;
	return Result;
}
TMPQRepackerOverrides InitOverrides()
{
	TMPQRepackerOverrides Result;
	Result.Search = InitOverride();
	Result.Unpack = InitOverride();
	Result.Pack =	InitOverride();
	return Result;
}
TPluginInfo InitPlgInfo()
{
	TPluginInfo Result;
	Result.szName = (char*)malloc(MAX_PATH);
	Result.szAuthor = (char*)malloc(MAX_PATH);
	Result.szDescription = (char*)malloc(1024);
	Result.bHookFileFind = false;
	Result.bHookFilePack = false;
	Result.bHookFileUnpack = false;
	return Result;
}
void UnInitPlgInfo(TPluginInfo& Info)
{
	free(Info.szName);
	free(Info.szAuthor);
	free(Info.szDescription);
}
__fastcall TPluginFramework::TPluginFramework(const UnicodeString DirWithPlugins)
	:TObject()
{
	this->Dir = IncludeTrailingBackslash(DirWithPlugins);
	this->PluginsCount = 0;
	this->PluginsList = NULL;
	this->Overrides = InitOverrides();
	FindPlugins();
}
__fastcall TPluginFramework::~TPluginFramework()
{
	if(PluginsList != NULL)
	{
		for(int i = 0; i < PluginsCount; i++)
		{
			UnloadPlugin(i);
			UnInitPlgInfo(PluginsList[i].Info);
		}
		delete[] PluginsList;
		PluginsList = NULL;
	}
}
void TPluginFramework::UnregisterOverridesByIndex(const int Index)
{
	if(Index < 0 || Index > PluginsCount) return; // Неправильный индекс
	if(Overrides.Search.PluginIndex == Index) Overrides.Search.Enabled = false;
	if(Overrides.Unpack.PluginIndex == Index) Overrides.Unpack.Enabled = false;
	if(Overrides.Pack.PluginIndex == Index) Overrides.Pack.Enabled = false;
}
void __fastcall TPluginFramework::GetPluginList(TStrings* S)
{
	if(S == NULL) return;
	else
	{
		S->Clear();
		for(int i = 0; i < this->PluginsCount; i++)
		{
			S->Add(this->PluginsList[i].Info.szName);
		}
	}
}
int TPluginFramework::IndexByName(const UnicodeString Name)
{
	for(unsigned int i = 0; i < this->PluginsCount; i++)
	{
		if(PluginsList[i].Info.szName == Name) return i;
	}
	return -1;
}
int TPluginFramework::IndexByHInstance(HINSTANCE hPlg)
{
	for(unsigned int i = 0; i < this->PluginsCount; i++)
	{
		if(PluginsList[i].hPlugin == hPlg) return i;
	}
	return -1;
}
void __fastcall TPluginFramework::UnloadPlugin(const int Index)
{
	if(this->PluginsList[Index].Enabled)
	{
		UnregisterOverridesByIndex(Index); // Снимает все оверрайды отключаемого плагина
		P_PluginLoad FreeProc = (P_PluginLoad)GetProcAddress(this->PluginsList[Index].hPlugin, "PluginUnload");
		if(FreeProc != NULL) (*FreeProc)();
		FreeLibrary(this->PluginsList[Index].hPlugin);
		this->PluginsList[Index].Enabled = false;
		this->PluginsList[Index].hPlugin = NULL;
    }
}
void __fastcall TPluginFramework::LoadPlugin(const int Index)
{
	if(!this->PluginsList[Index].Enabled)
	{
		this->PluginsList[Index].hPlugin = LoadLibrary(this->PluginsList[Index].Path.t_str());
		this->PluginsList[Index].Enabled = true;
		P_PluginLoad InitProc = (P_PluginLoad)GetProcAddress(this->PluginsList[Index].hPlugin, "PluginLoad");
		if(InitProc != NULL) (*InitProc)();
    }
}
TPluginInfo TPluginFramework::GetPluginInfo(const int Index)
{
    return this->PluginsList[Index].Info;
}
TPluginInfo TPluginFramework::RequestPlgInfo(UnicodeString Path)
{
	HINSTANCE hTemp = LoadLibrary(Path.t_str());
	P_RegisterPlg RegProc = (P_RegisterPlg)GetProcAddress(hTemp, "RegisterPlugin");
	TPluginInfo Result = InitPlgInfo();
	if(RegProc != NULL)
	{
		TPluginInfo __Result = (*RegProc)();
		Result.bHookFileFind = __Result.bHookFileFind;
		Result.bHookFileUnpack = __Result.bHookFileUnpack;
		Result.bHookFilePack = __Result.bHookFilePack;
		strcpy(Result.szName, __Result.szName);
		strcpy(Result.szAuthor, __Result.szAuthor);
		strcpy(Result.szDescription, __Result.szDescription);
	}
	if(hTemp != NULL) FreeLibrary(hTemp);
	return Result;
}
void __fastcall TPluginFramework::FindPlugins()
{
	if(PluginsList != NULL)
	{
		delete[] PluginsList;
		PluginsList = NULL;
    }
	TStringList* s = new TStringList();
	FileFinder(Dir, "*.dll", true, s);
	PluginsCount = s->Count;
	PluginsList = new TPlugin[PluginsCount];
	for(int i = 0; i < s->Count; i++)
	{
		if(FileExists(s->Strings[i]))
		{
			PluginsList[i].Path = s->Strings[i];
			PluginsList[i].Enabled = false;
			PluginsList[i].Info = RequestPlgInfo(s->Strings[i]);
			PluginsList[i].hPlugin = NULL;
        }
	}
	s->Free();
}
int __fastcall TPluginFramework::SendCode(const int Index, const int Code)
{
	if(PluginsList[Index].Enabled)
	{
		P_ReceiveCode CodeHandleProc = (P_ReceiveCode)GetProcAddress(PluginsList[Index].hPlugin, "ReceiveCode");
		if(CodeHandleProc != NULL) return (*CodeHandleProc)(Code);
    }
}
int TPluginFramework::CallSettings(const unsigned Index, bool Execute)
{
	if(Index < 0 || Index > PluginsCount)
		return ERROR_INVALID_PARAMETER; // Неправильный индекс

	P_Settings __sfunc = (P_Settings)GetProcAddress(PluginsList[Index].hPlugin, "PluginSettings");
	if(Execute && HANDLE_VALID(__sfunc)) return (*__sfunc)();

	if(HANDLE_VALID(__sfunc))
		return TRUE;
	else
		return FALSE;
}
bool CheckFlag(TPluginInfo Info, TPFCondition Flag)
{
	if(Flag == pfcFileFindHooked && Info.bHookFileFind) return true;
	if(Flag == pfcFileUnpackHooked && Info.bHookFileUnpack) return true;
	if(Flag == pfcFilePackHooked && Info.bHookFilePack) return true;
    return false;
}
void TPluginFramework::SendCodeToAll(const int Code, TPFCondition IfFlag)
{
	for(unsigned int i = 0; i < this->PluginsCount; i++)
	{
        if(CheckFlag(PluginsList[i].Info, IfFlag)) SendCode(i, Code);
    }
}
//---------------------------------------------------------------------------

#pragma package(smart_init)
