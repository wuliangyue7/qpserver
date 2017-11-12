#include "stdafx.h"
#include "UiWebBrowserEx.h"


CUiWebBrowserEx::CUiWebBrowserEx(void)
{
  m_pIDispWebEvent = NULL;
}


CUiWebBrowserEx::~CUiWebBrowserEx(void)
{
}

STDMETHODIMP CUiWebBrowserEx::GetExternal(IDispatch** ppDispatch)
{
  *ppDispatch = this;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE CUiWebBrowserEx::GetIDsOfNames(REFIID riid,  LPOLESTR *rgszNames, UINT cNames, LCID lcid,  DISPID *rgDispId)
{
  //DISP ID 从200开始
  if(_tcscmp(rgszNames[0],_FN_QQ_LOGON) == 0)
  {
    *rgDispId = DISP_QQ_LOGON;
    return S_OK;
  }
  else if(_tcscmp(rgszNames[0],_FN_ACCOUNT_LOGON) == 0)
  {
    *rgDispId = DISP_ACCOUNT_LOGON;
    return S_OK;
  }

  return __super::GetIDsOfNames(riid,rgszNames,cNames,lcid,rgDispId);
}

STDMETHODIMP CUiWebBrowserEx::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
  if((riid != IID_NULL))
  {
    return E_INVALIDARG;
  }

  switch(dispIdMember)
  {
    case DISP_QQ_LOGON:  //QQ登陆
      if(m_pIDispWebEvent != NULL)
      {
        CString strAccounts, strPassword;
        strAccounts.Format(L"%s", pDispParams->rgvarg[1].bstrVal); //OutputDebugString(strAccounts);
        strPassword.Format(L"%s", pDispParams->rgvarg[0].bstrVal);

        m_pIDispWebEvent->OnEventQQLogon(strAccounts,strPassword);
      }
      break;
    case DISP_ACCOUNT_LOGON:   //QQ登陆

      if(m_pIDispWebEvent != NULL)
      {
        CString strAccounts, strPassword;
        strAccounts.Format(L"%s", pDispParams->rgvarg[1].bstrVal); //OutputDebugString(strAccounts);
        strPassword.Format(L"%s", pDispParams->rgvarg[0].bstrVal);

        m_pIDispWebEvent->OnEventAccountLogon(strAccounts,strPassword);
      }
      break;
  }

  return __super::Invoke(dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr);
}


//设置接口
void CUiWebBrowserEx::SetIDispWebEvent(IDispWebEvent* pIDispWebEvent)
{
  m_pIDispWebEvent = pIDispWebEvent;
}


CWebBrowserEventHandlerEx::CWebBrowserEventHandlerEx(void)
{

}

CWebBrowserEventHandlerEx::~CWebBrowserEventHandlerEx(void)
{

}

HRESULT STDMETHODCALLTYPE CWebBrowserEventHandlerEx::GetHostInfo(/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
{
  if(pInfo != NULL)
  {
    pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;
  }
  return S_OK;
}










