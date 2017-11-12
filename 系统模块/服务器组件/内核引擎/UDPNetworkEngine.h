#ifndef UDP_NETWORK_ENGINE_HEAD_FILE
#define UDP_NETWORK_ENGINE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//网络引擎
class CUDPNetworkEngine : public IUDPNetworkEngine
{


  //函数定义
public:
  //构造函数
  CUDPNetworkEngine();
  //析构函数
  virtual ~CUDPNetworkEngine();

  //基础接口
public:
  //释放对象
  virtual VOID Release()
  {
    delete this;
  }
  //接口查询
  virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //配置接口
public:
  //启动服务
  virtual bool  StartService();
  //停止服务
  virtual bool  ConcludeService();

//信息接口
public:
  //配置端口
  virtual WORD GetServicePort();
  //当前端口
  virtual WORD GetCurrentPort();

  //配置接口
public:
  //设置接口
  virtual bool SetUDPNetworkEngineEvent(IUnknownEx * pIUnknownEx);
  //设置参数
  virtual bool SetServiceParameter(WORD wServicePort, WORD wMaxConnect);

  //发送接口
public:
  //发送函数
  virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID);
  //发送函数
  virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

  //控制接口
public:
  //关闭连接
  virtual bool CloseSocket(DWORD dwSocketID);
  //设置关闭
  virtual bool ShutDownSocket(DWORD dwSocketID);
};
#endif