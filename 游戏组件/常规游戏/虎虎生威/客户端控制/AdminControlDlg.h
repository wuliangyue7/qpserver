#pragma once
#include "Resource.h"
#include "../游戏客户端/ClientControl.h"
#include "afxcmn.h"
#include "afxwin.h"

// CAdminControlDlg 对话框
#define IDM_ADMIN_COMMDN WM_USER+207

#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#endif

class AFX_EXT_CLASS CAdminControlDlg :public IClientControlDlg
{
  DECLARE_DYNAMIC(CAdminControlDlg)

public:
  CAdminControlDlg(CWnd* pParent = NULL);   // 标准构造函数
  virtual ~CAdminControlDlg();
  LONGLONG            m_lStorageStart;            //库存数值
  LONGLONG            m_lStorageDeduct;           //库存衰减
  LONGLONG            m_AreaTotalBet[AREA_COUNT];       //区域总注
  LONGLONG            m_lUserTotalScore[GAME_PLAYER];     //玩家总输赢
  CListCtrl           m_SkinListCtrl;
  CListCtrl           m_ListTotalBet;
  // 对话框数据
  enum { IDD = IDD_DIALOG_SYSTEM };

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

  DECLARE_MESSAGE_MAP()

public:
  //更新控制
  virtual void __cdecl UpdateControl(CMD_S_ControlReturns* pControlReturns);
  //更新列表
  virtual void UpdateClistCtrl(LONGLONG lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair, WORD wViewChairID,IClientUserItem* pUserData);
  //清除列表
  virtual void ClearClist();
  //设置库存信息
  virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInof);
  //设置起始库存
  virtual void SetStorageInfo(LONGLONG lStartStorage);

protected:
  //信息
  void PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes);

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
  afx_msg void  OnFreshStorage();

  afx_msg void OnBnClickedBtDeduct2();

  afx_msg void OnBnClickedButton2();
  CEdit m_EditSerach;
  afx_msg void OnEnKillfocusSearchId();
  afx_msg void OnEnSetfocusSearchId();
};
