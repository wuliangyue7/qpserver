#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "GameLogic.h"
#include "DlgCustomRule.h"
#include "ServerControl.h"

//////////////////////////////////////////////////////////////////////////

//结束原因
#define GER_NO_PLAYER           0x10            //没有玩家

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction
{
  //游戏变量
protected:
  WORD              m_wOperaCount;              //操作次数
  WORD              m_wCurrentUser;             //当前用户
  BYTE              m_bSendCardCount;           //发牌数目
  BYTE              m_cbCardCount[GAME_PLAYER];       //扑克数目
  BYTE              m_cbHandCardData[GAME_PLAYER][MAX_COUNT];   //桌面扑克
  BYTE              m_cbGameEndReason;            //游戏结束理由
  bool              m_bGameEnd;               //是否结束
  bool              m_bShowHand;              //是否梭哈
  bool              m_bAndroidLose;             //机器人牌面小于玩家

  //金币信息
protected:
  LONGLONG              m_lPlayBeginGold[GAME_PLAYER];      //玩家开始积分
  LONGLONG              m_lMaxGold;               //最大下注
  LONGLONG              m_lTurnMaxGold;             //当轮最大下注
  LONGLONG              m_lBasicGold;             //单元下注
  LONGLONG              m_lTurnBasicGold;           //最少下注
  LONGLONG              m_lTableGold[2*GAME_PLAYER];      //下注金币
  LONGLONG              m_lUserScore[GAME_PLAYER];        //玩家金币
  LONGLONG              m_lLostScore[GAME_PLAYER];        //玩家扣分

  //辅助变量
protected:
  BYTE              m_cbPlayStatus[GAME_PLAYER];      //游戏状态

  //AI变量
protected:
  WORD              m_wTableCount;              //桌子数目
  WORD              m_wMeTableID;             //桌子号码
  static LONGLONG *       m_plStockScore;             //总输赢分
  LONGLONG            m_lStorageDeduct;           //回扣变量
  LONGLONG            m_lStockLimit;              //总输赢分
  LONGLONG            m_lStorageMin;              //库存下限
  LONGLONG            m_lStorageMax;              //库存封顶
  int               m_nStorageMul;              //系统输钱比例

  //服务控制
protected:
  HINSTANCE           m_hControlInst;
  IServerControl*         m_pServerContro;

  //组件变量
protected:
  CGameLogic            m_GameLogic;              //游戏逻辑
  ITableFrame           * m_pITableFrame;           //框架接口
  const tagGameServiceOption    * m_pGameServiceOption;         //配置参数
  tagGameServiceAttrib *      m_pGameServiceAttrib;         //游戏属性
  tagCustomRule *         m_pGameCustomRule;            //自定规则

  //属性变量
protected:
  static const WORD       m_wPlayerCount;             //游戏人数
  static const BYTE       m_GameStartMode;            //开始模式

  //函数定义
public:
  //构造函数
  CTableFrameSink();
  //析构函数
  virtual ~CTableFrameSink();

  //基础接口
public:
  //释放对象
  virtual VOID  Release();
  //是否有效
  virtual bool  IsValid()
  {
    return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false;
  }
  //接口查询
  virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

  //管理接口
public:
  //初始化
  virtual bool Initialization(IUnknownEx * pIUnknownEx);
  //复位桌子
  virtual VOID RepositionSink();

  //查询接口
public:
  //查询限额
  virtual LONGLONG QueryConsumeQuota(IServerUserItem * pIServerUserItem);
  //最少积分
  virtual LONGLONG QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
  //查询是否扣服务费
  virtual bool QueryBuckleServiceCharge(WORD wChairID)
  {
    return true;
  }

  //比赛接口
public:
  //设置基数
  virtual void SetGameBaseScore(LONG lBaseScore) {};


  //信息接口
public:
  //开始模式
  virtual BYTE  GetGameStartMode();
  //游戏状态
  virtual bool  IsUserPlaying(WORD wChairID);

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
  //时间事件
  virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
  //数据事件
  virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize);
  //积分事件
  virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

  //网络接口
public:
  //游戏消息
  virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
  //框架消息
  virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

  //用户事件
public:
  //用户断线
  virtual bool OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
  {
    return true;
  }
  //用户重入
  virtual bool OnActionUserConnect(WORD wChairID, IServerUserItem * pIServerUserItem)
  {
    return true;
  }
  //用户坐下
  virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
  //用户起立
  virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser);
  //用户同意
  virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
  {
    return true;
  }

  //游戏事件
protected:
  //放弃事件
  bool OnUserGiveUp(WORD wChairID,bool bLeave=false);
  //加注事件
  bool OnUserAddGold(WORD wChairID, LONGLONG lGold, bool bCancel);

  //游戏函数
protected:
  //推断胜者
  WORD EstimateWinner(BYTE cbStartPos, BYTE cbConcludePos);
  //扑克分析
  bool AnalyseCard();
  //扑克分析
  bool AnalyseCardEX();
};

//////////////////////////////////////////////////////////////////////////

#endif
