//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
#pragma argsused
#include "MPQRepacker_DynamicAPI.h"
//---------------------------------------------------------------------------
HINSTANCE HInstance = NULL;
//---------------------------------------------------------------------------
#define DLLEXPORT extern "C" __declspec(dllexport)
//---------------------------------------------------------------------------
DLLEXPORT int CALLBACK DummySearch()
{
	API_WriteLog("[SamplePlugin override]: ������ ����� ����� ����� ������ �����");
	// ���������: ����� ���� ������� ��������� �������� ������ ������ ��
	// %APPDATA%\MPQ Repacker\FILELIST_CACHE.txt
	return GetLastError();
}
DLLEXPORT int CALLBACK DummyUnpack()
{
	API_WriteLog("[SamplePlugin override]: � ������ ����������");
	// ���������: ������������� ����� ���� �� ��������� �����
	// ���� � ������� ����� �������� � ������� API-������� API_GetTempPath()
	return GetLastError();
}
DLLEXPORT int CALLBACK DummyPack()
{
	API_WriteLog("[SamplePlugin override]: � ���� ��������");
	// ���������: �������� ����� ���� �� ��������� ����� API_GetTempPath()
	// � ���� �����, ���� � ������� ��������� API_GetDestMap()
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginLoad()
{
	API_WriteLog("SamplePlugin 1.0 � ZxZ666");
	// ��������� - ������ �������
	// ��� ��������� ������ � ReceiveCode �������� ��������� ��������,
	// � � ��������-���������� �� �������� ��������������� ����� ������ ���������
	// �.�. �����, ����������, � ��������
	TOverrideInfo inf[3];
	inf[0].Code = OVERRIDE_SEARCH;
	inf[0].Function = DummySearch;
	inf[0].hPlugin = HInstance;
	//////////////////////////////
	inf[1].Code = OVERRIDE_UNPACK;
	inf[1].Function = DummyUnpack;
	inf[1].hPlugin = HInstance;
	//////////////////////////////
	inf[2].Code = OVERRIDE_PACK;
	inf[2].Function = DummyPack;
	inf[2].hPlugin = HInstance;
	//////////////////////////////
	API_RegisterOverride(inf[0]);
	API_RegisterOverride(inf[1]);
	API_RegisterOverride(inf[2]);
	// ������� ��������� �� ����, ��� ������� ���� ��������� ���� ��
	// ��������� ������� hPlugin, � ���� ��� - �� ��������� ������
	// �� ���������������� - �.�. ��������� ��������� ����������� hPlugin
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginUnload()
{
	API_WriteLog("[ SamplePlugin unloaded ]");
	return GetLastError();
}
DLLEXPORT TPluginInfo CALLBACK RegisterPlugin()
{
	TPluginInfo Result;
	Result.szName = "Sample plugin\0";									// �������� �������
	Result.szAuthor = "ZxZ666\0";										// �����
	Result.szDescription = "��� ������ �� �������� ������-������.\n(������� F1 ����� ������� ����� � �����������)\0";	// ��������
    // ��������� ����������:
	Result.bHookFileFind = true;	// CODE_FILE_SEARCH_START � CODE_FILE_SEARCH_END
	Result.bHookFileUnpack = true;	// CODE_FILES_UNPACKED
	Result.bHookFilePack = true;	// CODE_FILES_PACKED
	return Result;
}
DLLEXPORT int CALLBACK ReceiveCode(int Code)
{
	char szSourceMap[MAX_PATH], szDestMap[MAX_PATH], szTempDir[MAX_PATH], szAppDataDir[MAX_PATH];
	API_GetSourceMap(szSourceMap);		// ���� �� �������� �����
	API_GetDestMap(szDestMap);			// ���� �� ���������� (���� ����� ��������� �������������� �����)
	API_GetTempPath(szTempDir);			// ���� �� ��������� � ���������� ��������� �����
	API_GetAppDataPath(szAppDataDir);	// ���� �� ����� %APPDATA%\MPQ Repacker\ - ��� �������� �������, ���-����, ��������� � ��������� ����-���� FILELIST_CACHE.txt
	if(Code == CODE_FILE_SEARCH_START) MessageBox(NULL, "������ ������ ���� ������", "SamplePlugin", MB_ICONINFORMATION | MB_OK);
	if(Code == CODE_FILE_SEARCH_END) MessageBox(NULL, "����� ������ ������\n(����� �������������� %APPDATA%\\MPQ Repacker\\FILELIST_CACHE.txt - ��������� ��� ������������)", "SamplePlugin", MB_ICONINFORMATION | MB_OK);
	if(Code == CODE_FILES_UNPACKED) MessageBox(NULL, "����� ����������� �� ��������� �����\n(�� ����� �������������� �� ������� ������)", "SamplePlugin", MB_ICONINFORMATION | MB_OK);
	if(Code == CODE_FILES_PACKED) MessageBox(NULL, "����� ��������� � MPQ �����\n(������ ��� ����� ��������������)", "SamplePlugin", MB_ICONINFORMATION | MB_OK);
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginSettings() // ���������
{
	MessageBoxA(GetForegroundWindow(), "��� ������ ���� ���� �������� �������.", "SamplePlugin", MB_OK | MB_ICONINFORMATION);
	return GetLastError();
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	HInstance = hinstDLL;
	// ����� �� ������ ����� ������, ��� ����� ���� ������� PluginLoad() � PluginUnload().
	// DllMain(DLL_PROCESS_ATTACH) ���������� �� ������ ��� ��������� �������,
	// �� � ��� �������� ��� ��������� ���������� � �� (RegisterPlugin),
	// � PluginLoad/PluginUnload ���������� �������������� ������ ��� ���������/����������
    return 1;
}
//---------------------------------------------------------------------------
