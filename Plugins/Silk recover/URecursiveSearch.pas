unit URecursiveSearch;

interface
uses Classes, SysUtils;
procedure FileFinder(Path: string;maska: string;recur: boolean ;sl: TStringList);
function FullRemoveDir(Dir: string; DeleteAllFilesAndFolders, StopIfNotAllDeleted, RemoveRoot: boolean): Boolean;

implementation
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
   FindClose(sr);
end;

end.
