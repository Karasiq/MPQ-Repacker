//---------------------------------------------------------------------------

#ifndef USettingsFormH
#define USettingsFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmReplacesEdit : public TForm
{
__published:	// IDE-managed Components
	TMemo *mmoSource;
	TMemo *mmoReplace;
	TButton *btnSave;
	TButton *btnCancel;
	TLabel *lblRegexHelp;
	void __fastcall lblRegexHelpClick(TObject *Sender);
	void __fastcall mmoSourceChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfrmReplacesEdit(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmReplacesEdit *frmReplacesEdit;
//---------------------------------------------------------------------------
#endif
