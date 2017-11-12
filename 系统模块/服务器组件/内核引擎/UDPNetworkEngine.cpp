#include "StdAfx.h"
#include "Afxinet.h"
#include "UDPNetworkEngine.h"


//构造函数
CUDPNetworkEngine::CUDPNetworkEngine()
{
}

//析构函数
CUDPNetworkEngine::~CUDPNetworkEngine()
{
}

//接口查询
VOID * CUDPNetworkEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IUDPNetworkEngine,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IUDPNetworkEngine,Guid,dwQueryVer);
  return NULL;
}

//启动服务
bool  CUDPNetworkEngine::StartService()
{
  return true;
}
//停止服务
bool  CUDPNetworkEngine::ConcludeService()
{
  return true;
}

//配置端口
WORD CUDPNetworkEngine::GetServicePort()
{
  return 0;
}
//当前端口
WORD CUDPNetworkEngine::GetCurrentPort()
{
  return 0;
}


//设置接口
bool CUDPNetworkEngine::SetUDPNetworkEngineEvent(IUnknownEx * pIUnknownEx)
{
  return true;
}
//设置参数
bool CUDPNetworkEngine::SetServiceParameter(WORD wServicePort, WORD wMaxConnect)
{
  return true;
}

//发送函数
bool CUDPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)
{
  return true;
}

//发送函数
bool CUDPNetworkEngine::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  return true;
}

//关闭连接
bool CUDPNetworkEngine::CloseSocket(DWORD dwSocketID)
{
  return true;
}

//设置关闭
bool CUDPNetworkEngine::ShutDownSocket(DWORD dwSocketID)
{
  return true;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(UDPNetworkEngine);

//////////////////////////////////////////////////////////////////////////
