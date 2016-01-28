unit URecursiveSearch;

interface
uses Classes, SysUtils, StdCtrls, Windows, Forms;
procedure CaptureConsoleOutput(const ACommand, AParameters: String; AMemo: TMemo);
procedure FileFinder(Path: string;maska: string;recur: boolean ;sl: TStringList);
function FullRemoveDir(Dir: string; DeleteAllFilesAndFolders, StopIfNotAllDeleted, RemoveRoot: boolean): Boolean;

implementation
procedure CaptureConsoleOutput(const ACommand, AParameters: String; AMemo: TMemo);
 const
   CReadBuffer = 2400;
 var
   saSecurity: TSecurityAttributes;
   hRead: THandle;
   hWrite: THandle;
   suiStartup: TStartupInfo;
   piProcess: TProcessInformation;
   pBuffer: array[0..CReadBuffer] of AnsiChar;
   dRead: DWord;
   dRunning: DWord;
 begin
   saSecurity.nLength := SizeOf(TSecurityAttributes);
   saSecurity.bInheritHandle := True;
   saSecurity.lpSecurityDescriptor := nil;

   if CreatePipe(hRead, hWrite, @saSecurity, 0) then
   begin
     FillChar(suiStartup, SizeOf(TStartupInfo), #0);
     suiStartup.cb := SizeOf(TStartupInfo);
     suiStartup.hStdInput := hRead;
     suiStartup.hStdOutput := hWrite;
     suiStartup.hStdError := hWrite;
     suiStartup.dwFlags := STARTF_USESTDHANDLES or STARTF_USESHOWWINDOW;
     suiStartup.wShowWindow := SW_HIDE;

     if CreateProcess(nil, PChar(ACommand + ' ' + AParameters), @saSecurity,
       @saSecurity, True, NORMAL_PRIORITY_CLASS, nil, nil, suiStartup, piProcess)
       then
     begin
       repeat
         dRunning  := WaitForSingleObject(piProcess.hProcess, 100);
         Application.ProcessMessages();
         repeat
           dRead := 0;
           CloseHandle(piProcess.hProcess);
           piProcess.hProcess := OpenProcess(PROCESS_ALL_ACCESS, False, piProcess.dwProcessId);
           if piProcess.hProcess > 0 then
           begin
             ReadFile(hRead, pBuffer[0], CReadBuffer, dRead, nil);
             pBuffer[dRead] := #0;
           end;

           OemToAnsi(pBuffer, pBuffer);
           AMemo.Lines.Add(String(pBuffer));
         until (dRead < CReadBuffer);
       until (dRunning <> WAIT_TIMEOUT);
       CloseHandle(piProcess.hProcess);
       CloseHandle(piProcess.hThread);
     end;

     CloseHandle(hRead);
     CloseHandle(hWrite);
   end;
end;
{ **** UBPFD *********** by delphibase.endimus.com ****
>> Удаление непустого каталога вместе с подкаталогами

Удаление подкаталогов рекурсивное - функция вызывает саму себя.
Описание назначения агрументов:

-DeleteAllFilesAndFolder - если TRUE то функцией будут предприняты
попытки для установки атрибута faArchive любому файлу или папке
перед его(её) удалением;

-StopIfNotAllDeleted - если TRUE то работа функции моментально
прекращается если возникла ошибка удаления хотя бы одного файла или папки;

-RemoveRoot - если TRUE, указывает на необходимость удаления корня.

Зависимости: FileCtrl, SysUtils
Автор:       lipskiy, lipskiy@mail.ru, ICQ:51219290, Санкт-Петербург
Copyright:   Собственное написание (lipskiy)
Дата:        26 апреля 2002 г.
***************************************************** }
function FullRemoveDir(Dir: string; DeleteAllFilesAndFolders,
  StopIfNotAllDeleted, RemoveRoot: boolean): Boolean;
var
  i: Integer;
  SRec: TSearchRec;
  FN: string;
begin
  Result := False;
  if not DirectoryExists(Dir) then
    exit;
  Result := True;
  // Добавляем слэш в конце и задаем маску - "все файлы и директории"
  Dir := IncludeTrailingBackslash(Dir);
  i := FindFirst(Dir + '*', faAnyFile, SRec);
  try
    while i = 0 do
    begin
      // Получаем полный путь к файлу или директорию
      FN := Dir + SRec.Name;
      // Если это директория
      if SRec.Attr = faDirectory then
      begin
        // Рекурсивный вызов этой же функции с ключом удаления корня
        if (SRec.Name <> '') and (SRec.Name <> '.') and (SRec.Name <> '..') then
        begin
          if DeleteAllFilesAndFolders then
            FileSetAttr(FN, faArchive);
          Result := FullRemoveDir(FN, DeleteAllFilesAndFolders,
            StopIfNotAllDeleted, True);
          if not Result and StopIfNotAllDeleted then
            exit;
        end;
      end
      else // Иначе удаляем файл
      begin
        if DeleteAllFilesAndFolders then
          FileSetAttr(FN, faArchive);
        Result := SysUtils.DeleteFile(FN);
        if not Result and StopIfNotAllDeleted then
          exit;
      end;
      // Берем следующий файл или директорию
      i := FindNext(SRec);
    end;
  finally
    SysUtils.FindClose(SRec);
  end;
  if not Result then
    exit;
  if RemoveRoot then // Если необходимо удалить корень - удаляем
    if not RemoveDir(Dir) then
      Result := false;
end;
procedure FileFinder(Path: string;maska: string;recur: boolean ;sl: TStringList);
{
Path  - путь поиска
maska - маска поиска
recur - true  - искать и в подкаталогах
        false - искать только в указанной папке
sl    - список найденных файлов
}
var sr : TSearchRec;
begin
if (Path[Length(Path)]='\') then SetLength(Path,Length(Path)-1);
if FindFirst(Path+'\'+maska,faAnyFile,sr)=0
   then
      begin
         if (sr.Attr or faDirectory)=sr.Attr then
            begin
            if (recur=true) then
               begin
                  if (sr.name<>'.') and (sr.name<>'..') then
                  FileFinder(path+'\'+sr.Name,maska,recur,sl);
               end
            end
         else
         sl.Add(Path+'\'+sr.Name);
         while FindNext(sr) = 0 do
         if (sr.Attr or faDirectory)=sr.Attr then
            begin
            if (recur=true) then
               begin
                  if (sr.name<>'.') and (sr.name<>'..') then
                  FileFinder(path+'\'+sr.Name,maska,recur,sl);
               end
            end
         else
         sl.Add(Path+'\'+sr.Name);
      end;
   //FindClose(sr);
end;

end.
