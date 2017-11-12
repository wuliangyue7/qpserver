#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "ServerControl.h"
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////

//游戏桌子
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
  //游戏变量
protected:
  WORD              m_wBankerUser;              //庄家

  SCORE             m_lTableScore[GAME_PLAYER*2];     //桌面下注
  SCORE             m_lInsureScore[GAME_PLAYER*2];      //保险金

  BYTE              m_cbPlayStatus[GAME_PLAYER];      //玩家状态

  BYTE              m_bStopCard[GAME_PLAYER*2];       //是否停牌
  BYTE              m_bDoubleCard[GAME_PLAYER*2];     //是否加倍
  BYTE              m_bInsureCard[GAME_PLAYER*2];     //是否保险

  //扑克变量
protected:
  BYTE              m_cbCardCount[GAME_PLAYER*2];       //扑克数目
  BYTE              m_cbHandCardData[GAME_PLAYER*2][MAX_COUNT]; //桌面扑克
  BYTE              m_cbRepertoryCard[FULL_COUNT];      //剩余扑克
  BYTE              m_cbRepertoryCount;           //剩余扑克数

  //下注信息
protected:
  SCORE             m_lMaxScore;              //最大下注
  SCORE             m_lCellScore;             //单元下注
  SCORE             m_lMaxUserScore[GAME_PLAYER];
  SCORE             m_lTableScoreAll[GAME_PLAYER];

  //组件变量
protected:
  CGameLogic            m_GameLogic;              //游戏逻辑
  ITableFrame           * m_pITableFrame;           //框架接口
  tagGameServiceOption *      m_pGameServiceOption;         //游戏配置
  tagGameServiceAttrib *      m_pGameServiceAttrib;         //游戏属性

  //服务控制
protected:
  HINSTANCE           m_hControlInst;
  IServerControl*         m_pServerControl;

  //函数定义
public:
  //构造函数
  CTableFrameSink();
  //析构函数
  virtual ~CTableFrameSink();

  //基础接口
public:
  //释放对象
  virtual VOID Release();
  //接口查询
  virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //管理接口
public:
  //复位接口
  virtual VOID RepositionSink();
  //配置接口
  virtual bool Initialization(IUnknownEx * pIUnknownEx);
  //比赛接口
public:
  //设置基数
  virtual void SetGameBaseScore(LONG lBaseScore) {};

  //游戏事件
public:
  //游戏开始
  virtual bool OnEventGameStart();
  //游戏结束
  virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
  //发送场景
  virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

  //事件接口
public:
  //定时器事件
  virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
  //游戏消息
  virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
  //框架消息
  virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

  //用户事件
public:
  //用户断线
  virtual bool OnActionUserOffLine(WORD wChairID,IServerUserItem * pIServerUserItem);
  //用户重入
  virtual bool OnActionUserConnect(WORD wChairID,IServerUserItem * pIServerUserItem)
  {
    return true;
  }
  //用户坐下
  virtual bool OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
  //用户起立
  virtual bool OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser);
  //用户同意
  virtual bool OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
  {
    return true;
  }
  //查询接口
public:
  //查询限额
  virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem)
  {
    return 0;
  }
  //最少积分
  virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
  {
    return 0;
  }
  //数据事件
  virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)
  {
    return false;
  }
  //积分事件
  virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
  {
    return false;
  }
  //查询是否扣服务费
  virtual bool QueryBuckleServiceCharge(WORD wChairID)
  {
    return true;
  }
  //游戏事件
protected:
  //放弃事件
  bool OnUserStopCard(WORD wChairID);
  //加注事件
  bool OnUserAddScore(WORD wChairID, LONGLONG lScore);
  //玩家加倍
  bool OnUserDoubleScore(WORD wChairID);
  //玩家分牌
  bool OnUserSplitCard(WORD wChairID);
  //玩家下保险
  bool OnUserInsure(WORD wChairID);
  //玩家要牌
  bool OnUserGetCard(WORD wChairID);
  //扑克分析
  bool AnalyseCard(bool bContron);
  //扑克分析
  bool AnalyseCardEx();
  //扑克分析
  bool AnalyseGetCard(WORD wChairID, BYTE cbOperateIndex);
  //发送库存
  void SendStorage(WORD wChairID = INVALID_CHAIR);
  //发送消息
  void SendInfo(CString str, WORD wChairID = INVALID_CHAIR);
  //读取配置
  void ReadConfigInformation();
  //更新房间用户信息
  void UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction);
  //更新同桌用户控制
  void UpdateUserControl(IServerUserItem *pIServerUserItem);
  //除重用户控制
  void TravelControlList(ROOMUSERCONTROL keyroomusercontrol);
  //是否满足控制条件
  void IsSatisfyControl(ROOMUSERINFO &userInfo, bool &bEnableControl);
  //分析房间用户控制
  bool AnalyseRoomUserControl(ROOMUSERCONTROL &Keyroomusercontrol, POSITION &ptList);
  //获取控制类型
  void GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr);
  //写日志文件
  void WriteInfo(LPCTSTR pszString);
};

//////////////////////////////////////////////////////////////////////////////////

#endif