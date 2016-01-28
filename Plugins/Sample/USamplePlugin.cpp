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
	API_WriteLog("[SamplePlugin override]: Плагин кагбе нашел файлы вместо проги");
	// Подсказка: После этой функции программа загрузит список файлов из
	// %APPDATA%\MPQ Repacker\FILELIST_CACHE.txt
	return GetLastError();
}
DLLEXPORT int CALLBACK DummyUnpack()
{
	API_WriteLog("[SamplePlugin override]: А теперь распаковал");
	// Подсказка: распаковывать файлы надо во временную папку
	// путь к которой можно получить с помощью API-функции API_GetTempPath()
	return GetLastError();
}
DLLEXPORT int CALLBACK DummyPack()
{
	API_WriteLog("[SamplePlugin override]: И даже упаковал");
	// Подсказка: паковать файлы надо из временной папки API_GetTempPath()
	// в файл карты, путь к которой возращает API_GetDestMap()
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginLoad()
{
	API_WriteLog("SamplePlugin 1.0 © ZxZ666");
	// Оверрайды - замены функций
	// Код обработки файлов в ReceiveCode помогает программе работать,
	// А в функциях-оверрайдах он заменяет соответствующие этапы работы программы
	// Т.е. поиск, распаковку, и упаковку
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
	// Снимать оверрайды не надо, это сделает сама программа если вы
	// Правильно указали hPlugin, а если нет - то оверрайды просто
	// Не зарегистрируются - т.к. программа проверяет правильнось hPlugin
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
	Result.szName = "Sample plugin\0";									// Название плагина
	Result.szAuthor = "ZxZ666\0";										// Автор
	Result.szDescription = "Это ничего не делающий плагин-пример.\n(нажмите F1 чтобы открыть архив с исходниками)\0";	// Описание
    // Настройки прерываний:
	Result.bHookFileFind = true;	// CODE_FILE_SEARCH_START и CODE_FILE_SEARCH_END
	Result.bHookFileUnpack = true;	// CODE_FILES_UNPACKED
	Result.bHookFilePack = true;	// CODE_FILES_PACKED
	return Result;
}
DLLEXPORT int CALLBACK ReceiveCode(int Code)
{
	char szSourceMap[MAX_PATH], szDestMap[MAX_PATH], szTempDir[MAX_PATH], szAppDataDir[MAX_PATH];
	API_GetSourceMap(szSourceMap);		// Путь до исходной карты
	API_GetDestMap(szDestMap);			// Путь до результата (куда будет сохранена перепакованная карта)
	API_GetTempPath(szTempDir);			// Путь до указанной в настройках временной папки
	API_GetAppDataPath(szAppDataDir);	// Путь до папки %APPDATA%\MPQ Repacker\ - там хранятся плагины, чит-паки, настройки и временный файл-лист FILELIST_CACHE.txt
	if(Code == CODE_FILE_SEARCH_START) MessageBox(NULL, "Начало поиска имен файлов", "SamplePlugin", MB_ICONINFORMATION | MB_OK);
	if(Code == CODE_FILE_SEARCH_END) MessageBox(NULL, "Конец поиска файлов\n(можно модифицировать %APPDATA%\\MPQ Repacker\\FILELIST_CACHE.txt - программа его перезагрузит)", "SamplePlugin", MB_ICONINFORMATION | MB_OK);
	if(Code == CODE_FILES_UNPACKED) MessageBox(NULL, "Файлы распакованы во временную папку\n(их можно модифицировать из плагина сейчас)", "SamplePlugin", MB_ICONINFORMATION | MB_OK);
	if(Code == CODE_FILES_PACKED) MessageBox(NULL, "Файлы упакованы в MPQ архив\n(сейчас его можно оптимизировать)", "SamplePlugin", MB_ICONINFORMATION | MB_OK);
	return GetLastError();
}
DLLEXPORT int CALLBACK PluginSettings() // Настройки
{
	MessageBoxA(GetForegroundWindow(), "Тут должно быть окно настроек плагина.", "SamplePlugin", MB_OK | MB_ICONINFORMATION);
	return GetLastError();
}
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fwdreason, LPVOID lpvReserved)
{
	HInstance = hinstDLL;
	// Лучше не писать здесь ничего, для этого есть функции PluginLoad() и PluginUnload().
	// DllMain(DLL_PROCESS_ATTACH) вызывается не только при включении плагина,
	// но и при загрузке для получения информации о нём (RegisterPlugin),
	// а PluginLoad/PluginUnload вызываются соответственно только при включении/выключении
    return 1;
}
//---------------------------------------------------------------------------
