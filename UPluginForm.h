//---------------------------------------------------------------------------

#ifndef UPluginFormH
#define UPluginFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include "UMainForm.h"
#include "UPluginFramework.h"
//---------------------------------------------------------------------------
class TfrmPlugins : public TForm
{
__published:	// IDE-managed Components
	TCheckListBox *chklstPlugins;
	TLabel *lblPlgInfo;
	TButton *btnSettings;
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall chklstPluginsClickCheck(TObject *Sender);
	void __fastcall chklstPluginsClick(TObject *Sender);
	void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall btnSettingsClick(TObject *Sender);
private:	// User declarations
	void Save();
	void Load();
public:		// User declarations
	__fastcall TfrmPlugins(TComponent* Owner);
	bool IsPluginsPresent();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPlugins *frmPlugins;
const UnicodeString PluginsDir =  GetAppDataDir() + "MPQ Repacker\\" + "Plugins\\";
void UnpackSamplePlugin();
void __strcpy(char* szDest, UnicodeString Source);
//---------------------------------------------------------------------------
enum TMPQRepackerStatus {mrsIdle = 0, mrsRePacking = 1, mrsBatchRePacking = 2, mrsClosing = 3};
extern TMPQRepackerStatus __Status;
enum TMPQCompression {mcHUFFMAN = 0, mcZLIB = 1, mcPKWARE = 2, mcLZMA = 3, mcBZIP2 = 4, mcSPARSE = 5};
enum TMPQRepackerOption {bUseXdep, bReplaceW3I, bReplaceDOO, bInsertCheats, bPause, bDeleteTemp, bUnpackUnknown, bW3iWtsMapRename, bNotUseInternalListfile};
//---------------------------------------------------------------------------
#endif
