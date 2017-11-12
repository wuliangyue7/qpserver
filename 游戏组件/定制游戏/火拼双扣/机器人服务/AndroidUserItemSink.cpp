#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_LESS         4                 //最少时间
#define TIME_DISPATCH       5                 //发牌时间

#ifdef _DEBUG

//游戏时间
#define TIME_OUT_CARD       1                 //出牌时间
#define TIME_START_GAME       2                 //开始时间
#define TIME_THROW_CARD       5                 //进贡时间
#define TIME_INCEP_CARD       5                 //回贡定时器

#else
//游戏时间
#define TIME_OUT_CARD       4                 //出牌时间
#define TIME_START_GAME       6                 //开始时间
#define TIME_THROW_CARD       5                 //进贡时间
#define TIME_INCEP_CARD       5                 //回贡定时器

#endif

//游戏时间
#define IDI_START_GAME        100                 //开始时间
#define IDI_OUT_CARD        101                 //出牌时间
#define IDI_PASS_CARD       102                 //pass时间



//////////////////////////////////////////////////////////////////////////

//构造函数
CAndroidUserItemSink::CAndroidUserItemSink()
{
  //游戏变量
  m_wBankerUser=INVALID_CHAIR;

  m_wOutCardUser = INVALID_CHAIR ;
  m_wCurrentUser=INVALID_CHAIR;


  //扑克变量
  m_cbTurnCardCount=0;

  ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

  //手上扑克
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
  ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));


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
VOID *  CAndroidUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(IAndroidUserItemSink,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(IAndroidUserItemSink,Guid,dwQueryVer);
  return NULL;
}

//初始接口
bool  CAndroidUserItemSink::Initialization(IUnknownEx * pIUnknownEx)
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
bool  CAndroidUserItemSink::RepositionSink()
{
  //游戏变量
  m_wBankerUser=INVALID_CHAIR;


  //扑克变量
  m_cbTurnCardCount=0;
  ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

  //手上扑克
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
  ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

  return true;
}

//时间消息
bool  CAndroidUserItemSink::OnEventTimer(UINT nTimerID)
{
  switch(nTimerID)
  {
    case IDI_START_GAME:  //开始游戏
    {
      m_pIAndroidUserItem->SendUserReady(NULL,0);
      return true;
    }
    case IDI_PASS_CARD:   //pass
    {
      m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
      return true;
    }
    case IDI_OUT_CARD:    //用户出牌
    {
      //出牌处理
      if(m_wCurrentUser == m_pIAndroidUserItem->GetChairID())
      {
        //删除时间
        m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);

        //分析结果
        tagOutCardResult OutCardResult;
        ZeroMemory(&OutCardResult,sizeof(OutCardResult));

        ////出牌处理
        //if (m_cbTurnCardCount==0)
        //{
        //  //分析结果
        //  tagOutCardResult OutCardResult;
        //  m_GameLogic.SearchOutCard(m_cbHandCardData[m_pIAndroidUserItem->GetChairID()],m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()],m_cbTurnCardData,m_cbTurnCardCount,OutCardResult);

        //  //设置界面
        //  if (OutCardResult.cbCardCount>0)
        //  {
        //
        //  }
        //  else
        //  {
        //    m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
        //    return true;
        //  }
        //}
        //else if (m_cbTurnCardCount>0)
        //{
        //  //if (rand()%2==0)
        //  //{
        //  //  m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
        //  //  return true;
        //  //}
        //
        //}

        //分析结果
        bool bValid=false;
        WORD wPlayerID = (m_pIAndroidUserItem->GetChairID()+1)%GAME_PLAYER;
        m_GameLogic.SortCardList(m_cbHandCardData[m_pIAndroidUserItem->GetChairID()],m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()],ST_ORDER);


        bValid = m_GameLogic.AiSearchOutCard(m_cbHandCardData[m_pIAndroidUserItem->GetChairID()],m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()],
                                             m_cbTurnCardData,m_cbTurnCardCount,m_cbHandCardData[wPlayerID],m_cbHandCardCount[wPlayerID],OutCardResult);

        if(!bValid)
        {
          m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
          return true;
        }
        else
        {
          if(VerdictOutCard(OutCardResult.cbResultCard,OutCardResult.cbCardCount))
          {
            if(OutCardResult.cbCardCount>0)
            {
              //获取扑克
              BYTE cbCardData[MAX_COUNT];
              BYTE cbCardCount=OutCardResult.cbCardCount;
              CopyMemory(cbCardData,OutCardResult.cbResultCard,sizeof(cbCardData));

              //排列扑克
              m_GameLogic.SortCardList(cbCardData,cbCardCount,ST_ORDER);

              if(m_GameLogic.RemoveCard(cbCardData,cbCardCount,m_cbHandCardData[m_pIAndroidUserItem->GetChairID()],m_cbHandCardCount[m_pIAndroidUserItem->GetChairID()])==false)
              {
                ASSERT(false);
                m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
                return true;
              }
              //发送数据
              CMD_C_OutCard OutCard;
              OutCard.cbCardCount=cbCardCount;
              CopyMemory(OutCard.cbCardData,cbCardData,cbCardCount);
              m_pIAndroidUserItem->SendSocketData(SUB_C_OUT_CARD,&OutCard,sizeof(OutCard)*sizeof(BYTE));
              return true;
            }

          }
          else
          {
            m_pIAndroidUserItem->SendSocketData(SUB_C_PASS_CARD);
            return true;
          }
        }

      }

      return true;
    }
  }

  return false;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_START:    //游戏开始
    {
      return OnSubGameStart(pData,wDataSize);
    }
    case SUB_S_OUT_CARD:    //用户出牌
    {
      return OnSubOutCard(pData,wDataSize);
    }
    case SUB_S_PASS_CARD:   //放弃出牌
    {
      return OnSubPassCard(pData,wDataSize);
    }
    case SUB_S_GAME_END:    //游戏结束
    {
      return OnSubGameEnd(pData,wDataSize);
    }
    case SUB_S_TRUSTEE:     //用户托管
    {
      return true;
    }
    case SUB_S_ON_READY:    //用户准备
    {
      return true;
    }
    case SUB_S_CHEAT_CARD:    //看牌指令
    {
      return true;
    }
  }

  //错误断言
  ASSERT(FALSE);

  return true;
}

//游戏消息
bool  CAndroidUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  return true;
}

//场景消息
bool  CAndroidUserItemSink::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
  switch(cbGameStatus)
  {
    case GAME_STATUS_FREE:  //空闲状态
    {
      //效验数据
      ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
      if(wDataSize!=sizeof(CMD_S_StatusFree))
      {
        return false;
      }

      //变量定义
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
      IServerUserItem * pIServerUserItem=m_pIAndroidUserItem->GetMeUserItem();

      //玩家设置
      UINT nElapse=rand()%TIME_START_GAME+TIME_LESS;
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

      //出牌变量
      m_cbTurnCardCount=pStatusPlay->cbTurnCardCount;
      CopyMemory(m_cbTurnCardData,pStatusPlay->cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

      //扑克数据
      WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
      CopyMemory(m_cbHandCardCount,pStatusPlay->cbHandCardCount,sizeof(m_cbHandCardCount));
      CopyMemory(m_cbHandCardData,pStatusPlay->cbHandCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);

      //玩家设置
      if(pStatusPlay->wCurrentUser==m_pIAndroidUserItem->GetChairID())
      {
        UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
        m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
      }

      return true;
    }
  }

  //错误断言
  ASSERT(FALSE);

  return false;
}

//用户进入
VOID  CAndroidUserItemSink::OnEventUserEnter(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户离开
VOID  CAndroidUserItemSink::OnEventUserLeave(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户积分
VOID  CAndroidUserItemSink::OnEventUserScore(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//用户状态
VOID  CAndroidUserItemSink::OnEventUserStatus(IAndroidUserItem * pIAndroidUserItem, bool bLookonUser)
{
  return;
}

//游戏开始
bool CAndroidUserItemSink::OnSubGameStart(const void * pData, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_GameStart));
  if(wDataSize!=sizeof(CMD_S_GameStart))
  {
    return false;
  }

  //消息处理
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

  m_wBankerUser=pGameStart->wCurrentUser;
  //游戏变量
  m_wCurrentUser=pGameStart->wCurrentUser;

  //出牌变量
  m_cbTurnCardCount=0;
  ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
  ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));

  //设置扑克
  WORD wMeChairID=m_pIAndroidUserItem->GetChairID();
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_cbHandCardCount[i]=CountArray(pGameStart->cbCardData);
  }
  CopyMemory(m_cbHandCardData[wMeChairID],pGameStart->cbCardData,sizeof(BYTE)*m_cbHandCardCount[wMeChairID]);
  CopyMemory(m_cbHandCardData[(wMeChairID+1)%GAME_PLAYER],pGameStart->cbPlayerCardData,sizeof(BYTE)*m_cbHandCardCount[(wMeChairID+1)%GAME_PLAYER]);

  if(m_wCurrentUser==wMeChairID)
  {
    //下注时间
    UINT nElapse=rand()%(4)+TIME_DISPATCH;
    m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
  }

  return true;
}

//用户出牌
bool CAndroidUserItemSink::OnSubOutCard(const void * pData, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_OutCard));
  if(wDataSize!=sizeof(CMD_S_OutCard))
  {
    return false;
  }
  //变量定义
  CMD_S_OutCard * pOutCard=(CMD_S_OutCard *)pData;

  //变量定义
  WORD wMeChairID=m_pIAndroidUserItem->GetChairID();

  //出牌变量
  m_wCurrentUser=pOutCard->wCurrentUser;
  m_cbTurnCardCount=pOutCard->cbCardCount;
  CopyMemory(m_cbTurnCardData,pOutCard->cbCardData,sizeof(BYTE)*pOutCard->cbCardCount);

  //获取牌型
  tagAnalyseResult analyseResult;
  memset(&analyseResult,0,sizeof(analyseResult));
  m_GameLogic.AnalysebCardData(pOutCard->cbCardData,pOutCard->cbCardCount,analyseResult);
  m_GameLogic.FindValidCard(analyseResult,m_LastOutCard);
  memcpy(&m_LastOutCard,&analyseResult,sizeof(tagAnalyseResult));



  //出牌设置
  if(wMeChairID!=pOutCard->wOutCardUser)
  {
    m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
  }
  else
  {
    //删除扑克
    BYTE cbSourceCount=m_cbHandCardCount[wMeChairID];
    m_cbHandCardCount[pOutCard->wOutCardUser]-=pOutCard->cbCardCount;
    m_GameLogic.RemoveCardEx(pOutCard->cbCardData,pOutCard->cbCardCount,m_cbHandCardData[m_pIAndroidUserItem->GetChairID()],cbSourceCount);
  }

  //玩家设置
  if(m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
  {
    //激活框架
    BYTE cbCardCount=m_cbHandCardCount[pOutCard->wCurrentUser];
    if(cbCardCount==1&&pOutCard->cbCardCount>=2)
    {
      //pass时间
      UINT nElapse=rand()%(2)+2;
      m_pIAndroidUserItem->SetGameTimer(IDI_PASS_CARD,nElapse);
    }
    else
    {
      //出牌时间
      UINT nElapse=rand()%(TIME_OUT_CARD)+TIME_DISPATCH;
      m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
    }

  }
  return true;
}

//用户放弃
bool CAndroidUserItemSink::OnSubPassCard(const void * pData, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_PassCard));
  if(wDataSize!=sizeof(CMD_S_PassCard))
  {
    return false;
  }
  //变量定义
  CMD_S_PassCard * pPassCard=(CMD_S_PassCard *)pData;

  //设置变量
  m_wCurrentUser=pPassCard->wCurrentUser;

  //一轮判断
  if(pPassCard->cbTurnOver==TRUE)
  {
    //出牌变量
    m_cbTurnCardCount=0;
    ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
    ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
  }

  //玩家设置
  if(m_wCurrentUser==m_pIAndroidUserItem->GetChairID())
  {
    //出牌时间
    UINT nElapse=rand()%(4)+1;
    m_pIAndroidUserItem->SetGameTimer(IDI_OUT_CARD,nElapse);
  }

  return true;
}

//游戏结束
bool CAndroidUserItemSink::OnSubGameEnd(const void * pData, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_GameEnd));
  if(wDataSize!=sizeof(CMD_S_GameEnd))
  {
    return false;
  }

  //变量定义
  CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pData;

  //设置变量
  m_cbTurnCardCount=0;
  ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
  ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

  //删除时间
  m_pIAndroidUserItem->KillGameTimer(IDI_OUT_CARD);

  //开始设置
  UINT nElapse=rand()%TIME_START_GAME+2;
  m_pIAndroidUserItem->SetGameTimer(IDI_START_GAME,nElapse);

  return true;
}
//出牌判断
bool CAndroidUserItemSink::VerdictOutCard(BYTE cbOutCardData[], BYTE cbOutCardCount)
{

  //状态判断
  if(m_wCurrentUser!=m_pIAndroidUserItem->GetChairID())
  {
    return false;
  }

  //出牌判断
  if(cbOutCardCount>0L)
  {
    memset(&m_OutCardInfo,0,sizeof(m_OutCardInfo));
    m_GameLogic.AnalysebCardData(cbOutCardData,cbOutCardCount,m_OutCardInfo);

    return m_GameLogic.CompareCard(m_OutCardInfo,m_LastOutCard);
  }
  return false;

}
//组件创建函数
DECLARE_CREATE_MODULE(AndroidUserItemSink);

//////////////////////////////////////////////////////////////////////////
