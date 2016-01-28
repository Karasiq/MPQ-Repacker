unit URecursiveSearch;

interface
uses Classes, SysUtils;
procedure FileFinder(Path: string;maska: string;recur: boolean ;sl: TStringList);
function FullRemoveDir(Dir: string; DeleteAllFilesAndFolders, StopIfNotAllDeleted, RemoveRoot: boolean): Boolean;

implementation
{ **** UBPFD *********** by delphibase.endimus.com ****
>> �������� ��������� �������� ������ � �������������

�������� ������������ ����������� - ������� �������� ���� ����.
�������� ���������� ����������:

-DeleteAllFilesAndFolder - ���� TRUE �� �������� ����� �����������
������� ��� ��������� �������� faArchive ������ ����� ��� �����
����� ���(�) ���������;

-StopIfNotAllDeleted - ���� TRUE �� ������ ������� �����������
������������ ���� �������� ������ �������� ���� �� ������ ����� ��� �����;

-RemoveRoot - ���� TRUE, ��������� �� ������������� �������� �����.

�����������: FileCtrl, SysUtils
�����:       lipskiy, lipskiy@mail.ru, ICQ:51219290, �����-���������
Copyright:   ����������� ��������� (lipskiy)
����:        26 ������ 2002 �.
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
  // ��������� ���� � ����� � ������ ����� - "��� ����� � ����������"
  Dir := IncludeTrailingBackslash(Dir);
  i := FindFirst(Dir + '*', faAnyFile, SRec);
  try
    while i = 0 do
    begin
      // �������� ������ ���� � ����� ��� ����������
      FN := Dir + SRec.Name;
      // ���� ��� ����������
      if SRec.Attr = faDirectory then
      begin
        // ����������� ����� ���� �� ������� � ������ �������� �����
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
      else // ����� ������� ����
      begin
        if DeleteAllFilesAndFolders then
          FileSetAttr(FN, faArchive);
        Result := SysUtils.DeleteFile(FN);
        if not Result and StopIfNotAllDeleted then
          exit;
      end;
      // ����� ��������� ���� ��� ����������
      i := FindNext(SRec);
    end;
  finally
    SysUtils.FindClose(SRec);
  end;
  if not Result then
    exit;
  if RemoveRoot then // ���� ���������� ������� ������ - �������
    if not RemoveDir(Dir) then
      Result := false;
end;
procedure FileFinder(Path: string;maska: string;recur: boolean ;sl: TStringList);
{
Path  - ���� ������
maska - ����� ������
recur - true  - ������ � � ������������
        false - ������ ������ � ��������� �����
sl    - ������ ��������� ������
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
