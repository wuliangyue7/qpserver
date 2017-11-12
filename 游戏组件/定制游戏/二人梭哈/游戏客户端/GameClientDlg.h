#pragma once

#include "Stdafx.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏对话框
class CGameClientEngine : public CGameFrameEngine
{
  //游戏变量
protected:
  LONGLONG              m_lTurnMaxGold;           //最大下注
  LONGLONG              m_lTurnBasicGold;         //跟注数目
  LONGLONG              m_lBasicGold;           //单元数目
  BYTE              m_bPlayStatus[GAME_PLAYER];     //游戏状态
  WORD              m_wCurrentUser;           //
  LONGLONG              m_lShowHandScore;         //限制最高分
  LONGLONG              m_lUserScore[GAME_PLAYER];      //


  //辅助变量
protected:
  bool              m_bShowHand;            //是否梭哈
  LONGLONG              m_lGoldShow;            //加注筹码
  TCHAR             m_szName[GAME_PLAYER][LEN_ACCOUNTS];  //玩家名字
  CMD_S_GameEnd         m_GameEnd;              //

  //控件变量
public:
  CGameClientView         m_GameClientView;         //游戏视图
  CGlobalUnits *                  m_pGlobalUnits;                         //全局单元

  //函数定义
public:
  //构造函数
  CGameClientEngine();
  //析构函数
  virtual ~CGameClientEngine();

  //常规继承
private:
  //初始函数
  virtual bool OnInitGameEngine();
  //重置框架
  virtual bool OnResetGameEngine();
  //游戏设置
  virtual void OnGameOptionSet();

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

  //消息处理
protected:
  //游戏开始
  bool OnSubGameStart(const void * pBuffer, WORD wDataSize);
  //用户加注
  bool OnSubAddGold(const void * pBuffer, WORD wDataSize);
  //用户放弃
  bool OnSubGiveUp(const void * pBuffer, WORD wDataSize);
  //发牌消息
  bool OnSubSendCard(const void * pBuffer, WORD wDataSize);
  //游戏结束
  bool OnSubGameEnd(const void * pBuffer, WORD wDataSize);
  //控制
  bool OnSubAdminControl(const void * pBuffer, WORD wDataSize);
  //控制
  bool OnSubAdminControlEx(const void * pBuffer, WORD wDataSize);

  //消息映射
protected:
  //开始按钮
  LRESULT OnStart(WPARAM wParam, LPARAM lParam);
  //放弃按钮
  LRESULT OnGiveUp(WPARAM wParam, LPARAM lParam);
  //跟注按钮
  LRESULT OnFollow(WPARAM wParam, LPARAM lParam);
  //梭哈按钮
  LRESULT OnShowHand(WPARAM wParam, LPARAM lParam);
  //加注消息
  LRESULT OnAddGold(WPARAM wParam, LPARAM lParam);
  //发牌完成
  LRESULT OnSendCardFinish(WPARAM wParam, LPARAM lParam);
  //控制
  LRESULT OnAdminControl(WPARAM wParam, LPARAM lParam);
  //控制
  LRESULT OnAdminControlEx(WPARAM wParam, LPARAM lParam);

private:
  //
  void UpdateScoreControl();
  //
  void HideScoreControl();
  //
  void PerformGameEnd();

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
