#ifndef WEB_SOCKET_SERVICE_HEAD_FILE
#define WEB_SOCKET_SERVICE_HEAD_FILE

#pragma once

#include "KernelEngineHead.h"

//网络事件
class CWEBSocketEvent : public IWEBSocketEvent
{
  //函数定义
public:
  //构造函数
  CWEBSocketEvent() {};
  //析构函数
  virtual ~CWEBSocketEvent() {};

  //状态接口
public:
  //连接消息
  virtual bool OnEventWEBSocketLink(WORD wServiceID, WORD wRequestID, INT nErrorCode)
  {
    return true;
  };
  //关闭消息
  virtual bool OnEventWEBSocketShut(WORD wServiceID, WORD wRequestID, BYTE cbShutReason)
  {
    return true;
  };

  //数据接口
public:
  //数据包流
  virtual bool OnEventWEBSocketMain(WORD wServiceID, WORD wRequestID, VOID * pcbMailData, WORD wStreamSize)
  {
    return true;
  };
  //数据包头
  virtual bool OnEventWEBSocketHead(WORD wServiceID, WORD wRequestID, VOID * pcbHeadData, WORD wHeadSize, INT nStatusCode)
  {
    return true;
  };
};

//WEB引擎
class CWEBSocketService : public IWEBSocketService
{

//函数定义
public:
  //构造函数
  CWEBSocketService();
  //析构函数
  virtual ~CWEBSocketService();

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

//配置接口
public:
  //配置函数
  virtual bool SetServiceID(WORD wServiceID);
  //设置接口
  virtual bool SetWEBSocketEvent(IUnknownEx * pIUnknownEx);

//功能接口
public:
  //关闭连接
  virtual bool CloseSocket(BYTE cbShutReason);
  //连接操作
  virtual bool ConnectServer(LPCTSTR pszURL, WORD wPort);
  //发送请求
  virtual bool SendRequestData(VOID * pData, WORD wDataSize);
};
#endif