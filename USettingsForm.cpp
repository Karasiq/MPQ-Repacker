//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "USettingsForm.h"
#include "UMainForm.h"
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmSettings *frmSettings;
bool bAnalyze[7] = {false};
bool bDontUseInternalListfile = false;
bool bMapRename = true;
TMPQCompression nCompression = mcBZIP2;
//---------------------------------------------------------------------------
void __fastcall TfrmSettings::SaveMiscSettings()
{
	TIniFile* ini = new TIniFile(IniPath);
	ini->WriteBool("Advanced", "MapRename", chkMapRename->Checked);
	ini->WriteBool("Advanced", "NotUseInternalListfile", chkInternalListfile->Checked);
	ini->WriteInteger("Advanced", "Compression", cbbCompression->ItemIndex);
	LOOP(7)
	{
        ini->WriteBool("Advanced", "Analyze" + IntToStr(i), chklstAnalyze->Checked[i]);
	}
	VCL_FREE(ini);
}
void __fastcall TfrmSettings::LoadMiscSettings()
{
	TIniFile* ini = new TIniFile(IniPath);
	chkMapRename->Checked = ini->ReadBool("Advanced", "MapRename", chkMapRename->Checked);
	chkInternalListfile->Checked = ini->ReadBool("Advanced", "NotUseInternalListfile", chkInternalListfile->Checked);
	cbbCompression->ItemIndex = ini->ReadInteger("Advanced", "Compression", cbbCompression->ItemIndex);
	LOOP(7)
	{
        chklstAnalyze->Checked[i] = ini->ReadBool("Advanced", "Analyze" + IntToStr(i), true);
	}
	VCL_FREE(ini);
	AdvSettingsFromFormToVars();
}
void TfrmSettings::AdvSettingsFromFormToVars()
{
	bDontUseInternalListfile = chkInternalListfile->Checked;
	bMapRename = chkMapRename->Checked;
	nCompression = cbbCompression->ItemIndex;
	LOOP(7)
	{
		bAnalyze[i] = chklstAnalyze->Checked[i];
	}
}
__fastcall TfrmSettings::TfrmSettings(TComponent* Owner)
	: TForm(Owner)
{
	LoadMiscSettings();
}
//---------------------------------------------------------------------------


void __fastcall TfrmSettings::btnOkClick(TObject *Sender)
{
	SaveMiscSettings();
	AdvSettingsFromFormToVars();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSettings::btnCancelClick(TObject *Sender)
{
	LoadMiscSettings();
}
//---------------------------------------------------------------------------

