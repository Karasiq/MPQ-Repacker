//---------------------------------------------------------------------------

#ifndef UMainFormH
#define UMainFormH
#pragma once
//---------------------------------------------------------------------------
#include <vcl.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "JvExMask.hpp"
#include "JvToolEdit.hpp"
#include <Mask.hpp>
#include <ExtCtrls.hpp>
#include "ZipForge.hpp"
#include "UPluginForm.h"
#include <ImgList.hpp>
//---------------------------------------------------------------------------
const int nVersion = 12598;
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TMemo *mmoLog;
	TJvFilenameEdit *edtSourceMap;
	TLabel *lblSourceMap;
	TJvFilenameEdit *edtDestMap;
	TLabel *lblDestMap;
	TGroupBox *grpOptions;
	TCheckBox *chkUseXdep;
	TLabel *lblTempDir;
	TCheckBox *chkManualEdit;
	TJvDirectoryEdit *edtTempDir;
	TCheckBox *chkDeleteTemp;
	TJvFilenameEdit *edtExtFileList;
	TLabel *lblExtFileList;
	TButton *btnRePack;
	TCheckBox *chkReplaceW3I;
	TCheckBox *chkExtractUnknownFiles;
	TGroupBox *grpCheats;
	TComboBox *cbbCheatPack;
	TEdit *edtActivator;
	TLabel *lblActivator;
	TButton *btnCommands;
	TCheckBox *chkInsertCheats;
	TCheckBox *chkReplaceDOO;
	TButton *btnPlugins;
	TZipForge *zpfrgPacker;
	TButton *btnSettings;
	TButton *btnBatchRepack;
	TButton *btnTestMap;
	TImageList *imglist;
	void __fastcall mmoLogDblClick(TObject *Sender);
	void __fastcall btnRePackClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnCommandsClick(TObject *Sender);
	void __fastcall btnPluginsClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall edtActivatorDblClick(TObject *Sender);
	void __fastcall edtSourceMapChange(TObject *Sender);
	void __fastcall FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall btnSettingsClick(TObject *Sender);
	void __fastcall btnBatchRepackClick(TObject *Sender);
	void __fastcall btnTestMapClick(TObject *Sender);
private:	// User declarations
	void FindCheatPacks();
	void SaveSettings();
	void LoadSettings();
public:		// User declarations
	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
extern UnicodeString __SourceMap;
extern UnicodeString __DestMap;
const UnicodeString AppPath = ExtractFilePath(Application->ExeName);
const UnicodeString AppDataPath = GetAppDataDir() + "MPQ Repacker\\";
const UnicodeString IniPath = AppDataPath + "Settings.ini";
//---------------------------------------------------------------------------
bool RePackMap(const UnicodeString SourceMap, const UnicodeString DestMap, const bool bInsertCheats = false, const UnicodeString CheatPack = "", const UnicodeString Activator = "-MPQRepacker", const bool bEmptyDOO = false, const bool bUnpackUnknown = false, const bool bPatchW3I = true, const bool bManualEdit = false, const bool bUseXDep = true, const bool bDeleteTempDir = true, const UnicodeString ListFile = AppPath + "listfile.txt", UnicodeString TempDir = AppPath + "Temp\\");
static int ExtractFile(UnicodeString Archive, UnicodeString ArcFile, UnicodeString SavedFile);
static int ExtractFile(HANDLE hMpq, UnicodeString ArcFile, UnicodeString SavedFile);
bool AnalyzeMainScript(TStrings* List, const UnicodeString Archive);
void RemoveDoubles(TStrings* S);
void GenerateWar3mapIMP(TStrings* FileList, const UnicodeString Archive, const UnicodeString Dir = AppPath + "Temp\\");
inline void MergeLists(TStrings* Source, TStrings* Dest);
inline bool IsImportFile(const UnicodeString File);
bool VerifyFile(UnicodeString Archive, UnicodeString File);
bool BuildFileList(const UnicodeString Dir, TStrings* List);
void RetrieveKnownFiles(TStrings* List, UnicodeString Archive);
unsigned int GetFileCount(UnicodeString Archive);
bool ExtractUnknownFiles(UnicodeString Archive, TStrings* List, const UnicodeString Dir, bool bWriteLog = true);
bool AnalyzeBinaryFile(TStrings* List, const UnicodeString Archive, const UnicodeString File, bool bLocalFile = false);
bool ExtractAllFiles(const UnicodeString Archive, bool bRewriteW3I = true, bool bRewriteDOO = false, bool bUnpackUnknown = false, const UnicodeString Dir = AppPath + "Temp\\", const UnicodeString Listfile = "filelist.txt", bool bWriteLog = true);
bool CreateMapWithXDep(const UnicodeString CreatedMap, const UnicodeString DirWithFiles = AppPath + "Temp\\", bool bWriteLog = true);
bool AnalyzeTextFile(TStrings* List, const UnicodeString Archive, const UnicodeString File);
void AutoSearch(TStrings* List, const UnicodeString Archive);
bool MPQIsValid(UnicodeString Archive, bool bWriteLog = true);
//---------------------------------------------------------------------------
typedef HANDLE MPQHANDLE;
typedef MPQHANDLE (WINAPI *P_MpqOpenArchiveForUpdate)(LPCSTR lpFileName, DWORD dwFlags, DWORD dwMaximumFilesInArchive);
typedef DWORD (WINAPI *P_MpqCloseUpdatedArchive)(MPQHANDLE hMPQ, DWORD dwUnknown2);
typedef BOOL (WINAPI *P_MpqAddFileToArchiveEx)(MPQHANDLE hMPQ, LPCSTR lpSourceFileName, LPCSTR lpDestFileName, DWORD dwFlags, DWORD dwCompressionType, DWORD dwCompressLevel);
// MpqOpenArchiveForUpdate flags
#define MOAU_CREATE_NEW        0x00
#define MOAU_CREATE_ALWAYS     0x08 //Was wrongly named MOAU_CREATE_NEW
#define MOAU_OPEN_EXISTING     0x04
#define MOAU_OPEN_ALWAYS       0x20
#define MOAU_READ_ONLY         0x10 //Must be used with MOAU_OPEN_EXISTING
#define MOAU_MAINTAIN_LISTFILE 0x01

// MpqAddFileToArchive flags
#define MAFA_EXISTS           0x80000000 //Will be added if not present
#define MAFA_UNKNOWN40000000  0x40000000
#define MAFA_MODCRYPTKEY      0x00020000
#define MAFA_ENCRYPT          0x00010000
#define MAFA_COMPRESS         0x00000200
#define MAFA_COMPRESS2        0x00000100
#define MAFA_REPLACE_EXISTING 0x00000001

// MpqAddFileToArchiveEx compression flags
#define MAFA_COMPRESS_STANDARD 0x08 //Standard PKWare DCL compression
#define MAFA_COMPRESS_DEFLATE  0x02 //ZLib's deflate compression
#define MAFA_COMPRESS_WAVE     0x81 //Standard wave compression
#define MAFA_COMPRESS_WAVE2    0x41 //Unused wave compression

// Flags for individual compression types used for wave compression
#define MAFA_COMPRESS_WAVECOMP1 0x80 //Main compressor for standard wave compression
#define	MAFA_COMPRESS_WAVECOMP2 0x40 //Main compressor for unused wave compression
#define MAFA_COMPRESS_WAVECOMP3 0x01 //Secondary compressor for wave compression

// ZLib deflate compression level constants (used with MpqAddFileToArchiveEx and MpqAddFileFromBufferEx)
#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
//---------------------------------------------------------------------------
#endif
