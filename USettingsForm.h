//---------------------------------------------------------------------------

#ifndef USettingsFormH
#define USettingsFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include "UPluginForm.h"
//---------------------------------------------------------------------------
class TfrmSettings : public TForm
{
__published:	// IDE-managed Components
	TComboBox *cbbCompression;
	TLabel *lblCompression;
	TLabel *lblAnalyze;
	TCheckListBox *chklstAnalyze;
	TCheckBox *chkMapRename;
	TCheckBox *chkInternalListfile;
	TButton *btnOk;
	TButton *btnCancel;
	void __fastcall btnOkClick(TObject *Sender);
	void __fastcall btnCancelClick(TObject *Sender);
private:	// User declarations
	void __fastcall SaveMiscSettings();
	void __fastcall LoadMiscSettings();
public:		// User declarations
	__fastcall TfrmSettings(TComponent* Owner);
	void AdvSettingsFromFormToVars();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmSettings *frmSettings;
#define ANALYZE_W3_BIN 0
#define ANALYZE_IMPLIST 1
#define ANALYZE_JSCRIPT 2
#define ANALYZE_MDX 3
#define ANALYZE_TXT 4
#define ANALYZE_SLK 5
#define ANALYZE_LISTFILE 6
extern bool bAnalyze[7];
extern bool bDontUseInternalListfile;
extern bool bMapRename;
extern TMPQCompression nCompression;
//---------------------------------------------------------------------------
#endif
