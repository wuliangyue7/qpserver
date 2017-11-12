#include "StdAfx.h"
#include "TCPSocket.h"
#include "NetworkManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CWHNetwork::CWHNetwork()
{
}

//析构函数
CWHNetwork::~CWHNetwork()
{
}

//接口查询
VOID *  CWHNetwork::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IWHNetwork,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IWHNetwork,Guid,dwQueryVer);
  return NULL;
}

//枚举代理
bool  CWHNetwork::EnumProxyServerType(WORD wIndex, tagProxyDescribe & ProxyDescribe)
{
  //变量定义
  BYTE bProxyType=(BYTE)wIndex;

  //信息填充
  switch(bProxyType)
  {
    case PROXY_NONE:    //不使用代理
    {
      ProxyDescribe.cbProxyType=bProxyType;
      lstrcpyn(ProxyDescribe.szDescribe,TEXT("不使用代理"),CountArray(ProxyDescribe.szDescribe));
      return true;
    }
    case PROXY_HTTP:    //HTTP 代理
    {
      ProxyDescribe.cbProxyType=bProxyType;
      lstrcpyn(ProxyDescribe.szDescribe,TEXT("HTTP 代理"),CountArray(ProxyDescribe.szDescribe));
      return true;
    }
    case PROXY_SOCKS4:    //SOCKS 4 代理
    {
      ProxyDescribe.cbProxyType=bProxyType;
      lstrcpyn(ProxyDescribe.szDescribe,TEXT("SOCKS 4 代理"),CountArray(ProxyDescribe.szDescribe));
      return true;
    }
    case PROXY_SOCKS5:    //SOCKS 5 代理
    {
      ProxyDescribe.cbProxyType=bProxyType;
      lstrcpyn(ProxyDescribe.szDescribe,TEXT("SOCKS 5 代理"),CountArray(ProxyDescribe.szDescribe));
      return true;
    }
  }

  return false;
}

//代理测试
BYTE  CWHNetwork::ProxyServerTesting(BYTE cbProxyType, const tagProxyServer & ProxyServer)
{
  //效验参数
  ASSERT(cbProxyType!=PROXY_NONE);
  if(cbProxyType==PROXY_NONE)
  {
    return CONNECT_EXCEPTION;
  }

  //代理设置
  CTCPSocket TCPSocket;
  if(TCPSocket.SetProxyServerInfo(cbProxyType,ProxyServer)==false)
  {
    return CONNECT_EXCEPTION;
  }

  //代理测试
  return TCPSocket.ProxyServerTesting();
}

//////////////////////////////////////////////////////////////////////////

//输出管理
CWHNetwork g_NetworkManager;

//组件创建函数
extern "C" __declspec(dllexport) VOID *  CreateWHNetwork(const GUID & Guid, DWORD dwInterfaceVer)
{
  return g_NetworkManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
