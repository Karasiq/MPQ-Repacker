//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
#ifndef UPluginFrameworkH
#define UPluginFrameworkH
//---------------------------------------------------------------------------
#define DLLEXPORT extern "C" __declspec(dllexport) int __stdcall
//---------------------------------------------------------------------------
#include <System.hpp>
#include <Classes.hpp>
//---------------------------------------------------------------------------
// ����������� ������������ � �������:
typedef int __stdcall (*OverrideFunc)(void);
struct TPluginInfo
{
	char* szName;
	char* szDescription;
	char* szAuthor;
    bool bHookFileFind;
	bool bHookFileUnpack;
	bool bHookFilePack;
};
struct TOverrideInfo
{
	DWORD Code;
	OverrideFunc Function;
	HINSTANCE hPlugin;
};
// ���� MPQ Repacker
#define CODE_FILE_SEARCH_START 0	// ������ ������ ������
#define CODE_FILE_SEARCH_END 1		// ����� ������ ������
#define CODE_FILES_UNPACKED 2		// ����� ����������� �� ��������� �����
#define CODE_FILES_PACKED 3			// ����� ���������� � MPQ �����
// -----------------
#define OVERRIDE_SEARCH	0			// ������ ����� ��������
#define OVERRIDE_UNPACK 1 			// ������������� ����� ��������
#define OVERRIDE_PACK 2 			// ������������ ����� ��������
// -----------------
// ����������� ������������ ������ � ���������:
enum TPFCondition {pfcFileFindHooked, pfcFileUnpackHooked, pfcFilePackHooked}; // Plugin Framework Condition
struct TPlugin
{
	UnicodeString Path;
	TPluginInfo Info;
	bool Enabled;
	HINSTANCE hPlugin;
};
struct TOverride
{
	bool Enabled;
	int PluginIndex;
	OverrideFunc Function;
};
struct TMPQRepackerOverrides
{
	TOverride Search;
	TOverride Unpack;
	TOverride Pack;
};
class TPluginFramework : public TObject
{
	protected:
		void __fastcall FindPlugins();
		TPlugin* PluginsList;
		unsigned int PluginsCount;
		UnicodeString Dir;
		TPluginInfo RequestPlgInfo(UnicodeString Path);
		void UnregisterOverridesByIndex(const int Index);
	public:
		TMPQRepackerOverrides Overrides;
		__fastcall TPluginFramework(const UnicodeString DirWithPlugins);
		__fastcall ~TPluginFramework();
		void __fastcall GetPluginList(TStrings* S);
		int IndexByName(const UnicodeString Name); // ��� ������ ���������� -1
		int IndexByHInstance(HINSTANCE hPlg); // ��� ������ ���������� -1
		void __fastcall UnloadPlugin(const int Index);
		void __fastcall LoadPlugin(const int Index);
		TPluginInfo GetPluginInfo(const int Index);
		int __fastcall SendCode(const int Index, const int Code); // ���������� ����� �������
		void SendCodeToAll(const int Code, const TPFCondition IfFlag);
		int CallSettings(const unsigned Index, bool Execute = true); // ���� Execute == false, �� ������ �������� ����� � ��������� FALSE � ������ ���� ��� ������� ��������
};
//---------------------------------------------------------------------------
extern TPluginFramework* PluginManager; // ���������� ������-��������
//---------------------------------------------------------------------------
#endif
