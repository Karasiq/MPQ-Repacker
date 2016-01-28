//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "USettingsForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmReplacesEdit *frmReplacesEdit;
//---------------------------------------------------------------------------
__fastcall TfrmReplacesEdit::TfrmReplacesEdit(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfrmReplacesEdit::lblRegexHelpClick(TObject *Sender)
{
	ShellExec("http://www.regular-expressions.info/tutorial.html");
}
//---------------------------------------------------------------------------
void __fastcall TfrmReplacesEdit::mmoSourceChange(TObject *Sender)
{
	while(mmoReplace->Lines->Count < mmoSource->Lines->Count)
	{
		mmoReplace->Lines->Add("");
	}
	while(mmoReplace->Lines->Count > mmoSource->Lines->Count &&
		mmoReplace->Lines->Strings[mmoReplace->Lines->Count - 1].Length() < 1)
	{
		mmoReplace->Lines->Delete(mmoReplace->Lines->Count - 1);
	}
}
//---------------------------------------------------------------------------

