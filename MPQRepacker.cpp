//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#pragma once
#include <tchar.h>
//---------------------------------------------------------------------------
USEFORM("UPluginForm.cpp", frmPlugins);
USEFORM("USettingsForm.cpp", frmSettings);
USEFORM("UMainForm.cpp", MainForm);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
		HANDLE hMutex = OpenMutex( MUTEX_ALL_ACCESS, 0, "MPQ_REPACKER");
		if(!hMutex)
		{
			hMutex = CreateMutex(0, 0, "MPQ_REPACKER");
		}
		else
		{
			 HWND hWnd = FindWindow( 0, "MPQ RePacker v1.2 by ZxZ666");
			 if (hWnd) {SetForegroundWindow(hWnd);return 0;}
			 ExitProcess(0);
			 return 0;
		}
		// Распаковка StormLib.dll из ресурсов:
		UnicodeString StormLibPath = GetTempDir() + "StormLib.dll";
		if(ResUnpack("StormLib", StormLibPath)) LoadLibrary(StormLibPath.t_str());
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TfrmPlugins), &frmPlugins);
		Application->Run();
		ReleaseMutex(hMutex);
		ExitProcess(0); // Иначе почему-то остается висеть в списке процессов
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
