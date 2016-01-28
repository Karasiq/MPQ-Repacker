//---------------------------------------------------------------------------
#pragma hdrstop
//---------------------------------------------------------------------------
#include "UApiHook.h"
#include <ntstatus.h>
//---------------------------------------------------------------------------
OldCode HookAPI(PVOID API, PVOID NewFunc)
{
	try
	{
		if(API == NULL || NewFunc == NULL)
		{
			OldCode __NULLCODE;
			__NULLCODE.One = 0;
			__NULLCODE.TWO = 0;
			return __NULLCODE;
		}
		far_jmp Fnjp;
		OldCode Old;

		DWORD dwOldProtect = 0;
		SIZE_T lpNumberOfBytesWritten = 0;
        VirtualProtect(API, sizeof(far_jmp), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		ReadProcessMemory(GetCurrentProcess(), API, &Old, sizeof(OldCode), NULL);

		Fnjp.PushOp  = 0x68;
		Fnjp.PushArg = NewFunc;
		Fnjp.RetOp   = 0xC3;

		if(API != NULL && NewFunc != NULL) WriteProcessMemory(GetCurrentProcess(), API, &Fnjp, sizeof(far_jmp), &lpNumberOfBytesWritten);
		return Old;
	}
	catch(...){}
}
//---------------------------------------------------------------------------
void UnhookAPI(OldCode OldProc, PVOID API)
{
	try
	{
		if(API == NULL) return;
		DWORD dwOldProtect = 0;
		VirtualProtect(API, sizeof(OldCode), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		WriteProcessMemory(GetCurrentProcess(), API, &OldProc, sizeof(OldCode), NULL);
	}
	catch(...){}
}
//---------------------------------------------------------------------------
#pragma package(smart_init)
