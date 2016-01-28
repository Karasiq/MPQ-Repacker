//---------------------------------------------------------------------------

#include <vcl.h>
#include "HashLib.h"
#pragma hdrstop

#include "UPluginForm.h"
#include "USettingsForm.h"
#include "URecursiveSearch.hpp"
#include "IniFiles.hpp"
#include "UFileLock.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
TfrmPlugins *frmPlugins;
TPluginFramework* PluginManager;
const UnicodeString PlgIni = AppDataPath + "Plugins.ini";
bool bDisableOverrides = false;
TLockedFile* IniLock = NULL;
//---------------------------------------------------------------------------
__fastcall TfrmPlugins::TfrmPlugins(TComponent* Owner)
	: TForm(Owner)
{
	IniLock = new TLockedFile(PlgIni);
	IniLock->Lock();
	if(!DirectoryExists(PluginsDir))
	{
        UnpackSamplePlugin();
	}
	PluginManager = new TPluginFramework(PluginsDir);
	PluginManager->GetPluginList(chklstPlugins->Items);
	if(chklstPlugins->Count)
	{
		chklstPlugins->ItemIndex = 0;
		chklstPluginsClick(this);
	}
	Load();
}
bool TfrmPlugins::IsPluginsPresent()
{
	if(chklstPlugins->Count) return true;
	else return false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPlugins::FormDestroy(TObject *Sender)
{
	Save();
	PluginManager->Free();
	IniLock->Free();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPlugins::chklstPluginsClickCheck(TObject *Sender)
{
	const unsigned int i = chklstPlugins->ItemIndex;
	if(chklstPlugins->Checked[i])
		PluginManager->LoadPlugin(i);
	else
		PluginManager->UnloadPlugin(i);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPlugins::chklstPluginsClick(TObject *Sender)
{
	const unsigned int i = chklstPlugins->ItemIndex;
	TPluginInfo inf = PluginManager->GetPluginInfo(i);
	lblPlgInfo->Caption =
		(UnicodeString)"Автор: " + inf.szAuthor + "\n" +
		"Название: " + inf.szName + "\n" +
		"Описание: " + inf.szDescription;
	BOOL bSettingsPresent = PluginManager->CallSettings(i, false);
	btnSettings->Enabled = bSettingsPresent;
}
//---------------------------------------------------------------------------
void UnpackSamplePlugin()
{
	__MkDir(PluginsDir);
	ResUnpack("SamplePlugin", PluginsDir + "SamplePlugin.dll");
	ResUnpack("SamplePlugin_source", PluginsDir + "SamplePlugin_source.zip");
}
//---------------------------------------------------------------------------
void TfrmPlugins::Load()
{
	IniLock->Unlock();
	TIniFile* ini = new TIniFile(PlgIni);
	bDisableOverrides = ini->ReadBool("Debug", "DisableOverrides", false);
	if(ini->ReadString("MainSection", "ListHash", "") == SHA512(chklstPlugins->Items->Text))
	{
		const unsigned int PlgCount = ini->ReadInteger("MainSection", "Count", 0);
		for(int i = 0; i < PlgCount; i++)
		{
			chklstPlugins->Checked[i] = ini->ReadBool("Flags", IntToStr(i), false);
			if(chklstPlugins->Checked[i]) PluginManager->LoadPlugin(i);
        }
	}
	ini->Free();
	IniLock->Lock();
}
//---------------------------------------------------------------------------
void TfrmPlugins::Save()
{
	IniLock->Unlock();
	TIniFile* ini = new TIniFile(PlgIni);
	if(ini->SectionExists("Flags")) ini->EraseSection("Flags");
	const UnicodeString ListHash = SHA512(chklstPlugins->Items->Text);
	ini->WriteString("MainSection", "ListHash", ListHash);
	const unsigned int PlgCount = chklstPlugins->Count;
	ini->WriteInteger("MainSection", "Count", PlgCount);
	ini->WriteBool("Debug", "DisableOverrides", bDisableOverrides);
	for(int i = 0; i < PlgCount; i++)
	{
		ini->WriteBool("Flags", IntToStr(i), chklstPlugins->Checked[i]);
	}
	ini->Free();
	IniLock->Lock();
}
//---------------------------------------------------------------------------
DLLEXPORT API_GetTempPath(char* szBuffer)
{
	__strcpy(szBuffer, MainForm->edtTempDir->Text);
	return GetLastError();
}
DLLEXPORT API_GetDestMap(char* szBuffer)
{
	__strcpy(szBuffer, __DestMap);//MainForm->edtDestMap->Text);
	return GetLastError();
}
DLLEXPORT API_GetSourceMap(char* szBuffer)
{
	__strcpy(szBuffer, __SourceMap);//MainForm->edtSourceMap->Text);
	return GetLastError();
}
DLLEXPORT API_GetAppDataPath(char* szBuffer)
{
	__strcpy(szBuffer, AppDataPath);
	return GetLastError();
}
DLLEXPORT API_WriteLog(const char* szBuffer)
{
	MainForm->mmoLog->Lines->Add(UnicodeString(szBuffer));
	return GetLastError();
}
DLLEXPORT API_RegisterOverride(TOverrideInfo Info)
{
	if(bDisableOverrides) return ERROR_ACCESS_DENIED;

	if(PluginManager->IndexByHInstance(Info.hPlugin) == -1)
	{
		MainForm->mmoLog->Lines->Add("Ошибка регистрации оверрайда: неправильный hPlugin");
		return ERROR_INVALID_PARAMETER;
	}

	switch(Info.Code)
	{
		case OVERRIDE_SEARCH:
		{
			if(PluginManager->Overrides.Search.Enabled)
			{
				const UnicodeString a = frmPlugins->chklstPlugins->Items->Strings[PluginManager->Overrides.Search.PluginIndex];
				const UnicodeString b = frmPlugins->chklstPlugins->Items->Strings[PluginManager->IndexByHInstance(Info.hPlugin)];
				MainForm->mmoLog->Lines->Add("Плагин " + b + " конфликтует с плагином " + a + "!");
            }
			PluginManager->Overrides.Search.Enabled = true;
			PluginManager->Overrides.Search.Function = Info.Function;
			PluginManager->Overrides.Search.PluginIndex = PluginManager->IndexByHInstance(Info.hPlugin);
            break;
		}
		case OVERRIDE_UNPACK:
		{
			if(PluginManager->Overrides.Unpack.Enabled)
			{
				const UnicodeString a = frmPlugins->chklstPlugins->Items->Strings[PluginManager->Overrides.Search.PluginIndex];
				const UnicodeString b = frmPlugins->chklstPlugins->Items->Strings[PluginManager->IndexByHInstance(Info.hPlugin)];
				MainForm->mmoLog->Lines->Add("Плагин " + b + " конфликтует с плагином " + a + "!");
            }
			PluginManager->Overrides.Unpack.Enabled = true;
			PluginManager->Overrides.Unpack.Function = Info.Function;
			PluginManager->Overrides.Unpack.PluginIndex = PluginManager->IndexByHInstance(Info.hPlugin);
			break;
		}
		case OVERRIDE_PACK:
		{
			if(PluginManager->Overrides.Pack.Enabled)
			{
				const UnicodeString a = frmPlugins->chklstPlugins->Items->Strings[PluginManager->Overrides.Search.PluginIndex];
				const UnicodeString b = frmPlugins->chklstPlugins->Items->Strings[PluginManager->IndexByHInstance(Info.hPlugin)];
				MainForm->mmoLog->Lines->Add("Плагин " + b + " конфликтует с плагином " + a + "!");
            }
			PluginManager->Overrides.Pack.Enabled = true;
			PluginManager->Overrides.Pack.Function = Info.Function;
			PluginManager->Overrides.Pack.PluginIndex = PluginManager->IndexByHInstance(Info.hPlugin);
            break;
		}
		default:
		{
            return ERROR_INVALID_PARAMETER;
        }
	}
	return GetLastError();
}
DLLEXPORT API_UnregisterOverride(TOverrideInfo Info)
{
	if(bDisableOverrides) return ERROR_ACCESS_DENIED;
	const unsigned i = PluginManager->IndexByHInstance(Info.hPlugin);
	if(i == -1)
	{
		MainForm->mmoLog->Lines->Add("Ошибка снятия оверрайда: неправильный hPlugin");
		return ERROR_INVALID_PARAMETER;
	}

	switch(Info.Code)
	{
		case OVERRIDE_SEARCH:
		{
			if(PluginManager->Overrides.Search.PluginIndex != i)
			{
				const UnicodeString a = frmPlugins->chklstPlugins->Items->Strings[PluginManager->Overrides.Search.PluginIndex];
				const UnicodeString b = frmPlugins->chklstPlugins->Items->Strings[i];
				MainForm->mmoLog->Lines->Add("Плагин " + b + " пытается снять оверрайд плагина " + a + "!");
				return ERROR_ACCESS_DENIED;
			}
			PluginManager->Overrides.Search.Enabled = false;
            break;
		}
		case OVERRIDE_UNPACK:
		{
			if(PluginManager->Overrides.Unpack.PluginIndex != i)
			{
				const UnicodeString a = frmPlugins->chklstPlugins->Items->Strings[PluginManager->Overrides.Search.PluginIndex];
				const UnicodeString b = frmPlugins->chklstPlugins->Items->Strings[i];
				MainForm->mmoLog->Lines->Add("Плагин " + b + " пытается снять оверрайд плагина " + a + "!");
				return ERROR_ACCESS_DENIED;
			}
			PluginManager->Overrides.Unpack.Enabled = false;
			break;
		}
		case OVERRIDE_PACK:
		{
			if(PluginManager->Overrides.Pack.PluginIndex != i)
			{
				const UnicodeString a = frmPlugins->chklstPlugins->Items->Strings[PluginManager->Overrides.Search.PluginIndex];
				const UnicodeString b = frmPlugins->chklstPlugins->Items->Strings[i];
				MainForm->mmoLog->Lines->Add("Плагин " + b + " пытается снять оверрайд плагина " + a + "!");
				return ERROR_ACCESS_DENIED;
			}
			PluginManager->Overrides.Pack.Enabled = false;
            break;
		}
		default:
		{
            return ERROR_INVALID_PARAMETER;
        }
	}
	return GetLastError();
}
DLLEXPORT API_GetMPQFileCount(char* szMPQFile)
{
	int nResult = GetFileCount(szMPQFile);
	return nResult;
}
DLLEXPORT API_GetFlagState(TMPQRepackerOption Opt)
{
	switch(Opt)
	{
		case bUseXdep:
			return MainForm->chkUseXdep->Checked;
		case bReplaceW3I:
			return MainForm->chkReplaceW3I->Checked;
		case bReplaceDOO:
			return MainForm->chkReplaceDOO->Checked;
		case bInsertCheats:
			return MainForm->chkInsertCheats->Checked;
		case bPause:
			return MainForm->chkManualEdit->Checked;
		case bDeleteTemp:
			return MainForm->chkDeleteTemp->Checked;
		case bUnpackUnknown:
			return MainForm->chkExtractUnknownFiles->Checked;
		case bW3iWtsMapRename:
			return bMapRename;
		case bNotUseInternalListfile:
			return bDontUseInternalListfile;
		default:
		{
            return ERROR_INVALID_PARAMETER;
        }
    }
}
DLLEXPORT API_SetFlagState(TMPQRepackerOption Opt, const bool bState)
{
	switch(Opt)
	{
		case bUseXdep:
		{
			MainForm->chkUseXdep->Checked = bState;
			break;
		}
		case bReplaceW3I:
		{
			MainForm->chkReplaceW3I->Checked = bState;
			break;
		}
		case bReplaceDOO:
		{
			MainForm->chkReplaceDOO->Checked = bState;
			break;
		}
		case bInsertCheats:
		{
			MainForm->chkInsertCheats->Checked = bState;
			break;
		}
		case bPause:
		{
			MainForm->chkManualEdit->Checked = bState;
			break;
		}
		case bDeleteTemp:
		{
			MainForm->chkDeleteTemp->Checked = bState;
			break;
		}
		case bUnpackUnknown:
		{
			MainForm->chkExtractUnknownFiles->Checked = bState;
			break;
		}
		case bW3iWtsMapRename:
		{
			bMapRename = bState;
			break;
        }
		case bNotUseInternalListfile:
		{
			bDontUseInternalListfile = bState;
			break;
		}
		default:
		{
            return ERROR_INVALID_PARAMETER;
        }
	}
	return GetLastError();
}
DLLEXPORT API_GetProgramStatus()
{
    return __Status;
}
DLLEXPORT API_GetCompression()
{
    return nCompression;
}
DLLEXPORT API_GetVersion()
{
    return nVersion; // Константа (UMainForm.h)
}
//---------------------------------------------------------------------------
void __fastcall TfrmPlugins::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
	const UnicodeString SourceFile = PluginsDir + "SamplePlugin_source.zip";
	if(Key == VK_F1)
	{
		if(ResUnpack("SamplePlugin_source", SourceFile))
			ShellExec(SourceFile);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPlugins::btnSettingsClick(TObject *Sender)
{
	PluginManager->CallSettings(chklstPlugins->ItemIndex);
}
//---------------------------------------------------------------------------

