#pragma once
#include "../游戏客户端/ClientControl.h"


// CClientControlItemSinkDlg 对话框
#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#define myatoi      _wtoi
#define myatoi64  _wtoi64
#endif


class AFX_EXT_CLASS CClientControlItemSinkDlg : public IClientControlDlg
{
  DECLARE_DYNAMIC(CClientControlItemSinkDlg)

protected:
  BYTE m_cbControlStyle;
  BYTE m_cbExcuteTimes;

public:
  CClientControlItemSinkDlg(CWnd* pParent = NULL);   // 标准构造函数
  virtual ~CClientControlItemSinkDlg();

// 对话框数据
  enum { IDD = IDD_CLIENT_CONTROL_EX };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

  DECLARE_MESSAGE_MAP()

public:
  //更新控制
  virtual void  UpdateControl(CMD_S_ControlReturns* pControlReturns);
  //初始控件
  virtual bool  InitControl();

protected:
  //信息解析
  void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);
  //重置界面
  void ReSetAdminWnd();



public:
  //初始化
  virtual BOOL OnInitDialog();
  //设置颜色
  afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  //取消控制
  afx_msg void OnBnClickedButtonReset();
  //本局控制
  afx_msg void OnBnClickedButtonSyn();
  //开启控制
  afx_msg void OnBnClickedButtonOk();
  //取消关闭
  afx_msg void OnBnClickedButtonCancel();

  afx_msg void OnBnClickedRadioWin();
  afx_msg void OnBnClickedRadioLose();
};
