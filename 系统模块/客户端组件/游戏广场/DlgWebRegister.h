#ifndef UILAUNCHER_HEAD_FILE
#define UILAUNCHER_HEAD_FILE

#pragma once
#include "stdafx.h"
#include "UiWebBrowserEx.h"

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


#define VALIDATE_COUNT        4                 //验证长度
//密码等级
#define PASSWORD_LEVEL_0      0                 //没有等级
#define PASSWORD_LEVEL_1      1                 //没有等级
#define PASSWORD_LEVEL_2      2                 //没有等级
#define PASSWORD_LEVEL_3      3                 //没有等级

//主框架类
class CDlgWebRegister : public CDialog, public INotifyUI, public IDialogBuilderCallback ,public IDispWebEvent
{
  //变量定义
protected:

  //界面定义
public:
  CPaintManagerUI   m_PaintManager;
  UINT            m_uSizeStatus;
  bool            m_bSubclassed;
  WNDPROC           m_OldWndProc;

  CSkinButton           m_btCancel;             //关闭按钮
  //
public:
  enum_Logon_Type       m_enLogonType;

  stLogonAccounts       m_stLogonAccounts;      //登陆信息

  bool            m_bLogonCreat;
  bool            m_bRegtistCreat;
  bool            m_bWebCreat;


  TCHAR           m_szWebRegisterLink[MAX_PATH];
  TCHAR           m_szQQLogonLink[MAX_PATH];

  //函数定义
public:
  //构造函数
  CDlgWebRegister();
  //析构函数
  virtual ~CDlgWebRegister();

  //重载函数
public:
  virtual LPCTSTR GetResFile()
  {
    return NULL;
  }
  virtual LPCTSTR GetResType()
  {
    return NULL;
  }
  virtual void OnInit() { }
  virtual void OnSized(UINT uCmd) { }

  virtual CControlUI* CreateControl(LPCTSTR pstrClass);
  virtual bool HitControl(CControlUI* pControl);
  virtual LRESULT HitTestNCA(HWND hWnd, POINT ptMouse);

  //重载函数
protected:
  //控件绑定
  virtual VOID DoDataExchange(CDataExchange * pDX);
  //取消函数
  VOID OnCancel();
  //duilib事件
  virtual bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
  //duilib事件
  virtual void Notify(TNotifyUI& msg);
  //设置属性
  virtual void SetCustomAttribute(LPCTSTR pszName,LPCTSTR pszValue) {};

public:
  //按钮事件
  bool ButtonEvent(TNotifyUI& msg);

  //QQ登陆
  void SetQQlogon(bool bCreat, CString strWebBrowser);
  //QQ登陆
  void SetRegisterlogon(bool bCreat, CString strWebBrowser);

  virtual VOID OnEventWebRight(CString WebRight) {}

  //QQ登陆
  virtual VOID OnEventQQLogon(CString strAccounts, CString strPassword);

  //登陆数据
  virtual VOID OnEventAccountLogon(CString strAccounts, CString strPassword);

  WORD ConstructWebRegisterPacket(BYTE cbBuffer[], WORD wBufferSize, BYTE cbValidateFlags);

private:
  bool Subclass();
  bool UnSubclass();
  static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
  afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCS);
  afx_msg void OnNcDestroy();
  afx_msg void OnNcPaint(void);
  afx_msg BOOL OnNcActivate(BOOL bActive);
  afx_msg void OnNcCalcSize(BOOL bCal, NCCALCSIZE_PARAMS* lpNCP);
  afx_msg LRESULT OnNcHitTest(CPoint ptMouse);

  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

  afx_msg void OnSize(UINT nType, int cx, int cy);


  DECLARE_MESSAGE_MAP()
};

#endif
