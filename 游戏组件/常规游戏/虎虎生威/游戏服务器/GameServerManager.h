#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"
//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IAndroidService INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidService= {0xf1c76760,0x4236,0x40ec,0xa5,0x58,0xfb,0x64,0xee,0x78,0xe2,0xd6};
#else
#define VER_IAndroidService INTERFACE_VERSION(1,1)
static const GUID IID_IAndroidService= {0xcf6e68b2,0x8455,0x4022,0x95,0xcc,0xad,0x4c,0xd1,0x3d,0x67,0xfe};
#endif

//机器人接口
interface IAndroidService :
public IUnknownEx
{
  //功能接口
public:
  //创建机器人
  virtual VOID* __cdecl CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
};

DECLARE_MODULE_DYNAMIC(AndroidService);

//游戏服务器管理类
//游戏服务器管理类
class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule
{
  //控件变量
protected:
  CDlgCustomRule            m_DlgCustomRule;        //自定规则

  //变量定义
protected:
  tagGameServiceAttrib        m_GameServiceAttrib;      //服务属性

  //组件变量
  CGameServiceManagerHelper       m_AndroidServiceHelper;     //机器人服务
  HINSTANCE             m_hDllInstance;         //DLL 句柄
  //函数定义
public:
  //构造函数
  CGameServiceManager(void);
  //析构函数
  virtual ~CGameServiceManager(void);

  //基础接口
public:
  //释放对象
  virtual VOID Release()
  {
    return;
  }
  //接口查询
  virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);
  //创建接口
public:
  //创建桌子
  virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer);
  //创建机器
  virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer);
  //创建数据
  virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer);

  //参数接口
public:
  //组件属性
  virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
  //调整参数
  virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption);

  //配置接口
public:
  //默认配置
  virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
  //获取配置
  virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
  //创建窗口
  virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);
};

//////////////////////////////////////////////////////////////////////////

#endif
