object frmReplacesEdit: TfrmReplacesEdit
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = #1053#1072#1089#1090#1088#1086#1081#1082#1080
  ClientHeight = 250
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
  object lblRegexHelp: TLabel
    Left = 16
    Top = 224
    Width = 54
    Height = 13
    Cursor = crHandPoint
    Caption = 'RegEx help'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsUnderline]
    ParentFont = False
    OnClick = lblRegexHelpClick
  end
  object mmoSource: TMemo
    Left = 13
    Top = 5
    Width = 225
    Height = 209
    Lines.Strings = (
      'mmoSource')
    ScrollBars = ssBoth
    TabOrder = 0
    OnChange = mmoSourceChange
  end
  object mmoReplace: TMemo
    Left = 244
    Top = 5
    Width = 225
    Height = 209
    Lines.Strings = (
      'mmoSource')
    ScrollBars = ssBoth
    TabOrder = 1
  end
  object btnSave: TButton
    Left = 163
    Top = 220
    Width = 75
    Height = 25
    Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100
    ModalResult = 1
    TabOrder = 2
  end
  object btnCancel: TButton
    Left = 244
    Top = 220
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 3
  end
end
