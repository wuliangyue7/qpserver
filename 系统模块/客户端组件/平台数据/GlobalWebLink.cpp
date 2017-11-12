#include "StdAfx.h"
#include "GlobalWebLink.h"
#include "GlobalUserInfo.h"
#include "OptionParameter.h"

//////////////////////////////////////////////////////////////////////////////////
//静态变量
CGlobalWebLink * CGlobalWebLink::m_pGlobalWebLink=NULL;

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalWebLink::CGlobalWebLink()
{
  //设置变量
  if(m_pGlobalWebLink==NULL)
  {
    m_pGlobalWebLink=this;
  }

  //设置变量
  m_szSpreadLink[0]=0;
  m_szUpdateLink[0]=0;
  m_szPlatformLink[0]=0;
  m_szValidateLink[0]=0;
  m_szPlatformLinkMax[0]=0;
  m_szPlatformLinkMin[0]=0;
  m_szQQLogonLink[0]=0;
  m_szHelpLink[0]=0;
  m_szLogonLink[0]=0;
  m_szFindPasswordLink[0]=0;
  m_szPayLink[0]=0;
  m_szTranRecordLink[0]=0;
  m_szPayRecordLink[0]=0;
  m_szWebRegisterLink[0]=0;
  m_szrLink1[0]=0;
  m_szrLink2[0]=0;
  m_szrLink3[0]=0;

  m_dwSessionVaule=17298;
  m_dwSessionVaule2=17298;
  m_dwUserID = 0;

}

//析构函数
CGlobalWebLink::~CGlobalWebLink()
{
  //设置变量
  if(m_pGlobalWebLink==this)
  {
    m_pGlobalWebLink=NULL;
  }
}

//验证链接
LPCTSTR CGlobalWebLink::GetValidateLink()
{
  //获取对象
  CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
  tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

  //变量定义
  DWORD dwUserID = pGlobalUserData?pGlobalUserData->dwUserID:0L;
  DWORD dwTickCount = GetTickCount()-pGlobalUserData->dwLogonTickCount;

  //生成签名
  TCHAR szSignature[MAX_PATH]=TEXT("");
  _sntprintf(szSignature,CountArray(szSignature),TEXT("%d%s%d%s"),dwUserID,pGlobalUserData->szDynamicPass,dwTickCount,szValidateKey);

  //加密签名
  TCHAR szMD5Signature[LEN_MD5] = TEXT("");
  CWHEncrypt::MD5Encrypt(szSignature,szMD5Signature);

  //生成链接
  _sntprintf(m_szValidateLink,CountArray(m_szValidateLink),szValidateLink,dwUserID,dwTickCount,szMD5Signature);

  return m_szValidateLink;
}

//读取数值
UINT CGlobalWebLink::ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault)
{
  //获取目录
  TCHAR szDirectory[MAX_PATH]=TEXT("");
  CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

  //构造路径
  TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
  _sntprintf(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\PlatformLink.INI"),szDirectory);

  //读取数据
  UINT nValue=GetPrivateProfileInt(pszKeyName,pszItemName,nDefault,szServerInfoPath);

  return nValue;
}

//读取数据
VOID CGlobalWebLink::ReadFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount)
{
  //获取目录
  TCHAR szDirectory[MAX_PATH]=TEXT("");
  CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

  //构造路径
  TCHAR szServerInfoPath[MAX_PATH]=TEXT("");
  _sntprintf(szServerInfoPath,CountArray(szServerInfoPath),TEXT("%s\\PlatformLink.INI"),szDirectory);

  //读取数据
  TCHAR szResultEncrypt[MAX_ENCRYPT_LEN];
  GetPrivateProfileString(pszKeyName,pszItemName,TEXT(""),szResultEncrypt,CountArray(szResultEncrypt),szServerInfoPath);

  //解密数据
  szResult[0]=0;
  CWHEncrypt::XorCrevasse(szResultEncrypt,szResult,wMaxCount);

  return;
}

//读取链接
VOID CGlobalWebLink::LoadPaltformLink()
{
  //平台链接
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("PlatformLink"),m_szPlatformLink,MAX_PATH);
  if(m_szPlatformLink[0]==0)
  {
    lstrcpyn(m_szPlatformLink,szPlatformLink,CountArray(m_szPlatformLink));
  }

  //推广链接
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("SpreadLink"),m_szSpreadLink,MAX_PATH);
  if(m_szSpreadLink[0]==0)
  {
    lstrcpyn(m_szSpreadLink,m_szPlatformLink,CountArray(m_szSpreadLink));
  }

  //更新链接
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("UpdateLink"),m_szUpdateLink,MAX_PATH);
  if(m_szUpdateLink[0]==0)
  {
    lstrcpyn(m_szUpdateLink,m_szPlatformLink,CountArray(m_szUpdateLink));
  }
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("PlatformLinkMax"),m_szPlatformLinkMax,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("PlatformLinkMin"),m_szPlatformLinkMin,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("QQLogonLink"),m_szQQLogonLink,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("HelpLink"),m_szHelpLink,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("LogonLink"),m_szLogonLink,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("FindPasswordLink"),m_szFindPasswordLink,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("PayLink"),m_szPayLink,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("TranRecordLink"),m_szTranRecordLink,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("PayRecordLink"),m_szPayRecordLink,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("WebRegisterLink"),m_szWebRegisterLink,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("Link1"),m_szrLink1,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("Link2"),m_szrLink2,MAX_PATH);
  ReadFileString(TEXT("PLATFORMLINK"),TEXT("Link3"),m_szrLink3,MAX_PATH);

  return;
}
//获取全连接
bool CGlobalWebLink::MakeLinkURL(OUT TCHAR szURL[],IN BYTE cbCode)
{
  //连接密码加密
  TCHAR szResPassword[128];
  TCHAR szMD5Password[LEN_MD5];
  ZeroMemory(szMD5Password,sizeof(szMD5Password));
  ZeroMemory(szResPassword,sizeof(szResPassword));
  //_sntprintf(szResPassword,sizeof(szResPassword),TEXT("%sCbY5G48PcdqL8oG0"),m_GlobalUserData.szPassword);
  _sntprintf(szResPassword,sizeof(szResPassword),TEXT("%ld Kp,9023*Js-d6@2[i]"),m_dwSessionVaule);
  CWHEncrypt::MD5Encrypt(szResPassword,szMD5Password);

  TCHAR szURLLongonLink[MAX_PATH];
  ZeroMemory(szURLLongonLink,sizeof(szURLLongonLink));

  if(cbCode==6)
  {
    _sntprintf(szURLLongonLink,sizeof(szURLLongonLink),TEXT("%s"),m_szPayRecordLink);
  }
  else
  {
    _sntprintf(szURLLongonLink,sizeof(szURLLongonLink),TEXT("%s"),m_szTranRecordLink);
  }

  //构建连接地址
  _sntprintf(szURL,sizeof(TCHAR)*MAX_PATH,TEXT("%s/WebRule.aspx?u=%d&k=%s&c=%s"),m_szPlatformLink,m_dwUserID,szMD5Password,szURLLongonLink);

  return true;
}
void CGlobalWebLink::SetSessionVaule(DWORD dwSessionVaule, DWORD dwSessionVaule2)
{
  m_dwSessionVaule=dwSessionVaule;
  m_dwSessionVaule2=dwSessionVaule2;
}
void CGlobalWebLink::SetUserID(DWORD dwUserID)
{
  m_dwUserID=dwUserID;
}
DWORD CGlobalWebLink::GetSessionKey(TCHAR szSessionKey[LEN_MD5])
{
  DWORD dwSessionVaule=0;
  CWHRegKey RegUserInfo;
  if(RegUserInfo.OpenRegKey(REG_USER_INFO,false))
  {
    dwSessionVaule=RegUserInfo.GetValue(TEXT("GUIDKey"),0L);
    if(dwSessionVaule==0)
    {
      dwSessionVaule=17298+rand()%1000000+(rand()%100)*2;
    }

    dwSessionVaule=dwSessionVaule+2;
    RegUserInfo.WriteValue(TEXT("GUIDKey"),dwSessionVaule);
  }
  if(dwSessionVaule==0)
  {
    dwSessionVaule=17298;
  }

  m_dwSessionVaule=dwSessionVaule;

  TCHAR szResPassword[128];
  ZeroMemory(szSessionKey,sizeof(szSessionKey));
  ZeroMemory(szResPassword,sizeof(szResPassword));
  _sntprintf(szResPassword,sizeof(szResPassword),TEXT("%ld Kp,9023*Js-d6@2[i]"),dwSessionVaule);
  CWHEncrypt::MD5Encrypt(szResPassword,szSessionKey);

  return dwSessionVaule;
}
//////////////////////////////////////////////////////////////////////////////////
