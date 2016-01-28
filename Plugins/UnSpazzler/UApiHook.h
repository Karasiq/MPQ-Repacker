//---------------------------------------------------------------------------
#ifndef UApiHookH
#define UApiHookH
//---------------------------------------------------------------------------
#include <windows.h>
//---------------------------------------------------------------------------
#pragma pack (push, 1)
typedef struct _far_jmp{
    BYTE  PushOp;
    PVOID PushArg;
    BYTE  RetOp;
} far_jmp, *pfar_jmp;
typedef struct _OldCode{
    USHORT One;
    ULONG  TWO;
} OldCode, *POldCode;
#pragma pack (pop)
//---------------------------------------------------------------------------
OldCode HookAPI(PVOID API, PVOID NewFunc);
void UnhookAPI(OldCode OldProc, PVOID API);
//---------------------------------------------------------------------------
#endif
