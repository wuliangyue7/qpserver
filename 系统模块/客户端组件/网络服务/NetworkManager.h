#ifndef NETWORK_MANAGER_HEAD_FILE
#define NETWORK_MANAGER_HEAD_FILE

#pragma once

#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////

//网络管理
class CWHNetwork : public IWHNetwork
{
  //函数定义
public:
  //构造函数
  CWHNetwork();
  //析构函数
  virtual ~CWHNetwork();

  //基础接口
public:
  //释放对象
  virtual VOID  Release()
  {
    return;
  }
  //接口查询
  virtual VOID *  QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //性能查询
public:
  //枚举代理
  virtual bool  EnumProxyServerType(WORD wIndex, tagProxyDescribe & ProxyDescribe);
  //代理测试
  virtual BYTE  ProxyServerTesting(BYTE cbProxyType, const tagProxyServer & ProxyServer);
};

//////////////////////////////////////////////////////////////////////////

#endif