#ifndef DLG_WEB_REGISTER_HEAD_FILE
#define DLG_WEB_REGISTER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "UiWebBrowserEx.h"

//////////////////////////////////////////////////////////////////////////////////
//选择状态 QQLogonWeb
enum enum_Logon_Type
{
  en_Account_Logon,
  en_QQ_Logon,
  en_Register_Logon
};

//帐号登录
struct stLogonAccounts
{
  //系统信息
  DWORD             dwClientAddr;           //客户端IP

  //登录信息
  TCHAR             szPassword[LEN_MD5];        //登录密码
  TCHAR             szAccounts[LEN_ACCOUNTS];     //登录帐号
};
//////////////////////////////////////////////////////////////////////////////////////
//注册窗口
class CDlgWebRegister : public CFGuiDialog ,public IDispWebEvent
{
public:
  enum_Logon_Type       m_enLogonType;

  stLogonAccounts       m_stLogonAccounts;      //登陆信息


  TCHAR           m_szWebRegisterLink[MAX_PATH];
  TCHAR           m_szQQLogonLink[MAX_PATH];
private:
  WNDPROC           m_OldWndProc;
  bool            m_bSubclassed;
  //函数定义
public:
  //构造函数
  CDlgWebRegister();
  //析构函数
  virtual ~CDlgWebRegister();
  //重载函数
protected:
  //消息解释
  virtual BOOL PreTranslateMessage(MSG * pMsg);
  //配置函数
  virtual BOOL OnInitDialog();
  //取消函数
  virtual VOID OnCancel();
  //功能函数
public:
  //QQ登陆
  void SetQQlogon(bool bCreat, CString strWebBrowser);
  //QQ登陆
  void SetRegisterlogon(bool bCreat, CString strWebBrowser);
  WORD ConstructWebRegisterPacket(BYTE cbBuffer[], WORD wBufferSize, BYTE cbValidateFlags);

  //重载函数
public:
  //皮肤名称
  virtual LPCTSTR GetSkinFile();
  //初始控件
  virtual void InitControlUI();
  //消息提醒
  virtual void Notify(TNotifyUI &  msg);

  virtual VOID OnEventWebRight(CString WebRight) {}
  virtual bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
  //处理消息
  virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
  //QQ登陆
  virtual VOID OnEventQQLogon(CString strAccounts, CString strPassword);

  //登陆数据
  virtual VOID OnEventAccountLogon(CString strAccounts, CString strPassword);
private:
  bool Subclass();
  bool UnSubclass();
  static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  virtual CControlUI* CreateControl(LPCTSTR pstrClass);
  afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCS);
  afx_msg void OnNcDestroy();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif