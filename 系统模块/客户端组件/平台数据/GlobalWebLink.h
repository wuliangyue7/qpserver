#ifndef GLOBAL_WEBLINK_HEAD_FILE
#define GLOBAL_WEBLINK_HEAD_FILE

#pragma once

#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////////////

//全局服务器
class PLATFORM_DATA_CLASS CGlobalWebLink
{
  //变量定义
protected:
  TCHAR             m_szSpreadLink[MAX_PATH];         //推广链接
  TCHAR             m_szUpdateLink[MAX_PATH];         //更新链接
  TCHAR             m_szPlatformLink[MAX_PATH];         //平台链接
  TCHAR             m_szValidateLink[MAX_PATH];         //验证链接
  TCHAR             m_szPlatformLinkMax[MAX_PATH];          //平台链接
  TCHAR             m_szPlatformLinkMin[MAX_PATH];          //平台链接
  TCHAR             m_szQQLogonLink[MAX_PATH];
  TCHAR             m_szHelpLink[MAX_PATH];
  TCHAR             m_szLogonLink[MAX_PATH];
  TCHAR             m_szFindPasswordLink[MAX_PATH];
  TCHAR             m_szPayLink[MAX_PATH];
  TCHAR             m_szTranRecordLink[MAX_PATH];
  TCHAR             m_szPayRecordLink[MAX_PATH];
  TCHAR             m_szWebRegisterLink[MAX_PATH];
  TCHAR             m_szrLink1[MAX_PATH];
  TCHAR             m_szrLink2[MAX_PATH];
  TCHAR             m_szrLink3[MAX_PATH];

  DWORD             m_dwSessionVaule;
  DWORD             m_dwSessionVaule2;
  DWORD             m_dwUserID;

  //静态变量
protected:
  static CGlobalWebLink *     m_pGlobalWebLink;             //全局链接

  //函数定义
public:
  //构造函数
  CGlobalWebLink();
  //析构函数
  virtual ~CGlobalWebLink();

  //功能函数
public:
  //读取链接
  VOID LoadPaltformLink();

  //链接信息
public:
  //验证链接
  LPCTSTR GetValidateLink();
  //推广链接
  LPCTSTR GetSpreadLink()
  {
    return m_szSpreadLink;
  }
  //更新链接
  LPCTSTR GetUpdateLink()
  {
    return m_szUpdateLink;
  }
  //平台链接
  LPCTSTR GetPlatformLink()
  {
    return m_szPlatformLink;
  }
  LPCTSTR GetPlatformLinkMax()
  {
    return m_szPlatformLinkMax;
  }
  LPCTSTR GetPlatformLinkMin()
  {
    return m_szPlatformLinkMin;
  }
  LPCTSTR GetQQLogonLink()
  {
    return m_szQQLogonLink;
  }
  LPCTSTR GetHelpLink()
  {
    return m_szHelpLink;
  }
  LPCTSTR GetLogonLink()
  {
    return m_szLogonLink;
  }
  LPCTSTR GetFindPasswordLink()
  {
    return m_szFindPasswordLink;
  }
  LPCTSTR GetPayLink()
  {
    return m_szPayLink;
  }
  LPCTSTR GetTranRecordLink()
  {
    return m_szTranRecordLink;
  }
  LPCTSTR GetPayRecordLink()
  {
    return m_szPayRecordLink;
  }
  LPCTSTR GetWebRegisterLink()
  {
    return m_szWebRegisterLink;
  }
  LPCTSTR GetLink1()
  {
    return m_szrLink1;
  }
  LPCTSTR GetLink2()
  {
    return m_szrLink2;
  }
  LPCTSTR GetLink3()
  {
    return m_szrLink3;
  }
  //获取全连接
  bool MakeLinkURL(OUT TCHAR szURL[],IN BYTE cbCode);
  void SetSessionVaule(DWORD dwSessionVaule, DWORD dwSessionVaule2);
  void SetUserID(DWORD dwUserID);
  DWORD GetSessionKey(TCHAR szSessionKey[LEN_MD5]);

  //读取函数
protected:
  //读取数值
  UINT ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
  //读取数据
  VOID ReadFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount);

  //静态函数
public:
  //获取对象
  static CGlobalWebLink * GetInstance()
  {
    return m_pGlobalWebLink;
  }
};

//////////////////////////////////////////////////////////////////////////////////

#endif