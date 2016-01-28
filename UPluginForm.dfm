object frmPlugins: TfrmPlugins
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = #1055#1083#1072#1075#1080#1085#1099
  ClientHeight = 212
  ClientWidth = 457
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnDestroy = FormDestroy
  OnKeyUp = FormKeyUp
  PixelsPerInch = 96
  TextHeight = 13
  object lblPlgInfo: TLabel
    AlignWithMargins = True
    Left = 239
    Top = 3
    Width = 215
    Height = 174
    AutoSize = False
    Caption = #1040#1074#1090#1086#1088': '#13#10#1053#1072#1079#1074#1072#1085#1080#1077': '#13#10#1054#1087#1080#1089#1072#1085#1080#1077': '
    WordWrap = True
  end
  object chklstPlugins: TCheckListBox
    Left = 0
    Top = 0
    Width = 233
    Height = 212
    OnClickCheck = chklstPluginsClickCheck
    Align = alLeft
    ItemHeight = 13
    TabOrder = 0
    OnClick = chklstPluginsClick
  end
  object btnSettings: TButton
    Left = 239
    Top = 184
    Width = 215
    Height = 25
    Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080' '#1087#1083#1072#1075#1080#1085#1072
    Enabled = False
    TabOrder = 1
    OnClick = btnSettingsClick
  end
end
