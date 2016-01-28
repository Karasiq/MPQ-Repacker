//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UMainForm.h"
#include "USettingsForm.h"
#include "StormLib.h"
#include "URecursiveSearch.hpp"
#include "IniFiles.hpp"
#include "UFileLock.h"
#include "UBinaryFile.h"
#define WriteLog MainForm->mmoLog->Lines->Add
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "JvExMask"
#pragma link "JvToolEdit"
#pragma link "ZipForge"
#pragma resource "*.dfm"
TMainForm *MainForm;
//#define FileNameIsValid(x) (__FileNameIsValid(x) || IsImportFile(x.LowerCase()))
//#define FileNameIsValid __FileNameIsValid
//---------------------------------------------------------------------------
unsigned int nLastUnpackedFilesCount = 0;
// TLockedFile* IniLock = NULL;
bool bDebugSaveNames = false;
bool bFullAnalyze = false;
const DWORD nDefaultMapNameSize = 200;
UnicodeString __SourceMap = "", __DestMap = "";
TMPQRepackerStatus __Status = mrsIdle;
//---------------------------------------------------------------------------
inline DWORD CompIndexToCompressionFlag(TMPQCompression c)
{
	switch(c)
	{
		case mcHUFFMAN:
			return MPQ_COMPRESSION_HUFFMANN;
		case mcZLIB:
			return MPQ_COMPRESSION_ZLIB;
		case mcPKWARE:
			return MPQ_COMPRESSION_PKWARE;
		case mcBZIP2:
			return MPQ_COMPRESSION_BZIP2;
		case mcSPARSE:
			return MPQ_COMPRESSION_SPARSE;
		case mcLZMA:
			return MPQ_COMPRESSION_LZMA;
		default:
			return NULL;
    }
}
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
	//IniLock = new TLockedFile(IniPath);
	//IniLock->Lock();
	frmSettings = new TfrmSettings(this);
	mmoLog->Clear();
	zpfrgPacker->BaseDir = AppPath;
	edtTempDir->Text = GetTempDir() + "MPQRepackerTemp\\";
	FindCheatPacks();
	LoadSettings();
}
bool StreamToStrings(TStream* From, TStrings* To, bool Clear = false)
{
	try
	{
		char* szBuffer = (char*)malloc(From->Size);
		From->Position = 0;
		From->Read(szBuffer, From->Size);
		if(Clear) To->Clear();
		To->Text = To->Text + UnicodeString(szBuffer);
		free(szBuffer);
		return true;
	}
	catch(...){return false;}
}
void RemoveDoubles(TStrings* S)
{
	return; // Не выполнять функцию
	TStrings* __S = new TStringList();
	for(int i = 0; i < S->Count-1; i++)
		if(!StrInList(__S, S->Strings[i])) __S->Add(S->Strings[i]);
	S->Clear();
	S->Text = __S->Text;
	VCL_FREE(__S);
}
DWORD ReadFileHeader(const UnicodeString File)
{
	DWORD dw = 0;
	Classes::TFileStream* fs = new Classes::TFileStream(File, fmOpenRead);
	fs->Read(&dw, 4);
	VCL_FREE(fs);
	return dw;
}
void AnalyzeAllUnknown(TStrings* List, const UnicodeString Archive)
{
	const UnicodeString TempDir = GetTempDir() + "MPQRepacker\\";
	__MkDir(TempDir);
	TStrings* __s = new TStringList(), *s = new TStringList();
	ExtractUnknownFiles(Archive, __s, TempDir, false);
	for(int i = 0; i < __s->Count; i++)
	{
		Application->ProcessMessages();
		if(FileExists(TempDir + __s->Strings[i]))
		{
//			WriteLog("Анализ " + __s->Strings[i] + "...");
			bool bResult = false;
			const UnicodeString File = TempDir + __s->Strings[i];
			const UnicodeString Ext = ExtractFileExt(File);
			if(Ext == ".slk" || Ext == ".blp") continue;
			if(Ext == ".mdx" || Ext == ".xxx" || __s->Strings[i].Pos("war3map")) bResult = AnalyzeBinaryFile(s, Archive, File, true);
			if(Ext == ".txt") bResult = AnalyzeTextFile(s, Archive, File);
			if(bResult)
			{
				WriteLog("Анализ " + __s->Strings[i] + "...");
				AutoSearch(s, Archive);
				MergeLists(s, List);
				if(FileExists(File)) DeleteFile(File);
			}
			s->Clear();
		}
	}
	//AutoSearch(s, Archive);

	s->Free();
	__s->Free();
}
void AnalyzeAllModels(TStrings* FileList, const UnicodeString Archive) // Анализирует все файлы MDX и MDL (в них можно найти имена файлов текстур)
{
	TStrings* s = new TStringList(), *__s = new TStringList();
	for(int i = 0; i < FileList->Count; i++)
	{
		Application->ProcessMessages();
		const UnicodeString Ext = ExtractFileExt(FileList->Strings[i]);
		if(!StrInList(__s, FileList->Strings[i]) && (Ext == ".mdx" || Ext == ".mdl") && VerifyFile(Archive, FileList->Strings[i]))
		{
			WriteLog("Анализ " + FileList->Strings[i] + "...");
			AnalyzeBinaryFile(s, Archive, FileList->Strings[i]);
			__s->Add(FileList->Strings[i]); // Список проанализированных файлов
		}
	}
	MergeLists(s, FileList);
	__s->Free();
	s->Free();
}
void AnalyzeAllTextFiles(TStrings* FileList, const UnicodeString Archive) // Анализирует все файлы TXT
{
	TStrings* s = new TStringList(), *__s = new TStringList();
	for(int i = 0; i < FileList->Count; i++)
	{
		Application->ProcessMessages();
		const UnicodeString Ext = ExtractFileExt(FileList->Strings[i]);
		if(!StrInList(__s, FileList->Strings[i]) && (Ext == ".txt"))
		{
			WriteLog("Анализ " + FileList->Strings[i] + "...");
			AnalyzeTextFile(s, Archive, FileList->Strings[i]);
			__s->Add(FileList->Strings[i]); // Список проанализированных файлов
		}
	}
	MergeLists(s, FileList);
	__s->Free();
	s->Free();
}
bool AnalyzeAllSLK(TStrings* List, const UnicodeString Archive)
{
	try
	{
		const UnicodeString Path = GetTempDir();
		TStrings* s = new TStringList(), *analyzed = new TStringList();
		const UnicodeString ScriptFile = Path + "analyze_tmp.slk";
		LOOP(List->Count)
		{
			if(ExtractFileExt(List->Strings[i].LowerCase()) != ".slk") continue;
			if(StrInList(analyzed, List->Strings[i])) continue;
			if(ExtractFile(Archive, List->Strings[i], ScriptFile) != ERROR_SUCCESS) continue;
			WriteLog("Анализ " + ExtractFileName(List->Strings[i]) + "...");
			if(!FileExists(ScriptFile))
			{
				s->Free();
				return false;
			}
			else s->LoadFromFile(ScriptFile);

			UnicodeString ScriptText = s->Text;
			s->Clear();

			// Ищем все строковые константы (хуйню в двойных кавычках):
			int Pos1 = PosEx("\"", ScriptText, 1), Pos2 = PosEx("\"", ScriptText, Pos1 + 1);
			while((Pos1 && Pos2) && (Pos1 < ScriptText.Length() && Pos2 < ScriptText.Length()))
			{
				try
				{
					if(ScriptText[Pos1 + 1] != '"')
					{
						const int nStrSize = (Pos2 - Pos1 - 1);
						const UnicodeString Str = ReplaceStr(ScriptText.SubString(Pos1 + 1, nStrSize), "\\\\", "\\");

						if(FileNameIsValid(Str)) s->Add(Str);
					}

					Pos1 = PosEx("\"", ScriptText, Pos2 + 1);
					Pos2 = PosEx("\"", ScriptText, Pos1 + 1);
				}
				catch(...){break;}
			}
			analyzed->Add(List->Strings[i]);
			if(bDebugSaveNames) s->SaveToFile(GetTempDir() + "MPQRepacker\\" + ExtractFileName(List->Strings[i]) + ".txt"); // Отладка
			AutoSearch(s, Archive);
			MergeLists(s, List);
			DeleteFile(ScriptFile);
		}
		VCL_FREE(s);
		VCL_FREE(analyzed);
		return true;
	}
	catch(...){return false;}
}
void AutoSearch(TStrings* List, const UnicodeString Archive)
{
	TStrings* s = new TStringList();
	for(int i = 0; i < List->Count; i++)
	{
		// Уголок копипаста
		Application->ProcessMessages();
		if(List->Strings[i].Pos("BTN")) // Иконки кнопок
		{
			UnicodeString ReplacedStr = " ";
			//----------------------------------------------------
			ReplacedStr = ReplaceStr(List->Strings[i], "BTN", "DISBTN");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
			ReplacedStr = ReplaceStr(List->Strings[i], "CommandButtons\\BTN", "CommandButtonsDisabled\\DISBTN");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
			ReplacedStr = ReplaceStr(List->Strings[i], "CommandButtons\\BTN", "PassiveButtons\\PASBTN");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
			ReplacedStr = ReplaceStr(List->Strings[i], "CommandButtonsDisabled\\DISBTN", "CommandButtons\\BTN");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
			ReplacedStr = ReplaceStr(List->Strings[i], "PassiveButtons\\PASBTN", "CommandButtons\\BTN");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
			ReplacedStr = ReplaceStr(List->Strings[i], "CommandButtonsDisabled\\DISBTN", "PassiveButtons\\PASBTN");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
			ReplacedStr = ReplaceStr(List->Strings[i], "PassiveButtons\\PASBTN", "CommandButtonsDisabled\\DISBTN");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
		}
		if(ExtractFileExt(List->Strings[i]) == ".tga" || ExtractFileExt(List->Strings[i]) == ".blp" || ExtractFileExt(List->Strings[i]) == ".mdl" || ExtractFileExt(List->Strings[i]) == ".mdx")
		{
			UnicodeString ReplacedStr = " ";
            //----------------------------------------------------
			ReplacedStr = ChangeFileExt(List->Strings[i], ".mdx");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
            //----------------------------------------------------
			ReplacedStr = ChangeFileExt(List->Strings[i], ".mdl");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
			ReplacedStr = ChangeFileExt(List->Strings[i], ".blp");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
			ReplacedStr = ChangeFileExt(List->Strings[i], ".tga");
			if(!StrInList(s, ReplacedStr) && FileNameIsValid(ReplacedStr)) s->Add(ReplacedStr);
			//----------------------------------------------------
		}
		if(!StrInList(s, List->Strings[i]) && FileNameIsValid(List->Strings[i])) s->Add(List->Strings[i]);
	}
	RetrieveKnownFiles(s, Archive);
	List->Text = s->Text;
	s->Free();
}
void InsertStrToBuffer(const UnicodeString Str, char* szBuffer, const unsigned int Offset)
{
	for(int i = 0; i < Str.Length(); i++)
	{
		szBuffer[i + Offset] = Str[i+1];
	}
}
inline bool IsImportFile(const UnicodeString File)
{
	const UnicodeString Ext = ExtractFileExt(File);
	if(Ext == ".mp3" || Ext == ".mdx" || Ext == ".mdl" || Ext == ".tga" || Ext == ".blp" || Ext == ".avi" || Ext == ".ogg") return true;
	else return false;
}
void GenerateWar3mapIMP(TStrings* FileList, const UnicodeString Archive, const UnicodeString Dir)
{
	__MkDir(Dir);
	const UnicodeString ImpPath = Dir + "war3map.imp";
	if(FileExists(ImpPath)) DeleteFile(ImpPath);
	TStrings* __s = new TStringList();
	Classes::TFileStream* fs = new Classes::TFileStream(ImpPath, fmCreate);
	BYTE szFirstLine[8] = {0x01, 0x00, 0x00, 0x00, 0x86, 0x00, 0x00, 0x00};
	fs->Write(szFirstLine, 8);
	for(int i = 0; i < FileList->Count; i++)
	{
		Application->ProcessMessages();
		if(FileList->Strings[i].Length()
			&& IsImportFile(FileList->Strings[i])
			&& !StrInList(__s, FileList->Strings[i])
			&& VerifyFile(Archive, FileList->Strings[i]))
		{
			const unsigned int nBufferSize = FileList->Strings[i].Length() + 2;
			BYTE* szBuffer = (BYTE*)malloc(nBufferSize);
			szBuffer[0] = 0x0D;
			InsertStrToBuffer(FileList->Strings[i], (char*)szBuffer, 1);
			szBuffer[nBufferSize - 1] = 0x00;
			fs->Write(szBuffer, nBufferSize);
			free(szBuffer);
			__s->Add(FileList->Strings[i]);
		}
	}
	fs->Position = 0;
	const int nVersion = 0x01;
	const int nCount = __s->Count;
	fs->Write(&nVersion, 4);
	fs->Write(&nCount, 4);
	fs->Free();
	__s->Free();
}
unsigned int GetFileCount(UnicodeString Archive)
{
	char * szArchiveName = Archive.t_str();
	HANDLE hMpq   = NULL;          // Open archive handle
	int    nError = ERROR_SUCCESS; // Error value
	unsigned int nFileCount = 0; // Result

	// Open an archive, e.g. "d2music.mpq"
	if(nError == ERROR_SUCCESS)
	{
		if(!SFileOpenArchive(szArchiveName, 0, 0, &hMpq))
			nError = GetLastError();
	}
	if(nError == ERROR_SUCCESS)
	{
		DWORD pcbLengthNeeded = sizeof(unsigned int);
		if(!SFileGetFileInfo(hMpq, SFILE_INFO_NUM_FILES, &nFileCount, sizeof(unsigned int), &pcbLengthNeeded))
        	nError = GetLastError();
    }

	if(hMpq != NULL)
		SFileCloseArchive(hMpq);

	return nFileCount;
}
unsigned int GetFileCount(HANDLE hMPQ)
{
	unsigned int nFileCount = 0; // Result
	DWORD pcbLengthNeeded = sizeof(unsigned int);
	if(!SFileGetFileInfo(hMPQ, SFILE_INFO_NUM_FILES, &nFileCount, sizeof(unsigned int), &pcbLengthNeeded))
		return 0;
	else return nFileCount;
}
inline double Ratio(double _100Proc, double _xProc)
{
	if (_100Proc == 0) return 0;
	try{return (_xProc / _100Proc) * 100;}
	catch(Exception &e)
	{
		WriteLog(e.Message);
		return 0;
	}
}
UnicodeString GetUnknownFileExt(HANDLE hFile)
{
	//-----------------------------------------------------------
	const BYTE szBLPSignature[] = "BLP1\0";
	const BYTE szMDXSignature[] = "MDLXV";
	const BYTE szMP3Signature[] = {0xFF, 0xFB, 0x38, 0xC4, 0x00};
	//-----------------------------------------------------------
	BYTE szFirstBytes[5];
	DWORD nReaded = 0;
	SFileReadFile(hFile, szFirstBytes, 5, &nReaded, NULL);
//	Msg(IntToStr((int)nReaded));
	UnicodeString Result = ".xxx";
	if(strncmp(szFirstBytes, szBLPSignature, 5) == 0) Result = ".blp";
	if(strncmp(szFirstBytes, szMDXSignature, 5) == 0) Result = ".mdx";
	if(strncmp(szFirstBytes, szMP3Signature, 5) == 0) Result = ".mp3";
	return Result;
}
UnicodeString GetFileName(HANDLE hFile)
{
	UnicodeString Result = "";
	char __szBuffer[MAX_PATH];
	char* szBuffer = __szBuffer;
	try
	{
		SFileGetFileName(hFile, szBuffer);
		Result = __SubString(szBuffer, 0, strlen(szBuffer));
	}
	catch(...){}
	return Result;
}
DWORD GetFileSize(HANDLE hFile)
{
	try
	{
		DWORD nSize = SFileGetFileSize(hFile, NULL);
		return nSize;
	}
	catch(...){return 0;}
}
bool AddFilelistToMPQ(TStrings* List, HANDLE hMPQ)
{
	try
	{
		if(hMPQ == NULL || List == NULL) return false;
		const UnicodeString TempList = GetTempDir() + "~FLIST.tmp";
		List->SaveToFile(TempList);
		SFileAddListFile(hMPQ, UnicodeString(TempList).t_str());
		if(FileExists(TempList)) DeleteFile(TempList);
		return true;
	}
	catch(...) {return false;}
}
void InsertListToList(TStrings* Source, TStrings* Dest, unsigned int nInsertIndex)
{
	for(int i = 0; i < Source->Count; i++)
	{
		Dest->Insert(nInsertIndex + i, Source->Strings[i]);
	}
}
inline UnicodeString AddQuotes(const UnicodeString Source)
{
	return "\"" + Source + "\"";
}
#ifndef ADVANCED_VCL_H
#define DeleteTransfers RemoveTransfers
inline UnicodeString RemoveTransfers(const UnicodeString Source)
{
	UnicodeString Result = Source;
	Result = ReplaceStr(Result, "\r", "");
	Result = ReplaceStr(Result, "\n", "");
	Result = ReplaceStr(Result, "\r\n", "");
	return Result.Trim();
}
#endif

bool InsertCheats(const UnicodeString War3mapJ, const UnicodeString CP, UnicodeString Activator)
{
	try
	{
		const UnicodeString CPPath = AppDataPath + "CheatPacks\\" + CP + "\\";
		if(!DirectoryExists(CPPath))  return false;
		if(!FileExists(War3mapJ)) return false;

		TStrings* s = new TStringList(), *__s = new TStringList();
		const UnicodeString DefaultActivator = RemoveTransfers(ReadFile(CPPath + "activator.txt"));
		Activator = AddQuotes(Activator);

		//s->LoadFromFile(War3mapJ);
		s->Text = UTF8ReadFile(War3mapJ);

		int nInsertIndex = FindStr(s, "globals") + 1;
		__s->LoadFromFile(CPPath + "globals.txt");
		InsertListToList(__s, s, nInsertIndex);

		nInsertIndex = FindStr(s, "function main takes nothing returns nothing") + 1;
		while(s->Strings[nInsertIndex].SubString(1,5) == "local") nInsertIndex++; // После локалов
		__s->LoadFromFile(CPPath + "function main.txt");
		InsertListToList(__s, s, nInsertIndex);

		nInsertIndex = FindStr(s, "endglobals") + 1;
		__s->LoadFromFile(CPPath + "endglobals.txt");
		InsertListToList(__s, s, nInsertIndex);

		s->Text = ReplaceStr(s->Text, DefaultActivator, Activator); \
		char* szBuffer = UTF8String(s->Text).c_str();

		UTF8WriteFile(War3mapJ, s->Text);
		// s->SaveToFile(War3mapJ);

		if(FileExists(CPPath + "strings.txt")) // GUI cheat pack
		{
			const UnicodeString War3mapWTS = ExtractFilePath(War3mapJ) + "war3map.wts";
			s->Clear();
			__s->LoadFromFile(CPPath + "strings.txt");
			if(FileExists(War3mapWTS)) s->Text = UTF8ReadFile(War3mapWTS);
			MergeLists(__s, s);
			UTF8WriteFile(War3mapWTS, s->Text);
			// s->SaveToFile(War3mapWTS);
		}

		s->Free();
		__s->Free();
		return true;
	}
	catch(Exception &e)
	{
		Msg("Ошибка в функции InsertCheats: " + e.Message, true);
		return false;
    }
}
BOOL AddFileToMPQWithSFMPQ(const UnicodeString Archive, const UnicodeString File, const UnicodeString ArcFile, const unsigned nMaxFiles)
{
	try
	{
		const UnicodeString sfmpq_dll = GetTempDir() + "sfmpq_api.dll";
		if(!ResUnpack("SFMPQ_dll_internal", sfmpq_dll, 0)) return FALSE;
		LoadDLL(hDll, sfmpq_dll);
		P_MpqOpenArchiveForUpdate __open = (P_MpqOpenArchiveForUpdate)GetProcAddress(hDll, "MpqOpenArchiveForUpdate");
		P_MpqCloseUpdatedArchive __close = (P_MpqCloseUpdatedArchive)GetProcAddress(hDll, "MpqCloseUpdatedArchive");
		P_MpqAddFileToArchiveEx __add = (P_MpqAddFileToArchiveEx)GetProcAddress(hDll, "MpqAddFileToArchiveEx");
		MPQHANDLE hMpq = (*__open)(BUF(Archive), MOAU_OPEN_EXISTING, nMaxFiles);
		BOOL bResult = (*__add)(hMpq, BUF(File), BUF(ArcFile), MAFA_ENCRYPT + MAFA_COMPRESS + MAFA_REPLACE_EXISTING, MAFA_COMPRESS_DEFLATE, Z_BEST_COMPRESSION);
		if(HANDLE_VALID(hMpq)) (*__close)(hMpq, 0);
		// Debug:
		if(bResult) WriteLog("Файл " + ArcFile + " упакован в архив с помощью SFMPQ!");
		else WriteLog("Ошибка упаковки файла с помощью SFMPQ.");
		//--------
		FreeDLL(hDll);
		return bResult;
	}
	catch(...)
	{
        return FALSE;
    }
}
void TMainForm::FindCheatPacks()
{
	const UnicodeString Path = AppDataPath + "CheatPacks\\";
	if(!DirectoryExists(Path))
	{
		__MkDir(Path);
		const UnicodeString Archive = GetTempDir() + "CheatPacks.zip";
		if(!ResUnpack("CheatPacks", Archive))
		{
			RemoveDir(Path);
			return;
        }
		zpfrgPacker->BaseDir = Path;
		zpfrgPacker->FileName = Archive;
		zpfrgPacker->OpenArchive(fmOpenRead);
		zpfrgPacker->ExtractFiles("*.txt", faAnyFile, "");
		zpfrgPacker->CloseArchive();
	}
	cbbCheatPack->Clear();
	TSearchRec s_rec;
	int r = FindFirst(Path + "*", faDirectory, s_rec);
	while (r == 0)
	{
		if(s_rec.Name != "." && s_rec.Name != "..") cbbCheatPack->Items->Add(s_rec.Name);
		r = FindNext(s_rec);
	}
	if(cbbCheatPack->Items->Count < 1)
	{
		grpCheats->Enabled = false;
		cbbCheatPack->Text = "Читы не найдены";
		chkInsertCheats->Checked = false;
		chkInsertCheats->Enabled = false;
    }
	else
	{
		grpCheats->Enabled = true;
	}
	cbbCheatPack->ItemIndex = 0;
}
bool ExtractUnknownFiles(UnicodeString Archive, TStrings* List, const UnicodeString Dir, bool bWriteLog)
{
	char * szArchiveName = Archive.t_str();
	HANDLE hMpq   = NULL;          // Open archive handle
	int    nError = ERROR_SUCCESS; // Result value
	bool bResult = true;

	// Open an archive, e.g. "d2music.mpq"
	if(nError == ERROR_SUCCESS)
	{
		bResult = SFileOpenArchive(szArchiveName, 0, 0, &hMpq);
		if(!bResult)
			nError = GetLastError();
	}
	if(nError == ERROR_SUCCESS)
	{
		AddFilelistToMPQ(List, hMpq);

		TFileEntry* FEntry = ((TMPQArchive*)hMpq)->pFileTable;

		unsigned int nFileCount = GetFileCount(hMpq);
		for(int i = 0; i < nFileCount; i++)
		{
			if(FEntry[i].szFileName == NULL) // Имя файла не определено
			{
				HANDLE hFile = NULL;
				SFileOpenFileEx(hMpq, (const char*)FEntry[i].dwHashIndex, SFILE_OPEN_FROM_MPQ | SFILE_OPEN_BY_INDEX, &hFile);
				if(hFile == INVALID_HANDLE_VALUE) continue;

				const UnicodeString NewFilename = GetFileName(hFile);
				if(FileNameIsValid(NewFilename) && !StrInList(List, NewFilename) && GetFileSize(hFile))
				{
					const char* szFileName = (Dir + NewFilename).t_str();
					HANDLE hLocalFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
					char  szBuffer[0x10000];
					DWORD dwBytes = 1;
					while(dwBytes > 0)
					{
						SFileReadFile(hFile, szBuffer, sizeof(szBuffer), &dwBytes, NULL);
						if(dwBytes > 0)
							WriteFile(hLocalFile, szBuffer, dwBytes, &dwBytes, NULL);
					}
					if(hLocalFile != NULL)
						CloseHandle(hLocalFile);
					if(FileExists(szFileName) && GetFileSize(szFileName) == 0) DeleteFile(szFileName);
					else
					{
						if(bWriteLog) WriteLog("Распаковка " + NewFilename + "...");
						nLastUnpackedFilesCount++;
					}
					List->Add(NewFilename);
				}
				if(hFile != NULL)
					SFileCloseFile(hFile);
			}
		}
		bResult = true;
	}
	if(hMpq != NULL)
		SFileCloseArchive(hMpq);
	return bResult;
}
void RetrieveKnownFiles(TStrings* List, UnicodeString Archive)
{
	char * szArchiveName = Archive.t_str();
	HANDLE hMpq   = NULL;          // Open archive handle
	int    nError = ERROR_SUCCESS; // Result value

	// Open an archive, e.g. "d2music.mpq"
	if(nError == ERROR_SUCCESS)
	{
		if(!SFileOpenArchive(szArchiveName, 0, 0, &hMpq))
			nError = GetLastError();
	}
	if(nError == ERROR_SUCCESS)
	{
		AddFilelistToMPQ(List, hMpq);
		List->Clear();

		TFileEntry* FEntry = ((TMPQArchive*)hMpq)->pFileTable;
		unsigned int nFileCount = GetFileCount(hMpq);

		for(int i = 0; i < nFileCount; i++)
		{
			if(FEntry[i].szFileName != NULL) List->Add(FEntry[i].szFileName);
		}
	}
	if(hMpq != NULL)
		SFileCloseArchive(hMpq);
}
inline void WriteChars(Classes::TFileStream* fs, BYTE __BYTE, const unsigned int nCount)
{
	BYTE szBuffer[] = {__BYTE};
	for(int i = 0; i < nCount; i++) fs->Write(szBuffer, 1);
}
inline void __Insert(BYTE* szSource, BYTE* szDest, unsigned int nCount, unsigned int nOffset)
{
	for(int i = 0; i < nCount; i++)
	{
        szDest[i + nOffset] = szSource[i];
    }
}
void WriteWar3Header(const UnicodeString File)
{
	Classes::TFileStream* fs = new Classes::TFileStream(File, fmOpenReadWrite);
	fs->Position = 0;
	BYTE szStartBuffer[] = {0x48, 0x4D, 0x33, 0x57, 0x00, 0x00, 0x00, 0x00};
	BYTE* szMediumBuffer = "MPQ RePacker";
	BYTE szEndBuffer[] = {0x00, 0x10, 0xDC};
	const unsigned int nMediumBufferSize = strlen(szMediumBuffer);
	const unsigned int nHeaderSize = 8 + 3 + nMediumBufferSize;
	const unsigned int nOldFileSize = fs->Size;
	BYTE* szOldFile = (BYTE*)malloc(nOldFileSize);
	BYTE* szHeader = (BYTE*)malloc(nHeaderSize);
	fs->Read(szOldFile, nOldFileSize);
	__Insert(szStartBuffer, szHeader, 8, 0);
	__Insert(szMediumBuffer, szHeader, nMediumBufferSize, 8);
	__Insert(szEndBuffer, szHeader, 3, nMediumBufferSize + 8);
	fs->Position = 0;
	fs->Size = 0;
	fs->Write(szHeader, nHeaderSize);
	WriteChars(fs, 0x00, 2);
	WriteChars(fs, 0x01, 1);
	WriteChars(fs, 0x00, 472);
	fs->Write(szOldFile, nOldFileSize);
	fs->Free();
	free(szHeader);
	free(szOldFile);
}
bool CreateMPQ(UnicodeString MPQPath, HANDLE* hMPQ, DWORD nMaxFileCount = 500)
{
	try
	{
		char* szMPQPath = MPQPath.t_str();
		bool bResult = false;
		ResUnpack("DummyMap", MPQPath);
		bResult = SFileOpenArchive(szMPQPath, 0, 0, hMPQ);
//		bResult = SFileCreateArchive(szMPQPath, MPQ_CREATE_ARCHIVE_V1 & MPQ_CREATE_ATTRIBUTES, nMaxFileCount, hMPQ);
		return bResult;
	}
	catch(...){return false;}
}
void ChooseRule(const UnicodeString File, DWORD* Flags, DWORD* Compression)
{
	const UnicodeString Ext = ExtractFileExt(File);
	if(Ext == ".wav")
	{
		*Flags = MPQ_FILE_COMPRESS + MPQ_FILE_ENCRYPTED;
		*Compression = MPQ_COMPRESSION_HUFFMANN + MPQ_COMPRESSION_ADPCM_STEREO;
		return;
	}
	if(Ext == ".blp" || Ext == ".mpq" || Ext == ".w3m")
	{
		*Flags = 0;
		*Compression = 0;
		return;
	}
	if(Ext == ".mdx" || Ext == ".slk" || Ext == ".tga")
	{
		*Flags = MPQ_FILE_COMPRESS;
		*Compression = CompIndexToCompressionFlag(nCompression);
		return;
	}
	// Какой-то другой файл
	*Flags = MPQ_FILE_COMPRESS + MPQ_FILE_ENCRYPTED;
	*Compression = CompIndexToCompressionFlag(nCompression);
}
bool AddFileToMPQ(HANDLE hMPQ, UnicodeString File, UnicodeString ArcFile)
{
	try
	{
		DWORD dwFlags = 0, dwCompression = 0;
		ChooseRule(File, &dwFlags, &dwCompression);
		char *szFile = File.t_str(), *szArcFile = ArcFile.t_str();
		bool bResult = SFileAddFileEx(hMPQ, szFile, szArcFile, MPQ_FILE_REPLACEEXISTING + dwFlags, dwCompression);
		return bResult;
	}
	catch(Exception &e){WriteLog(e.Message); return false;}
}
void WriteWar3mapW3I(const UnicodeString Dir)
{
	const UnicodeString FullPath = Dir + "war3map.w3i";
	const BYTE War3mapW3I[] =
	{
        #include "war3map.w3i.inc"
	};
	if(FileExists(FullPath)) DeleteFile(FullPath);
	Classes::TFileStream* fs = new Classes::TFileStream(FullPath, fmCreate);
	fs->Write(War3mapW3I, sizeof(War3mapW3I));
	fs->Free();
	fs = NULL;
}
void WriteWar3mapUnitsDOO(const UnicodeString Dir)
{
	const UnicodeString FullPath = Dir + "war3mapUnits.doo";
	const BYTE War3mapUnitsDOO[] =
	{
        #include "war3mapUnits.doo.inc"
	};
	if(FileExists(FullPath)) return; // Не заменять
	else
	{
		Classes::TFileStream* fs = new Classes::TFileStream(FullPath, fmCreate);
		fs->Write(War3mapUnitsDOO, sizeof(War3mapUnitsDOO));
		fs->Free();
		fs = NULL;
	}
}
bool DeobfuscateJass(TStrings* JFile)
{
	for(unsigned int i = 0; i < JFile->Count; i++)
	{
//		UnicodeString First7 = JFile->Strings[i].SubString(1,7);
//		if(First7 != "functio" && First7 != "endfunc"
//			&& First7 != "globals" && First7 != "endglob")
//		{
//			if(JFile->Strings[i].Length() && JFile->Strings[i][1] != '\t') JFile->Strings[i] = "\t" + JFile->Strings[i];
//		}
		//elseJFile->Insert(i, ""); // Вставляем пустую строку
		if(JFile->Strings[i] == "function main takes nothing returns nothing") JFile->Strings[i] == "function main2 takes nothing returns nothing";
	}
	return true;
}
inline void MoveFile(UnicodeString From, UnicodeString To)
{
	if(FileExists(To)) DeleteFile(To);
    MoveFile((const char*)From.t_str(), (const char*)To.t_str());
}
bool CreateMapWithXDep(const UnicodeString CreatedMap, const UnicodeString DirWithFiles, bool bWriteLog)
{
	try
	{
		const UnicodeString Dir = GetTempDir() + "XDEP\\";
		const UnicodeString Exe = Dir + "xdep.exe";
		const UnicodeString Ini = Dir + "xdep.ini";
		const UnicodeString SfmpqExe = Dir + "sfmpq.exe";
		const UnicodeString SfmpqDll = Dir + "sfmpq.dll";
		const UnicodeString DummyMap = Dir + "TempMap.w3x";
		const UnicodeString ProcessedMap = Dir + "ProcessedMap.w3x";
		if(!DirectoryExists(Dir)) __MkDir(Dir);
		ResUnpack("xdepEXE", Exe);
		ResUnpack("xdepINI", Ini);
		ResUnpack("DummyMap", DummyMap);
		ResUnpack("sfmpqEXE", SfmpqExe);
		ResUnpack("sfmpqDLL", SfmpqDll);
		const UnicodeString TempDir = Dir + "TempMap.w3x.temp\\files\\";
		__MkDir(TempDir);
		TStrings* s = new TStringList();
		// BuildFileList(DirWithFiles, s);
		if(FileExists(DirWithFiles + "(listfile)")) s->LoadFromFile(DirWithFiles + "(listfile)");
		else BuildFileList(DirWithFiles, s);
		for(int i = 0; i < s->Count; i++)
		{
			UnicodeString From = DirWithFiles + s->Strings[i];
			UnicodeString To = TempDir + s->Strings[i];
			if(!DirectoryExists(ExtractFilePath(To))) __MkDir(ExtractFilePath(To));
			CopyFile(From.t_str(), To.t_str(), false);
		}
		const UnicodeString OldWorkingDir = GetCurrentDir();
		ChDir(Dir);
		if(bWriteLog) WriteLog("Упаковка файлов...");
		ExecuteProcess(Exe, "", INFINITE);
		Sleep(100);
		FORM_TO_FRONT(MainForm);
//		CaptureConsoleOutput(Exe, "", MainForm->mmoLog);
		ChDir(OldWorkingDir);
		DeleteFile(Exe);
		DeleteFile(Ini);
		DeleteFile(DummyMap);
		DeleteFile(SfmpqExe);
		DeleteFile(SfmpqDll);
		if(FileExists(ProcessedMap))
		{
			if(bWriteLog) WriteLog("Карта успешно собрана!");
			if(FileExists(CreatedMap)) DeleteFile(CreatedMap);
			MoveFile(ProcessedMap, CreatedMap);
			DeleteNonEmptyDir(Dir, MainForm->Handle);
			return true;
		}
		else
		{
			if(bWriteLog) WriteLog("Ошибка сборки карты.");
			DeleteNonEmptyDir(Dir, MainForm->Handle);
			return false;
        }
	}
	catch(...) {return false;}
}
void LocalizeFileList(TStrings* List, const UnicodeString Dir)
{
	for(int i = 0; i < List->Count; i++)
	{
		List->Strings[i] = ReplaceStr(List->Strings[i], Dir, "");
	}
}
bool BuildFileList(const UnicodeString Dir, TStrings* List)
{
	try
	{
		if(List == NULL) return false;
		List->Clear();
		FileFinder(Dir, "*", true, (TStringList*)List);
		LocalizeFileList(List, Dir);
		return true;
	}
	catch(...) {return false;}
}

bool PackFilesToMPQ(UnicodeString Archive, const UnicodeString Dir = AppPath + "Temp\\")
{
	if(ExtractFilePath(Archive).Length() < 3) Archive = AppPath + ExtractFileName(Archive);
	const char* szArchive = Archive.t_str();
	TStrings* s = new TStringList();
	HANDLE hMpq = NULL;
	int nError = ERROR_SUCCESS;

	if(FileExists(Dir + "(listfile)")) s->LoadFromFile(Dir + "(listfile)");
	else BuildFileList(Dir, s);

	const unsigned int nFileCount = s->Count;

	if(FileExists(Archive)) DeleteFile(Archive);
	bool bReturn = CreateMPQ(Archive, &hMpq, nFileCount);
	if(!bReturn) nError = GetLastError();
	if(hMpq == NULL) return false;

	WriteLog("Упаковка файлов в " + ExtractFileName(Archive));
	Sleep(100);
	HANDLE hTempFile = 0;
	try
	{
		SFileCreateFile(hMpq, "temp.txt", 0, 0, 0, MPQ_FILE_REPLACEEXISTING + MPQ_FILE_DELETE_MARKER, &hTempFile);
	}
	catch(...) {}
	if(HANDLE_VALID(hTempFile)) SFileFinishFile(hTempFile);
	for(int i = 0; i < s->Count; i++)
	{
		Application->ProcessMessages();
		if(s->Strings[i] == "(listfile)" || s->Strings[i] == "(attributes)" || s->Strings[i] == "(signature)")
			continue;

		if(s->Strings[i].LowerCase() == "scripts\\war3map.j")
         	s->Strings[i] = "war3map.j";

		if(AddFileToMPQ(hMpq, Dir + s->Strings[i], s->Strings[i]))
			WriteLog("Упаковка " + s->Strings[i] + "...");
		else
		{
			nError = GetLastError();
			WriteLog("Ошибка упаковки " + s->Strings[i] + " (Код ошибки: " + IntToStr(nError) + ")");
			if(hMpq != NULL)
			{
				SFileCloseArchive(hMpq);
            }
			AddFileToMPQWithSFMPQ(Archive, Dir + s->Strings[i], s->Strings[i], nFileCount);
			SFileOpenArchive(BUF(Archive), 0, 0, &hMpq);
			SFileUpdateFileAttributes(hMpq, BUF(s->Strings[i]));
			// 111111111111111111111111
        }
	}
	WriteLog("Упаковка завершена");
	AddFilelistToMPQ(s, hMpq);
	SFileCompactArchive(hMpq);
	SFileSetAttributes(hMpq, MPQ_ATTRIBUTE_ALL);
	if(hMpq != NULL) SFileCloseArchive(hMpq);
	VCL_FREE(s);
	return true;
}
void ReadInternalListfile(TStrings* List, const UnicodeString Archive)
{
	const UnicodeString TempFile = GetTempDir() + "(listfile)";
	if(!ExtractFile(Archive, LISTFILE_NAME, TempFile)) return;
	if(!FileExists(TempFile)) return;
	TStrings* s = new TStringList();
	s->LoadFromFile(TempFile);
	MergeLists(s, List);
	s->Free();
	if(FileExists(TempFile)) DeleteFile(TempFile);
}
bool MPQIsValid(UnicodeString Archive, bool bWriteLog)
{
	HANDLE hMpq = NULL;
	bool bResult = SFileOpenArchive(Archive.t_str(), 0, 0, &hMpq);
	if(bWriteLog && !bResult)
	{
		int nError = GetLastError();
		UnicodeString s = "";

		switch(nError)
		{
			case ERROR_FILE_CORRUPT:
				s = "Файл поврежден!";
				break;
			case ERROR_BAD_FORMAT:
				s = "Невозможно открыть файл как MPQ архив!";
				break;
			case ERROR_NOT_ENOUGH_MEMORY:
				s = "Не хватает памяти!";
				break;
			case ERROR_AVI_FILE:
				s = "AVI файл с расширением MPQ!";
				break;
			case ERROR_ACCESS_DENIED:
				s = "Отказано в доступе.";
				break;
			case ERROR_SHARING_VIOLATION:
			case ERROR_LOCK_VIOLATION:
				s = "Файл уже открыт в другой программе.";
				break;
			default:
				s = "Неизвестная ошибка! (Код ошибки: " + IntToStr(nError) + ")";
		}
		s = "Ошибка: " + s;
		WriteLog(s);
	}

	if(hMpq != NULL) SFileCloseArchive(hMpq);
	return bResult;
}
void LoadExternalListfile(TStrings* S, const UnicodeString File)
{
	if(!FileExists(File)) return;
	TStrings* _s = new TStringList();
	_s->LoadFromFile(File);
	MergeLists(_s, S);
	VCL_FREE(_s);
}
bool ExtractAllFiles(const UnicodeString Archive, bool bRewriteW3I, bool bRewriteDOO, bool bUnpackUnknown, const UnicodeString Dir, const UnicodeString Listfile, bool bWriteLog)
{
	try
	{
		if(!FileExists(Archive))
		{
			if(bWriteLog) WriteLog("Файл " + ExtractFileName(Archive) + " не найден!");
			return false;
		}
		TStrings* s = new TStringList(), *s1 = new TStringList();
		PluginManager->SendCodeToAll(CODE_FILE_SEARCH_START, pfcFileFindHooked);
		if(!PluginManager->Overrides.Search.Enabled)
		{
			const UnicodeString BinaryFiles[] = {/*"war3map.imp",*/ "war3map.w3a", "war3map.w3c", "war3map.w3d", "war3map.w3h", "war3map.w3g", "war3map.w3i", "war3map.w3p", "war3map.w3r", "war3map.w3s", "war3map.w3t", "war3map.w3u", "war3map.w3q"};
			if(!DirectoryExists(Dir)) CreateDir(Dir);
			SetHiddenAttribute(Dir, true);
			// if(FileExists(AppDataPath + "FILELIST_CACHE.txt")) LoadExternalListfile(s, AppDataPath + "FILELIST_CACHE.txt");
			// Ищем имена файлов
			if(bAnalyze[ANALYZE_LISTFILE])
			{
				ReadInternalListfile(s, Archive);
			}
			if(FileExists(Listfile))
			{
				LoadExternalListfile(s, Listfile);
				if(bWriteLog) WriteLog("Загружен внешний листфайл: " + Listfile);
			}
			else if(!bDontUseInternalListfile)
			{
				TResourceStream* ResStream = new TResourceStream(0, "FILELIST", (wchar_t*)RT_RCDATA);
				StreamToStrings(ResStream, s, false);
				ResStream->Free();
			}
			RetrieveKnownFiles(s, Archive);
			if(bAnalyze[ANALYZE_JSCRIPT])
			{
				AnalyzeMainScript(s, Archive);
				if(bWriteLog) WriteLog("Анализ war3map.j...");
				AutoSearch(s, Archive);
			}
			if(bAnalyze[ANALYZE_W3_BIN])
			{
				for(int i = 0; i < sizeof(BinaryFiles) / sizeof(UnicodeString); i++)
				{
					Application->ProcessMessages();
					if(VerifyFile(Archive, BinaryFiles[i]) && AnalyzeBinaryFile(s, Archive, BinaryFiles[i], false) && bWriteLog) WriteLog("Анализ " + BinaryFiles[i] + "...");
				}
			}
            if(bAnalyze[ANALYZE_IMPLIST])
			{
				AnalyzeBinaryFile(s, Archive, "war3map.imp");
				if(bWriteLog) WriteLog("Анализ war3map.imp...");
			}
			AutoSearch(s, Archive);
			if(bAnalyze[ANALYZE_TXT])
			{
				AnalyzeAllTextFiles(s, Archive);
				AutoSearch(s, Archive);
			}
			if(bAnalyze[ANALYZE_MDX])
			{
				AnalyzeAllModels(s, Archive);
				AutoSearch(s, Archive);
			}
			if(bAnalyze[ANALYZE_SLK])
			{
				AnalyzeAllSLK(s, Archive);
				AutoSearch(s, Archive);
			}
			if(bFullAnalyze) AnalyzeAllUnknown(s, Archive);
			if(bWriteLog) WriteLog("Генерация war3map.imp...");
			GenerateWar3mapIMP(s, Archive, Dir);
			// Распаковка неизвестных файлов:
			if(bUnpackUnknown) ExtractUnknownFiles(Archive, s, Dir);
			// Распаковка известных файлов:
			s->SaveToFile(AppDataPath + "FILELIST_CACHE.txt");
			nLastUnpackedFilesCount = 0;
			PluginManager->SendCodeToAll(CODE_FILE_SEARCH_END, pfcFileFindHooked);
			s->Clear();
		}
		else
		{
			(*PluginManager->Overrides.Search.Function)();
		}
		if(!PluginManager->Overrides.Unpack.Enabled)
		{
			nLastUnpackedFilesCount = 0;
			if(FileExists(AppDataPath + "FILELIST_CACHE.txt")) s->LoadFromFile(AppDataPath + "FILELIST_CACHE.txt");
			HANDLE hMPQ = NULL;
			SFileOpenArchive(BUF(Archive), 0, 0, &hMPQ);
			if(!HANDLE_VALID(hMPQ))
			{
				WriteLog("Ошибка распаковки файлов.");
				return 0;
            }
			for(int i = 0; i < s->Count; i++)
			{
				Application->ProcessMessages();
				if(!StrInList(s1, s->Strings[i]))
				{
					//const int r = ExtractFile(Archive, s->Strings[i], Dir + s->Strings[i]);
					const int r = ExtractFile(hMPQ, s->Strings[i], Dir + s->Strings[i]);
					if(r == ERROR_SUCCESS)
					{
						if(bWriteLog) WriteLog("Распаковка " + s->Strings[i] + "...");
						s1->Add(s->Strings[i]);
						nLastUnpackedFilesCount++;
					}
				}
			}
			if(HANDLE_VALID(hMPQ)) SFileCloseArchive(hMPQ);
			s1->Text = ReplaceStr(s1->Text, "Scripts\\war3map.j", "war3map.j");
			s1->SaveToFile(Dir + "(listfile)");
			const UnicodeString FilesFound = FloatToStrF(Ratio(GetFileCount(Archive), nLastUnpackedFilesCount), ffGeneral, 3, 1) + "%";
			if(bWriteLog) WriteLog("Распаковано " + IntToStr((int)nLastUnpackedFilesCount) + " файлов. (" + FilesFound + ")");
		}
		else
		{
			nLastUnpackedFilesCount = (*PluginManager->Overrides.Unpack.Function)();
        }
		PluginManager->SendCodeToAll(CODE_FILES_UNPACKED, pfcFileUnpackHooked);
		// Пост-обработка
		if(FileExists(Dir + "Scripts\\war3map.j"))
		{
			if(FileExists(Dir + "war3map.j")) DeleteFile(Dir + "war3map.j");
			RenameFile(Dir + "Scripts\\war3map.j", Dir + "war3map.j");
        }
		if(bWriteLog) WriteLog("Удаление (attributes)...");
		DeleteFile(Dir + "(attributes)");
		if(bRewriteW3I) WriteWar3mapW3I(Dir);
		if(bRewriteDOO) WriteWar3mapUnitsDOO(Dir);
		if(bWriteLog) WriteLog("Карта успешно распакована!");
		// s->SaveToFile("FILELIST_CACHE.txt");
		s1->Free();
		s->Free();
		return true;
	}
	catch(Exception &e)
	{
		Msg("Ошибка: " + e.Message, true);
        return false;
    }
}
//-----------------------------------------------------------------------------
// Extracts an archived file and saves it to the disk.
//
// Parameters :
//
//   char * szArchiveName  - Archive file name
//   char * szArchivedFile - Name/number of archived file.
//   char * szFileName     - Name of the target disk file.
static int ExtractFile(HANDLE hMpq, UnicodeString ArcFile, UnicodeString SavedFile)
{
	char *szArchivedFile = ArcFile.t_str(), *szFileName = SavedFile.t_str();
	HANDLE hFile  = NULL;          // Archived file handle
	HANDLE handle = NULL;          // Disk file handle
	int    nError = ERROR_SUCCESS; // Result value

    // Open a file in the archive, e.g. "data\global\music\Act1\tristram.wav"
    if(nError == ERROR_SUCCESS)
    {
        if(!SFileOpenFileEx(hMpq, szArchivedFile, 0, &hFile))
            nError = GetLastError();
    }

    // Create the target file
	if(nError == ERROR_SUCCESS)
	{
		const UnicodeString SubDir = ExtractFilePath(szFileName);
		__MkDir(SubDir);
		handle = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
        if(handle == INVALID_HANDLE_VALUE)
			nError = GetLastError();
    }

    // Read the file from the archive
    if(nError == ERROR_SUCCESS)
    {
        char  szBuffer[0x10000];
        DWORD dwBytes = 1;

        while(dwBytes > 0)
        {
            SFileReadFile(hFile, szBuffer, sizeof(szBuffer), &dwBytes, NULL);
            if(dwBytes > 0)
                WriteFile(handle, szBuffer, dwBytes, &dwBytes, NULL);
        }
    }

    // Cleanup and exit
    if(handle != NULL)
        CloseHandle(handle);
    if(hFile != NULL)
        SFileCloseFile(hFile);

	return nError;
}
static int ExtractFile(UnicodeString Archive, UnicodeString ArcFile, UnicodeString SavedFile)
{
	char * szArchiveName = Archive.t_str(), *szArchivedFile = ArcFile.t_str(), *szFileName = SavedFile.t_str();
	HANDLE hMpq   = NULL;          // Open archive handle
	HANDLE hFile  = NULL;          // Archived file handle
	HANDLE handle = NULL;          // Disk file handle
	int    nError = ERROR_SUCCESS; // Result value

	// Open an archive, e.g. "d2music.mpq"
	if(nError == ERROR_SUCCESS)
	{
		if(!SFileOpenArchive(szArchiveName, 0, 0, &hMpq))
			nError = GetLastError();
	}

    // Open a file in the archive, e.g. "data\global\music\Act1\tristram.wav"
    if(nError == ERROR_SUCCESS)
    {
        if(!SFileOpenFileEx(hMpq, szArchivedFile, 0, &hFile))
            nError = GetLastError();
    }

    // Create the target file
	if(nError == ERROR_SUCCESS)
	{
		const UnicodeString SubDir = ExtractFilePath(szFileName);
		__MkDir(SubDir);
		handle = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
        if(handle == INVALID_HANDLE_VALUE)
			nError = GetLastError();
    }

    // Read the file from the archive
    if(nError == ERROR_SUCCESS)
    {
        char  szBuffer[0x10000];
        DWORD dwBytes = 1;

        while(dwBytes > 0)
        {
            SFileReadFile(hFile, szBuffer, sizeof(szBuffer), &dwBytes, NULL);
            if(dwBytes > 0)
                WriteFile(handle, szBuffer, dwBytes, &dwBytes, NULL);
        }
    }

    // Cleanup and exit
    if(handle != NULL)
        CloseHandle(handle);
    if(hFile != NULL)
        SFileCloseFile(hFile);
    if(hMpq != NULL)
		SFileCloseArchive(hMpq);

	return nError;
}
bool VerifyFile(UnicodeString Archive, UnicodeString File)
{
	char * szArchiveName = Archive.t_str(), *szArchivedFile = File.t_str();
	HANDLE hMpq   = NULL;          // Open archive handle
	HANDLE hFile  = NULL;          // Archived file handle
	HANDLE handle = NULL;          // Disk file handle
	bool   bResult = false;
	int nError = ERROR_SUCCESS;

    if(nError == ERROR_SUCCESS)
	{
		if(!SFileOpenArchive(szArchiveName, 0, 0, &hMpq))
			nError = GetLastError();
	}

	// Open an archive
	if(nError == ERROR_SUCCESS)
	{
		bResult = SFileHasFile(hMpq,szArchivedFile);
	}

	// Cleanup and exit
    if(handle != NULL)
		CloseHandle(handle);
    if(hFile != NULL)
        SFileCloseFile(hFile);
    if(hMpq != NULL)
		SFileCloseArchive(hMpq);

	return bResult;
}
//---------------------------------------------------------------------------
inline void MergeLists(TStrings* Source, TStrings* Dest)
{
	Dest->Text = Dest->Text + ENDLINE + Source->Text;
//	RemoveDoubles(Dest);
}
bool AnalyzeMainScript(TStrings* List, const UnicodeString Archive)
{
	try
	{
		const UnicodeString Path = GetTempDir();
		TStrings* s = new TStringList();
		UnicodeString ScriptFile = Path + "111.tmp~.j";
		if(ExtractFile(Archive, "war3map.j", ScriptFile) != ERROR_SUCCESS)
			ExtractFile(Archive, "Scripts\\war3map.j", ScriptFile);

		if(!FileExists(ScriptFile))
		{
			s->Free();
			return false;
		}
		else s->LoadFromFile(ScriptFile);

		UnicodeString ScriptText = s->Text;
		s->Clear();

		// Ищем все строковые константы (хуйню в двойных кавычках):
		int Pos1 = PosEx("\"", ScriptText, 1), Pos2 = PosEx("\"", ScriptText, Pos1 + 1);
		while((Pos1 && Pos2) && (Pos1 < ScriptText.Length() && Pos2 < ScriptText.Length()))
		{    
			try
			{
				if(ScriptText[Pos1 + 1] != '"')
				{
					const int nStrSize = (Pos2 - Pos1 - 1);
					const UnicodeString Str = ReplaceStr(ScriptText.SubString(Pos1 + 1, nStrSize), "\\\\", "\\");

					if(FileNameIsValid(Str)) s->Add(Str);
				}

				Pos1 = PosEx("\"", ScriptText, Pos2 + 1);
				Pos2 = PosEx("\"", ScriptText, Pos1 + 1);
			}
			catch(...){break;}
		}
		if(bDebugSaveNames) s->SaveToFile(GetTempDir() + "MPQRepacker\\war3map.j.txt"); // Отладка
		AutoSearch(s, Archive);
		MergeLists(s, List);
		s->Free();
		DeleteFile(ScriptFile);
		return true;
	}
	catch(...){return false;}
}
bool AnalyzeTextFile(TStrings* List, const UnicodeString Archive, const UnicodeString File)
{
	try
	{
		const UnicodeString Path = GetTempDir();

		UnicodeString ScriptFile = Path + "111.tmp~.txt";
		if(ExtractFile(Archive, File, ScriptFile) != ERROR_SUCCESS) return false;

		TStrings* s = new TStringList();

		if(!FileExists(ScriptFile))
		{
			s->Free();
			return false;
		}
		else s->LoadFromFile(ScriptFile);

		UnicodeString ScriptText = s->Text;
		s->Clear();

		// Ищем все строки между равно и концом строки
		const UnicodeString LineEnd = "\r\n";
		int Pos1 = PosEx("=", ScriptText, 1), Pos2 = PosEx(LineEnd, ScriptText, Pos1 + 1);
		while((Pos1 && Pos2) && (Pos1 < ScriptText.Length() && Pos2 < ScriptText.Length()))
		{
			try
			{
				if(ScriptText[Pos1 + 1] != '"')
				{
					const int nStrSize = (Pos2 - Pos1 - 1);
					const UnicodeString Str = ReplaceStr(ScriptText.SubString(Pos1 + 1, nStrSize), "\\\\", "\\");

					if(FileNameIsValid(Str)) s->Add(Str);
				}

				Pos1 = PosEx("=", ScriptText, Pos2 + 1);
				Pos2 = PosEx(LineEnd, ScriptText, Pos1 + 1);
			}
			catch(...){break;}
		}
		AutoSearch(s, Archive);
		MergeLists(s, List);
		if(bDebugSaveNames)	s->SaveToFile(GetTempDir() + "MPQRepacker\\" + ExtractFileName(File) + ".txt"); // Отладка
		s->Free();
		DeleteFile(ScriptFile);
		return true;
	}
	catch(...){return false;}
}
//---------------------------------------------------------------------------

bool AnalyzeBinaryFile(TStrings* List, const UnicodeString Archive, const UnicodeString File, bool bLocalFile)
{
	try
	{
		TStrings* s = new TStringList();
		UnicodeString ScriptFile = "";
		BYTE* szBuffer = NULL;
		unsigned int nBufferSize = 0;

		// bLocalFile = FileExists(File); // Fix

		if(!bLocalFile)
		{
			ScriptFile = GetTempDir() + File;
			if(ExtractFile(Archive, File, ScriptFile) != ERROR_SUCCESS)
			{
                WriteLog("Ошибка распаковки файла для анализа: " + File);
				VCL_FREE(s);
				return false;
			}
		}
		else ScriptFile = File;

		DWORD dwHeader = ReadFileHeader(ScriptFile);
		if(dwHeader == 0x5733504D || dwHeader == 0x31504C42) return false; // MP3/BLP файл

		Classes::TFileStream* fs = new Classes::TFileStream(ScriptFile, fmOpenRead);
		szBuffer = (BYTE*)malloc(fs->Size);
		fs->Read(szBuffer, fs->Size);
		nBufferSize = fs->Size;
		VCL_FREE(fs);
		// Ищем все строки между NULL'ами:
		const BYTE NullChar = NULL;
		int Pos1 = __Pos(szBuffer, nBufferSize, NullChar, 0), Pos2 = __Pos(szBuffer, nBufferSize, NullChar, Pos1 + 1);
		while((Pos1 && Pos2) && (Pos1 < nBufferSize && Pos2 < nBufferSize))
		{
			Application->ProcessMessages();
			if(szBuffer[Pos1] != 0)
			{
				Pos2 = __Pos(szBuffer, nBufferSize, NullChar, Pos1 + 1);
				const int nStrSize = (Pos2 - Pos1);
				const UnicodeString Str = __SubString(szBuffer, Pos1, nStrSize);

				if(FileNameIsValid(Str)) s->Add(Str);
				Pos1 = __Pos(szBuffer, nBufferSize, NullChar, Pos2 + 1) + 1;
			}
            Pos1++;
		}
		if(bDebugSaveNames)	s->SaveToFile(GetTempDir() + "MPQRepacker\\" + ExtractFileName(File) + ".txt"); // Отладка
		AutoSearch(s, Archive);
		MergeLists(s, List);
		s->Free();
		DeleteFile(ScriptFile);
		free(szBuffer);
		return true;
	}
    catch(Exception &e) { WriteLog(e.Message); }
	catch(...){}
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mmoLogDblClick(TObject *Sender)
{
	mmoLog->Clear();
}

UnicodeString FindMapNameInW3I()
{
	const UnicodeString War3mapW3I = IncludeTrailingBackslash(MainForm->edtTempDir->Text) + "war3map.w3i";
	if(!FileExists(War3mapW3I)) return "";
	Classes::TFileStream* fs = new Classes::TFileStream(War3mapW3I, fmOpenRead);
	char* szBuffer = (char*)malloc(fs->Size);
	fs->Position = 12;
	fs->Read(szBuffer, fs->Size - 12);
	char szResult[256];
	for(int i = 0; szBuffer[i] != 0 && i < fs->Size - 12; i++)
	{
		szResult[i] = szBuffer[i];
	}
	VCL_FREE(fs);
	free(szBuffer);
	return UnicodeString(szResult);
}
UnicodeString FindTrigstrNumInW3I()
{
	const UnicodeString War3mapW3I = IncludeTrailingBackslash(MainForm->edtTempDir->Text) + "war3map.w3i";
	if(!FileExists(War3mapW3I)) return "";
	Classes::TFileStream* fs = new Classes::TFileStream(War3mapW3I, fmOpenRead);
	char* szBuffer = (char*)malloc(fs->Size);
	fs->Read(szBuffer, fs->Size);
	DWORD i = 13;
	while(szBuffer[i] != '_') i++;
	i++; // После _
	UnicodeString Result = "";
	while(szBuffer[i] != 0)
	{
		Result = Result + szBuffer[i];
		i++;
	}
	VCL_FREE(fs);
	free(szBuffer);
	return Result;
}
UnicodeString ToUTF8(const UnicodeString Source)
{
	char* buf = BUF(Source);
	char* result = UTF8String(buf).c_str();
    return UnicodeString(result);
}

void RewriteMap(UnicodeString Map, UnicodeString NewName)
{
	DWORD nPos = 8 + nDefaultMapNameSize; // nDefaultMapNameSize - длинна стандартного названия карты
	Classes::TFileStream* fs = new Classes::TFileStream(Map, fmOpenReadWrite);
	DWORD nSize = fs->Size - nPos;
	char* szBuffer = (char*)malloc(nSize);
	fs->Position = nPos;
	fs->Read(szBuffer, nSize);
	char* szMapName = UTF8String(BUF(NewName)).c_str();
	fs->Size = 0; // Стираем все
	char Header[8] = "HM3W\0\0\0\0";
	fs->Write(Header, 8);
	fs->Write(szMapName, NewName.Length());
	fs->Write(szBuffer, nSize);
	char __null_char = 0x00;
	fs->Write(&__null_char, 1);
	VCL_FREE(fs);
	free(szBuffer);
}
UnicodeString __ReadFile(const UnicodeString File)
{
	Classes::TFileStream* fs = new Classes::TFileStream(File, fmOpenRead);
	char* buf = new char[fs->Size];
	fs->Read(buf, fs->Size);
	VCL_FREE(fs);
	const UnicodeString Result = UnicodeString(buf);
	DEL_ARRAY(buf);
	return Result;
}
UnicodeString FindTriggerString(const UnicodeString War3mapWTS, const UnicodeString Num)
{
	TStrings* s = new TStringList();
	s->Text = __ReadFile(War3mapWTS);
	int i = FindStr(s, "STRING " + Num) + 2; // После этой строки и {
	UnicodeString Result = "";
	while(s->Strings[i] != "}")
	{
		Result = Result + s->Strings[i];
		if(s->Strings[i+1] != "}") Result = Result + "\r\n";
		i++;
	}
	s->Free();
	return Result;
}
UnicodeString ReservedSize(UnicodeString Source, int Size)
{
	if(Source.Length() > Size) Source = Source.SubString(1, Size);
	while(Source.Length() < Size) Source = Source + Char(0);
	return Source;
}
inline UnicodeString TrueWTSNumber(const UnicodeString W3INumber)
{
	return IntToStr(W3INumber.ToInt());
}
void GetMapAttributes(const UnicodeString Map, int &nPlayers, int &nFlags, char* szMapName)
{
	if(!FileExists(Map)) return;
	TBinaryFile* b = new TBinaryFile(Map);
	b->Position = 8;
	b->ReadNullTerminatedString(szMapName, 256);
	nFlags = b->ReadInt();
	nPlayers = b->ReadInt();
	VCL_FREE(b);
}
void SetMapAttributes(const UnicodeString Map, const int nPlayers, const int nFlags, char* szMapName)
{
	if(!FileExists(Map)) return;
	TBinaryFile* b = new TBinaryFile(Map);
	b->Position = 8;
	if(szMapName != NULL)
	{
		b->WriteBuffer(strlen(szMapName), szMapName);
		b->WriteByte(0);
	}
	else b->ReadNullTerminatedString(NULL, INFINITE);
	b->WriteInt(nFlags);
	b->WriteInt(nPlayers);
	b->SaveToFile(Map);
	VCL_FREE(b);
}
bool RePackMap(const UnicodeString SourceMap, const UnicodeString DestMap,
	const bool bInsertCheats, const UnicodeString CheatPack,
	const UnicodeString Activator, const bool bEmptyDOO, const bool bUnpackUnknown,
	const bool bPatchW3I, const bool bManualEdit, const bool bUseXDep,
	const bool bDeleteTempDir, const UnicodeString ListFile, UnicodeString TempDir)
{
	__SourceMap = SourceMap;
	__DestMap = DestMap;
	bool bResult = MPQIsValid(SourceMap);
	if(!bResult)
	{
		// WriteLog("Ошибка распаковки: файл " + ExtractFileName(SourceMap) + " не является MPQ архивом.");
		WriteLog("Ошибка распаковки: невозможно открыть файл " + ExtractFileName(SourceMap) + "!");
		return false;
	}

	UnicodeString DestDir = ExtractFilePath(DestMap);
	if(!DirectoryExists(DestDir)) MkDir(DestDir);

	TempDir = IncludeTrailingBackslash(TempDir);
	if(DirectoryExists(TempDir))
	{
		Sleep(200);
		CMD("del /f /s /q " + IncludeTrailingBackslash(TempDir) + "*", true);
		FullRemoveDir(TempDir, true, false, false);
		DeleteNonEmptyDir(TempDir);
	}
	WriteLog("\r\nПерепаковка карты " + ExtractFileName(SourceMap) + ":");
	bResult = ExtractAllFiles(SourceMap, bPatchW3I, bEmptyDOO, bUnpackUnknown, TempDir, ListFile);
	if(!bResult)
	{
		WriteLog("Ошибка распаковки. Операция прервана.");
		return false;
	}
	DeleteFile(DestMap);
	UnicodeString MapName = "";
	int nPlayers, nFlags;
	char szMapName[256];
	GetMapAttributes(SourceMap, nPlayers, nFlags, szMapName);
	if(bMapRename && FileExists(TempDir + "war3map.w3i"))
	{
		try
		{
			MapName = FindMapNameInW3I();
			if(MapName.Pos("TRIGSTR_"))
			{
				const UnicodeString TrigstrNum = TrueWTSNumber(MapName.SubString(9, MapName.Length() - 8));//(FindTrigstrNumInW3I());
				MapName = FindTriggerString(TempDir + "war3map.wts", TrigstrNum);
            }
//			MapName = ReservedSize(DeleteTransfers(MapName), nDefaultMapNameSize);
			if(MapName.Length()) WriteLog("Реальное имя карты найдено: " + MapName);
			else
			{
				Msg("Реальное имя карты не найдено!", true);
				InputBox("Имя карты", "Введите реальное имя карты:", "");
			}
		}
		catch(Exception &e){Msg(e.Message);}
	}
	if(bInsertCheats)
	{
		WriteLog("Вставка чит-пака...");
		if(InsertCheats(TempDir + "war3map.j", CheatPack, Activator))
			WriteLog("Чит-пак успешно вставлен в скрипт карты!");
		else
			WriteLog("Ошибка вставки чит-пака.");
    }
	if(bManualEdit) Msg("Внесите необходимые поправки в файлы карты и нажмите ОК.");
	if(!PluginManager->Overrides.Pack.Enabled)
	{
		if(bUseXDep) bResult = CreateMapWithXDep(DestMap, TempDir);
		else bResult = PackFilesToMPQ(DestMap, TempDir);
	}
	else
	{
        (*PluginManager->Overrides.Pack.Function)();
	}
	if(bMapRename)
	{
		if(FileExists(DestMap))
		{
			RewriteMap(DestMap, MapName);
			SetMapAttributes(DestMap, nPlayers, nFlags, NULL);
        }
		else WriteLog("Ошибка: Упакованная карта не найдена!");
	}
	else SetMapAttributes(DestMap, nPlayers, nFlags, szMapName);
	PluginManager->SendCodeToAll(CODE_FILES_PACKED, pfcFilePackHooked);
	if(bDeleteTempDir)
	{
		Sleep(200);
		CMD("del /f /s /q " + IncludeTrailingBackslash(TempDir) + "*", true);
		FullRemoveDir(TempDir, true, false, false);
		DeleteNonEmptyDir(TempDir);
	}

	if(!bResult) WriteLog("Ошибка упаковки!");
	else WriteLog("Перепаковка карты завершена!");

	return bResult;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::btnRePackClick(TObject *Sender)
{
	this->Enabled = false;
	try
	{
		if(!FileExists(edtSourceMap->Text))
		{
			Msg("Файл не найден: " + edtSourceMap->Text, true);
			// return;
		}
		else
		{
			__Status = mrsRePacking;
			RePackMap(edtSourceMap->Text, edtDestMap->Text,
				chkInsertCheats->Checked, cbbCheatPack->Text, edtActivator->Text,
				chkReplaceDOO->Checked, chkExtractUnknownFiles->Checked,
				chkReplaceW3I->Checked, chkManualEdit->Checked, chkUseXdep->Checked,
				chkDeleteTemp->Checked, edtExtFileList->Text, edtTempDir->Text);
		}
	}
	catch(Exception &e){Msg(e.Message, true);}
	__Status = mrsIdle;
	this->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	SaveSettings();
	VCL_FREE(frmPlugins);
	VCL_FREE(frmSettings);
	//VCL_FREE(IniLock);
}
//---------------------------------------------------------------------------
void TMainForm::LoadSettings()
{
	// Стандартные пути:
	TRegistry* R = new TRegistry();
	R->RootKey = HKEY_LOCAL_MACHINE;
	if(R->OpenKey("SOFTWARE\\Blizzard Entertainment\\Warcraft III", false))
	{
		UnicodeString War3Path = IncludeTrailingBackslash(R->ReadString("InstallPath"));
		if(FileExists(War3Path + "war3.exe")) // Проверка правильности пути
		{
			edtSourceMap->InitialDir = War3Path + "Maps\\Download\\";
			edtSourceMap->Text = edtSourceMap->InitialDir + "\\Map.w3x";
			edtDestMap->InitialDir = War3Path + "Maps\\Cheat\\";
			edtDestMap->Text = edtDestMap->InitialDir + "\\Map.w3x";
			if(!DirectoryExists(edtDestMap->InitialDir)) MkDir(edtDestMap->InitialDir);
			if(!DirectoryExists(edtSourceMap->InitialDir)) MkDir(edtSourceMap->InitialDir);
		}
		R->CloseKey();
	}
	VCL_FREE(R);

	if(!FileExists(IniPath)) return;
	//IniLock->Unlock();
	TIniFile* ini = new TIniFile(IniPath);
	bDebugSaveNames = ini->ReadBool("Debug", "AdvSaveFoundNames", false);
	bFullAnalyze = ini->ReadBool("Settings", "FullAnalyze", bFullAnalyze);
	edtSourceMap->Text = ini->ReadString("Maps", "Source", edtSourceMap->Text);
	edtDestMap->Text = ini->ReadString("Maps", "Dest", edtDestMap->Text);
	edtTempDir->Text = ini->ReadString("Settings", "TempDir", edtTempDir->Text);
	edtExtFileList->Text = ini->ReadString("Settings", "ExternalFileList", edtExtFileList->Text);
	edtActivator->Text = ini->ReadString("Cheats", "Activator", edtActivator->Text);
	cbbCheatPack->ItemIndex = ini->ReadInteger("Cheats", "CheatPack", cbbCheatPack->ItemIndex);
	chkDeleteTemp->Checked = ini->ReadBool("Settings", "DeleteTemp", chkDeleteTemp->Checked);
	chkExtractUnknownFiles->Checked = ini->ReadBool("Settings", "ExtractUnknown", chkExtractUnknownFiles->Checked);
	chkInsertCheats->Checked = ini->ReadBool("Cheats", "Enabled", chkInsertCheats->Checked);
	chkManualEdit->Checked = ini->ReadBool("Settings", "DoPause", chkManualEdit->Checked);
	chkReplaceDOO->Checked = ini->ReadBool("Recover", "EmptyDOO", chkReplaceDOO->Checked);
	chkReplaceW3I->Checked = ini->ReadBool("Recover", "EmptyW3I", chkReplaceW3I->Checked);
	chkUseXdep->Checked = ini->ReadBool("Settings", "PackWithXDEP", chkUseXdep->Checked);
	VCL_FREE(ini);
	//IniLock->Lock();
}
void TMainForm::SaveSettings()
{
	//IniLock->Unlock();
	TIniFile* ini = new TIniFile(IniPath);
	ini->WriteBool("Debug", "AdvSaveFoundNames", bDebugSaveNames);
	ini->WriteString("Maps", "Dest", edtDestMap->Text);
	ini->WriteString("Maps", "Source", edtSourceMap->Text);
	ini->WriteBool("Settings", "FullAnalyze", bFullAnalyze);
	ini->WriteString("Settings", "TempDir", edtTempDir->Text);
	ini->WriteString("Settings", "ExternalFileList", edtExtFileList->Text);
	ini->WriteString("Cheats", "Activator", edtActivator->Text);
	ini->WriteInteger("Cheats", "CheatPack", cbbCheatPack->ItemIndex);
	ini->WriteBool("Settings", "DeleteTemp", chkDeleteTemp->Checked);
	ini->WriteBool("Settings", "ExtractUnknown", chkExtractUnknownFiles->Checked);
	ini->WriteBool("Cheats", "Enabled", chkInsertCheats->Checked);
	ini->WriteBool("Settings", "DoPause", chkManualEdit->Checked);
	ini->WriteBool("Recover", "EmptyDOO", chkReplaceDOO->Checked);
	ini->WriteBool("Recover", "EmptyW3I", chkReplaceW3I->Checked);
	ini->WriteBool("Settings", "PackWithXDEP", chkUseXdep->Checked);
	VCL_FREE(ini);
	//IniLock->Lock();
}
void __fastcall TMainForm::btnCommandsClick(TObject *Sender)
{
	const UnicodeString CommandsPath = "CheatPacks\\" + cbbCheatPack->Text + "\\commands.txt"; // Relative
	if(FileExists(AppDataPath + CommandsPath))
	{
		ShellExec(AppDataPath + CommandsPath, this->Handle);
	}
	else
	{
		Msg("Файл commands.txt не найден", true);
    }
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::btnPluginsClick(TObject *Sender)
{
	frmPlugins->Show();
	frmPlugins->chklstPluginsClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormShow(TObject *Sender)
{
	btnPlugins->Enabled = frmPlugins->IsPluginsPresent();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::edtActivatorDblClick(TObject *Sender)
{
	const int nLength = edtActivator->Text.Length() - 1;
	edtActivator->Text = "-" + GenerateString(nLength, csFileName);
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::edtSourceMapChange(TObject *Sender)
{
	edtDestMap->Text =
		IncludeTrailingBackslash(ExtractFilePath(edtDestMap->Text)) +
		ExtractFileName(edtSourceMap->Text);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
	const UnicodeString TempFile = GetTempDir() + "MPQRepackerHelp.chm";
	if(Key == VK_F1)
	{
		if(ResUnpack("HELP", TempFile))
			ShellExec(TempFile);
    }
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::btnSettingsClick(TObject *Sender)
{
	frmSettings->ShowModal();
}
//---------------------------------------------------------------------------
inline void MapBatchRepack_FiltrateFileList(TStrings* List)
{
	int i = 0;
	while(i < List->Count)
	{
		const UnicodeString Ext = ExtractFileExt(List->Strings[i]);
		if(Ext != ".w3m" && Ext != ".w3x") List->Delete(i);
		else i++;
    }
}
void __fastcall TMainForm::btnBatchRepackClick(TObject *Sender)
{
	bool bInput = false;
	__Status = mrsBatchRePacking;
	UnicodeString tmp = ExtractFilePath(edtSourceMap->Text);
	bInput = InputQuery("Пакетная перепаковка", "Введите папку с исходными файлами:", tmp);
	if(!bInput) return;

	const UnicodeString SourceDir = IncludeTrailingBackslash(tmp);
	if(!DirectoryExists(SourceDir))
	{
    	WriteLog("Директория не существует: " + SourceDir);
		return;
	}
	tmp = ExtractFilePath(edtDestMap->Text);
	bInput = InputQuery("Пакетная перепаковка", "Введите папку для перепакованных файлов:", tmp);
	if(!bInput) return;
	const UnicodeString DestDir = IncludeTrailingBackslash(tmp);
	if(!__MkDir(DestDir))
	{
		WriteLog("Ошибка создания директории: " + DestDir);
		return;
    }
	const bool bSubDirs = Ask("Перепаковывать карты в подпапках?");
	TStrings* s = new TStringList();
	FileFinder(SourceDir, "*.*", bSubDirs, (TStringList*)s);
	MapBatchRepack_FiltrateFileList(s);
	LocalizeFileList(s, SourceDir);
	this->Enabled = false;
	for(int i = 0; i < s->Count; i++)
	{
		try
		{
			RePackMap(SourceDir + s->Strings[i], DestDir + s->Strings[i],
					chkInsertCheats->Checked, cbbCheatPack->Text, edtActivator->Text,
					chkReplaceDOO->Checked, chkExtractUnknownFiles->Checked,
					chkReplaceW3I->Checked, chkManualEdit->Checked, chkUseXdep->Checked,
					chkDeleteTemp->Checked, edtExtFileList->Text, edtTempDir->Text);
		}
		catch(...) {continue;}
	}
	this->Enabled = true;
	__Status = mrsIdle;
	Msg("Пакетная перепаковка завершена!");
}
//---------------------------------------------------------------------------
UnicodeString War3Exe = "";
void __fastcall TMainForm::btnTestMapClick(TObject *Sender)
{
	UnicodeString War3Path;
	if(!War3Exe.Length())
	{
		TRegistry* R = new TRegistry();
		R->RootKey = HKEY_LOCAL_MACHINE;
		if(R->OpenKey("SOFTWARE\\Blizzard Entertainment\\Warcraft III", false))
		{
			War3Path = IncludeTrailingBackslash(R->ReadString("InstallPath"));
			War3Exe = War3Path + "War3.exe";
			R->CloseKey();
		}
		VCL_FREE(R);
	}
	if(!FileExists(War3Exe)) Msg("Файл war3.exe не найден!", true);
	else ExecuteProcess(War3Exe, "", 0);
	/*else if(!FileExists(edtDestMap->Text)) Msg("Файл карты не найден!", true);
	else
	{
		UnicodeString TempMapFile = "Maps\\Test\\WorldEditTestMap" + ExtractFileExt(edtDestMap->Text);
		CopyFile(edtSourceMap->Text, War3Path + TempMapFile);
		SetFileAttributesA((War3Path + TempMapFile).t_str(), FILE_ATTRIBUTE_ARCHIVE);
		ExecuteProcess(War3Exe, (UnicodeString)"-loadfile " + TempMapFile, INFINITE, War3Path);
	}           */
}
//---------------------------------------------------------------------------

