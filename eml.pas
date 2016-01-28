//e-mail: vladsch@lens.spb.ru
//Использование: подключите этот модуль в файл проекта (*.dpr) в секцию uses
unit eml;

interface
uses
  Windows, Messages;

procedure ShutDown;
function WindowProc(hWnd,__Msg,wParam,lParam:Longint):Longint; stdcall;
procedure emulator;

var
wClass,wclass2,wclass3,wclass4:   TWndClass;  // class struct for main window
hInst:    HWND;
__Msg:      TMSG;       // message struct

implementation

procedure ShutDown;
begin
  UnRegisterClass('TAppBuilder',hInst);
  UnRegisterClass('TApplication',hInst);
  UnRegisterClass('TPropertyInspector',hInst);
  UnRegisterClass('TAlignPalette',hInst);
  ExitProcess(hInst); //end program
end;

function WindowProc(hWnd,__Msg,wParam,lParam:Longint):Longint; stdcall;
begin
  Result:=DefWindowProc(hWnd,__Msg,wParam,lParam);
end;

procedure emulator;
begin
  hInst:=GetModuleHandle(nil); // get the application instance

  with wClass do
  begin
    Style:=         0;
    hIcon:=         LoadIcon(hInst,'MAINICON');
    lpfnWndProc:=   @WindowProc;
    hInstance:=     hInst;
    hbrBackground:= COLOR_BTNFACE+1;
    lpszClassName:= 'TAppBuilder';
    hCursor:=       LoadCursor(0,IDC_ARROW);
  end;

  with wClass2 do
  begin
    Style:=         0;
    hIcon:=         LoadIcon(hInst,'MAINICON');
    lpfnWndProc:=   @WindowProc;
    hInstance:=     hInst;
    hbrBackground:= COLOR_BTNFACE+1;
    lpszClassName:= 'TApplication';
    hCursor:=       LoadCursor(0,IDC_ARROW);
  end;

  with wClass3 do
  begin
    Style:=         0;
    hIcon:=         LoadIcon(hInst,'MAINICON');
    lpfnWndProc:=   @WindowProc;
    hInstance:=     hInst;
    hbrBackground:= COLOR_BTNFACE+1;
    lpszClassName:= 'TAlignPalette';
    hCursor:=       LoadCursor(0,IDC_ARROW);
  end;

  with wClass4 do
  begin
    Style:=         0;
    hIcon:=         LoadIcon(hInst,'MAINICON');
    lpfnWndProc:=   @WindowProc;
    hInstance:=     hInst;
    hbrBackground:= COLOR_BTNFACE+1;
    lpszClassName:= 'TPropertyInspector';
    hCursor:=       LoadCursor(0,IDC_ARROW);
  end;

  RegisterClass(wClass);
  RegisterClass(wClass2);
  RegisterClass(wClass3);
  RegisterClass(wClass4);

  CreateWindow(
    'TAppBuilder',           // Registered Class Name
    'Delphi',                       // Title of Window
    WS_POPUP,              // Make it Visible
    -1,                      // Left
    -1,                      // Top
    0,                      // Width
    0,                      // Height
    0,                       // Parent Window Handle
    0,                       // Handle of Menu
    hInst,                   // Application Instance
    nil);                    // Structure for Creation Data


   CreateWindow(
    'TApplication',           // Registered Class Name
    'Embarcadero RAD Studio 2010',                       // Title of Window
    WS_POPUP,              // Make it Visible
    -1,                      // Left
    -1,                      // Top
    0,                      // Width
    0,                      // Height
    0,                       // Parent Window Handle
    0,                       // Handle of Menu
    hInst,                   // Application Instance
    nil);

    CreateWindow(
    'TAlignPalette',           // Registered Class Name
    'Embarcadero RAD Studio 2010',                       // Title of Window
    WS_POPUP,              // Make it Visible
    -1,                      // Left
    -1,                      // Top
    0,                      // Width
    0,                      // Height
    0,                       // Parent Window Handle
    0,                       // Handle of Menu
    hInst,                   // Application Instance
    nil);

    CreateWindow(
    'TPropertyInspector',           // Registered Class Name
    'Embarcadero RAD Studio 2010',                       // Title of Window
    WS_POPUP,              // Make it Visible
    -1,                      // Left
    -1,                      // Top
    0,                      // Width
    0,                      // Height
    0,                       // Parent Window Handle
    0,                       // Handle of Menu
    hInst,                   // Application Instance
    nil)
end;

initialization
	emulator
finalization
	shutdown
end.