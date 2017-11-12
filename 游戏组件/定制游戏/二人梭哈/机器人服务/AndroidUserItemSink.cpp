#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS         2                 //最少时间

//游戏时间
#define TIME_USER_CALL_BANKER   2                 //叫庄定时器
#define TIME_USER_START_GAME    3                 //开始定时器
#define TIME_USER_ADD_SCORE     3                 //下注定时器
#define TIME_USER_OPEN_CARD     12                  //摊牌定时器

//游戏时间
#define IDI_START_GAME      (100)     //开始时间
#define IDI_USER_ADD_SCORE    (102)     //下注时间
#define IDI_OPEN_CARD     (103)     //开牌时间
#define IDI_GIVE_UP       (104)     //放弃
#define IDI_SHOW_HAND     (105)     //梭哈
#define IDI_FOLLOW        (106)     //跟猪

//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
  m_lTurnMaxScore = 0;
  m_lBasicGold=0;
  m_lShowHandGold=0;
  m_lCurrentLessGold=0;
  ZeroMemory(m_HandCardCount,sizeof(m_HandCardCount));
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));


  //接口变量
  m_pIAndroidUserItem=NULL;
  srand((unsigned)time(NULL));

  return;
}

//析构函数
CAndroidUserItemSink::~CAndroidUserItemSink()
{
}

//接口查询
void * CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
  return NULL;
}

//初始接口
bool CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //查询接口
  m_pIAndroidUserItem=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IAndroidUserItem);
  if(m_pIAndroidUserItem==NULL)
  {
    return false;
  }

  return true;
}

//重置接口
bool CAndroidUserItemSink::RepositionSink()
{
  m_lTurnMaxScore = 0;
  m_lBasicGold=0;
  m_lShowHandGold=0;
  m_lCurrentLessGold=0;
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));

  return true;
}

//时间消息
bool CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
  switch(nTimerID)
  {
    case IDI_START_GAME:    //开始定时器
    {
      //发送准备
      m_pIAndroidUserItem->SendUserReady(NULL,0);
      //BankOperate();
      return true;
    }
    case IDI_USER_ADD_SCORE:  //加注定时
    {
      //可下注筹码
      LONGLONG lUserMaxScore[4]= {0,m_lBasicGold,m_lBasicGold*2,m_lBasicGold*3};
      //发送消息
      CMD_C_AddGold AddScore;
      AddScore.lGold=lUserMaxScore[rand()%4];
      m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_GOLD,&AddScore,sizeof(AddScore));

      return true;
    }
    case IDI_SHOW_HAND:
    {
      //发送消息
      CMD_C_AddGold AddScore;
      AddScore.lGold=m_lShowHandGold;
      m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_GOLD,&AddScore,sizeof(AddScore));
      return true;
    }
    case IDI_GIVE_UP:
    {
      m_pIAndroidUserItem->SendSocketData(SUB_C_GIVE_UP);
      return true;
    }
    case IDI_FOLLOW:
    {
      //发送消息
      CMD_C_AddGold AddScore;
      AddScore.lGold=m_lCurrentLessGold;
      m_pIAndroidUserItem->SendSocketData(SUB_C_ADD_GOLD,&AddScore,sizeof(AddScore));

      return true;
    }
  }

  return false;
}

//游戏消息
bool CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_START:  //游戏开始
    {
      //消息处理
      return OnSubGameStart(pData,wDataSize);
    }
    case SUB_S_GIVE_UP: //用户放弃
    {
      //消息处理
      return OnSubGiveUp(pData,wDataSize);
    }
    case SUB_S_SEND_CARD: //发牌消息
    {
      //消息处理
      return OnSubSendCard(pData,wDataSize);
    }
    case SUB_S_GAME_END:  //游戏结束
    {
      //消息处理
      return OnSubGameEnd(pData,wDataSize);
    }
    case SUB_S_ALL_CARD:
    {
      return true;
    }
    case SUB_S_ADD_GOLD:
    {
      return OnSubAddGold(pData,wDataSize);
    }
  }

  //错误断言
  ASSERT(FALSE);

  return true;
}

//游戏消息
bool CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, void * pData, WORD wDataSize)
{
  return true;
}

//场景消息
bool CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, void * pData, WORD wDataSize)
{
  switch(cbGameStatus)
  {
    case GAME_STATUS_FREE:    //空闲状态
    {
      //效验数据
      if(wDataSize!=sizeof(CMD_S_StatusFree))
      {
        return false;
      }

      //消息处理
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

      memcpy(m_szRoomName, pStatusFree->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation();

      BankOperate();

      //开始时间
      UINT nElapse=rand()%(5)+1;
      m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

      return true;
    }
    case GAME_STATUS_PLAY:  //游戏状态
    {
      //效验数据
      if(wDataSize!=sizeof(CMD_S_StatusPlay))
      {
        return false;
      }
      CMD_S_StatusPlay * pStatusPlay=(CMD_S_StatusPlay *)pData;

      memcpy(m_szRoomName, pStatusPlay->szGameRoomName, sizeof(m_szRoomName));

      ReadConfigInformation();

      //BankOperate();

      //设置变量
      m_lTurnMaxScore=pStatusPlay->lTurnMaxGold;
      //WORD wMeChiarID=m_pIAndroidUserItem->GetChairID();

      for(BYTE i=0; i<GAME_PLAYER; i++)
      {
        CopyMemory(m_HandCardData[i],pStatusPlay->bTableCardArray[i],MAX_COUNT);
        m_HandCardCount[i] = pStatusPlay->bTableCardCount[i];
      }


      return true;
    }
  }

  ASSERT(FALSE);

  return false;
}

//用户进入
void CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户离开
void CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户积分
void CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户状态
void CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户段位
void CAndroidUserItemSink::OnEventUserSegment(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_GameStart))
  {
    return false;
  }
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

  //设置变量
  m_lTurnMaxScore=pGameStart->lTurnMaxGold;
  m_lBasicGold=pGameStart->lBasicGold;

  //清除数据
  ZeroMemory(m_HandCardCount,sizeof(m_HandCardCount));
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));

  WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
  for(BYTE i=0; i<GAME_PLAYER; i++)
  {
    m_HandCardCount[i] = 2;

    if(wMeChairID == i)
    {
      m_HandCardData[i][0]=pGameStart->bFundusCard;
    }

    m_HandCardData[i][1]=pGameStart->bCardData[i];
  }

  //设置筹码
  if(pGameStart->wCurrentUser==m_pIAndroidUserItem->GetChairID())
  {
    //下注时间
    UINT nElapse=rand()%(4)+TIME_LESS;
    m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
    return true;
  }

  return true;
}

//用户下注
bool CAndroidUserItemSink::OnSubAddGold(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_AddGold))
  {
    return false;
  }
  CMD_S_AddGold * pAddScore=(CMD_S_AddGold *)pBuffer;
  m_lShowHandGold=pAddScore->lShowHandGold;
  m_lCurrentLessGold=pAddScore->lCurrentLessGold;


  WORD wMeChairID = m_pIAndroidUserItem->GetChairID();
  if(pAddScore->wCurrentUser==m_pIAndroidUserItem->GetChairID()/*&&!pAddScore->bShowHand*//*&&m_lCurrentLessGold!=0*/)
  {
    //梭哈概率
    bool bShowHand = false;
    if(m_GameLogic.GetCardGenre(m_HandCardData[wMeChairID],m_HandCardCount[wMeChairID])>=CT_TWO_DOUBLE)
    {
      bShowHand = true;
    }

    if(m_GameLogic.GetCardGenre(m_HandCardData[wMeChairID],m_HandCardCount[wMeChairID])==CT_ONE_DOUBLE &&
       m_HandCardCount[wMeChairID] == 3)
    {
      BYTE bCardCount[15]= {0};
      for(BYTE i=0; i<3; i++)
      {
        BYTE bCardValue = m_GameLogic.GetCardLogicValue(m_HandCardData[wMeChairID][i]);
        bCardCount[bCardValue] ++;
      }

      BYTE bDoubleCardValue = 0;
      for(BYTE i=0; i<15; i++)
      {
        if(bCardCount[i] == 2)
        {
          bDoubleCardValue = i;
        }
      }

      if(bDoubleCardValue>=12)
      {
        for(BYTE i=0; i<GAME_PLAYER; i++)
        {
          if(wMeChairID == i)
          {
            continue;
          }

          if(m_GameLogic.GetCardLogicValue(m_HandCardData[i][1]) == m_GameLogic.GetCardLogicValue(m_HandCardData[i][2]) &&
             m_GameLogic.GetCardLogicValue(m_HandCardData[i][1]) > 12)
          {
            bShowHand = false;
          }
          else
          {
            bShowHand = true;
          }
        }
      }
      else
      {
        bShowHand = false;
      }
    }

    if(bShowHand&&pAddScore->bCanShowHand/*&&(pAddScore->bAndroidLose == false)*/)
    {
      //下注时间
      UINT nElapse=rand()%5+TIME_LESS;
      m_pIAndroidUserItem->SetGameTimer(IDI_SHOW_HAND,nElapse);

      return true;
    }

    if(pAddScore->bShowHand && pAddScore->bNoGiveUp)
    {
      if(m_HandCardCount[wMeChairID] > 3)
      {
        //下注时间
        UINT nElapse=rand()%(5)+TIME_LESS;
        m_pIAndroidUserItem->SetGameTimer(IDI_SHOW_HAND,nElapse);

        return true;
      }
      else
      {
        for(BYTE i=0; i<GAME_PLAYER; i++)
        {
          if(wMeChairID == i)
          {
            continue;
          }

          if(!(m_GameLogic.GetCardLogicValue(m_HandCardData[i][1]) == m_GameLogic.GetCardLogicValue(m_HandCardData[i][2]) &&
               m_GameLogic.GetCardLogicValue(m_HandCardData[i][1]) > 12))
          {
            //下注时间
            UINT nElapse=rand()%(5)+TIME_LESS;
            m_pIAndroidUserItem->SetGameTimer(IDI_SHOW_HAND,nElapse);

            return true;
          }
        }
      }
      return true;
    }

    if(m_lCurrentLessGold==0)
    {
      //下注时间
      UINT nElapse=rand()%(4)+TIME_LESS;
      m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);
      return true;
    }
    else
    {
      if(pAddScore->bNoGiveUp && !pAddScore->bShowHand)
      {
        //下注时间
        UINT nElapse=rand()%(4)+TIME_LESS;
        m_pIAndroidUserItem->SetGameTimer(IDI_FOLLOW,nElapse);

      }
      else
      {
        //下注时间
        UINT nElapse=rand()%5+TIME_LESS;
        m_pIAndroidUserItem->SetGameTimer(IDI_GIVE_UP,nElapse);
      }
    }

    return true;
  }



  if(/*pAddScore->bNoGiveUp==false&&*/pAddScore->wCurrentUser==m_pIAndroidUserItem->GetChairID())
  {
    //下注时间
    UINT nElapse=rand()%5+TIME_LESS;
    m_pIAndroidUserItem->SetGameTimer(IDI_GIVE_UP,nElapse);
    return true;
  }
  //if (pAddScore->bShowHand&&pAddScore->wCurrentUser==m_pIAndroidUserItem->GetChairID()&&pAddScore->bNoGiveUp)
  //{
  //  //下注时间
  //  UINT nElapse=rand()%(5)+TIME_LESS;
  //  m_pIAndroidUserItem->SetGameTimer(IDI_SHOW_HAND,nElapse);
  //  return true;
  //}

  return true;
}

//发牌消息
bool CAndroidUserItemSink::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{

  //效验数据
  if(wDataSize!=sizeof(CMD_R_SendCard))
  {
    return false;
  }
  CMD_R_SendCard * pSendCard=(CMD_R_SendCard *)pBuffer;

  //设置数据
  WORD wMeChiarID=m_pIAndroidUserItem->GetChairID();

  //拷贝扑克
  for(WORD i=0; i<pSendCard->cbSendCardCount; i++)
  {
    if(m_HandCardCount[wMeChiarID]>MAX_COUNT)
    {
      break;
    }

    m_HandCardData[wMeChiarID][m_HandCardCount[wMeChiarID]++] =pSendCard->bUserCard[wMeChiarID][i];
  }

  if(pSendCard->wCurrentUser==wMeChiarID)
  {
    //梭哈概率
    bool bShowHand = false;
    if(m_GameLogic.GetCardGenre(m_HandCardData[wMeChiarID],m_HandCardCount[wMeChiarID])>=CT_TWO_DOUBLE)
    {
      bShowHand = true;
    }

    if(m_GameLogic.GetCardGenre(m_HandCardData[wMeChiarID],m_HandCardCount[wMeChiarID])==CT_ONE_DOUBLE &&
       m_HandCardCount[wMeChiarID] == 3)
    {
      BYTE bCardCount[15]= {0};
      for(BYTE i=0; i<3; i++)
      {
        BYTE bCardValue = m_GameLogic.GetCardLogicValue(m_HandCardData[wMeChiarID][i]);
        bCardCount[bCardValue] ++;
      }

      BYTE bDoubleCardValue = 0;
      for(BYTE i=0; i<15; i++)
      {
        if(bCardCount[i] == 2)
        {
          bDoubleCardValue = i;
        }
      }

      if(bDoubleCardValue>=12)
      {
        for(BYTE i=0; i<GAME_PLAYER; i++)
        {
          if(wMeChiarID == i)
          {
            continue;
          }

          if(m_GameLogic.GetCardLogicValue(m_HandCardData[i][1]) == m_GameLogic.GetCardLogicValue(m_HandCardData[i][2]) &&
             m_GameLogic.GetCardLogicValue(m_HandCardData[i][1]) > 12)
          {
            bShowHand = false;
          }
          else
          {
            bShowHand = true;
          }
        }
      }
      else
      {
        bShowHand = false;
      }
    }


    if(bShowHand && pSendCard->bCanShowHand/* &&(pSendCard->bAndroidLose == false)*/)
    {
      m_lShowHandGold=pSendCard->lShowHandGold;
      //下注时间
      UINT nElapse=rand()%(4)+TIME_LESS;
      m_pIAndroidUserItem->SetGameTimer(IDI_SHOW_HAND,nElapse);

      return true;
    }

    if(pSendCard->bNoGiveUp)
    {
      //下注时间
      UINT nElapse=rand()%(4)+TIME_LESS;
      m_pIAndroidUserItem->SetGameTimer(IDI_USER_ADD_SCORE,nElapse);

    }
    else
    {
      //下注时间
      UINT nElapse=rand()%(4)+TIME_LESS;
      m_pIAndroidUserItem->SetGameTimer(IDI_GIVE_UP,nElapse);
    }

    return true;
  }

  return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
  //效验参数
  if(wDataSize!=sizeof(CMD_S_GameEnd))
  {
    return false;
  }
  CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

  //删除定时器
  m_pIAndroidUserItem->KillGameTimer(IDI_USER_ADD_SCORE);
  m_pIAndroidUserItem->KillGameTimer(IDI_GIVE_UP);
  m_pIAndroidUserItem->KillGameTimer(IDI_SHOW_HAND);
  m_pIAndroidUserItem->KillGameTimer(IDI_OPEN_CARD);
  m_pIAndroidUserItem->KillGameTimer(IDI_FOLLOW);

  m_lTurnMaxScore = 0;
  m_lBasicGold=0;
  m_lShowHandGold=0;
  m_lCurrentLessGold=0;

  //开始时间
  UINT nElapse=rand()%(5)+TIME_LESS;
  m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

  //清理变量
  m_lTurnMaxScore = 0;
  ZeroMemory(m_HandCardData,sizeof(m_HandCardData));


  return true;
}

//放弃
bool CAndroidUserItemSink::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_GiveUp))
  {
    return false;
  }
  CMD_S_GiveUp * pGameStart=(CMD_S_GiveUp *)pBuffer;

  return true;
}

//银行操作
void CAndroidUserItemSink::BankOperate()
{
  //变量定义
  IServerUserItem *pUserItem = m_pIAndroidUserItem->GetMeUserItem();
  LONGLONG lRobotScore = pUserItem->GetUserScore();
  {
    //判断存取
    if(lRobotScore > m_lRobotScoreRange[1])
    {


      LONGLONG lSaveScore=0L;

      lSaveScore = LONGLONG(lRobotScore*m_nRobotBankStorageMul/100);
      if(lSaveScore > lRobotScore)
      {
        lSaveScore = lRobotScore;
      }

      if(lSaveScore > 0)
      {
        m_pIAndroidUserItem->PerformSaveScore(lSaveScore);
      }

      LONGLONG lRobotNewScore = pUserItem->GetUserScore();

    }
    else if(lRobotScore < m_lRobotScoreRange[0])
    {
      BYTE RandCount[10]= {10,20,30,40,50,60,70,80,90,100};
      int Index=rand()%10;
      SCORE lScore=(m_lRobotBankGetScoreBanker*RandCount[Index]/100)+m_lRobotBankGetScore;
      if(lScore > 0)
      {
        m_pIAndroidUserItem->PerformTakeScore(lScore);
      }

      LONGLONG lRobotNewScore = pUserItem->GetUserScore();

    }
  }
}

//读取配置
void CAndroidUserItemSink::ReadConfigInformation()
{
  //设置文件名
  TCHAR szPath[MAX_PATH] = TEXT("");
  TCHAR szConfigFileName[MAX_PATH] = TEXT("");
  TCHAR OutBuf[255] = TEXT("");
  GetCurrentDirectory(sizeof(szPath), szPath);
  myprintf(szConfigFileName, sizeof(szConfigFileName), _T("%s\\ShowHandExServer.ini"), szPath);

  //分数限制
  ZeroMemory(OutBuf, sizeof(OutBuf));
  GetPrivateProfileString(m_szRoomName, TEXT("RobotScoreMin"), _T("10000"), OutBuf, 255, szConfigFileName);
  myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotScoreRange[0]);

  ZeroMemory(OutBuf, sizeof(OutBuf));
  GetPrivateProfileString(m_szRoomName, TEXT("RobotScoreMax"), _T("1000000"), OutBuf, 255, szConfigFileName);
  myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotScoreRange[1]);

  if(m_lRobotScoreRange[1] < m_lRobotScoreRange[0])
  {
    m_lRobotScoreRange[1] = m_lRobotScoreRange[0];
  }

  //提款数额

  ZeroMemory(OutBuf, sizeof(OutBuf));
  GetPrivateProfileString(m_szRoomName, TEXT("RobotBankGet"), _T("20000000"), OutBuf, 255, szConfigFileName);
  myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotBankGetScore);

  //提款数额 (庄家)
  ZeroMemory(OutBuf, sizeof(OutBuf));
  GetPrivateProfileString(m_szRoomName, TEXT("RobotBankGetBanker"), _T("30000000"), OutBuf, 255, szConfigFileName);
  myscanf(OutBuf, mystrlen(OutBuf), _T("%I64d"), &m_lRobotBankGetScoreBanker);

  //存款倍数
  m_nRobotBankStorageMul = GetPrivateProfileInt(m_szRoomName, _T("RobotBankStoMul"), 20, szConfigFileName);

  if(m_nRobotBankStorageMul<0||m_nRobotBankStorageMul>100)
  {
    m_nRobotBankStorageMul =20;
  }
}


//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
