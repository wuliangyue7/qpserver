#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "..\游戏服务器\GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//机器人类
class CAndroidUserItemSink : public IAndroidUserItemSink
{
  //游戏变量
protected:
  WORD              m_wBankerUser;            //庄家用户
  WORD              m_wOutCardUser;           //出牌玩家
  WORD              m_wCurrentUser;           //当前用户

  //辅助变量
protected:
  tagAnalyseResult        m_OutCardInfo;            //出牌类型
  tagAnalyseResult        m_LastOutCard;            //最后出牌

  //扑克变量
protected:
  BYTE              m_cbTurnCardCount;          //出牌数目
  BYTE              m_cbTurnCardData[MAX_COUNT];    //出牌列表




  //手上扑克
protected:
  BYTE              m_cbHandCardData[GAME_PLAYER][MAX_COUNT]; //手上扑克
  BYTE              m_cbHandCardCount[GAME_PLAYER];   //扑克数目

  //历史扑克
protected:

  //控件变量
protected:
  CGameLogic            m_GameLogic;            //游戏逻辑
  IAndroidUserItem *        m_pIAndroidUserItem;        //用户接口

  //函数定义
public:
  //构造函数
  CAndroidUserItemSink();
  //析构函数
  virtual ~CAndroidUserItemSink();

  //基础接口
public:
  //释放对象
  virtual VOID  Release()
  {
    delete this;
  }
  //接口查询
  virtual VOID *  QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //控制接口
public:
  //初始接口
  virtual bool  Initialization(IUnknownEx * pIUnknownEx);
  //重置接口
  virtual bool  RepositionSink();

  //游戏事件
public:
  //时间消息
  virtual bool OnEventTimer(UINT nTimerID);
  //游戏消息
  virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
  //游戏消息
  virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
  //场景消息
  virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

  //用户事件
public:
  //用户进入
  virtual VOID OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //用户离开
  virtual VOID OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //用户积分
  virtual VOID OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);
  //用户状态
  virtual VOID OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser);

  //消息处理
protected:
  //游戏开始
  bool OnSubGameStart(const void * pData, WORD wDataSize);
  //用户出牌
  bool OnSubOutCard(const void * pData, WORD wDataSize);
  //用户放弃
  bool OnSubPassCard(const void * pData, WORD wDataSize);
  //游戏结束
  bool OnSubGameEnd(const void * pData, WORD wDataSize);

private:
  //出牌判断
  bool VerdictOutCard(BYTE cbOutCardData[], BYTE cbOutCardCount);
};

//////////////////////////////////////////////////////////////////////////

#endif