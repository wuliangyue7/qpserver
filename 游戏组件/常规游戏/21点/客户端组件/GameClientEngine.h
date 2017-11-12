#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////////////

//游戏引擎
class CGameClientEngine : public CGameFrameEngine
{
  //配置变量
protected:
  DWORD             m_dwCardHSpace;             //扑克象素

  WORD              m_wBankerUser;              //庄家

  BYTE              m_bStopCard[GAME_PLAYER*2];       //是否停牌
  BYTE              m_bInsureCard[2];           //是否保险
  BYTE              m_bDoubleCard[2];           //
  BYTE              m_cbCardCount[2];           //手上牌数目
  BYTE              m_cbHandCardData[2][MAX_COUNT];     //手上牌

  BYTE              m_bTurnBanker;              //是否轮到庄家
  BYTE              m_bSpliting;              //是否正在分牌
  WORD              m_wSplitingUser;            //正在分牌玩家

  //加注信息
  LONGLONG            m_lMaxScore;              //最大下注
  LONGLONG            m_lCellScore;             //单元下注

  //状态变量
  BYTE              m_cbPlayStatus[GAME_PLAYER];      //游戏状态
  TCHAR             m_szAccounts[GAME_PLAYER][LEN_ACCOUNTS];  //玩家名字

  //组件变量
public:
  CGameClientView         m_GameClientView;         //游戏视图
  CGameLogic            m_GameLogic;              //游戏逻辑
  //函数定义
public:
  //构造函数
  CGameClientEngine();
  //析构函数
  virtual ~CGameClientEngine();

  //系统事件
public:
  //创建函数
  virtual bool OnInitGameEngine();
  //重置函数
  virtual bool OnResetGameEngine();

  //时钟事件
public:
  //时钟删除
  virtual bool OnEventGameClockKill(WORD wChairID);
  //时钟信息
  virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

  //游戏事件
public:
  //旁观消息
  virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
  //游戏消息
  virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
  //场景消息
  virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);
  //银行消息
  virtual bool OnEventInsureMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
  {
    return true;
  }

  //消息处理
protected:
  //游戏开始
  bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
  //发牌消息
  bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
  //游戏结束
  bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
  //下注消息
  bool OnSubAddScore(const void *pBuffer, WORD wDataSize);
  //停牌消息
  bool OnSubStopCard(const void *pBuffer, WORD wDataSize);
  //加倍消息
  bool OnSubDoubleCard(const void *pBuffer, WORD wDataSize);
  //分牌消息
  bool OnSubSplitCard(const void *pBuffer, WORD wDataSize);
  //保险消息
  bool OnSubInsureCard(const void *pBuffer, WORD wDataSize);
  //要牌消息
  bool OnSubGetCard(const void *pBuffer, WORD wDataSize);

  bool OnSubGMCard(const void * pData, WORD wDataSize);

  //辅助函数
protected:
  //隐藏控制
  void HideScoreControl();
  //更新控制
  BOOL UpdateScoreControl();

  //消息映射
protected:
  //开始按钮
  LRESULT OnStart(WPARAM wParam, LPARAM lParam);
  //停牌按钮
  LRESULT OnStopCard(WPARAM wParam, LPARAM lParam);
  //加倍按钮
  LRESULT OnDoubleCard(WPARAM wParam, LPARAM lParam);
  //保险按钮
  LRESULT OnInsureCard(WPARAM wParam, LPARAM lParam);
  //分牌按钮
  LRESULT OnSplitCard(WPARAM wParam, LPARAM lParam);
  //要牌按钮
  LRESULT OnGetCard(WPARAM wParam, LPARAM lParam);
  //加注按钮
  LRESULT OnAddScore(WPARAM wParam, LPARAM lParam);

  //发牌完成
  LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
  LRESULT OnStorage(WPARAM wParam, LPARAM lParam);
  LRESULT OnStorageInfo(WPARAM wParam, LPARAM lParam);
  LRESULT OnRequestQueryUser(WPARAM wParam, LPARAM lParam);
  //用户控制
  LRESULT OnUserControl(WPARAM wParam, LPARAM lParam);
  //请求更新房间信息
  LRESULT OnRequestUpdateRoomInfo(WPARAM wParam, LPARAM lParam);
  //清空当前查询用户
  LRESULT OnClearCurrentQueryUser(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif