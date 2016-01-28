//---------------------------------------------------------------------------

#ifndef USettingsH
#define USettingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TfrmSettings : public TForm
{
__published:	// IDE-managed Components
	TMemo *mmoCheatStrings;
	TCheckBox *chkReplaceActivator;
	TButton *btnOk;
	TButton *btnCancel;
private:	// User declarations
public:		// User declarations
	__fastcall TfrmSettings(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSettings *frmSettings;
//---------------------------------------------------------------------------
#endif
