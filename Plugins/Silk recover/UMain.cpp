//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include <System.hpp>
#include <Classes.hpp>
#include <SysUtils.hpp>
#include "URecursiveSearch.hpp"
#include "StormLib.h"
#include "MPQRepacker_DynamicAPI.h"
#include "HashLib.h"
#pragma hdrstop
//---------------------------------------------------------------------------
#define DLLEXPORT extern "C" __declspec(dllexport)
//---------------------------------------------------------------------------
DLLEXPORT int CALLBACK PluginLoad()
{
	API_WriteLog("SLKRecover 1.0 © ZxZ666");
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginUnload()
{
	return GetLastError();
}
DLLEXPORT TPluginInfo CALLBACK RegisterPlugin()
{
	TPluginInfo Result;
	Result.szName = "SLKRecover\0";									// Название плагина
	Result.szAuthor = "ZxZ666\0";										// Автор
	Result.szDescription = "Восстановление объектных файлов (war3map.w3*) из файлов *.slk.\0";	// Описание
	// Настройки прерываний:
	Result.bHookFileFind = false;	// CODE_FILE_SEARCH_START и CODE_FILE_SEARCH_END
	Result.bHookFileUnpack = true;	// CODE_FILES_UNPACKED
	Result.bHookFilePack = false;	// CODE_FILES_PACKED
	return Result;
}
void FiltrateFileList(TStrings* List, const UnicodeString Ext)
{
	int i = 0;
	while(i < List->Count)
	{
		if(ExtractFileExt(List->Strings[i]) != Ext) List->Delete(i);
		else i++;
    }
}
void LocalizeFileList(TStrings* List, const UnicodeString Dir)
{
	for(int i = 0; i < List->Count; i++)
	{
		List->Strings[i] = ReplaceStr(List->Strings[i], IncludeTrailingBackslash(Dir), "");
	}
}
bool BuildFileList(const UnicodeString Mask, const UnicodeString Dir, TStrings* List)
{
	try
	{
		if(List == NULL) return false;
		const UnicodeString SearchMask = Dir + "*.*";
		List->Clear();
		FileFinder(Dir, Mask, true, (TStringList*)List);
		LocalizeFileList(List, Dir);
		return true;
	}
	catch(...) {return false;}
}
UnicodeString __ReadFile(const UnicodeString File)
{
	Classes::TFileStream* fs = new Classes::TFileStream(File, fmOpenRead);
	char* buf = new char[fs->Size + 1];
	fs->Read(buf, fs->Size);
	buf[fs->Size] = 0x0;
	VCL_FREE(fs);
	const UnicodeString Result = UnicodeString(buf);
	DEL_ARRAY(buf);
	return Result;
}
void AddToListFile(const UnicodeString Listfile, const UnicodeString File)
{
	TStrings* s = new TStringList();
	if(FileExists(Listfile)) s->LoadFromFile(Listfile);
	if(!StrInList(s, File))
	{
		s->Add(File);
		s->SaveToFile(Listfile);
    }
	VCL_FREE(s);
}
void DeleteFromListFile(const UnicodeString Listfile, const UnicodeString File)
{
	TStrings* s = new TStringList();
	if(FileExists(Listfile)) s->LoadFromFile(Listfile);
	if(StrInList(s, File)) s->Delete(FindStr(s, File));
	s->SaveToFile(Listfile);
	VCL_FREE(s);
}
void CopyAll(const UnicodeString SourceDir, const UnicodeString DestDir, const UnicodeString Mask)
{
	TStrings* s = new TStringList();
	BuildFileList(Mask, SourceDir, s);
	for(int i = 0; i < s->Count; i++)
	{
		UnicodeString From = SourceDir + s->Strings[i];
		UnicodeString To = DestDir + s->Strings[i];
		if(!DirectoryExists(ExtractFilePath(To))) __MkDir(ExtractFilePath(To));
		if(CopyFile(From.t_str(), To.t_str(), true) == TRUE)
			AddToListFile(DestDir + "(listfile)", s->Strings[i]);
	}
	s->Free();
}
bool AddFileToMPQ(HANDLE hMPQ, UnicodeString File, UnicodeString ArcFile)
{
	try
	{
		char *szFile = File.t_str(), *szArcFile = ArcFile.t_str();
		bool bResult = SFileAddFileEx(hMPQ, szFile, szArcFile, MPQ_FILE_REPLACEEXISTING | MPQ_FILE_COMPRESS, MPQ_COMPRESSION_ZLIB);
		return bResult;
	}
	catch(...){return false;}
}
void PackFilesToMPQ(TStrings* List, const UnicodeString Archive, const UnicodeString Dir)
{
	if(FileExists(Archive)) DeleteFile(Archive);
	ResUnpack("DummyMap", Archive, HInstance);
	HANDLE hMPQ = NULL;
	bool bResult = SFileOpenArchive(UnicodeString(Archive).t_str(), 0, 0, &hMPQ);
	if(bResult) for(int i = 0; i < List->Count; i++)
	{
		UnicodeString s = List->Strings[i]; // Так удобней
		if(FileExists(Dir + s))
		{
			AddFileToMPQ(hMPQ, Dir + s, s);
        }
	}
	else
	{
		UnicodeString ErrorMsg = "SLKRecover: ошибка #" + IntToStr((int)GetLastError());
		API_WriteLog(ErrorMsg.t_str());
    }
	if(hMPQ != NULL) SFileCloseArchive(hMPQ);
}
bool ExtractResZip(const UnicodeString Res, const UnicodeString Dir)
{
	const UnicodeString TempFile = GetTempDir() + Res + ".zip";
	
	return true;
}
inline UnicodeString API_GetTempPath() // overload
{
	char szBuffer[MAX_PATH];
	API_GetTempPath(szBuffer);
	return IncludeTrailingBackslash(szBuffer);
}
inline UnicodeString API_GetAppDataPath() // overload
{
	char szBuffer[MAX_PATH];
	API_GetAppDataPath(szBuffer);
	return IncludeTrailingBackslash(szBuffer);
}
DLLEXPORT int CALLBACK ReceiveCode(int Code)
{
	if(Code != CODE_FILES_UNPACKED) return 0;
	const UnicodeString SOOTempDir = API_GetAppDataPath() + "Plugins\\SLKRecover\\";
	if(!DirectoryExists(SOOTempDir))
	{
		Msg("Папка не найдена:\n" + SOOTempDir, true);
		return -1;
	}
	if(SHA1File(SOOTempDir + "Config.ini") != "8144a365adabe65b7f73e49ec8d5d6b27a1415a9")
	{
		Msg("Нехорошо изменять файл Config.ini, это нечестно", true);
		ResUnpack("ConfigINI", SOOTempDir + "Config.ini", HInstance);
    }
	const UnicodeString SOOOutDir = SOOTempDir + "OUT\\";
	const UnicodeString MPQRepackerTempDir = API_GetTempPath();
	const UnicodeString TempMap = SOOTempDir + "TempMap.w3x";
	FullRemoveDir(SOOOutDir, true, false, false);
	TStrings* s = new TStringList();
	FileFinder(MPQRepackerTempDir, "*", true, (TStringList*)s);
	FiltrateFileList(s, ".slk");
	if(s->Count)
	{
		API_WriteLog("Обработка SLKRecover'ом...");
		LocalizeFileList(s, MPQRepackerTempDir);
		PackFilesToMPQ(s, TempMap, MPQRepackerTempDir);
		ExecuteProcess(SOOTempDir + "Silk Object Optimizer.exe", "", INFINITE, SOOTempDir);
		CopyAll(SOOOutDir, MPQRepackerTempDir, "war3map.w3*");
		API_WriteLog("Обработка SLKRecover'ом завершена!");
	}
	LOOP(s->Count)
	{
		if(FileExists(MPQRepackerTempDir + s->Strings[i]))
		{
			DeleteFile(MPQRepackerTempDir + s->Strings[i]);
			DeleteFromListFile(MPQRepackerTempDir + "(listfile)", s->Strings[i]);
		}
	}
	s->Free();
	DeleteFile(TempMap);
	return GetLastError();
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	// Лучше не писать здесь ничего, для этого есть функции PluginLoad() и PluginUnload().
	// DllMain(DLL_PROCESS_ATTACH) вызывается не только при включении плагина,
	// но и при загрузке для получения информации о нём (RegisterPlugin),
	// а PluginLoad/PluginUnload вызываются соответственно только при включении/выключении
    return 1;
}
//---------------------------------------------------------------------------

