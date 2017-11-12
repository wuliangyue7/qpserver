#pragma once




//////////////////////////////////////////////////////////////////////////
//常量定义
#define INVALID_ITEM        0xFFFF                //无效子项
//间距定义
#define DEF_X_DISTANCE        17                  //默认间距
#define DEF_Y_DISTANCE        17                  //默认间距
#define DEF_SHOOT_DISTANCE      20                  //默认间距

//间距定义
#define DEF_X_DISTANCE_SMALL    17                  //默认间距
#define DEF_Y_DISTANCE_SMALL    17                  //默认间距
#define DEF_SHOOT_DISTANCE_SMALL  20                  //默认间距

//消息定义
#define IDM_LEFT_HIT_CARD     (WM_USER+500)           //左击扑克
#define IDM_RIGHT_HIT_CARD      (WM_USER+501)           //右击扑克
#define IDM_LEFT_DBHIT_CARD     (WM_USER+502)           //左键双击

#define MAX_CARD_COUNT        27                  //扑克数目
#define SPACE_CARD_DATA       255                 //间距扑克

//数值掩码
#define CARD_MASK_COLOR       0xF0                //花色掩码
#define CARD_MASK_VALUE       0x0F                //数值掩码



#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0500
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>
#include <AtlBase.h>
#include <AtlDbcli.h>
#include <AfxDtctl.h>

//////////////////////////////////////////////////////////////////////////////////

//编译环境
#include "..\..\..\SDKCondition.h"

//////////////////////////////////////////////////////////////////////////////////
#define SDK_CONDITION

#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//开发环境

#include "..\..\..\开发库\Include\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImage.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ServiceCore.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImage.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../开发库/Lib/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../开发库/Lib/Ansi/ServiceCoreD.lib")
#else
#pragma comment (lib,"../../../开发库/Lib/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../开发库/Lib/Unicode/ServiceCoreD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//系统环境

#include "..\..\..\..\系统模块\客户端组件\游戏框架\GameFrameHead.h"

#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/WHImage.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/GameFrame.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/GameEngine.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/SkinControl.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/ServiceCore.lib")
#else
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/WHImage.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/GameFrame.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/GameEngine.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/SkinControl.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/ServiceCore.lib")
#endif
#else
#ifndef _UNICODE
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/WHImageD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/GameFrameD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/GameEngineD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/SkinControlD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Ansi/ServiceCoreD.lib")
#else
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/WHImageD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/GameFrameD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/GameEngineD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/SkinControlD.lib")
#pragma comment (lib,"../../../../系统模块/链接库/Unicode/ServiceCoreD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif


#include "..\消息定义\CMD_Game.h"

//X 排列方式
enum enXCollocateMode
{
  enXLeft,            //左对齐
  enXCenter,            //中对齐
  enXRight,           //右对齐
};

//Y 排列方式
enum enYCollocateMode
{
  enYTop,             //上对齐
  enYCenter,            //中对齐
  enYBottom,            //下对齐
};
//扑克结构
struct tagCardItem
{
  bool              bShoot;               //弹起标志
  BYTE              cbCardData;             //扑克数据
};