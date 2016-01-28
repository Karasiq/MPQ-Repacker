object frmSettings: TfrmSettings
  Left = 0
  Top = 0
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsSingle
  Caption = #1044#1086#1087#1086#1083#1085#1080#1090#1077#1083#1100#1085#1099#1077' '#1085#1072#1089#1090#1088#1086#1081#1082#1080
  ClientHeight = 295
  ClientWidth = 192
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
  object lblCompression: TLabel
    Left = 16
    Top = 9
    Width = 43
    Height = 13
    Caption = #1057#1078#1072#1090#1080#1077':'
  end
  object lblAnalyze: TLabel
    Left = 16
    Top = 28
    Width = 40
    Height = 13
    Caption = #1040#1085#1072#1083#1080#1079':'
  end
  object cbbCompression: TComboBox
    Left = 65
    Top = 6
    Width = 112
    Height = 21
    Style = csDropDownList
    ItemIndex = 1
    TabOrder = 0
    Text = 'ZLIB'
    Items.Strings = (
      'Huffmann'
      'ZLIB'
      'PKWARE'
      'LZMA'
      'BZIP2'
      'SPARSE'
      #1041#1077#1079' '#1089#1078#1072#1090#1080#1103)
  end
  object chklstAnalyze: TCheckListBox
    Left = 15
    Top = 47
    Width = 161
    Height = 99
    Color = clBtnFace
    ItemHeight = 13
    Items.Strings = (
      'war3map.w3*'
      'war3map.imp'
      'war3map.j'
      '*.mdx'
      '*.txt'
      '*.slk'
      '(listfile)')
    TabOrder = 1
  end
  object chkMapRename: TCheckBox
    Left = 27
    Top = 152
    Width = 137
    Height = 34
    Caption = #1044#1072#1074#1072#1090#1100' '#1082#1072#1088#1090#1077' '#1085#1072#1079#1074#1072#1085#1080#1077' '#1080#1079' war3map.w3i/wts'
    Checked = True
    State = cbChecked
    TabOrder = 2
    WordWrap = True
  end
  object chkInternalListfile: TCheckBox
    Left = 27
    Top = 192
    Width = 137
    Height = 26
    Caption = #1053#1077' '#1080#1089#1087#1086#1083#1100#1079#1086#1074#1072#1090#1100' '#1074#1089#1090#1088#1086#1077#1085#1085#1099#1081' '#1083#1080#1089#1090#1092#1072#1081#1083
    TabOrder = 3
    WordWrap = True
  end
  object btnOk: TButton
    Left = 27
    Top = 232
    Width = 137
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 4
    OnClick = btnOkClick
  end
  object btnCancel: TButton
    Left = 27
    Top = 263
    Width = 137
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 5
    OnClick = btnCancelClick
  end
end
