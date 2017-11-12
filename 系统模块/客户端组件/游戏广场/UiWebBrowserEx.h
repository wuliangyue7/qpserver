#ifndef UI_WEBBROWSER_MENU_EX
#define UI_WEBBROWSER_MENU_EX

#pragma once
#include "stdafx.h"

#define _FN_QQ_LOGON           (_T("fnQQLogon"))
#define _FN_ACCOUNT_LOGON        (_T("fnAccountLogon"))

#define DISP_QQ_LOGON           300
#define DISP_ACCOUNT_LOGON          400

//管理接口
interface IDispWebEvent
{
  //登陆数据
  virtual VOID OnEventQQLogon(CString strAccounts, CString strPassword)=NULL;
  //登陆数据
  virtual VOID OnEventAccountLogon(CString strAccounts, CString strPassword)=NULL;
  //页面信息
  virtual VOID OnEventWebRight(CString WebRight)=NULL;
};



class CUiWebBrowserEx : public CWebBrowserUI
{
  //定义接口
public:
  IDispWebEvent*        m_pIDispWebEvent;

public:
  CUiWebBrowserEx(void);
  ~CUiWebBrowserEx(void);

  //设置接口
public:
  void SetIDispWebEvent(IDispWebEvent* pIDispWebEvent);
  void SendNotifyEx();


  //DIPS
public:
  //设置无3D边框
  virtual STDMETHODIMP GetExternal(IDispatch** ppDispatch);

  //页面调用
  virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid,  LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);

  virtual STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
};


class CWebBrowserEventHandlerEx :
  public CWebBrowserEventHandler
{
public:
  CWebBrowserEventHandlerEx(void);
  ~CWebBrowserEventHandlerEx(void);
  //主要是重载了这个方法
  virtual HRESULT STDMETHODCALLTYPE GetHostInfo(
    /* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);
};


#endif

