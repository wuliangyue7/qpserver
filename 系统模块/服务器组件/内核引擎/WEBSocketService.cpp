#include "StdAfx.h"
#include "Afxinet.h"
#include "WEBSocketService.h"


//构造函数
CWEBSocketService::CWEBSocketService()
{
}

//析构函数
CWEBSocketService::~CWEBSocketService()
{
}

//接口查询
VOID * CWEBSocketService::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IWEBSocketService,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IWEBSocketService,Guid,dwQueryVer);
  return NULL;
}

//启动服务
bool CWEBSocketService::StartService()
{
  return true;
}
//停止服务
bool CWEBSocketService::ConcludeService()
{
  return true;
}

//配置函数
bool CWEBSocketService::SetServiceID(WORD wServiceID)
{
  return true;
}

//设置接口
bool CWEBSocketService::SetWEBSocketEvent(IUnknownEx * pIUnknownEx)
{
  return true;
}

//关闭连接
bool CWEBSocketService::CloseSocket(BYTE cbShutReason)
{
  return true;
}

//连接操作
bool CWEBSocketService::ConnectServer(LPCTSTR pszURL, WORD wPort)
{
  return true;
}

//发送请求
bool CWEBSocketService::SendRequestData(VOID * pData, WORD wDataSize)
{
  return true;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(WEBSocketService);

//////////////////////////////////////////////////////////////////////////
