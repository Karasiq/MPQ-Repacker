unit MPQRepacker_DynamicAPI;
interface
uses Windows;
const
	CODE_FILE_SEARCH_START = 0;
	CODE_FILE_SEARCH_END = 1;
	CODE_FILE_UNPACKED = 2;
	CODE_FILE_PACKED = 3;
	OVERRIDE_SEARCH = 0;
	OVERRIDE_UNPACK = 1;
	OVERRIDE_PACK = 2;
type
	TPluginInfo = record
		szName:PChar;
		szDescription:PChar;
		szAuthor:PChar;
		bHookFileFind:Boolean;
		bHookFileUnpack:Boolean;
		bHookFilePack:Boolean;
	end;
	TMPQRepackerOption = (bUseXdep, bReplaceW3I, bReplaceDOO, bInsertCheats, bPause, bDeleteTemp, bUnpackUnknown);

function API_WriteLog(szBuffer:PChar):Integer
function API_GetTempPath(szBuffer:PChar):Integer
function API_GetSourceMap(szBuffer:PChar):Integer
function API_GetDestMap(szBuffer:PChar):Integer
function API_RegisterOverride(Info:TOverrideInfo):Integer
function API_GetFlagState(Info:TOverrideInfo):Integer
function API_SetFlagState(Info:TOverrideInfo;State:Boolean):Integer
//---------------------------------------------------------------------------
implementation
{$L OMF_MPQRepacker_DynamicAPI.obj}
end.