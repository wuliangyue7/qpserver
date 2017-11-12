#include "StdAfx.h"
#include "TableFrameSink.h"
#include "DlgCustomRule.h"
#include <conio.h>
#include <locale>

//////////////////////////////////////////////////////////////////////////////////

//房间玩家信息
CMap<DWORD, DWORD, ROOMUSERINFO, ROOMUSERINFO> g_MapRoomUserInfo; //玩家USERID映射玩家信息
//房间用户控制
CList<ROOMUSERCONTROL, ROOMUSERCONTROL&> g_ListRoomUserControl;   //房间用户控制链表
//操作控制记录
CList<CString, CString&> g_ListOperationRecord;           //操作控制记录

ROOMUSERINFO  g_CurrentQueryUserInfo;               //当前查询用户信息

//全局变量
LONGLONG            g_lRoomStorageStart = 0LL;                //房间起始库存
LONGLONG            g_lRoomStorageCurrent = 0LL;              //总输赢分
LONGLONG            g_lStorageDeductRoom = 0LL;               //回扣变量
LONGLONG            g_lStorageMax1Room = 0LL;               //库存封顶
LONGLONG            g_lStorageMul1Room = 0LL;               //系统输钱比例
LONGLONG            g_lStorageMax2Room = 0LL;               //库存封顶
LONGLONG            g_lStorageMul2Room = 0LL;               //系统输钱比例

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
  //游戏变量
  m_wBankerUser = INVALID_CHAIR;
  ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
  ZeroMemory(m_lInsureScore,sizeof(m_lInsureScore));
  ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
  ZeroMemory(m_bStopCard,sizeof(m_bStopCard));
  ZeroMemory(m_bDoubleCard,sizeof(m_bDoubleCard));
  ZeroMemory(m_bInsureCard,sizeof(m_bInsureCard));
  ZeroMemory(m_lMaxUserScore,sizeof(m_lMaxUserScore));
  ZeroMemory(m_lTableScoreAll,sizeof(m_lTableScoreAll));



  //扑克变量
  ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
  m_cbRepertoryCount = 0;
  ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

  //下注变量
  m_lMaxScore = 0L;
  m_lCellScore = 0L;

  //清空链表
  g_ListRoomUserControl.RemoveAll();
  g_ListOperationRecord.RemoveAll();
  ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

  //服务控制
  m_hControlInst = NULL;
  m_pServerControl = NULL;
  m_hControlInst = LoadLibrary(TEXT("BlackJackServerControl.dll"));
  if(m_hControlInst)
  {
    typedef void * (*CREATE)();
    CREATE ServerControl = (CREATE)GetProcAddress(m_hControlInst,"CreateServerControl");
    if(ServerControl)
    {
      m_pServerControl = static_cast<IServerControl*>(ServerControl());
    }
  }

  return;
}

//析构函数
CTableFrameSink::~CTableFrameSink()
{
  if(m_pServerControl)
  {
    delete m_pServerControl;
    m_pServerControl = NULL;
  }

  if(m_hControlInst)
  {
    FreeLibrary(m_hControlInst);
    m_hControlInst = NULL;
  }
}

VOID CTableFrameSink::Release()
{
  if(m_pServerControl)
  {
    delete m_pServerControl;
    m_pServerControl = NULL;
  }

  if(m_hControlInst)
  {
    FreeLibrary(m_hControlInst);
    m_hControlInst = NULL;
  }

  delete this;
}

//接口查询
VOID * CTableFrameSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
  QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
  return NULL;
}

//配置桌子
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //查询接口
  m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);

  //错误判断
  if(m_pITableFrame==NULL)
  {
    CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 ITableFrame 接口失败"),TraceLevel_Exception);
    return false;
  }

  //开始模式
  m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

  //获取配置
  m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

  //读取配置
  ReadConfigInformation();

  return true;
}

//复位桌子
VOID CTableFrameSink::RepositionSink()
{
  //游戏变量
  ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
  ZeroMemory(m_lInsureScore,sizeof(m_lInsureScore));
  ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
  ZeroMemory(m_bStopCard,sizeof(m_bStopCard));
  ZeroMemory(m_bDoubleCard,sizeof(m_bDoubleCard));
  ZeroMemory(m_bInsureCard,sizeof(m_bInsureCard));
  ZeroMemory(m_lMaxUserScore,sizeof(m_lMaxUserScore));
  ZeroMemory(m_lTableScoreAll,sizeof(m_lTableScoreAll));

  //扑克变量
  ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
  m_cbRepertoryCount = 0;
  ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

  //下注变量
  m_lMaxScore = 0L;
  m_lCellScore = 0L;

  return;
}

//用户断线
bool CTableFrameSink::OnActionUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)
{
  //更新房间用户信息
  UpdateRoomUserInfo(pIServerUserItem, USER_OFFLINE);

  return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
  //设置状态
  m_pITableFrame->SetGameStatus(GAME_SCENE_ADD_SCORE);
  //初始化变量
  ZeroMemory(m_bStopCard,sizeof(m_bStopCard));
  //库存衰减
  if(g_lRoomStorageCurrent > 0)
  {
    g_lRoomStorageCurrent = g_lRoomStorageCurrent - g_lRoomStorageCurrent*g_lStorageDeductRoom/1000;
  }

  //更新房间用户信息
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取用户
    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem != NULL)
    {
      UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
    }

  }

  //发送库存
  SendStorage();

  //发送数据
  CMD_S_GameStart GameStart;
  ZeroMemory(&GameStart,sizeof(GameStart));
  //用户状态
  ASSERT(m_wBankerUser != INVALID_CHAIR);
  SCORE lMinScore = -1;

  for(WORD i = 0; i < GAME_PLAYER; i++)
  {
    IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);
    if(pServerUserItem)
    {
      //设置玩家状态
      m_cbPlayStatus[i] = TRUE;

      if(lMinScore == -1)
      {
        lMinScore = pServerUserItem->GetUserScore();
      }
      else if(pServerUserItem->GetUserScore() < lMinScore)
      {
        lMinScore = pServerUserItem->GetUserScore();
      }
      m_lMaxUserScore[i]=pServerUserItem->GetUserScore();
    }
  }
  m_lCellScore =__max(lMinScore/600L,1);
  m_lMaxScore = m_lCellScore*50;

  GameStart.lCellScore = m_lCellScore;
  GameStart.lMaxScore = m_lMaxScore;
  GameStart.wBankerUser = m_wBankerUser;

  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
  for(WORD i = 0; i < GAME_PLAYER; i++)
  {
    if(m_cbPlayStatus[i]==TRUE)
    {
      GameStart.lMaxScore = __min(m_lMaxUserScore[i],m_lMaxScore);
      m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
    }
  }

  return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
  switch(cbReason)
  {
    case GER_NORMAL:    //常规结束
    case GER_NO_PLAYER:   //没有玩家
    {
      //定义变量
      CMD_S_GameEnd GameEnd;
      ZeroMemory(&GameEnd,sizeof(GameEnd));

      //输分
      if(AnalyseCard(false))
      {
        for(WORD i=0; i<GAME_PLAYER; i++)
        {
          //获取用户
          IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
          if(pIServerUserItem!=NULL)
          {
            if(m_cbPlayStatus[i]==FALSE)
            {
              continue;
            }
            if(pIServerUserItem->IsAndroidUser())
            {
              for(int j = 0; j < 2; j++)
              {
                BYTE cbOperateIndex = i * 2 + j;
                if(m_cbHandCardData[cbOperateIndex][0] != 0)
                {
                  bool bWin = AnalyseCardEx();;
                  BYTE cbGetCardData=0;
                  BYTE cbRepertoryCount = m_cbRepertoryCount;
                  BYTE cbHandCardData0 = m_cbHandCardData[cbOperateIndex][0];

                  while(bWin && cbRepertoryCount > 0)
                  {
                    //设置扑克
                    cbGetCardData = m_cbRepertoryCard[--cbRepertoryCount];
                    m_cbHandCardData[cbOperateIndex][0] = cbGetCardData;
                    bWin = AnalyseCardEx();
                    if(!bWin)
                    {
                      //SendInfo(TEXT("结束控制底牌！"));
                      m_cbRepertoryCard[cbRepertoryCount] = cbHandCardData0;
                      break;
                    }
                  }

                  if(bWin)    //没合适的
                  {
                    m_cbHandCardData[cbOperateIndex][0] = cbHandCardData0;
                  }
                }
              }
            }
          }
        }
      }

      //变量定义
      LONGLONG lGameScore[GAME_PLAYER];
      ZeroMemory(lGameScore,sizeof(lGameScore));
      for(WORD i = 0; i < GAME_PLAYER; i++)
      {
        if(!m_cbPlayStatus[i])
        {
          lGameScore[m_wBankerUser] += m_lTableScore[i*2]+m_lTableScore[i*2+1]+m_lInsureScore[i*2]+m_lInsureScore[i*2+1];
          continue;
        }
        //设置牌数据
        if(cbReason != GER_NO_PLAYER)
        {
          for(BYTE j = 0; j < 2; j++)
          {
            if(m_cbCardCount[i*2+j] < 2)
            {
              continue;
            }
            //如果牌面没有爆牌
            if(m_GameLogic.GetCardType(&m_cbHandCardData[i*2+j][1],m_cbCardCount[i*2+j]-1,true) > CT_BAOPAI)
            {
              GameEnd.cbCardData[i*2+j] = m_cbHandCardData[i*2+j][0];
            }
          }
        }
      }

      //比牌
      BYTE cbBankerCardType = m_GameLogic.GetCardType(m_cbHandCardData[m_wBankerUser*2],m_cbCardCount[m_wBankerUser*2],false);
      for(WORD i = 0; i < GAME_PLAYER; i++)
      {
        if(i == m_wBankerUser || !m_cbPlayStatus[i])
        {
          continue;
        }
        for(BYTE j = 0; j < 2; j++)
        {
          if(m_cbCardCount[i*2+j] == 0)
          {
            continue;
          }
          BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[i*2+j],m_cbCardCount[i*2+j],m_cbCardCount[i*2+1]>0);
          //庄赢
          if(cbCardType == CT_BAOPAI || cbCardType < cbBankerCardType)
          {
            lGameScore[m_wBankerUser] += m_lTableScore[i*2+j];
            lGameScore[i] -= m_lTableScore[i*2+j];
            //如果闲家下了保险,且庄家BJ
            if(m_lInsureScore[i*2+j] > 0L && cbBankerCardType == CT_BJ)
            {
              lGameScore[m_wBankerUser] -= m_lInsureScore[i*2+j];
              lGameScore[i] += m_lInsureScore[i*2+j];
            }
            else
            {
              lGameScore[m_wBankerUser] += m_lInsureScore[i*2+j];
              lGameScore[i] -= m_lInsureScore[i*2+j];
            }
          }
          //闲家赢
          else if(cbCardType > cbBankerCardType)
          {
            lGameScore[m_wBankerUser] -= m_lTableScore[i*2+j];
            lGameScore[i] += m_lTableScore[i*2+j];

            //如果闲家下了保险
            if(m_lInsureScore[i*2+j] > 0L)
            {
              if(cbBankerCardType != CT_BJ)
              {
                lGameScore[m_wBankerUser] += m_lInsureScore[i*2+j];
                lGameScore[i] -= m_lInsureScore[i*2+j];
              }
              else
              {
                lGameScore[m_wBankerUser] -= m_lInsureScore[i*2+j];
                lGameScore[i] += m_lInsureScore[i*2+j];
              }
            }
          }
          else
          {
            //庄家,闲家都BJ,且闲家下了保险,闲家赢得保险分，庄家输相应分
            if(CT_BJ == cbBankerCardType && cbBankerCardType == cbCardType && m_lInsureScore[i*2+j] > 0L)
            {
              lGameScore[m_wBankerUser] -= m_lInsureScore[i*2+j];
              lGameScore[i] += m_lInsureScore[i*2+j];
            }
          }
        }
      }

      //积分变量
      tagScoreInfo ScoreInfoArray[GAME_PLAYER];
      ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

      //扣税

      for(WORD i = 0; i < GAME_PLAYER; i++)
      {
        if(!m_cbPlayStatus[i])
        {
          continue;
        }

        if(lGameScore[i] > 0L)
        {
          ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
        }
        else if(lGameScore[i] < 0L)
        {
          ScoreInfoArray[i].cbType = SCORE_TYPE_LOSE;
        }
        else
        {
          ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;
        }
        //防止负分
        if(lGameScore[i] < 0L)
        {
          ASSERT(m_pITableFrame->GetTableUserItem(i));
          ASSERT(m_pITableFrame->GetTableUserItem(i)->GetUserScore()>= -lGameScore[i]);
          if(m_pITableFrame->GetTableUserItem(i)->GetUserScore() < -lGameScore[i])
          {
            lGameScore[i] = -m_pITableFrame->GetTableUserItem(i)->GetUserScore();
          }
        }
        if(GAME_GENRE_SCORE != m_pGameServiceOption->wServerType && lGameScore[i] > 0L)
        {
          ScoreInfoArray[i].lRevenue = lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000L;
          lGameScore[i] -= ScoreInfoArray[i].lRevenue;
        }
        GameEnd.lGameTax[i] = ScoreInfoArray[i].lRevenue;
        GameEnd.lGameScore[i] = lGameScore[i];

        ScoreInfoArray[i].lScore = lGameScore[i];

        if(m_pITableFrame->GetTableUserItem(i)->IsAndroidUser())
        {
          g_lRoomStorageCurrent += lGameScore[i];
        }
      }

      //发送信息
      m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      //写分
      m_pITableFrame->WriteTableScore(ScoreInfoArray,GAME_PLAYER);
      //轮流坐庄
      m_wBankerUser = (m_wBankerUser+1)%GAME_PLAYER;
      while(!m_cbPlayStatus[m_wBankerUser])
      {
        m_wBankerUser = (m_wBankerUser+1)%GAME_PLAYER;
      }

      //结束游戏
      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

      //更新房间用户信息
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

        if(!pIServerUserItem)
        {
          continue;
        }

        UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
      }

      //发送库存
      CString strInfo;
      strInfo.Format(TEXT("当前库存：%I64d"), g_lRoomStorageCurrent);
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
          continue;
        }
        if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
        {
          m_pITableFrame->SendGameMessage(pIServerUserItem, strInfo, SMT_CHAT);

          CMD_S_ADMIN_STORAGE_INFO StorageInfo;
          ZeroMemory(&StorageInfo, sizeof(StorageInfo));
          StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
          StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
          StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
          StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
          StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
          StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
          StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
          m_pITableFrame->SendTableData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
          m_pITableFrame->SendLookonData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        }
      }

      return true;
    }
    case GER_USER_LEAVE:    //用户强退
    case GER_NETWORK_ERROR:     //网络中断
    {
      //效验参数
      ASSERT(pIServerUserItem!=NULL);
      ASSERT(wChairID<GAME_PLAYER);

      //如果庄家强退
      if(wChairID == m_wBankerUser)
      {
        LONGLONG lGameScore[GAME_PLAYER];
        ZeroMemory(lGameScore,sizeof(lGameScore));

        for(WORD i = 0; i < GAME_PLAYER; i++)
        {
          if(i == m_wBankerUser || !m_cbPlayStatus[i])
          {
            continue;
          }

          lGameScore[i] += m_lTableScore[i*2] + m_lTableScore[i*2+1];
          lGameScore[i] += m_lInsureScore[i*2] + m_lInsureScore[i*2+1];

          lGameScore[m_wBankerUser] -= lGameScore[i];
        }

        CMD_S_GameEnd GameEnd;
        ZeroMemory(&GameEnd,sizeof(GameEnd));

        //积分变量
        tagScoreInfo ScoreInfoArray[GAME_PLAYER];
        ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

        //扣税
        LONGLONG lTax[GAME_PLAYER];
        ZeroMemory(lTax,sizeof(lTax));
        for(WORD i = 0; i < GAME_PLAYER; i++)
        {
          if(!m_cbPlayStatus[i])
          {
            continue;
          }

          if(GAME_GENRE_SCORE != m_pGameServiceOption->wServerType &&
             lGameScore[i] > 0L)
          {
            lTax[i] =lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000L;
            lGameScore[i] -= lTax[i];
          }

          if(lGameScore[i] > 0L)
          {
            ScoreInfoArray[i].cbType = SCORE_TYPE_WIN;
          }
          else if(lGameScore[i] < 0L)
          {
            ScoreInfoArray[i].cbType = SCORE_TYPE_LOSE;
          }
          else
          {
            ScoreInfoArray[i].cbType = SCORE_TYPE_DRAW;
          }
          GameEnd.lGameTax[i] = lTax[i];
          GameEnd.lGameScore[i] = lGameScore[i];
        }

        //防止负分
        for(WORD i = 0; i < GAME_PLAYER; i++)
        {
          if(m_cbPlayStatus[i]==FALSE)
          {
            continue;
          }

          if(GameEnd.lGameScore[i] < 0L)
          {
            ASSERT(m_pITableFrame->GetTableUserItem(i)->GetUserScore() >= -GameEnd.lGameScore[i]);
            if(m_pITableFrame->GetTableUserItem(i)->GetUserScore() < -GameEnd.lGameScore[i])
            {
              GameEnd.lGameScore[i] = -m_pITableFrame->GetTableUserItem(i)->GetUserScore();
            }
          }
          ScoreInfoArray[i].lRevenue =GameEnd.lGameTax[i];
          ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
        }

        //发送信息
        m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
        m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

        //写分
        m_pITableFrame->WriteTableScore(ScoreInfoArray,GAME_PLAYER);

        //结束游戏
        m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
      }
      //否则
      else
      {
        LONGLONG lGameScore = -(m_lTableScore[wChairID*2]+m_lTableScore[wChairID*2+1]+m_lInsureScore[wChairID*2]+m_lInsureScore[wChairID*2+1]);
        //防止负分
        ASSERT(-lGameScore <= m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore());
        if(-lGameScore > m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore())
        {
          lGameScore = -m_pITableFrame->GetTableUserItem(wChairID)->GetUserScore();
        }
        tagScoreInfo ScoreInfo;
        ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
        ScoreInfo.lScore=lGameScore;
        ScoreInfo.cbType=SCORE_TYPE_FLEE;
        m_pITableFrame->WriteUserScore(wChairID,ScoreInfo);

        //设置状态
        m_cbPlayStatus[wChairID] = FALSE;

        //如果没有闲家
        BYTE cbPlayCount = 0;
        for(WORD i = 0; i < GAME_PLAYER; i++)
        {
          if(m_cbPlayStatus[i])
          {
            cbPlayCount++;
          }
        }

        if(cbPlayCount < 2)
        {
          return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NO_PLAYER);
        }
      }

      //更新房间用户信息
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

        if(!pIServerUserItem)
        {
          continue;
        }

        UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
      }

      //发送库存
      CString strInfo;
      strInfo.Format(TEXT("当前库存：%I64d"), g_lRoomStorageCurrent);
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
        if(!pIServerUserItem)
        {
          continue;
        }
        if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
        {
          m_pITableFrame->SendGameMessage(pIServerUserItem, strInfo, SMT_CHAT);

          CMD_S_ADMIN_STORAGE_INFO StorageInfo;
          ZeroMemory(&StorageInfo, sizeof(StorageInfo));
          StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
          StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
          StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
          StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
          StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
          StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
          StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
          m_pITableFrame->SendTableData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
          m_pITableFrame->SendLookonData(i, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        }
      }

      return true;
    }
    case GER_DISMISS:
    {
      //定义变量
      CMD_S_GameEnd GameEnd;
      ZeroMemory(&GameEnd,sizeof(GameEnd));

      //发送信息
      m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

      //结束游戏
      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

      //更新房间用户信息
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        //获取用户
        IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);

        if(!pIServerUserItem)
        {
          continue;
        }

        UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);
      }

      return true;
    }
  }

  ASSERT(FALSE);
  return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
  switch(cbGameStatus)
  {
    case GAME_SCENE_FREE:   //空闲状态
    {
      //构造数据
      CMD_S_StatusFree StatusFree;
      ZeroMemory(&StatusFree,sizeof(StatusFree));
      StatusFree.lRoomStorageStart = g_lRoomStorageStart;
      StatusFree.lRoomStorageCurrent = g_lRoomStorageCurrent;

      //获取自定义配置
      tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
      ASSERT(pCustomRule);
      tagCustomAndroid CustomAndroid;
      ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
      CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
      CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
      CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
      CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
      CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
      CopyMemory(&StatusFree.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
      {
        CMD_S_ADMIN_STORAGE_INFO StorageInfo;
        ZeroMemory(&StorageInfo, sizeof(StorageInfo));
        StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
        StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
        StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
        StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
        StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
        StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
        StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;

        m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
      }

      //发送场景
      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
    }
    case GAME_SCENE_ADD_SCORE:
    {
      CMD_S_StatusAddScore StatusScore;

      //下注变量
      StatusScore.lCellScore = m_lCellScore;
      StatusScore.lMaxScore = m_lMaxScore;

      //庄家
      StatusScore.wBankerUser = m_wBankerUser;

      //桌面下注
      for(WORD i = 0; i < GAME_PLAYER; i++)
      {
        StatusScore.lTableScore[i] = m_lTableScore[i*2]+m_lTableScore[i*2+1]+m_lInsureScore[i*2]+m_lInsureScore[i*2+1];
      }
      StatusScore.lRoomStorageStart = g_lRoomStorageStart;
      StatusScore.lRoomStorageCurrent = g_lRoomStorageCurrent;

      //获取自定义配置
      tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
      ASSERT(pCustomRule);
      tagCustomAndroid CustomAndroid;
      ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
      CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
      CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
      CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
      CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
      CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
      CopyMemory(&StatusScore.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));

      //更新房间用户信息
      UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
      {
        CMD_S_ADMIN_STORAGE_INFO StorageInfo;
        ZeroMemory(&StorageInfo, sizeof(StorageInfo));
        StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
        StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
        StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
        StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
        StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
        StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
        StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
        m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
      }

      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
    }
    case GAME_SCENE_GET_CARD: //游戏状态
    {
      CMD_S_StatusGetCard StatusGetCard;

      //下注变量
      StatusGetCard.lCellScore = m_lCellScore;

      //庄家
      StatusGetCard.wBankerUser = m_wBankerUser;

      //桌面下注
      for(WORD i = 0; i < GAME_PLAYER; i++)
      {
        StatusGetCard.lTableScore[i] = m_lTableScore[i*2]+m_lTableScore[i*2+1]+m_lInsureScore[i*2]+m_lInsureScore[i*2+1];
      }

      //扑克变量
      CopyMemory(StatusGetCard.cbCardCount,m_cbCardCount,sizeof(StatusGetCard.cbCardCount));
      CopyMemory(StatusGetCard.cbHandCardData,m_cbHandCardData,sizeof(StatusGetCard.cbHandCardData));

      //手上扑克
      for(WORD i = 0; i < GAME_PLAYER; i++)
      {
        if(i == wChairID)
        {
          continue;
        }

        StatusGetCard.cbHandCardData[i*2][0] = 0;
        StatusGetCard.cbHandCardData[i*2+1][0] = 0;
      }

      CopyMemory(StatusGetCard.bStopCard,m_bStopCard,sizeof(StatusGetCard.bStopCard));
      CopyMemory(StatusGetCard.bDoubleCard,m_bDoubleCard,sizeof(StatusGetCard.bDoubleCard));
      CopyMemory(StatusGetCard.bInsureCard,m_bInsureCard,sizeof(StatusGetCard.bInsureCard));

      StatusGetCard.lRoomStorageStart = g_lRoomStorageStart;
      StatusGetCard.lRoomStorageCurrent = g_lRoomStorageCurrent;

      //获取自定义配置
      tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
      ASSERT(pCustomRule);
      tagCustomAndroid CustomAndroid;
      ZeroMemory(&CustomAndroid, sizeof(CustomAndroid));
      CustomAndroid.lRobotBankGet = pCustomRule->lRobotBankGet;
      CustomAndroid.lRobotBankGetBanker = pCustomRule->lRobotBankGetBanker;
      CustomAndroid.lRobotBankStoMul = pCustomRule->lRobotBankStoMul;
      CustomAndroid.lRobotScoreMax = pCustomRule->lRobotScoreMax;
      CustomAndroid.lRobotScoreMin = pCustomRule->lRobotScoreMin;
      CopyMemory(&StatusGetCard.CustomAndroid, &CustomAndroid, sizeof(CustomAndroid));

      //更新房间用户信息
      UpdateRoomUserInfo(pIServerUserItem, USER_RECONNECT);

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) && !pIServerUserItem->IsAndroidUser())
      {
        CMD_S_ADMIN_STORAGE_INFO StorageInfo;
        ZeroMemory(&StorageInfo, sizeof(StorageInfo));
        StorageInfo.lRoomStorageStart = g_lRoomStorageStart;
        StorageInfo.lRoomStorageCurrent = g_lRoomStorageCurrent;
        StorageInfo.lRoomStorageDeduct = g_lStorageDeductRoom;
        StorageInfo.lMaxRoomStorage[0] = g_lStorageMax1Room;
        StorageInfo.lMaxRoomStorage[1] = g_lStorageMax2Room;
        StorageInfo.wRoomStorageMul[0] = (WORD)g_lStorageMul1Room;
        StorageInfo.wRoomStorageMul[1] = (WORD)g_lStorageMul2Room;
        m_pITableFrame->SendTableData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
        m_pITableFrame->SendLookonData(wChairID, SUB_S_ADMIN_STORAGE_INFO, &StorageInfo, sizeof(StorageInfo));
      }

      //发送场景
      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusGetCard,sizeof(StatusGetCard));;
    }
  }

  //效验错误
  ASSERT(FALSE);

  return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
  return false;
}

//游戏消息
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
  switch(wSubCmdID)
  {
    case SUB_C_ADD_SCORE:     //用户加注
    {
      //效验数据
      ASSERT(wDataSize==sizeof(CMD_C_AddScore));
      if(wDataSize!=sizeof(CMD_C_AddScore))
      {
        return false;
      }

      //变量定义
      CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pData;

      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
      if(m_cbPlayStatus[pUserData->wChairID]==FALSE)
      {
        return false;
      }

      ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_ADD_SCORE);
      if(m_pITableFrame->GetGameStatus() != GAME_SCENE_ADD_SCORE)
      {
        return false;
      }

      //消息处理
      return OnUserAddScore(pUserData->wChairID,pAddScore->lScore);
    }
    case SUB_C_GET_CARD:
    {
      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
      if(m_cbPlayStatus[pUserData->wChairID]==FALSE)
      {
        return false;
      }

      ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD);
      if(m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD)
      {
        return false;
      }

      return OnUserGetCard(pUserData->wChairID);
    }
    case SUB_C_STOP_CARD:       //用户放弃
    {
      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
      if(m_cbPlayStatus[pUserData->wChairID]==FALSE)
      {
        return false;
      }

      ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD);
      if(m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD)
      {
        return false;
      }

      //消息处理
      return OnUserStopCard(pUserData->wChairID);
    }
    case SUB_C_DOUBLE_SCORE:
    {
      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
      if(m_cbPlayStatus[pUserData->wChairID]==FALSE)
      {
        return false;
      }

      ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD);
      if(m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD)
      {
        return false;
      }

      return OnUserDoubleScore(pUserData->wChairID);
    }
    case SUB_C_INSURE:
    {
      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
      if(m_cbPlayStatus[pUserData->wChairID]==FALSE)
      {
        return false;
      }

      ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD);
      if(m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD)
      {
        return false;
      }

      return OnUserInsure(pUserData->wChairID);
    }
    case SUB_C_SPLIT_CARD:
    {
      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      //状态判断
      ASSERT(m_cbPlayStatus[pUserData->wChairID]==TRUE);
      if(m_cbPlayStatus[pUserData->wChairID]==FALSE)
      {
        return false;
      }

      ASSERT(m_pITableFrame->GetGameStatus() == GAME_SCENE_GET_CARD);
      if(m_pITableFrame->GetGameStatus() != GAME_SCENE_GET_CARD)
      {
        return false;
      }

      return OnUserSplitCard(pUserData->wChairID);
    }
    case SUB_C_STORAGE:
    {
      ASSERT(wDataSize==sizeof(CMD_C_UpdateStorage));
      if(wDataSize!=sizeof(CMD_C_UpdateStorage))
      {
        return false;
      }

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
      {
        return false;
      }

      CMD_C_UpdateStorage *pUpdateStorage=(CMD_C_UpdateStorage *)pData;
      g_lRoomStorageCurrent = pUpdateStorage->lRoomStorageCurrent;
      g_lStorageDeductRoom = pUpdateStorage->lRoomStorageDeduct;

      //20条操作记录
      if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
      {
        g_ListOperationRecord.RemoveHead();
      }

      CString strOperationRecord;
      CTime time = CTime::GetCurrentTime();
      strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s],修改当前库存为 %I64d,衰减值为 %I64d"),
                                time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(),
                                g_lRoomStorageCurrent, g_lStorageDeductRoom);

      g_ListOperationRecord.AddTail(strOperationRecord);

      //写入日志
      strOperationRecord += TEXT("\n");
      WriteInfo(strOperationRecord);

      //变量定义
      CMD_S_Operation_Record OperationRecord;
      ZeroMemory(&OperationRecord, sizeof(OperationRecord));
      POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
      WORD wIndex = 0;//数组下标
      while(posListRecord)
      {
        CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

        CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
        wIndex++;
      }

      ASSERT(wIndex <= MAX_OPERATION_RECORD);

      //发送数据
      m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
      m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

      return true;
    }
    case SUB_C_STORAGEMAXMUL:
    {
      ASSERT(wDataSize==sizeof(CMD_C_ModifyStorage));
      if(wDataSize!=sizeof(CMD_C_ModifyStorage))
      {
        return false;
      }

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight())==false)
      {
        return false;
      }

      CMD_C_ModifyStorage *pModifyStorage = (CMD_C_ModifyStorage *)pData;
      g_lStorageMax1Room = pModifyStorage->lMaxRoomStorage[0];
      g_lStorageMax2Room = pModifyStorage->lMaxRoomStorage[1];
      g_lStorageMul1Room = (SCORE)(pModifyStorage->wRoomStorageMul[0]);
      g_lStorageMul2Room = (SCORE)(pModifyStorage->wRoomStorageMul[1]);

      //20条操作记录
      if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
      {
        g_ListOperationRecord.RemoveHead();
      }

      CString strOperationRecord;
      CTime time = CTime::GetCurrentTime();
      strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d,控制账户[%s], 修改库存上限值1为 %I64d,赢分概率1为 %I64d,上限值2为 %I64d,赢分概率2为 %I64d"),
                                time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), g_lStorageMax1Room, g_lStorageMul1Room, g_lStorageMax2Room, g_lStorageMul2Room);

      g_ListOperationRecord.AddTail(strOperationRecord);

      //写入日志
      strOperationRecord += TEXT("\n");
      WriteInfo(strOperationRecord);

      //变量定义
      CMD_S_Operation_Record OperationRecord;
      ZeroMemory(&OperationRecord, sizeof(OperationRecord));
      POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
      WORD wIndex = 0;//数组下标
      while(posListRecord)
      {
        CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

        CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
        wIndex++;
      }

      ASSERT(wIndex <= MAX_OPERATION_RECORD);

      //发送数据
      m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
      m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

      return true;
    }
    case SUB_C_REQUEST_QUERY_USER:
    {
      ASSERT(wDataSize == sizeof(CMD_C_RequestQuery_User));
      if(wDataSize != sizeof(CMD_C_RequestQuery_User))
      {
        return false;
      }

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser())
      {
        return false;
      }

      CMD_C_RequestQuery_User *pQuery_User = (CMD_C_RequestQuery_User *)pData;

      //遍历映射
      POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
      DWORD dwUserID = 0;
      ROOMUSERINFO userinfo;
      ZeroMemory(&userinfo, sizeof(userinfo));

      CMD_S_RequestQueryResult QueryResult;
      ZeroMemory(&QueryResult, sizeof(QueryResult));

      while(ptHead)
      {
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
        if(pQuery_User->dwGameID == userinfo.dwGameID || _tcscmp(pQuery_User->szNickName, userinfo.szNickName) == 0)
        {
          //拷贝用户信息数据
          QueryResult.bFind = true;
          CopyMemory(&(QueryResult.userinfo), &userinfo, sizeof(userinfo));

          ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
          CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
        }
      }

      //发送数据
      m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));
      m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_QUERY_RESULT, &QueryResult, sizeof(QueryResult));

      return true;
    }
    case SUB_C_USER_CONTROL:
    {
      ASSERT(wDataSize == sizeof(CMD_C_UserControl));
      if(wDataSize != sizeof(CMD_C_UserControl))
      {
        return false;
      }

      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
      {
        return false;
      }

      CMD_C_UserControl *pUserControl = (CMD_C_UserControl *)pData;

      //遍历映射
      POSITION ptMapHead = g_MapRoomUserInfo.GetStartPosition();
      DWORD dwUserID = 0;
      ROOMUSERINFO userinfo;
      ZeroMemory(&userinfo, sizeof(userinfo));

      //20条操作记录
      if(g_ListOperationRecord.GetSize() == MAX_OPERATION_RECORD)
      {
        g_ListOperationRecord.RemoveHead();
      }

      //变量定义
      CMD_S_UserControl serverUserControl;
      ZeroMemory(&serverUserControl, sizeof(serverUserControl));

      //激活控制
      if(pUserControl->userControlInfo.bCancelControl == false)
      {
        ASSERT(pUserControl->userControlInfo.control_type == CONTINUE_WIN || pUserControl->userControlInfo.control_type == CONTINUE_LOST);

        while(ptMapHead)
        {
          g_MapRoomUserInfo.GetNextAssoc(ptMapHead, dwUserID, userinfo);
          if(pUserControl->dwGameID == userinfo.dwGameID || _tcscmp(pUserControl->szNickName, userinfo.szNickName) == 0)
          {
            //激活控制标识
            bool bEnableControl = false;
            IsSatisfyControl(userinfo, bEnableControl);

            //满足控制
            if(bEnableControl)
            {
              ROOMUSERCONTROL roomusercontrol;
              ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));
              CopyMemory(&(roomusercontrol.roomUserInfo), &userinfo, sizeof(userinfo));
              CopyMemory(&(roomusercontrol.userControl), &(pUserControl->userControlInfo), sizeof(roomusercontrol.userControl));


              //遍历链表，除重
              TravelControlList(roomusercontrol);

              //压入链表（不压入同GAMEID和NICKNAME)
              g_ListRoomUserControl.AddHead(roomusercontrol);

              //复制数据
              serverUserControl.dwGameID = userinfo.dwGameID;
              CopyMemory(serverUserControl.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
              serverUserControl.controlResult = CONTROL_SUCCEED;
              serverUserControl.controlType = pUserControl->userControlInfo.control_type;
              serverUserControl.cbControlCount = pUserControl->userControlInfo.cbControlCount;

              //操作记录
              CString strOperationRecord;
              CString strControlType;
              GetControlTypeString(serverUserControl.controlType, strControlType);
              CTime time = CTime::GetCurrentTime();
              strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s], 控制玩家%s,%s,控制局数%d "),
                                        time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName, strControlType, serverUserControl.cbControlCount);

              g_ListOperationRecord.AddTail(strOperationRecord);

              //写入日志
              strOperationRecord += TEXT("\n");
              WriteInfo(strOperationRecord);
            }
            else  //不满足
            {
              //复制数据
              serverUserControl.dwGameID = userinfo.dwGameID;
              CopyMemory(serverUserControl.szNickName, userinfo.szNickName, sizeof(userinfo.szNickName));
              serverUserControl.controlResult = CONTROL_FAIL;
              serverUserControl.controlType = pUserControl->userControlInfo.control_type;
              serverUserControl.cbControlCount = 0;

              //操作记录
              CString strOperationRecord;
              CString strControlType;
              GetControlTypeString(serverUserControl.controlType, strControlType);
              CTime time = CTime::GetCurrentTime();
              strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s], 控制玩家%s,%s,失败！"),
                                        time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName, strControlType);

              g_ListOperationRecord.AddTail(strOperationRecord);

              //写入日志
              strOperationRecord += TEXT("\n");
              WriteInfo(strOperationRecord);
            }

            //发送数据
            m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
            m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

            CMD_S_Operation_Record OperationRecord;
            ZeroMemory(&OperationRecord, sizeof(OperationRecord));
            POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
            WORD wIndex = 0;//数组下标
            while(posListRecord)
            {
              CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

              CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
              wIndex++;
            }

            ASSERT(wIndex <= MAX_OPERATION_RECORD);

            //发送数据
            m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
            m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

            return true;
          }
        }

        ASSERT(FALSE);
        return false;
      }
      else  //取消控制
      {
        ASSERT(pUserControl->userControlInfo.control_type == CONTINUE_CANCEL);

        POSITION ptListHead = g_ListRoomUserControl.GetHeadPosition();
        POSITION ptTemp;
        ROOMUSERCONTROL roomusercontrol;
        ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

        //遍历链表
        while(ptListHead)
        {
          ptTemp = ptListHead;
          roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);
          if(pUserControl->dwGameID == roomusercontrol.roomUserInfo.dwGameID || _tcscmp(pUserControl->szNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
          {
            //复制数据
            serverUserControl.dwGameID = roomusercontrol.roomUserInfo.dwGameID;
            CopyMemory(serverUserControl.szNickName, roomusercontrol.roomUserInfo.szNickName, sizeof(roomusercontrol.roomUserInfo.szNickName));
            serverUserControl.controlResult = CONTROL_CANCEL_SUCCEED;
            serverUserControl.controlType = pUserControl->userControlInfo.control_type;
            serverUserControl.cbControlCount = 0;

            //移除元素
            g_ListRoomUserControl.RemoveAt(ptTemp);

            //发送数据
            m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
            m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

            //操作记录
            CString strOperationRecord;
            CTime time = CTime::GetCurrentTime();
            strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s], 取消对玩家%s的控制！"),
                                      time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName);

            g_ListOperationRecord.AddTail(strOperationRecord);

            //写入日志
            strOperationRecord += TEXT("\n");
            WriteInfo(strOperationRecord);

            CMD_S_Operation_Record OperationRecord;
            ZeroMemory(&OperationRecord, sizeof(OperationRecord));
            POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
            WORD wIndex = 0;//数组下标
            while(posListRecord)
            {
              CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

              CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
              wIndex++;
            }

            ASSERT(wIndex <= MAX_OPERATION_RECORD);

            //发送数据
            m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
            m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

            return true;
          }
        }

        //复制数据
        serverUserControl.dwGameID = pUserControl->dwGameID;
        CopyMemory(serverUserControl.szNickName, pUserControl->szNickName, sizeof(serverUserControl.szNickName));
        serverUserControl.controlResult = CONTROL_CANCEL_INVALID;
        serverUserControl.controlType = pUserControl->userControlInfo.control_type;
        serverUserControl.cbControlCount = 0;

        //发送数据
        m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));
        m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_USER_CONTROL, &serverUserControl, sizeof(serverUserControl));

        //操作记录
        CString strOperationRecord;
        CTime time = CTime::GetCurrentTime();
        strOperationRecord.Format(TEXT("操作时间: %d/%d/%d-%d:%d:%d, 控制账户[%s], 取消对玩家%s的控制，操作无效！"),
                                  time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), pIServerUserItem->GetNickName(), serverUserControl.szNickName);

        g_ListOperationRecord.AddTail(strOperationRecord);

        //写入日志
        strOperationRecord += TEXT("\n");
        WriteInfo(strOperationRecord);

        CMD_S_Operation_Record OperationRecord;
        ZeroMemory(&OperationRecord, sizeof(OperationRecord));
        POSITION posListRecord = g_ListOperationRecord.GetHeadPosition();
        WORD wIndex = 0;//数组下标
        while(posListRecord)
        {
          CString strRecord = g_ListOperationRecord.GetNext(posListRecord);

          CopyMemory(OperationRecord.szRecord[wIndex], strRecord, sizeof(OperationRecord.szRecord[wIndex]));
          wIndex++;
        }

        ASSERT(wIndex <= MAX_OPERATION_RECORD);

        //发送数据
        m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));
        m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_OPERATION_RECORD, &OperationRecord, sizeof(OperationRecord));

      }

      return true;
    }
    case SUB_C_REQUEST_UDPATE_ROOMINFO:
    {
      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
      {
        return false;
      }

      CMD_S_RequestUpdateRoomInfo_Result RoomInfo_Result;
      ZeroMemory(&RoomInfo_Result, sizeof(RoomInfo_Result));

      RoomInfo_Result.lRoomStorageCurrent = g_lRoomStorageCurrent;


      DWORD dwKeyGameID = g_CurrentQueryUserInfo.dwGameID;
      TCHAR szKeyNickName[LEN_NICKNAME];
      ZeroMemory(szKeyNickName, sizeof(szKeyNickName));
      CopyMemory(szKeyNickName, g_CurrentQueryUserInfo.szNickName, sizeof(szKeyNickName));

      //遍历映射
      POSITION ptHead = g_MapRoomUserInfo.GetStartPosition();
      DWORD dwUserID = 0;
      ROOMUSERINFO userinfo;
      ZeroMemory(&userinfo, sizeof(userinfo));

      while(ptHead)
      {
        g_MapRoomUserInfo.GetNextAssoc(ptHead, dwUserID, userinfo);
        if(dwKeyGameID == userinfo.dwGameID && _tcscmp(szKeyNickName, userinfo.szNickName) == 0)
        {
          //拷贝用户信息数据
          CopyMemory(&(RoomInfo_Result.currentqueryuserinfo), &userinfo, sizeof(userinfo));

          ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));
          CopyMemory(&(g_CurrentQueryUserInfo), &userinfo, sizeof(userinfo));
        }
      }

      //
      //变量定义
      POSITION ptListHead = g_ListRoomUserControl.GetHeadPosition();
      POSITION ptTemp;
      ROOMUSERCONTROL roomusercontrol;
      ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

      //遍历链表
      while(ptListHead)
      {
        ptTemp = ptListHead;
        roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

        //寻找玩家
        if((dwKeyGameID == roomusercontrol.roomUserInfo.dwGameID) &&
           _tcscmp(szKeyNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
        {
          RoomInfo_Result.bExistControl = true;
          CopyMemory(&(RoomInfo_Result.currentusercontrol), &(roomusercontrol.userControl), sizeof(roomusercontrol.userControl));
          break;
        }
      }

      //发送数据
      m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));
      m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT, &RoomInfo_Result, sizeof(RoomInfo_Result));

      return true;
    }
    case SUB_C_CLEAR_CURRENT_QUERYUSER:
    {
      //权限判断
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false || pIServerUserItem->IsAndroidUser() == true)
      {
        return false;
      }

      ZeroMemory(&g_CurrentQueryUserInfo, sizeof(g_CurrentQueryUserInfo));

      return true;
    }
  }

  return false;
}

//框架消息
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
  return false;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  if(!bLookonUser && m_wBankerUser == INVALID_CHAIR)
  {
    m_wBankerUser = pIServerUserItem->GetChairID();
  }

  //更新房间用户信息
  UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);

  //更新同桌用户控制
  UpdateUserControl(pIServerUserItem);

  return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  if(!bLookonUser)
  {
    if(wChairID == m_wBankerUser)
    {
      m_wBankerUser = INVALID_CHAIR;
      for(WORD i = (wChairID+1)%GAME_PLAYER; i != wChairID; i = (i+1)%GAME_PLAYER)
      {
        if(m_pITableFrame->GetTableUserItem(i))
        {
          m_wBankerUser = i;
          break;
        }
      }
    }
  }

  //更新房间用户信息
  UpdateRoomUserInfo(pIServerUserItem, USER_STANDUP);

  return true;
}

//放弃事件
bool CTableFrameSink::OnUserStopCard(WORD wChairID)
{
  //确定操作索引
  BYTE cbOperateIndex = wChairID*2;
  if(m_bStopCard[cbOperateIndex])
  {
    cbOperateIndex++;
  }

  //效验
  ASSERT(m_bStopCard[cbOperateIndex] == FALSE);
  if(m_bStopCard[cbOperateIndex])
  {
    return true;
  }

  //设置停牌
  m_bStopCard[cbOperateIndex] = TRUE;

  //如果不是庄家停牌
  if(wChairID != m_wBankerUser)
  {
    CMD_S_StopCard StopCard;
    StopCard.wStopCardUser = wChairID;

    //检查是否已全部停牌]
    bool bStopCardComplete = true;
    for(WORD i = 0; i < GAME_PLAYER; i++)
    {
      if(!m_cbPlayStatus[i] || i == m_wBankerUser)
      {
        continue;
      }
      if(!m_bStopCard[i*2] || (m_cbCardCount[i*2+1]>0 && !m_bStopCard[i*2+1]))
      {
        bStopCardComplete = false;
        break;
      }
    }
    //如果全部停牌
    if(bStopCardComplete)
    {
      StopCard.bTurnBanker = TRUE;
    }
    else
    {
      StopCard.bTurnBanker = FALSE;
    }

    //发送数据
    m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));
    m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));
  }
  //否则
  else
  {
    //发送数据
    CMD_S_StopCard StopCard;
    StopCard.wStopCardUser = wChairID;
    StopCard.bTurnBanker = FALSE;
    m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));
    m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_STOP_CARD,&StopCard,sizeof(StopCard));

    //结束游戏
    OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
  }

  return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
  //效验
  ASSERT(lScore > 0L && lScore <= m_lMaxScore &&(lScore<=m_lMaxUserScore[wChairID]));
  if(lScore <= 0L || lScore > m_lMaxScore ||lScore>m_lMaxUserScore[wChairID])
  {
    return false;
  }
  ASSERT(m_lTableScore[wChairID*2] == 0L);
  if(m_lTableScore[wChairID*2] != 0L)
  {
    return true;
  }

  //添加下注
  m_lTableScore[wChairID*2] = lScore;
  m_lTableScoreAll[wChairID]+=lScore;

  //发送数据
  CMD_S_AddScore AddScore;
  AddScore.wAddScoreUser = wChairID;
  AddScore.lAddScore = lScore;
  m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

  //判断是否已全部下注
  bool bAddScoreComplete = true;
  for(WORD i = 0; i < GAME_PLAYER; i++)
  {
    if(!m_cbPlayStatus[i] || i == m_wBankerUser)
    {
      continue;
    }
    if(m_lTableScore[i*2] == 0L)
    {
      bAddScoreComplete = false;
      break;
    }
  }
  //若已全部下注
  if(bAddScoreComplete)
  {
    //设置游戏状态
    m_pITableFrame->SetGameStatus(GAME_SCENE_GET_CARD);

    //更新房间用户信息
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //获取用户
      IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
      if(pIServerUserItem != NULL)
      {
        UpdateRoomUserInfo(pIServerUserItem, USER_SITDOWN);
      }

    }

    //混乱扑克
    m_GameLogic.RandCardList(m_cbRepertoryCard,FULL_COUNT);
    m_cbRepertoryCount = FULL_COUNT;

    CopyMemory(m_cbHandCardData[m_wBankerUser*2],&m_cbRepertoryCard[m_cbRepertoryCount-2],2*sizeof(BYTE));
    m_cbRepertoryCount -= 2;
    m_cbCardCount[m_wBankerUser*2] = 2;
    for(WORD i = (m_wBankerUser+1)%GAME_PLAYER; i != m_wBankerUser; i = (i+1)%GAME_PLAYER)
    {
      if(m_cbPlayStatus[i])
      {
        m_cbCardCount[i*2] = 2;
        CopyMemory(m_cbHandCardData[i*2],&m_cbRepertoryCard[m_cbRepertoryCount-2],sizeof(BYTE)*2);
        m_cbRepertoryCount -= 2;
      }
    }

    bool bWin = AnalyseCard(true);         //暂时有问题，不启用

    //变量定义
    ROOMUSERCONTROL roomusercontrol;
    ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));
    POSITION posKeyList;

    //控制
    if(m_pServerControl != NULL && AnalyseRoomUserControl(roomusercontrol, posKeyList))
    {
      //校验数据
      ASSERT(roomusercontrol.roomUserInfo.wChairID != INVALID_CHAIR && roomusercontrol.userControl.cbControlCount != 0
             && roomusercontrol.userControl.control_type != CONTINUE_CANCEL);

      if(m_pServerControl->ControlResult(m_cbHandCardData, m_cbCardCount, roomusercontrol, m_pITableFrame, m_wBankerUser, m_cbPlayStatus))
      {
        //获取元素
        ROOMUSERCONTROL &tmproomusercontrol = g_ListRoomUserControl.GetAt(posKeyList);

        //校验数据
        ASSERT(roomusercontrol.userControl.cbControlCount == tmproomusercontrol.userControl.cbControlCount);

        //控制局数
        tmproomusercontrol.userControl.cbControlCount--;

        CMD_S_UserControlComplete UserControlComplete;
        ZeroMemory(&UserControlComplete, sizeof(UserControlComplete));
        UserControlComplete.dwGameID = roomusercontrol.roomUserInfo.dwGameID;
        CopyMemory(UserControlComplete.szNickName, roomusercontrol.roomUserInfo.szNickName, sizeof(UserControlComplete.szNickName));
        UserControlComplete.controlType = roomusercontrol.userControl.control_type;
        UserControlComplete.cbRemainControlCount = tmproomusercontrol.userControl.cbControlCount;

        for(WORD i=0; i<GAME_PLAYER; i++)
        {
          IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
          if(!pIServerUserItem)
          {
            continue;
          }
          if(pIServerUserItem->IsAndroidUser() == true || CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()) == false)
          {
            continue;
          }

          //发送数据
          m_pITableFrame->SendTableData(i, SUB_S_USER_CONTROL_COMPLETE, &UserControlComplete, sizeof(UserControlComplete));
          m_pITableFrame->SendLookonData(i, SUB_S_USER_CONTROL_COMPLETE, &UserControlComplete, sizeof(UserControlComplete));

        }
      }
    }

    //发送数据
    CMD_S_SendCard SendCard;
    ZeroMemory(&SendCard,sizeof(SendCard));
    SendCard.bWin = bWin;

    for(WORD i = 0; i < GAME_PLAYER; i++)
    {
      SendCard.cbHandCardData[i][1] = m_cbHandCardData[i*2][1];
    }
    for(WORD i = 0; i < GAME_PLAYER; i++)
    {
      if(!m_cbPlayStatus[i])
      {
        continue;
      }

      SendCard.cbHandCardData[i][0] = m_cbHandCardData[i*2][0];

      m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
      m_pITableFrame->SendLookonData(i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
      SendCard.cbHandCardData[i][0] = 0;
    }

    if(m_pServerControl)

    {
      CMD_S_CheatCard CheatCard ;
      ZeroMemory(&CheatCard, sizeof(CheatCard));
      CopyMemory(CheatCard.cbCardData,m_cbHandCardData,sizeof(CheatCard.cbCardData));

      m_pServerControl->ServerControl(&CheatCard, m_pITableFrame);
    }
  }

  return true;
}

//玩家加倍
bool CTableFrameSink::OnUserDoubleScore(WORD wChairID)
{

  //确定操作索引
  BYTE cbOperateIndex = wChairID*2;
  if(m_bStopCard[cbOperateIndex])
  {
    cbOperateIndex++;
  }

  //效验
  ASSERT(m_bStopCard[cbOperateIndex] == FALSE);
  if(m_bStopCard[cbOperateIndex])
  {
    return true;
  }

  ASSERT(m_cbCardCount[cbOperateIndex] == 2);
  if(m_cbCardCount[cbOperateIndex] != 2)
  {
    return false;
  }

  SCORE lScoreAll = m_lTableScoreAll[wChairID]+(m_lTableScore[cbOperateIndex] *2+m_lInsureScore[cbOperateIndex]*2);
  ASSERT(lScoreAll<=m_lMaxUserScore[wChairID]);
  if(lScoreAll>m_lMaxUserScore[wChairID])
  {
    return false;
  }

  //设置加倍标志
  m_bDoubleCard[cbOperateIndex] = TRUE;

  //加倍下注与保险金
  m_lTableScore[cbOperateIndex] *= 2;
  m_lInsureScore[cbOperateIndex] *= 2;

  m_lTableScoreAll[wChairID]+=m_lTableScore[cbOperateIndex] ;
  m_lTableScoreAll[wChairID]+=m_lInsureScore[cbOperateIndex] ;

  //设置扑克
  BYTE cbGetCardData = m_cbRepertoryCard[--m_cbRepertoryCount];
  m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]++] = cbGetCardData;

  //发送数据
  CMD_S_DoubleScore DoubleScore;
  DoubleScore.cbCardData = cbGetCardData;
  DoubleScore.wDoubleScoreUser = wChairID;
  DoubleScore.lAddScore = m_lTableScore[cbOperateIndex]/2+m_lInsureScore[cbOperateIndex]/2;
  m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_DOUBLE_SCORE,&DoubleScore,sizeof(DoubleScore));
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_DOUBLE_SCORE,&DoubleScore,sizeof(DoubleScore));

  //自动停牌
  return OnUserStopCard(wChairID);
}

//玩家要牌
bool CTableFrameSink::OnUserGetCard(WORD wChairID)
{
  //确定操作索引
  BYTE cbOperateIndex = wChairID*2;
  if(m_bStopCard[cbOperateIndex])
  {
    cbOperateIndex++;
  }

  //效验
  ASSERT(m_bStopCard[cbOperateIndex] == FALSE);
  if(m_bStopCard[cbOperateIndex])
  {
    return true;
  }

  //机器人要牌转停牌
  if(AnalyseGetCard(wChairID, cbOperateIndex))
  {
    //SendInfo(TEXT("控制要牌转停牌！"));
    return true;
  }


  bool bWin = false;
  BYTE cbGetCardData=0;
  BYTE cbRepertoryCount = m_cbRepertoryCount;
  do
  {
    //设置扑克
    cbGetCardData = m_cbRepertoryCard[--cbRepertoryCount];
    m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]++] = cbGetCardData;
    bWin = AnalyseCardEx();
    m_cbCardCount[cbOperateIndex]--;
    if(!bWin)
    {
//      if (m_cbRepertoryCount - 1 > cbRepertoryCount)
//      {
//        SendInfo(TEXT("控制要牌！"));
//      }

      break;
    }

  }
  while(bWin && cbRepertoryCount > 0);


  if(!bWin && cbRepertoryCount != m_cbRepertoryCount - 1)     //有合适的
  {
    cbGetCardData = m_cbRepertoryCard[cbRepertoryCount];
    m_cbRepertoryCard[cbRepertoryCount] = m_cbRepertoryCard[m_cbRepertoryCount - 1];
    m_cbRepertoryCard[m_cbRepertoryCount - 1] = cbGetCardData;
  }

  cbGetCardData = m_cbRepertoryCard[--m_cbRepertoryCount];
  m_cbHandCardData[cbOperateIndex][m_cbCardCount[cbOperateIndex]++] = cbGetCardData;


  //发送数据
  CMD_S_GetCard GetCard;
  GetCard.cbCardData = cbGetCardData;
  GetCard.wGetCardUser = wChairID;
  m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GET_CARD,&GetCard,sizeof(GetCard));
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GET_CARD,&GetCard,sizeof(GetCard));

  //是否牌面爆牌
  BYTE cbCardType = m_GameLogic.GetCardType(&m_cbHandCardData[cbOperateIndex][1],m_cbCardCount[cbOperateIndex]-1,true);
  if(cbCardType == CT_BAOPAI || cbCardType == CT_BJ-1)
  {
    if(wChairID == m_wBankerUser)
    {
      return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
    }
    else
    {
      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

      if(pIServerUserItem->IsAndroidUser()==false)
      {
        return OnUserStopCard(wChairID);
      }
      else
      {
        return true;
      }
    }

  }

  return true;
}

//玩家下保险
bool CTableFrameSink::OnUserInsure(WORD wChairID)
{


  //确定操作索引
  BYTE cbOperateIndex = wChairID*2;
  if(m_bStopCard[cbOperateIndex])
  {
    cbOperateIndex++;
  }

  SCORE lScoreAll = m_lTableScoreAll[wChairID]+m_lTableScore[cbOperateIndex]/2;
  ASSERT(lScoreAll<=m_lMaxUserScore[wChairID]);
  if(lScoreAll>m_lMaxUserScore[wChairID])
  {
    return false;
  }

  //效验
  ASSERT(m_cbCardCount[m_wBankerUser*2] == 2 && m_GameLogic.GetCardValue(m_cbHandCardData[m_wBankerUser*2][1]) == 1);
  if(m_cbCardCount[m_wBankerUser*2] != 2 || m_GameLogic.GetCardValue(m_cbHandCardData[m_wBankerUser*2][1]) != 1)
  {
    return false;
  }

  ASSERT(m_bStopCard[cbOperateIndex] == FALSE);
  if(m_bStopCard[cbOperateIndex])
  {
    return true;
  }
  ASSERT(m_lInsureScore[cbOperateIndex] == 0L);
  if(m_lInsureScore[cbOperateIndex] != 0L)
  {
    return true;
  }

  //设置保险标志
  m_bInsureCard[cbOperateIndex] = TRUE;

  //设置保险金
  m_lInsureScore[cbOperateIndex] = m_lTableScore[cbOperateIndex]/2;

  m_lTableScoreAll[wChairID]+=m_lInsureScore[cbOperateIndex];

  //发送数据
  CMD_S_Insure UserInsure;
  UserInsure.lInsureScore = m_lInsureScore[cbOperateIndex];
  UserInsure.wInsureUser = wChairID;
  m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_INSURE,&UserInsure,sizeof(UserInsure));
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_INSURE,&UserInsure,sizeof(UserInsure));

  return true;
}

//玩家分牌
bool CTableFrameSink::OnUserSplitCard(WORD wChairID)
{
  //确定操作索引
  BYTE cbOperateIndex = wChairID*2;

  //效验
  ASSERT(wChairID != m_wBankerUser);
  if(wChairID == m_wBankerUser)
  {
    return false;
  }

  //效验
  ASSERT(m_bStopCard[cbOperateIndex] == FALSE);
  if(m_bStopCard[cbOperateIndex])
  {
    return true;
  }

  ASSERT(m_cbCardCount[cbOperateIndex+1] == 0);
  if(m_cbCardCount[cbOperateIndex+1] != 0)
  {
    return false;
  }

  ASSERT(m_cbCardCount[cbOperateIndex] == 2 && m_GameLogic.GetCardValue(m_cbHandCardData[cbOperateIndex][0]) ==
         m_GameLogic.GetCardValue(m_cbHandCardData[cbOperateIndex][1]));
  if(m_cbCardCount[cbOperateIndex] != 2 || m_GameLogic.GetCardValue(m_cbHandCardData[cbOperateIndex][0]) !=
     m_GameLogic.GetCardValue(m_cbHandCardData[cbOperateIndex][1]))
  {
    return false;
  }

  SCORE lScoreAll = m_lTableScoreAll[wChairID]+m_lTableScore[cbOperateIndex]+m_lInsureScore[cbOperateIndex];
  ASSERT(lScoreAll<=m_lMaxUserScore[wChairID]);
  if(lScoreAll>m_lMaxUserScore[wChairID])
  {
    return false;
  }

  //加倍注数与保险金
  m_lTableScore[cbOperateIndex+1] = m_lTableScore[cbOperateIndex];
  m_lInsureScore[cbOperateIndex+1] = m_lInsureScore[cbOperateIndex];

  m_lTableScoreAll[wChairID]+=m_lTableScore[cbOperateIndex];
  m_lTableScoreAll[wChairID]+=m_lInsureScore[cbOperateIndex];

  //设置保险
  if(m_bInsureCard[cbOperateIndex])
  {
    m_bInsureCard[cbOperateIndex+1] = TRUE;
  }

  //设置扑克
  m_cbHandCardData[cbOperateIndex+1][1] = m_cbHandCardData[cbOperateIndex][1];
  m_cbHandCardData[cbOperateIndex][0] = m_cbRepertoryCard[--m_cbRepertoryCount];
  m_cbHandCardData[cbOperateIndex+1][0] = m_cbRepertoryCard[--m_cbRepertoryCount];
  m_cbCardCount[cbOperateIndex+1] = 2;

  //发送数据
  CMD_S_SplitCard SplitCard;
  SplitCard.wSplitUser = wChairID;
  SplitCard.lAddScore = m_lTableScore[cbOperateIndex]+m_lInsureScore[cbOperateIndex];
  SplitCard.bInsured = m_lInsureScore[cbOperateIndex]>0L?TRUE:FALSE;
  for(WORD i = 0; i < GAME_PLAYER; i++)
  {
    if(!m_cbPlayStatus[i])
    {
      continue;
    }
    if(i == wChairID)
    {
      SplitCard.cbCardData[0] = m_cbHandCardData[cbOperateIndex][0];
      SplitCard.cbCardData[1] = m_cbHandCardData[cbOperateIndex+1][0];
    }
    else
    {
      ZeroMemory(SplitCard.cbCardData,sizeof(SplitCard.cbCardData));
    }
    m_pITableFrame->SendTableData(i,SUB_S_SPLIT_CARD,&SplitCard,sizeof(SplitCard));
    m_pITableFrame->SendLookonData(i,SUB_S_SPLIT_CARD,&SplitCard,sizeof(SplitCard));
  }
  if(m_pServerControl)
  {
    CMD_S_CheatCard CheatCard ;
    ZeroMemory(&CheatCard, sizeof(CheatCard));
    CopyMemory(CheatCard.cbCardData,m_cbHandCardData,sizeof(CheatCard.cbCardData));

    m_pServerControl->ServerControl(&CheatCard, m_pITableFrame);
  }

  return true;
}

//扑克分析
bool CTableFrameSink::AnalyseCard(bool bContron)
{
  //机器人数
  bool bIsAiBanker = false;
  WORD wAiCount = 0;
  WORD wPlayerCount = 0;
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取用户
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem!=NULL)
    {
      if(m_cbPlayStatus[i]==FALSE)
      {
        continue;
      }
      if(pIServerUserItem->IsAndroidUser())
      {
        wAiCount++ ;
        if(!bIsAiBanker && i==m_wBankerUser)
        {
          bIsAiBanker = true;
        }
      }
      wPlayerCount++;
    }
  }

  //全部机器
  if(wPlayerCount == wAiCount || wAiCount==0)
  {
    return false;
  }

  //扑克变量
  BYTE cbUserCardData[GAME_PLAYER*2][MAX_COUNT];
  CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

  //牛牛数据
  BYTE bUserOxData[GAME_PLAYER*2];
  ZeroMemory(bUserOxData,sizeof(bUserOxData));
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    if(m_cbPlayStatus[i]==FALSE)
    {
      continue;
    }
    bUserOxData[i*2] = m_GameLogic.GetCardType(cbUserCardData[i*2],m_cbCardCount[i*2],false);
  }

  //变量定义
  LONGLONG lAndroidScore=0;

  //机器庄家
  if(bIsAiBanker)
  {
    //对比扑克
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //用户过滤
      if((i==m_wBankerUser)||(m_cbPlayStatus[i]==FALSE))
      {
        continue;
      }

      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //机器过滤
      if((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser()))
      {
        continue;
      }

      //庄家输
      if(bUserOxData[i*2] > bUserOxData[m_wBankerUser*2])
      {
        lAndroidScore-=m_lTableScore[i*2];
      }
      else//庄家赢
      {
        lAndroidScore+=m_lTableScore[i*2];
      }
    }
  }
  else//用户庄家
  {
    //对比扑克
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //用户过滤
      if((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser()))
      {
        continue;
      }

      //庄家输
      if(bUserOxData[i*2] > bUserOxData[m_wBankerUser*2])
      {
        lAndroidScore+=m_lTableScore[i*2];
      }
      else
      {
        lAndroidScore-=m_lTableScore[i*2];
      }
    }
  }

  if((g_lRoomStorageCurrent+lAndroidScore) > 0)
  {
    return false;
  }

  if(bContron)
  {
    //变量定义
    WORD wWinUser=m_wBankerUser;

    //查找数据
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //用户过滤
      if(m_cbPlayStatus[i]==FALSE || i == m_wBankerUser)
      {
        continue;
      }

      if(bIsAiBanker)
      {
        if(bUserOxData[i*2] > bUserOxData[wWinUser*2])
        {
          wWinUser=i;
          continue;
        }
      }
      else
      {
        if(bUserOxData[i*2] < bUserOxData[wWinUser*2])
        {
          wWinUser=i;
          continue;
        }
      }
    }

    //交换数据
    BYTE cbTempData[MAX_COUNT];
    CopyMemory(cbTempData,m_cbHandCardData[m_wBankerUser*2],MAX_COUNT);
    CopyMemory(m_cbHandCardData[m_wBankerUser*2],m_cbHandCardData[wWinUser*2],MAX_COUNT);
    CopyMemory(m_cbHandCardData[wWinUser*2],cbTempData,MAX_COUNT);

    //SendInfo(TEXT("控制发牌！"));

    //对比扑克
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //用户过滤
      if(pIServerUserItem != NULL && pIServerUserItem->IsAndroidUser())
      {
        if(m_GameLogic.GetCardLogicValue(m_cbHandCardData[i * 2][1]) < m_GameLogic.GetCardLogicValue(m_cbHandCardData[i * 2][0]))
        {
          BYTE cbTemp = m_cbHandCardData[i * 2][1];
          m_cbHandCardData[i * 2][1] = m_cbHandCardData[i * 2][0];
          m_cbHandCardData[i * 2][0] = cbTemp;
        }
      }
    }
  }


  return true;
}


//扑克分析
bool CTableFrameSink::AnalyseCardEx()
{
  //机器人数
  bool bIsAiBanker = false;
  WORD wAiCount = 0;
  WORD wPlayerCount = 0;
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取用户
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem!=NULL)
    {
      if(m_cbPlayStatus[i]==FALSE)
      {
        continue;
      }
      if(pIServerUserItem->IsAndroidUser())
      {
        wAiCount++ ;
        if(!bIsAiBanker && i==m_wBankerUser)
        {
          bIsAiBanker = true;
        }
      }
      wPlayerCount++;
    }
  }

  //全部机器
  if(wPlayerCount == wAiCount || wAiCount==0)
  {
    return false;
  }

  //扑克变量
  BYTE cbUserCardData[GAME_PLAYER*2][MAX_COUNT];
  CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

  //牛牛数据
  BYTE bUserOxData[GAME_PLAYER*2];
  ZeroMemory(bUserOxData,sizeof(bUserOxData));
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    if(m_cbPlayStatus[i]==FALSE)
    {
      continue;
    }
    for(BYTE j = 0; j < 2; j++)
    {
      if(m_cbCardCount[i*2+j] < 2)
      {
        continue;
      }
      bUserOxData[i*2 + j] = m_GameLogic.GetCardType(cbUserCardData[i*2 + j],m_cbCardCount[i*2 + j],false);
    }
  }

  //变量定义
  LONGLONG lAndroidScore=0;

  //机器庄家
  if(bIsAiBanker)
  {
    //对比扑克
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //用户过滤
      if((i==m_wBankerUser)||(m_cbPlayStatus[i]==FALSE))
      {
        continue;
      }

      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //机器过滤
      if((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser()))
      {
        continue;
      }

      for(BYTE j = 0; j < 2; j++)
      {
        if(m_cbCardCount[i*2+j] < 2)
        {
          continue;
        }

        if(bUserOxData[i*2+j] > bUserOxData[m_wBankerUser*2]) //闲家输
        {
          lAndroidScore-=m_lTableScore[i*2+j];
        }
        else                        //庄家赢
        {
          lAndroidScore+=m_lTableScore[i*2+j];
        }
      }
    }
  }
  else//用户庄家
  {
    //对比扑克
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      //获取用户
      IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

      //用户过滤
      if((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser()))
      {
        continue;
      }


      for(BYTE j = 0; j < 2; j++)
      {
        if(m_cbCardCount[i*2+j] < 2)
        {
          continue;
        }
        if(bUserOxData[i*2+j] > bUserOxData[m_wBankerUser*2])         //庄家输
        {
          lAndroidScore+=m_lTableScore[i*2+j];
        }
        else
        {
          lAndroidScore-=m_lTableScore[i*2+j];
        }
      }
    }
  }

  if(g_lRoomStorageCurrent + lAndroidScore > 0)
  {
    return false;
  }

  if((g_lRoomStorageCurrent > g_lStorageMax2Room && rand()%100 < g_lStorageMul2Room)
     || (g_lRoomStorageCurrent > g_lStorageMax1Room && rand()%100 < g_lStorageMul1Room))
  {
    if(lAndroidScore < 0 && g_lRoomStorageCurrent + lAndroidScore > 0)
    {
      return false;
    }
  }

  return true;
}

//扑克分析
bool CTableFrameSink::AnalyseGetCard(WORD wChairID, BYTE cbOperateIndex)
{
  //不是机器人不处理
  IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
  if(pIServerUserItem!=NULL && !pIServerUserItem->IsAndroidUser())
  {
    return false;
  }

  //不是机器人不处理
  pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
  if(pIServerUserItem!=NULL && pIServerUserItem->IsAndroidUser() && m_wBankerUser != wChairID)
  {
    return false;
  }

  //机器人数
  bool bIsAiBanker = false;
  WORD wAiCount = 0;
  WORD wPlayerCount = 0;
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取用户
    IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUserItem!=NULL)
    {
      if(m_cbPlayStatus[i]==FALSE)
      {
        continue;
      }
      if(pIServerUserItem->IsAndroidUser())
      {
        wAiCount++ ;
        if(!bIsAiBanker && i==m_wBankerUser)
        {
          bIsAiBanker = true;
        }
      }
      wPlayerCount++;
    }
  }

  //全部机器
  if(wPlayerCount == wAiCount || wAiCount==0)
  {
    return false;
  }

  //机器人庄家
  if(bIsAiBanker)
  {
    //庄家要牌
    if(!AnalyseCardEx())
    {
      bool bSplitCard = m_cbCardCount[cbOperateIndex] > 0 ? true : false;
      BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[cbOperateIndex],m_cbCardCount[cbOperateIndex],bSplitCard);
      if(cbCardType >= 15)
      {
        OnUserStopCard(wChairID);
        return true;
      }
    }
  }
  else      //真人庄家
  {

  }




  return false;
}

//发送库存
void CTableFrameSink::SendStorage(WORD wChairID/* = INVALID_CHAIR*/)
{
  //通知消息
  TCHAR szMessage[128]=TEXT("");
  _sntprintf(szMessage,CountArray(szMessage),TEXT("当前库存：%I64d"), g_lRoomStorageCurrent);

  //用户状态
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取用户
    IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUser != NULL)
    {
      if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight()))
      {
        m_pITableFrame->SendGameMessage(pIServerUser,szMessage,SMT_CHAT);
      }
    }
  }

  int nLookonCount = 0;
  IServerUserItem* pLookonUserItem = m_pITableFrame->EnumLookonUserItem(nLookonCount);
  while(pLookonUserItem)
  {
    if(CUserRight::IsGameCheatUser(pLookonUserItem->GetUserRight()))
    {
      m_pITableFrame->SendGameMessage(pLookonUserItem,szMessage,SMT_CHAT);
    }

    nLookonCount++;
    pLookonUserItem = m_pITableFrame->EnumLookonUserItem(nLookonCount);
  }
}

//发送消息
void CTableFrameSink::SendInfo(CString str, WORD wChairID)
{
  //通知消息
  TCHAR szMessage[128]=TEXT("");
  _sntprintf(szMessage,CountArray(szMessage),TEXT("%s"), str);

  //用户状态
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取用户
    IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);
    if(pIServerUser != NULL)
    {
      if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight()))
      {
        m_pITableFrame->SendGameMessage(pIServerUser,szMessage,SMT_CHAT);
      }
    }
  }

  int nLookonCount = 0;
  IServerUserItem* pLookonUserItem = m_pITableFrame->EnumLookonUserItem(nLookonCount);
  while(pLookonUserItem)
  {
    if(CUserRight::IsGameCheatUser(pLookonUserItem->GetUserRight()))
    {
      m_pITableFrame->SendGameMessage(pLookonUserItem,szMessage,SMT_CHAT);
    }

    nLookonCount++;
    pLookonUserItem = m_pITableFrame->EnumLookonUserItem(nLookonCount);
  }
}

//读取配置
void CTableFrameSink::ReadConfigInformation()
{
  //获取自定义配置
  tagCustomRule *pCustomRule = (tagCustomRule *)m_pGameServiceOption->cbCustomRule;
  ASSERT(pCustomRule);

  g_lRoomStorageStart = pCustomRule->lRoomStorageStart;
  g_lRoomStorageCurrent = pCustomRule->lRoomStorageStart;
  g_lStorageDeductRoom = pCustomRule->lRoomStorageDeduct;
  g_lStorageMax1Room = pCustomRule->lRoomStorageMax1;
  g_lStorageMul1Room = pCustomRule->lRoomStorageMul1;
  g_lStorageMax2Room = pCustomRule->lRoomStorageMax2;
  g_lStorageMul2Room = pCustomRule->lRoomStorageMul2;

  if(g_lStorageDeductRoom < 0 || g_lStorageDeductRoom > 1000)
  {
    g_lStorageDeductRoom = 0;
  }
  if(g_lStorageDeductRoom > 1000)
  {
    g_lStorageDeductRoom = 1000;
  }
  if(g_lStorageMul1Room < 0 || g_lStorageMul1Room > 100)
  {
    g_lStorageMul1Room = 50;
  }
  if(g_lStorageMul2Room < 0 || g_lStorageMul2Room > 100)
  {
    g_lStorageMul2Room = 80;
  }
}

//更新房间用户信息
void CTableFrameSink::UpdateRoomUserInfo(IServerUserItem *pIServerUserItem, USERACTION userAction)
{
  //变量定义
  ROOMUSERINFO roomUserInfo;
  ZeroMemory(&roomUserInfo, sizeof(roomUserInfo));

  roomUserInfo.dwGameID = pIServerUserItem->GetGameID();
  CopyMemory(&(roomUserInfo.szNickName), pIServerUserItem->GetNickName(), sizeof(roomUserInfo.szNickName));
  roomUserInfo.cbUserStatus = pIServerUserItem->GetUserStatus();
  roomUserInfo.cbGameStatus = m_pITableFrame->GetGameStatus();

  roomUserInfo.bAndroid = pIServerUserItem->IsAndroidUser();

  //用户坐下和重连
  if(userAction == USER_SITDOWN || userAction == USER_RECONNECT)
  {
    roomUserInfo.wChairID = pIServerUserItem->GetChairID();
    roomUserInfo.wTableID = pIServerUserItem->GetTableID() + 1;
  }
  else if(userAction == USER_STANDUP || userAction == USER_OFFLINE)
  {
    roomUserInfo.wChairID = INVALID_CHAIR;
    roomUserInfo.wTableID = INVALID_TABLE;
  }

  g_MapRoomUserInfo.SetAt(pIServerUserItem->GetUserID(), roomUserInfo);
}

//更新同桌用户控制
void CTableFrameSink::UpdateUserControl(IServerUserItem *pIServerUserItem)
{
  //变量定义
  POSITION ptListHead;
  POSITION ptTemp;
  ROOMUSERCONTROL roomusercontrol;

  //初始化
  ptListHead = g_ListRoomUserControl.GetHeadPosition();
  ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

  //遍历链表
  while(ptListHead)
  {
    ptTemp = ptListHead;
    roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

    //寻找已存在控制玩家
    if((pIServerUserItem->GetGameID() == roomusercontrol.roomUserInfo.dwGameID) &&
       _tcscmp(pIServerUserItem->GetNickName(), roomusercontrol.roomUserInfo.szNickName) == 0)
    {
      //获取元素
      ROOMUSERCONTROL &tmproomusercontrol = g_ListRoomUserControl.GetAt(ptTemp);

      //重设参数
      tmproomusercontrol.roomUserInfo.wChairID = pIServerUserItem->GetChairID();
      tmproomusercontrol.roomUserInfo.wTableID = m_pITableFrame->GetTableID() + 1;

      return;
    }
  }
}

//除重用户控制
void CTableFrameSink::TravelControlList(ROOMUSERCONTROL keyroomusercontrol)
{
  //变量定义
  POSITION ptListHead;
  POSITION ptTemp;
  ROOMUSERCONTROL roomusercontrol;

  //初始化
  ptListHead = g_ListRoomUserControl.GetHeadPosition();
  ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

  //遍历链表
  while(ptListHead)
  {
    ptTemp = ptListHead;
    roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

    //寻找已存在控制玩家在用一张桌子切换椅子
    if((keyroomusercontrol.roomUserInfo.dwGameID == roomusercontrol.roomUserInfo.dwGameID) &&
       _tcscmp(keyroomusercontrol.roomUserInfo.szNickName, roomusercontrol.roomUserInfo.szNickName) == 0)
    {
      g_ListRoomUserControl.RemoveAt(ptTemp);
    }
  }
}

//是否满足控制条件
void CTableFrameSink::IsSatisfyControl(ROOMUSERINFO &userInfo, bool &bEnableControl)
{
  if(userInfo.wChairID == INVALID_CHAIR || userInfo.wTableID == INVALID_TABLE)
  {
    bEnableControl = FALSE;
    return;
  }

  if(userInfo.cbUserStatus == US_SIT || userInfo.cbUserStatus == US_READY || userInfo.cbUserStatus == US_PLAYING)
  {
    if(userInfo.cbGameStatus == GAME_STATUS_FREE)
    {
      bEnableControl = TRUE;
      return;
    }
    else
    {
      bEnableControl = FALSE;
      return;
    }
  }
  else
  {
    bEnableControl = FALSE;
    return;
  }
}

//分析房间用户控制
bool CTableFrameSink::AnalyseRoomUserControl(ROOMUSERCONTROL &Keyroomusercontrol, POSITION &ptList)
{
  //变量定义
  POSITION ptListHead;
  POSITION ptTemp;
  ROOMUSERCONTROL roomusercontrol;

  //遍历链表
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
    if(!pIServerUserItem)
    {
      continue;
    }

    //初始化
    ptListHead = g_ListRoomUserControl.GetHeadPosition();
    ZeroMemory(&roomusercontrol, sizeof(roomusercontrol));

    //遍历链表
    while(ptListHead)
    {
      ptTemp = ptListHead;
      roomusercontrol = g_ListRoomUserControl.GetNext(ptListHead);

      //寻找玩家
      if((pIServerUserItem->GetGameID() == roomusercontrol.roomUserInfo.dwGameID) &&
         _tcscmp(pIServerUserItem->GetNickName(), roomusercontrol.roomUserInfo.szNickName) == 0)
      {
        //清空控制局数为0的元素
        if(roomusercontrol.userControl.cbControlCount == 0)
        {
          g_ListRoomUserControl.RemoveAt(ptTemp);
          break;
        }

        if(roomusercontrol.userControl.control_type == CONTINUE_CANCEL)
        {
          g_ListRoomUserControl.RemoveAt(ptTemp);
          break;
        }

        //拷贝数据
        CopyMemory(&Keyroomusercontrol, &roomusercontrol, sizeof(roomusercontrol));
        ptList = ptTemp;

        return true;
      }

    }

  }

  return false;
}

void CTableFrameSink::GetControlTypeString(CONTROL_TYPE &controlType, CString &controlTypestr)
{
  switch(controlType)
  {
    case CONTINUE_WIN:
    {
      controlTypestr = TEXT("控制类型为连赢");
      break;
    }
    case CONTINUE_LOST:
    {
      controlTypestr = TEXT("控制类型为连输");
      break;
    }
    case CONTINUE_CANCEL:
    {
      controlTypestr = TEXT("控制类型为取消控制");
      break;
    }
  }
}

//写日志文件
void CTableFrameSink::WriteInfo(LPCTSTR pszString)
{
  //设置语言区域
  char* old_locale = _strdup(setlocale(LC_CTYPE,NULL));
  setlocale(LC_CTYPE, "chs");

  CStdioFile myFile;
  CString strFileName = TEXT("21点控制LOG.txt");
  BOOL bOpen = myFile.Open(strFileName, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
  if(bOpen)
  {
    myFile.SeekToEnd();
    myFile.WriteString(pszString);
    myFile.Flush();
    myFile.Close();
  }

  //还原区域设定
  setlocale(LC_CTYPE, old_locale);
  free(old_locale);
}
//////////////////////////////////////////////////////////////////////////////////
