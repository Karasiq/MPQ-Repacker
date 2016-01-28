object frmSettings: TfrmSettings
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsDialog
  Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080' Extendable Cheatpack Detector:'
  ClientHeight = 253
  ClientWidth = 482
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object mmoCheatStrings: TMemo
    Left = 8
    Top = 6
    Width = 466
    Height = 209
    TabOrder = 0
  end
  object chkReplaceActivator: TCheckBox
    Left = 91
    Top = 227
    Width = 129
    Height = 17
    Caption = #1047#1072#1084#1077#1085#1103#1090#1100' '#1072#1082#1090#1080#1074#1072#1090#1086#1088
    TabOrder = 3
  end
  object btnOk: TButton
    Left = 235
    Top = 223
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
  end
  object btnCancel: TButton
    Left = 316
    Top = 223
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 2
  end
end
