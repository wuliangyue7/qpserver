#include "StdAfx.h"
#include "TableFrameSink.h"
#include<cmath>

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD      CTableFrameSink::m_wPlayerCount=GAME_PLAYER;      //游戏人数
const BYTE      CTableFrameSink::m_GameStartMode=START_MODE_ALL_READY;  //开始模式

char *g_strCardColor[]= {"方片","梅花","红桃","黑桃",""};
char *g_strCardValue[]= {"A","2","3","4","5","6","7","8","9","10","J","Q","K","小王","大王"};
//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{

  //游戏变量
  m_wBankerUser=INVALID_CHAIR;
  m_wCurrentUser=INVALID_CHAIR;
  ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

  //历史积分
  memset(m_lAllTurnScore,0,sizeof(m_lAllTurnScore));
  memset(m_lLastTurnScore,0,sizeof(m_lLastTurnScore));
  memcpy(m_cbRandCard,CGameLogic::m_cbCardData,sizeof(m_cbRandCard));
  //开始信息
  m_nScoreGrade=0;
  m_cbCurGrade=0;

  //胜利信息
  m_wGameWiner=0xFFFF;
  //出牌信息
  m_cbTurnCardCount=0;
  m_wTurnWiner=INVALID_CHAIR;
  ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
  ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
  ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
  ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

  //炸弹记录
  ZeroMemory(m_cbBombList,sizeof(m_cbBombList));

  //扑克信息
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
  ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

  //组件变量
  m_pITableFrame=NULL;
  m_pGameServiceOption=NULL;
  m_bHasReqHunter=false;

  srand((unsigned)time(NULL));

  ZeroMemory(m_lBombScore,sizeof(m_lBombScore));

  m_wAdminChairID=INVALID_CHAIR;

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

  if(m_pCheatServer)
  {
    delete m_pCheatServer;
    m_pCheatServer = NULL;
  }

  if(m_hInst)
  {
    FreeLibrary(m_hInst);
    m_hInst = NULL;
  }
}

//接口查询
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
  QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
  QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
  QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
  return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
  //查询接口
  ASSERT(pIUnknownEx!=NULL);
  m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
  if(m_pITableFrame==NULL)
  {
    return false;
  }

  //获取参数
  m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
  ASSERT(m_pGameServiceOption!=NULL);

  //开始模式
  m_pITableFrame->SetStartMode(START_MODE_FULL_READY);

  //读取配置
  memcpy(m_szGameRoomName, m_pGameServiceOption->szServerName, sizeof(m_szGameRoomName));

  //设置文件名
  TCHAR szPath[MAX_PATH]=TEXT("");
  GetCurrentDirectory(sizeof(szPath),szPath);
  _sntprintf(m_szConfigFileName,sizeof(m_szConfigFileName),TEXT("%s\\ShuangKouHPConfig.ini"),szPath);

  ZeroMemory(m_lScoreList,sizeof(m_lScoreList));

  if(ReadConfigInformation(true)==false)
  {
    //AfxMessageBox(TEXT("无法加载配置文件，请检查游戏目录！"));
    //return false;
  }

  //服务控制
  m_hInst = NULL;
  m_pServerControl = NULL;
  m_hInst = LoadLibrary(TEXT("ShuangKouHPServerControl.dll"));
  if(m_hInst)
  {
    typedef void * (*CREATE)();
    CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl");
    if(ServerControl)
    {
      m_pServerControl = static_cast<IServerControl*>(ServerControl());
    }
  }

  //做弊服务
  m_hInst = NULL;
  m_pCheatServer = NULL;
  m_hInst = LoadLibrary(TEXT("CheatServerControl.dll"));
  if(m_hInst)
  {
    typedef void * (*CREATE)();
    CREATE CheatServer = (CREATE)GetProcAddress(m_hInst,"CreateCheatServerControl");
    if(CheatServer)
    {
      m_pCheatServer = static_cast<ICheatServer*>(CheatServer());
    }
  }




//#ifdef _DEBUG
//
//  BYTE cbCard1[MAX_COUNT]={0x01,0x11,0x21,0x31,0x02,0x12,0x22,0x32,0x03,0x13,
//    0x23,0x33,0x04,0x14,0x24,0x34,0x05,0x15,0x25,0x35,
//    0x01,0x12,0x22,0x33,0x03,0x13,0x4E};
//  BYTE cbCard2[MAX_COUNT]={0x06,0x16,0x26,0x36,0x07,0x17,0x27,0x37,0x08,0x18,
//    0x28,0x38,0x09,0x19,0x29,0x39,0x0A,0x1A,0x2A,0x3A,
//    0x06,0x17,0x27,0x38,0x08,0x18,0x4F};
//
//  m_cbHandCardCount[0]=MAX_COUNT;
//  m_cbHandCardCount[1]=MAX_COUNT;
//  CopyMemory(&m_cbHandCardData[0],cbCard1,sizeof(BYTE)*MAX_COUNT);
//  CopyMemory(&m_cbHandCardData[1],cbCard2,sizeof(BYTE)*MAX_COUNT);
//
//  //排列扑克
//  for (WORD i=0;i<GAME_PLAYER;i++)
//  {
//    m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i]);
//  }
//
//  BYTE cbBombList[GAME_PLAYER][BOMB_TYPE_COUNT];
//  ZeroMemory(cbBombList,sizeof(m_cbBombList));
//  //LONGLONG lBombScore[GAME_PLAYER];
//  //ZeroMemory(lBombScore,sizeof(lBombScore));
//
//  GetBombListByHandCard(cbBombList);
//
//
//#endif

  return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{

  //游戏变量
  m_wCurrentUser=INVALID_CHAIR;
  ZeroMemory(m_bTrustee,sizeof(m_bTrustee));

  ZeroMemory(m_lBombScore,sizeof(m_lBombScore));

  //胜利信息
  m_wGameWiner=0xFFFF;
  //出牌信息
  m_cbTurnCardCount=0;
  m_wTurnWiner=INVALID_CHAIR;
  ZeroMemory(m_cbTurnCardData,sizeof(m_cbTurnCardData));
  ZeroMemory(m_cbOutCardData,sizeof(m_cbOutCardData));
  ZeroMemory(m_cbOutCardCount,sizeof(m_cbOutCardCount));

  //扑克信息
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
  ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
  m_bHasReqHunter=false;

  return;
}

//开始模式
BYTE CTableFrameSink::GetGameStartMode()
{
  return m_GameStartMode;
}

//游戏状态
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
  return true;
}


//得到王数
BYTE CTableFrameSink::GetRandomKingCount()
{
  int n1=m_nKingPercent[0] ;
  int n2=m_nKingPercent[1] ;
  int n3=m_nKingPercent[2] ;
  int n4=m_nKingPercent[3] ;
  int n5=m_nKingPercent[4] ;

  if(n1+n2+n3+n4+n5==0)
  {
    return 0;
  }

  INT cbControlArea[5] = { 1, 2,  3,  4,   5};
  INT cbnChance[5]    = {n1,n2,n3,n4,n5};
  m_GameLogic.RandArray(cbControlArea, CountArray(cbControlArea), cbnChance, CountArray(cbnChance));
  //随机倍数
  DWORD wTick = GetTickCount();
  //几率和值
  INT nChanceAndValue = 0;
  for(int n = 0; n < CountArray(cbnChance); ++n)
  {
    nChanceAndValue += cbnChance[n];
  }

  INT nMuIndex = 0;
  int nRandNum = 0;         //随机辅助
  static int nStFluc = 1;
  nRandNum = (rand() + wTick + nStFluc*3) % nChanceAndValue;
  for(int j = 0; j < CountArray(cbnChance); j++)
  {
    nRandNum -= cbnChance[j];
    if(nRandNum < 0)
    {
      nMuIndex = j;
      break;
    }
  }
  nStFluc = nStFluc%3 + 1;

  return cbControlArea[nMuIndex]-1;

}
//发牌控制
void CTableFrameSink::DispatchTableCard()
{

  //混乱扑克
  ZeroMemory(m_cbRandCard,sizeof(m_cbRandCard));
  m_GameLogic.RandCardList(m_cbRandCard,FULL_COUNT);

  //用户扑克
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_cbHandCardCount[i]=MAX_COUNT;
    CopyMemory(&m_cbHandCardData[i],&m_cbRandCard[i*m_cbHandCardCount[i]],sizeof(BYTE)*m_cbHandCardCount[i]);
  }

#ifdef _DEBUG

  BYTE cbCard1[MAX_COUNT]= {0x01,0x11,0x21,0x31,0x02,0x12,0x22,0x32,0x03,0x13,
                            0x23,0x33,0x04,0x14,0x24,0x34,0x05,0x15,0x25,0x35,
                            0x0B,0x1B,0x2B,0x3B,0x0C,0x4F,0x4E
                           };
  BYTE cbCard2[MAX_COUNT]= {0x06,0x16,0x26,0x36,0x07,0x17,0x27,0x37,0x08,0x18,
                            0x28,0x38,0x09,0x19,0x29,0x39,0x0A,0x1A,0x2A,0x3A,
                            0x07,0x17,0x27,0x38,0x08,0x18,0x1C
                           };

  CopyMemory(&m_cbHandCardData[0],cbCard1,sizeof(BYTE)*MAX_COUNT);
  CopyMemory(&m_cbHandCardData[1],cbCard2,sizeof(BYTE)*MAX_COUNT);


#endif


  if(m_pServerControl != NULL && m_pServerControl->NeedControl())
  {

    int ControlType;
    m_pServerControl->ReturnControlType(ControlType);
    if(ControlType!=0)
    {
      while(true)
      {
        //派发扑克
        ZeroMemory(m_cbRandCard,sizeof(m_cbRandCard));
        m_GameLogic.RandCardList(m_cbRandCard,FULL_COUNT);

        //用户扑克
        for(WORD i=0; i<GAME_PLAYER; i++)
        {
          m_cbHandCardCount[i]=MAX_COUNT;
          CopyMemory(&m_cbHandCardData[i],&m_cbRandCard[i*m_cbHandCardCount[i]],sizeof(BYTE)*m_cbHandCardCount[i]);
        }

        //试探性判断
        if(ProbeJudge(ControlType))
        {
          break;
        }
      }
    }


  }

}
//根据手上的牌得到炸弹列表
void CTableFrameSink::GetBombListByHandCard(BYTE cbBombList[GAME_PLAYER][BOMB_TYPE_COUNT])
{

  for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
  {
    tagAnalyseInitResult AnalyseResult;
    ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
    m_GameLogic.AnalyseInitCardData(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID],AnalyseResult);

    bool bCalMaxXiang=false;
    for(int cbIndex=7; cbIndex>=0; cbIndex--)
    {
      if(AnalyseResult.cbBlockCount[cbIndex]>=1)
      {
        if(!bCalMaxXiang)
        {
          BYTE cbMaxXiang=cbIndex+1;
          AnalyseResult.cbBlockCount[cbIndex]--;
          bCalMaxXiang=true;

          cbMaxXiang+=AnalyseResult.cbKingCount;
          cbMaxXiang+=AnalyseResult.cbSubKingCount;
          if(cbMaxXiang>=4)
          {
            cbBombList[wChairID][cbMaxXiang-4]++;
          }
        }

        if(cbIndex>=3)
        {
          cbBombList[wChairID][cbIndex-3]+=AnalyseResult.cbBlockCount[cbIndex];
        }
      }
    }

  }
}

//试探性判断
bool CTableFrameSink::ProbeJudge(int ControlType)
{

  BYTE cbBombList[GAME_PLAYER][BOMB_TYPE_COUNT];
  ZeroMemory(cbBombList,sizeof(m_cbBombList));
  //LONGLONG lBombScore[GAME_PLAYER];
  //ZeroMemory(lBombScore,sizeof(lBombScore));

  GetBombListByHandCard(cbBombList);

  //初始化变量
  for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
  {
    m_GameLogic.TransBombGrade(cbBombList[wChairID]);
  }

  WORD wAnotherChairID=(m_wAdminChairID+1)%GAME_PLAYER;
  //贡献得分
  LONGLONG lBombScore[GAME_PLAYER];
  ZeroMemory(lBombScore,sizeof(lBombScore));
  //GetBombScore(lBombScore);

  {

    for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
    {
      m_GameLogic.TransBombGrade(cbBombList[wChairID]);
      for(int nIndex=0; nIndex<BOMB_TYPE_COUNT; nIndex++)
      {
        lBombScore[wChairID]+=m_lScoreList[m_cbCurGrade][nIndex+4]*cbBombList[wChairID][nIndex];
      }
    }
  }

  LONGLONG score1=lBombScore[m_wAdminChairID],score2=lBombScore[wAnotherChairID];

  //对家输
  if(ControlType==CS_BANKER_LOSE)
  {
    if(score1>score2)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else if(ControlType==CS_BANKER_WIN)
  {
    if(score1<score2)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  return true;
}


//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
  //设置状态
  m_pITableFrame->SetGameStatus(GS_SK_PLAYING);
  ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
  //炸弹记录
  ZeroMemory(m_cbBombList,sizeof(m_cbBombList));

  ZeroMemory(m_lBombScore,sizeof(m_lBombScore));

  //确定当前计分等级
  GetUserMinGrade(m_cbCurGrade);

  DispatchTableCard();

  memcpy(m_cbRandCard,m_cbHandCardData,MAX_COUNT*2);

  //定义变量
  m_wBankerUser =INVALID_CHAIR;
  BYTE cbStartRandcard=0XFF;
  WORD wGetRandCardID=INVALID_CHAIR;
  BYTE cbGetCardPosition=0xFF;
  BYTE cbMainCard=0xFF;
  //设置用户
  if(m_wBankerUser == INVALID_CHAIR)
  {
    int randNomber=rand()%(MAX_COUNT*GAME_PLAYER);
    cbMainCard=m_cbRandCard[randNomber-1];
    cbGetCardPosition=randNomber%MAX_COUNT;
    m_wBankerUser=randNomber/MAX_COUNT;
    wGetRandCardID=m_wBankerUser;

    TCHAR szMessage[128]=TEXT("");
    //通知消息
    IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
    _sntprintf(szMessage,CountArray(szMessage),TEXT("[%s]抓到亮牌%s%s，该玩家为当前庄家"),pIServerUserItem->GetNickName(),g_strCardColor[m_GameLogic.GetCardColor(cbMainCard)>>4],g_strCardValue[m_GameLogic.GetCardValue(cbMainCard)-1]);
    SendTableMessage(szMessage,SMT_PROMPT);
  }

  //游戏变量
  m_wTurnWiner=m_wBankerUser;
  m_wCurrentUser=m_wBankerUser;

  //构造变量
  CMD_S_GameStart GameStart;
  ZeroMemory(&GameStart,sizeof(GameStart));

  //设置变量
  GameStart.wCurrentUser=m_wCurrentUser;
  GameStart.lCellScore[0]=m_lScoreList[m_cbCurGrade][1];
  GameStart.lCellScore[1]=m_lScoreList[m_cbCurGrade][2];
  GameStart.lCellScore[2]=m_lScoreList[m_cbCurGrade][3];
  CopyMemory(GameStart.bTrustee,m_bTrustee,sizeof(m_bTrustee));

  //发送数据
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //构造扑克
    ASSERT(CountArray(GameStart.cbCardData)>=m_cbHandCardCount[i]);
    CopyMemory(GameStart.cbCardData,m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
    GameStart.wGetRandCardID=wGetRandCardID;
    GameStart.cbGetCardPosition=cbGetCardPosition;
    GameStart.cbStartRandCard=cbMainCard;

    //把对方扑克发给机器人
    IServerUserItem *UserItem=m_pITableFrame->GetTableUserItem(i);
    if(UserItem->IsAndroidUser())
    {
      CopyMemory(GameStart.cbPlayerCardData,m_cbHandCardData[(i+1)%GAME_PLAYER],sizeof(BYTE)*m_cbHandCardCount[(i+1)%GAME_PLAYER]);
    }
    else
    {
      ZeroMemory(GameStart.cbPlayerCardData,MAX_COUNT);
    }


    //发送数据
    m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
    m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
    ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
  }

  //排列扑克
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_GameLogic.SortCardList(m_cbHandCardData[i],m_cbHandCardCount[i]);
  }

  //WB改
  SendCheatCard();

  return true;
}


//随机炸弹控制索引
BYTE CTableFrameSink::GetRandBombSetIndex()
{
  int n1=m_nBombPercent[0] ;
  int n2=m_nBombPercent[1] ;
  int n3=m_nBombPercent[2] ;
  int n4=m_nBombPercent[3] ;
  int n5=m_nBombPercent[4] ;
  int n6=m_nBombPercent[5] ;
  int n7=m_nBombPercent[6] ;
  int n8=m_nBombPercent[7] ;
  int n9=m_nBombPercent[8];
  int n10=m_nBombPercent[9];
  int n11=m_nBombPercent[10];

  INT cbControlArea[11] = { 1, 2,  3,  4,   5,  6,  7,  8,   9, 10, 11};
  INT cbnChance[11]   = {n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11};

  m_GameLogic.RandArray(cbControlArea, CountArray(cbControlArea), cbnChance, CountArray(cbnChance));

  //随机倍数
  DWORD wTick = GetTickCount();

  //几率和值
  INT nChanceAndValue = 0;
  for(int n = 0; n < CountArray(cbnChance); ++n)
  {
    nChanceAndValue += cbnChance[n];
  }

  INT nMuIndex = 0;
  int nRandNum = 0;         //随机辅助
  static int nStFluc = 1;
  nRandNum = (rand() + wTick + nStFluc*3) % nChanceAndValue;
  for(int j = 0; j < CountArray(cbnChance); j++)
  {
    nRandNum -= cbnChance[j];
    if(nRandNum < 0)
    {
      nMuIndex = j;
      break;
    }
  }
  nStFluc = nStFluc%3 + 1;

  return cbControlArea[nMuIndex];

}

//计算得分
bool CTableFrameSink::CalculationRates(tagScoreInfo ScoreInfo[])
{
  return true;
}

//算分等级
BYTE CTableFrameSink::GetUserMinGrade(BYTE& cbGrade)
{
  cbGrade=0;
  LONGLONG lMinScore=0x0FFFFFFFFFFFFFFF;
  IServerUserItem*pServerUserIntem=NULL;
  int nCurGrade=-1;
  //获取最小分数
  for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
  {
    if((pServerUserIntem=m_pITableFrame->GetTableUserItem(wChairID))!=NULL)
    {
      lMinScore=min(lMinScore,pServerUserIntem->GetUserScore());
    }
  }

  //获取算分等级
  for(BYTE cbIndex=0; cbIndex<m_nScoreGrade; cbIndex++)
  {
    if(lMinScore>=m_lScoreList[cbIndex][0])
    {
      nCurGrade=cbIndex;
    }
    else
    {
      break;
    }
  }

  if(nCurGrade<0)
  {
    nCurGrade=0;
  }

  cbGrade=(BYTE)(min(nCurGrade,m_nScoreGrade-1));

  return cbGrade;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
  switch(cbReason)
  {
    case GER_DISMISS:   //游戏解散
    {
      //效验参数
      ASSERT(pIServerUserItem!=NULL);
      ASSERT(wChairID<m_wPlayerCount);

      //构造数据
      CMD_S_GameEnd GameEnd;
      memset(&GameEnd,0,sizeof(GameEnd));

      //用户扑克
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
        CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
      }

      //发送信息
      m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

      //结束游戏
      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

      return true;
    }
    case GER_NORMAL:    //常规结束
    {
      //定义变量
      CMD_S_GameEnd GameEnd;
      tagScoreInfo ScoreInfo[GAME_PLAYER];
      ZeroMemory(&GameEnd,sizeof(GameEnd));
      ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

      //剩余扑克
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
        CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
      }
      int cbFinishType=2;
      if(m_cbHandCardCount[(m_wGameWiner==0)?1:0]>=12)
      {
        cbFinishType=3;
      }
      else
      {
        if(m_cbHandCardCount[(m_wGameWiner==0)?1:0]<=6)
        {
          cbFinishType=1;

        }
      }
      GameEnd.cbResultType = cbFinishType;
      //基础得分翻倍
      int cbBombGrade=m_GameLogic.GetMaxBombGrade(m_cbBombList[m_wGameWiner]);
      //5倍开始计算翻倍
      int  nMutile=(int)pow(2,cbBombGrade);
      //游戏积分
      LONGLONG  lMutileScore=m_lScoreList[m_cbCurGrade][cbFinishType]*nMutile;
      //贡献得分
      LONGLONG lBombScore[GAME_PLAYER];
      ZeroMemory(lBombScore,sizeof(lBombScore));
      GetBombScore(lBombScore);
      ScoreInfo[m_wGameWiner].lScore =lMutileScore+(lBombScore[m_wGameWiner]-lBombScore[(m_wGameWiner+1)%GAME_PLAYER]);
      LONGLONG score1=lBombScore[m_wGameWiner],score2=lBombScore[(m_wGameWiner+1)%GAME_PLAYER];

      GameEnd.nMutile=nMutile;
      GameEnd.nBaseScore=m_lScoreList[m_cbCurGrade][cbFinishType];

      if(ScoreInfo[m_wGameWiner].lScore>0)
      {
        ScoreInfo[m_wGameWiner].cbType = SCORE_TYPE_WIN;
        //不能大于对方的分数
        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
        {
          LONGLONG lScore=m_pITableFrame->GetTableUserItem((m_wGameWiner+1)%GAME_PLAYER)->GetUserScore();
          if(ScoreInfo[m_wGameWiner].lScore>lScore)
          {
            ScoreInfo[m_wGameWiner].lScore=lScore;
          }
        }
        ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].cbType = SCORE_TYPE_LOSE;
        ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;

        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
        {
          //赢家赢的分数不能大于自己现在的分数
          LONGLONG lWinnerScore=m_pITableFrame->GetTableUserItem(m_wGameWiner)->GetUserScore();
          if(lWinnerScore>0&&ScoreInfo[m_wGameWiner].lScore>lWinnerScore)
          {
            ScoreInfo[m_wGameWiner].lScore=lWinnerScore;
            ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;

          }
        }

      }
      else
      {
        ScoreInfo[m_wGameWiner].cbType = SCORE_TYPE_LOSE;
        //不能大于对方的分数
        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
        {
          LONGLONG lScore=m_pITableFrame->GetTableUserItem(m_wGameWiner)->GetUserScore();
          if(ScoreInfo[m_wGameWiner].lScore<(-lScore))
          {
            ScoreInfo[m_wGameWiner].lScore=-lScore;
          }
        }
        ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].cbType = SCORE_TYPE_WIN;
        ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;


        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
        {
          //赢家赢的分数不能大于自己现在的分数
          LONGLONG lWinnerScore=m_pITableFrame->GetTableUserItem((m_wGameWiner+1)%GAME_PLAYER)->GetUserScore();
          if(lWinnerScore>0&&ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore>lWinnerScore)
          {
            ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=lWinnerScore;
            ScoreInfo[m_wGameWiner].lScore=-ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore;
          }

        }

      }

      //计算税收 写入积分
      for(WORD wIndex=0; wIndex<GAME_PLAYER; wIndex++)
      {
        //扣税变量
        WORD wRevenue=m_pGameServiceOption->wRevenueRatio;
        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD&&ScoreInfo[wIndex].lScore>0L)
        {
          //积分调整
          ScoreInfo[wIndex].lRevenue=m_pITableFrame->CalculateRevenue(wIndex,ScoreInfo[wIndex].lScore);
          GameEnd.lGameTax=ScoreInfo[wIndex].lRevenue;
        }
        GameEnd.lGameScore[wIndex]=ScoreInfo[wIndex].lScore-ScoreInfo[wIndex].lRevenue;
        if(ScoreInfo[wIndex].lRevenue>0L)
        {
          ScoreInfo[wIndex].lScore-=ScoreInfo[wIndex].lRevenue;
        }

        m_lAllTurnScore[wIndex]+=GameEnd.lGameScore[wIndex];
        m_lLastTurnScore[wIndex]=GameEnd.lGameScore[wIndex];
      }
      //写入积分
      m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));


      for(BYTE i=0; i<GAME_PLAYER; i++)
      {
        GameEnd.lGameScore[i]=ScoreInfo[i].lScore;
      }

      memcpy(GameEnd.lLastTurnScore,m_lLastTurnScore,sizeof(m_lLastTurnScore));
      memcpy(GameEnd.lAllTurnScore,m_lAllTurnScore,sizeof(m_lAllTurnScore));

      memcpy(GameEnd.lBombScore,lBombScore,sizeof(lBombScore));
      memcpy(GameEnd.cbBombList,m_cbBombList,sizeof(m_cbBombList));

      //发送信息
      m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

      //切换庄家
      m_wBankerUser=m_wGameWiner;
      //结束游戏
      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
      return true;
    }
    case GER_HUNTER:    //玩家抄底
    {
      //定义变量
      CMD_S_GameEnd GameEnd;
      tagScoreInfo ScoreInfo[GAME_PLAYER];
      ZeroMemory(&GameEnd,sizeof(GameEnd));
      ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
      //贡献得分
      LONGLONG lBombScore[GAME_PLAYER];
      ZeroMemory(lBombScore,sizeof(lBombScore));
      GetBombScore(lBombScore);
      m_wGameWiner=(lBombScore[0]>lBombScore[1])?0:1;
      ScoreInfo[m_wGameWiner].lScore =lBombScore[m_wGameWiner]-lBombScore[(m_wGameWiner+1)%GAME_PLAYER];
      ScoreInfo[m_wGameWiner].cbType = SCORE_TYPE_WIN;
      //金币场积分调整
      if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
      {
        LONGLONG lScore=m_pITableFrame->GetTableUserItem((m_wGameWiner+1)%GAME_PLAYER)->GetUserScore();
        if(ScoreInfo[m_wGameWiner].lScore>lScore)
        {
          ScoreInfo[m_wGameWiner].lScore=lScore;
        }
      }
      ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].cbType = SCORE_TYPE_LOSE;
      ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;


      if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
      {
        //赢家赢的分数不能大于自己现在的分数
        LONGLONG lWinnerScore=m_pITableFrame->GetTableUserItem(m_wGameWiner)->GetUserScore();
        if(lWinnerScore>0&&ScoreInfo[m_wGameWiner].lScore>lWinnerScore)
        {
          ScoreInfo[m_wGameWiner].lScore=lWinnerScore;
          ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;

        }
      }


      //计算税收 写入积分
      for(WORD wIndex=0; wIndex<GAME_PLAYER; wIndex++)
      {
        //扣税变量
        WORD wRevenue=m_pGameServiceOption->wRevenueRatio;
        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD&&ScoreInfo[wIndex].lScore>0L)
        {
          //积分调整
          ScoreInfo[wIndex].lRevenue=m_pITableFrame->CalculateRevenue(wIndex,ScoreInfo[wIndex].lScore);
          GameEnd.lGameTax=ScoreInfo[wIndex].lRevenue;
        }
        GameEnd.lGameScore[wIndex]=ScoreInfo[wIndex].lScore-ScoreInfo[wIndex].lRevenue;

        m_lAllTurnScore[wIndex]+=GameEnd.lGameScore[wIndex];
        m_lLastTurnScore[wIndex]=GameEnd.lGameScore[wIndex];
      }

      for(BYTE i=0; i<GAME_PLAYER; i++)
      {
        GameEnd.lGameScore[i]=ScoreInfo[i].lScore;
      }
      //写入积分
      m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

      memcpy(GameEnd.lBombScore,lBombScore,sizeof(lBombScore));
      memcpy(GameEnd.cbBombList,m_cbBombList,sizeof(m_cbBombList));

      //剩余扑克
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
        CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
      }

      GameEnd.bHunter=true;

      //发送信息
      m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

      //切换庄家
      m_wBankerUser=m_wGameWiner;
      //结束游戏
      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
      return true;
    }
    case GER_USER_LEAVE:  //用户强退
    case GER_NETWORK_ERROR: //网络中断
    {
      //定义变量
      CMD_S_GameEnd GameEnd;
      tagScoreInfo ScoreInfo[GAME_PLAYER];
      ZeroMemory(&GameEnd,sizeof(GameEnd));
      ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

      m_wGameWiner=(wChairID+1)%GAME_PLAYER;

      //用户扑克
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        GameEnd.cbCardCount[i]=m_cbHandCardCount[i];
        CopyMemory(&GameEnd.cbCardData[i],m_cbHandCardData[i],m_cbHandCardCount[i]*sizeof(BYTE));
      }

      /*if(GameEnd.lGameScore[wChairID]==0)
      {
        GameEnd.lGameScore[wChairID] -=m_pGameServiceOption->lCellScore*15 ;
      }*/

      //计算积分 按对方手上最大项双扣+贡献分计算
      /*m_wGameWiner=(wChairID+1)%GAME_PLAYER;
      tagAnalyseInitResult AnalyseResult;
      ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
      m_GameLogic.AnalyseInitCardData(m_cbHandCardData[m_wGameWiner],m_cbHandCardCount[m_wGameWiner],AnalyseResult);

      BYTE cbMaxXiang=1;
      for(BYTE cbIndex=0;cbIndex<8;cbIndex++)
      {
        if(AnalyseResult.cbBlockCount[cbIndex]>=1)
        {
          cbMaxXiang=cbIndex+1;
        }
      }

      cbMaxXiang+=AnalyseResult.cbKingCount;
      cbMaxXiang+=AnalyseResult.cbSubKingCount;

      if (cbMaxXiang>=4)
      {
        m_cbBombList[m_wGameWiner][cbMaxXiang-4]++;
      }*/



      tagAnalyseResult AnalyseResult;
      ZeroMemory(&AnalyseResult,sizeof(tagAnalyseResult));
      m_GameLogic.AnalysebCardData(m_cbHandCardData[m_wGameWiner],m_cbHandCardCount[m_wGameWiner],AnalyseResult);
      //进行补炸弹操作
      for(BYTE cbIndex=0; cbIndex<13; cbIndex++)
      {
        if(AnalyseResult.m_CardStyle[cbIndex].m_cbCount>=4)
        {
          if(AnalyseResult.m_nJockerCount>0&&AnalyseResult.m_CardStyle[cbIndex].m_cbCount==AnalyseResult.m_nMaxCount)
          {
            m_cbBombList[m_wGameWiner][AnalyseResult.m_CardStyle[cbIndex].m_cbCount+AnalyseResult.m_nJockerCount-4]++;
            AnalyseResult.m_nJockerCount=0;
          }
          else
          {
            m_cbBombList[m_wGameWiner][AnalyseResult.m_CardStyle[cbIndex].m_cbCount-4]++;
          }
        }
      }

      //逃跑者已出炸弹无效
      for(int i=0; i<BOMB_TYPE_COUNT; i++)
      {
        m_cbBombList[wChairID][i]=0;
      }

      int cbFinishType=3;

      GameEnd.cbResultType = cbFinishType;

      //基础得分翻倍
      int cbBombGrade=m_GameLogic.GetMaxBombGrade(m_cbBombList[m_wGameWiner]);
      //5倍开始计算翻倍
      int  nMutile=(int)pow(2,cbBombGrade);

      //游戏积分
      LONGLONG  lMutileScore=m_lScoreList[m_cbCurGrade][cbFinishType]*nMutile;

      GameEnd.nMutile=nMutile;
      GameEnd.nBaseScore=m_lScoreList[m_cbCurGrade][cbFinishType];

      //贡献得分
      LONGLONG lBombScore[GAME_PLAYER];
      ZeroMemory(lBombScore,sizeof(lBombScore));
      GetBombScore(lBombScore);
      ScoreInfo[m_wGameWiner].lScore =lMutileScore+(lBombScore[m_wGameWiner]-lBombScore[(m_wGameWiner+1)%GAME_PLAYER]);
      LONGLONG score1=lBombScore[m_wGameWiner],score2=lBombScore[(m_wGameWiner+1)%GAME_PLAYER];

      if(ScoreInfo[m_wGameWiner].lScore>0)
      {
        //金币场积分调整
        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
        {
          LONGLONG lScore=m_pITableFrame->GetTableUserItem((m_wGameWiner+1)%GAME_PLAYER)->GetUserScore();
          if(ScoreInfo[m_wGameWiner].lScore>lScore)
          {
            ScoreInfo[m_wGameWiner].lScore=lScore;
          }
        }
        ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;

        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
        {
          //赢家赢的分数不能大于自己现在的分数
          LONGLONG lWinnerScore=m_pITableFrame->GetTableUserItem(m_wGameWiner)->GetUserScore();
          if(ScoreInfo[m_wGameWiner].lScore>lWinnerScore)
          {
            ScoreInfo[m_wGameWiner].lScore=lWinnerScore;
            ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;

          }
        }

      }
      else
      {
        //金币场积分调整
        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
        {
          LONGLONG lScore=m_pITableFrame->GetTableUserItem(m_wGameWiner)->GetUserScore();
          if(ScoreInfo[m_wGameWiner].lScore<(-lScore))
          {
            ScoreInfo[m_wGameWiner].lScore=-lScore;
          }
        }
        ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=-ScoreInfo[m_wGameWiner].lScore;

        if(m_pGameServiceOption->wServerType==GAME_GENRE_GOLD)
        {
          //赢家赢的分数不能大于自己现在的分数
          LONGLONG lWinnerScore=m_pITableFrame->GetTableUserItem((m_wGameWiner+1)%GAME_PLAYER)->GetUserScore();
          if(ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore>lWinnerScore)
          {
            ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore=lWinnerScore;
            ScoreInfo[m_wGameWiner].lScore=-ScoreInfo[(m_wGameWiner+1)%GAME_PLAYER].lScore;
          }

        }

      }

      //修改积分
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        GameEnd.lGameScore[i]=ScoreInfo[i].lScore;

        if(i==wChairID)
        {
          ScoreInfo[i].cbType=SCORE_TYPE_FLEE;
        }
        else
        {
          ScoreInfo[i].cbType=SCORE_TYPE_WIN;
        }
      }

      //历史积分
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        if(i==wChairID)
        {
          m_lAllTurnScore[i]-=GameEnd.lGameScore[i];
          m_lLastTurnScore[i]=GameEnd.lGameScore[i];

        }
        GameEnd.lAllTurnScore[i]=m_lAllTurnScore[i];
        GameEnd.lLastTurnScore[i]=m_lLastTurnScore[i];
      }


      memcpy(GameEnd.lBombScore,lBombScore,sizeof(lBombScore));
      memcpy(GameEnd.cbBombList,m_cbBombList,sizeof(m_cbBombList));

      //发送信息
      m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
      m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

      m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));

      //结束游戏
      m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

      //切换庄家
      m_wBankerUser=INVALID_CHAIR;
      return true;
    }
  }
  ASSERT(FALSE);

  return false;
}

bool CTableFrameSink::SendTableMessage(LPCTSTR lpszMessage, WORD wMessageType)
{
  IServerUserItem * pISendUserItem=NULL;
  for(WORD i=0; i<m_wPlayerCount; i++)
  {
    pISendUserItem=m_pITableFrame->GetTableUserItem(i);
    if(pISendUserItem!=NULL)
    {
      m_pITableFrame->SendGameMessage(pISendUserItem,lpszMessage,wMessageType);
    }
  }

  WORD wIndex=0;
  do
  {
    pISendUserItem=m_pITableFrame->EnumLookonUserItem(wIndex++);
    if(pISendUserItem==NULL)
    {
      break;
    }
    m_pITableFrame->SendGameMessage(pISendUserItem,lpszMessage,wMessageType);
  }
  while(true);
  return true;
}


//作弊用户
bool CTableFrameSink::OnCheatCard(IServerUserItem * pIServerUserItem)
{
  if(m_pCheatServer)
  {
    CMD_S_CheatCard CheatCard ;
    ZeroMemory(&CheatCard, sizeof(CheatCard));

    for(WORD i = 0; i < GAME_PLAYER; i++)
    {
      CheatCard.wCardUser[CheatCard.cbUserCount] = i;
      CheatCard.cbCardCount[CheatCard.cbUserCount] = m_cbHandCardCount[i];
      m_GameLogic.SortCardList(m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
      CopyMemory(CheatCard.cbCardData[CheatCard.cbUserCount++],m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
    }
    //发送作弊
    //WB改 这里这样发包存在BUG
    //m_pITableFrame->SendTableData(wChairID,SUB_S_CHEAT_CARD,&CheatCard,sizeof(CheatCard));
    //m_pITableFrame->SendLookonData(wChairID,SUB_S_CHEAT_CARD,&CheatCard,sizeof(CheatCard));
    m_pCheatServer->CheatServer(&CheatCard, m_pITableFrame, pIServerUserItem);
  }

  return true;
}
//发送作弊信息
void CTableFrameSink::SendCheatCard()
{
  if(m_pCheatServer)
  {
    CMD_S_CheatCard CheatCard ;
    ZeroMemory(&CheatCard, sizeof(CheatCard));

    for(WORD i = 0; i < GAME_PLAYER; i++)
    {
      CheatCard.wCardUser[CheatCard.cbUserCount] = i;
      CheatCard.cbCardCount[CheatCard.cbUserCount] = m_cbHandCardCount[i];
      m_GameLogic.SortCardList(m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
      CopyMemory(CheatCard.cbCardData[CheatCard.cbUserCount++],m_cbHandCardData[i],sizeof(BYTE)*m_cbHandCardCount[i]);
    }
    m_pCheatServer->CheatServer(&CheatCard, m_pITableFrame);
  }
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
  switch(cbGameStatus)
  {
    case GS_SK_FREE:    //空闲状态
    {
      //构造数据
      CMD_S_StatusFree StatusFree;
      memset(&StatusFree,0,sizeof(StatusFree));
      //游戏变量
      StatusFree.wBankerUser=m_wBankerUser;
      //发送场景
      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
    }
    case GS_SK_PLAYING:   //游戏状态
    {
      //构造数据
      CMD_S_StatusPlay StatusPlay;
      memset(&StatusPlay,0,sizeof(StatusPlay));

      //游戏变量
      StatusPlay.wCurrentUser=m_wCurrentUser;
      StatusPlay.lCellScore[0]=m_lScoreList[m_cbCurGrade][1];
      StatusPlay.lCellScore[1]=m_lScoreList[m_cbCurGrade][2];
      StatusPlay.lCellScore[2]=m_lScoreList[m_cbCurGrade][3];

      //历史积分
      CopyMemory(StatusPlay.lAllTurnScore,m_lAllTurnScore,sizeof(m_lAllTurnScore));
      CopyMemory(StatusPlay.lLastTurnScore,m_lLastTurnScore,sizeof(m_lLastTurnScore));

      //庄家用户
      StatusPlay.wBankerUser=m_wBankerUser;

      //胜利信息
      CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));
      CopyMemory(&(StatusPlay.LastOutCard),&m_LastOutCard,sizeof(tagAnalyseResult));


      //出牌信息
      StatusPlay.wTurnWiner=m_wTurnWiner;
      StatusPlay.cbTurnCardCount=m_cbTurnCardCount;
      CopyMemory(StatusPlay.cbTurnCardData,m_cbTurnCardData,m_cbTurnCardCount*sizeof(BYTE));

      //出牌变量
      for(WORD i=0; i<m_wPlayerCount; i++)
      {
        StatusPlay.cbOutCardCount[i]=m_cbOutCardCount[i];
        CopyMemory(StatusPlay.cbOutCardData[i],m_cbOutCardData[i],sizeof(BYTE)*m_cbOutCardCount[i]);
      }
      //扑克信息
      CopyMemory(StatusPlay.cbHandCardCount,m_cbHandCardCount,sizeof(m_cbHandCardCount));
      CopyMemory(StatusPlay.cbHandCardData,m_cbHandCardData[wChairID],sizeof(BYTE)*m_cbHandCardCount[wChairID]);

      if(pIServerUserItem)
      {
        //作弊用户
        if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
        {
          OnCheatCard(pIServerUserItem);
        }
      }

      //发送场景
      return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));


    }
  }

  return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
  return false;
}


//数据事件
bool CTableFrameSink::OnGameDataBase(WORD wRequestID, VOID * pData, WORD wDataSize)
{
  return false;
}
//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
  switch(wSubCmdID)
  {
    case SUB_C_OUT_CARD:    //用户出牌
    {
      //变量定义
      ASSERT(wDataSize==sizeof(CMD_C_OutCard));
      if(wDataSize!=sizeof(CMD_C_OutCard))
      {
        return false;
      }

      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }

      CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pData;

      OnCheatCard(pIServerUserItem);
      //消息处理
      return OnSubOutCard(pUserData->wChairID,pOutCard->cbCardData,pOutCard->cbCardCount);
    }
    case SUB_C_PASS_CARD:   //用户Pass
    {
      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      if(pUserData->cbUserStatus!=US_PLAYING)
      {
        return true;
      }
      //消息处理
      return OnSubPassCard(pUserData->wChairID);
    }
    case SUB_C_TRUSTEE:     //用户托管
    {
      CMD_C_Trustee *pTrustee =(CMD_C_Trustee *)pData;
      if(wDataSize != sizeof(CMD_C_Trustee))
      {
        return false;
      }
      //用户效验
      tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
      //变量设置
      m_bTrustee[pUserData->wChairID]=pTrustee->bTrustee;
      return OnSubTrustee(pUserData->wChairID,pTrustee->bTrustee);
    }
    case SUB_C_REQ_HUNTERS: //抄底请求
    {
      return OnSubReqHunter(pIServerUserItem->GetUserInfo()->wChairID,pIServerUserItem);
    }

    case SUB_C_ADMIN_COMMDN: //控制功能
    {
      ASSERT(wDataSize==sizeof(CMD_C_ControlApplication));
      if(wDataSize!=sizeof(CMD_C_ControlApplication))
      {
        return false;
      }
      if(m_pServerControl == NULL)
      {
        return true;
      }

      if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        return false;
      }

      m_wAdminChairID=pIServerUserItem->GetUserInfo()->wChairID;

      return m_pServerControl->ServerControl(wSubCmdID, pData, wDataSize, pIServerUserItem, m_pITableFrame);
    }
    default:
      break;
  }

  ASSERT(FALSE);
  return false;
}




//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{

  return false;
}
//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{
  //庄家设置
  if((bLookonUser==false)&&(m_wBankerUser==INVALID_CHAIR))
  {
    //m_wBankerUser=pIServerUserItem->GetChairID();
  }

  return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID,IServerUserItem * pIServerUserItem, bool bLookonUser)
{

  //庄家设置
  if(bLookonUser==false)
  {
    m_wBankerUser=INVALID_CHAIR;

    //积分设置
    m_lAllTurnScore[wChairID]=0L;
    m_lLastTurnScore[wChairID]=0L;
  }


  return true;
}

//用户准备
bool CTableFrameSink::OnActionUserOnReady(WORD wChairID,IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{

  //构造数据
  CMD_S_OnReady OnReady;
  OnReady.wChairId=wChairID;
  //发送数据
  m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ON_READY,&OnReady,sizeof(CMD_S_OnReady));
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ON_READY,&OnReady,sizeof(CMD_S_OnReady));

  return true;
}

//用户出牌
bool CTableFrameSink::OnSubOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
{
  //效验状态
  if(m_pITableFrame->GetGameStatus()!=GS_SK_PLAYING)
  {
    return true;
  }
  if(wChairID!=m_wCurrentUser)
  {
    return false;
  }

  //出牌判断

  tagAnalyseResult CurOutCard;
  memset(&CurOutCard,0,sizeof(tagAnalyseResult));
  m_GameLogic.AnalysebCardData(cbCardData,cbCardCount,CurOutCard);

  //对比扑克
  if(m_GameLogic.CompareCard(CurOutCard,m_LastOutCard))
  {
    //删除扑克
    if(m_GameLogic.RemoveCardEx(cbCardData,cbCardCount,m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
    {
      ASSERT(FALSE);
      return false;
    }

    if(CurOutCard.m_cbCardType>=CT_BOMB)
    {
      m_cbBombList[wChairID][CurOutCard.m_cbBombGrade-4]++;
    }
    //设置变量
    m_cbTurnCardCount=cbCardCount;
    m_cbHandCardCount[wChairID]-=cbCardCount;
    CopyMemory(m_cbTurnCardData,cbCardData,sizeof(BYTE)*cbCardCount);
    memcpy(&m_LastOutCard,&CurOutCard,sizeof(tagAnalyseResult));
    m_cbOutCardCount[wChairID]=cbCardCount;
    CopyMemory(m_cbOutCardData[wChairID],cbCardData,sizeof(BYTE)*cbCardCount);

    //胜利判断
    if(m_cbHandCardCount[wChairID]==0)
    {
      //设置用户
      m_wGameWiner=wChairID;
      m_wCurrentUser=INVALID_CHAIR;
    }
    else
    {
      //设置用户
      m_wTurnWiner=wChairID;
      m_wCurrentUser=(wChairID+1)%GAME_PLAYER;
    }

    //构造数据
    CMD_S_OutCard OutCard;
    OutCard.wOutCardUser=wChairID;
    OutCard.cbCardCount=cbCardCount;
    OutCard.wCurrentUser=m_wCurrentUser;
    CopyMemory(OutCard.cbCardData,m_cbTurnCardData,sizeof(m_cbTurnCardData));




    //贡献分
    ZeroMemory(m_lBombScore,sizeof(m_lBombScore));

    BYTE cbBombList[GAME_PLAYER][BOMB_TYPE_COUNT];
    ZeroMemory(cbBombList,sizeof(cbBombList));

    //GetBombScore(m_lBombScore);

    memcpy(cbBombList,m_cbBombList,sizeof(cbBombList));

    //初始化变量
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      m_GameLogic.TransBombGrade(cbBombList[i]);
      for(int nIndex=0; nIndex<BOMB_TYPE_COUNT; nIndex++)
      {
        m_lBombScore[i]+=m_lScoreList[m_cbCurGrade][nIndex+4]*cbBombList[i][nIndex];
      }
    }


    CopyMemory(OutCard.lBombScore,m_lBombScore,sizeof(m_lBombScore));


    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      IServerUserItem *pUserData=m_pITableFrame->GetTableUserItem(i);
      if(pUserData==NULL)
      {
        continue;
      }


      ZeroMemory(OutCard.cbCheatCardData,MAX_COUNT);

      //作弊用户
      if(CUserRight::IsGameCheatUser(pUserData->GetUserRight()))
      {
        CopyMemory(OutCard.cbCheatCardData,m_cbHandCardData[(i+1)%GAME_PLAYER],sizeof(BYTE)*(m_cbHandCardCount[(i+1)%GAME_PLAYER]));
      }

      //发送数据
      m_pITableFrame->SendTableData(i,SUB_S_OUT_CARD,&OutCard,sizeof(CMD_S_OutCard));
      m_pITableFrame->SendLookonData(i,SUB_S_OUT_CARD,&OutCard,sizeof(CMD_S_OutCard));
    }



    //结束判断
    if(m_wCurrentUser==INVALID_CHAIR)
    {
      OnEventGameConclude(wChairID,NULL,GER_NORMAL);
    }

  }
  else
  {
    return false;
  }
  return true;
}

//用户放弃
bool CTableFrameSink::OnSubPassCard(WORD wChairID)
{
  //效验状态
  if(m_pITableFrame->GetGameStatus()!=GS_SK_PLAYING)
  {
    return true;
  }
  if((wChairID!=m_wCurrentUser)||(m_cbTurnCardCount==0))
  {
    return false;
  }

  //设置用户
  m_wCurrentUser=(m_wCurrentUser+1)%GAME_PLAYER;

  //完成判断
  if(m_wCurrentUser==m_wTurnWiner)
  {
    //设置变量
    m_cbTurnCardCount=0;
    ZeroMemory(&m_LastOutCard,sizeof(tagAnalyseResult));
  }

  //构造消息
  CMD_S_PassCard PassCard;
  PassCard.wPassCardUser=wChairID;
  PassCard.wCurrentUser=m_wCurrentUser;
  PassCard.cbTurnOver=(m_cbTurnCardCount==0)?TRUE:FALSE;

  //发送数据
  m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PASS_CARD,&PassCard,sizeof(PassCard));

  return true;
}



//用户托管
bool CTableFrameSink::OnSubTrustee(WORD wChairID,bool bTrustee)
{
  //数据校验
  ASSERT(wChairID>=0&&wChairID<GAME_PLAYER);
  if(wChairID>GAME_PLAYER)
  {
    return false;
  }

  //构造变量
  CMD_S_Trustee Trustee;
  Trustee.bTrustee=bTrustee;
  Trustee.wChairID = wChairID;

  //发送数据
  m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
  m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));

  return true;
}

//玩家抄底
bool CTableFrameSink::OnSubReqHunter(WORD wChairID,IServerUserItem * pIServerUserItem)
{

  //有一位玩家已经抄底
  if(m_bHasReqHunter)
  {
    return true;
  }
  bool bReturn=true;
  bool bHaveBomb=false;

  if(!m_GameLogic.HasNotBombCard(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID]))
  {
    bHaveBomb=true;
  }

  //如果无炸弹则有权进行抄底
  if(!bHaveBomb)
  {

    m_bHasReqHunter=true;

    m_cbBombList[wChairID][(6)-4]++;
    //分析对家
    wChairID=(wChairID+1)%GAME_PLAYER;

    //抄底，只算最大张数+大小王
    BYTE cbMaxXiang=1;
    tagAnalyseInitResult AnalyseResult;
    ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));
    m_GameLogic.AnalyseInitCardData(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID],AnalyseResult);

    for(BYTE cbIndex=0; cbIndex<8; cbIndex++)
    {
      if(AnalyseResult.cbBlockCount[cbIndex]>=1)
      {
        cbMaxXiang=cbIndex+1;
      }
    }

    cbMaxXiang+=AnalyseResult.cbKingCount;
    cbMaxXiang+=AnalyseResult.cbSubKingCount;
    if(cbMaxXiang>=4)
    {
      m_cbBombList[wChairID][cbMaxXiang-4]++;
    }

    OnEventGameConclude(NULL,NULL,GER_HUNTER);
  }
  else
  {
    bReturn=false;
  }
  return bReturn;
}

//获取炸弹得分/贡献分
bool CTableFrameSink::GetBombScore(LONGLONG *lBombScore)
{
  try
  {
    //初始化变量
    for(WORD wChairID=0; wChairID<GAME_PLAYER; wChairID++)
    {
      m_GameLogic.TransBombGrade(m_cbBombList[wChairID]);
      for(int nIndex=0; nIndex<BOMB_TYPE_COUNT; nIndex++)
      {
        lBombScore[wChairID]+=m_lScoreList[m_cbCurGrade][nIndex+4]*m_cbBombList[wChairID][nIndex];
      }
    }
  }
  catch(...)
  {
    return false;
  }
  return true;
}

//读取配置
bool CTableFrameSink::ReadConfigInformation(bool bReadFresh)
{
  TCHAR OutBuf[255] = TEXT("");
  TCHAR SeachBuf[255] = TEXT("");
  m_nScoreGrade=GetPrivateProfileInt(m_szGameRoomName, TEXT("Row"), -1, m_szConfigFileName);

  if(m_nScoreGrade<= 0||m_nScoreGrade>MAX_SCOREGRADE)
  {
    m_nScoreGrade=1;
    //return false;
  }

  int col=GetPrivateProfileInt(m_szGameRoomName, TEXT("Col"), -1, m_szConfigFileName);
  if(col<0||col>13)
  {
    col=1;
    //return false;
  }
  //炸弹控制
  ZeroMemory(&m_nBombPercent,sizeof(m_nBombPercent));
  m_nBombPercent[0]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Bomb_0"), 5, m_szConfigFileName);
  m_nBombPercent[1]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Bomb_1"), 20, m_szConfigFileName);
  m_nBombPercent[2]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Bomb_2"), 10, m_szConfigFileName);
  m_nBombPercent[3]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Bomb_3"), 10, m_szConfigFileName);
  m_nBombPercent[4]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Xiang_6"), 15, m_szConfigFileName);
  m_nBombPercent[5]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Xiang_7"), 10, m_szConfigFileName);
  m_nBombPercent[6]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Xiang_8"), 10, m_szConfigFileName);
  m_nBombPercent[7]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Xiang_9"), 10, m_szConfigFileName);
  m_nBombPercent[8]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Xiang_10"), 7, m_szConfigFileName);
  m_nBombPercent[9]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Xiang_11"), 2, m_szConfigFileName);
  m_nBombPercent[10]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Xiang_12"), 1, m_szConfigFileName);

  m_nKingPercent[0]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_King_0"), 20, m_szConfigFileName);
  m_nKingPercent[1]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_King_1"), 20, m_szConfigFileName);
  m_nKingPercent[2]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_King_2"), 20, m_szConfigFileName);
  m_nKingPercent[3]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_King_3"), 20, m_szConfigFileName);
  m_nKingPercent[4]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_King_4"), 20, m_szConfigFileName);

  m_nKingPercent[5]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Other_King_3"), 5, m_szConfigFileName);
  m_nKingPercent[6]=GetPrivateProfileInt(m_szGameRoomName, TEXT("Percent_Other_King_4"), 5, m_szConfigFileName);

  if(m_nKingPercent[0]+m_nKingPercent[1]+m_nKingPercent[2]+m_nKingPercent[3]+m_nKingPercent[4]==0)
  {
    m_nKingPercent[0]=100;
  }

  for(int nrow = 0; nrow<m_nScoreGrade; nrow++)
  {
    for(int ncol=0; ncol<col; ncol++)
    {
      ZeroMemory(OutBuf,sizeof(OutBuf));
      ZeroMemory(SeachBuf,sizeof(SeachBuf));
      switch(ncol)
      {
        case 0:
          _sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("GradeScore_%d"),nrow);
          break;
        case 1:
          _sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("PingKou_%d"),nrow);
          break;
        case 2:
          _sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("DanKou_%d"),nrow);
          break;
        case 3:
          _sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("ShuangKou_%d"),nrow);
          break;
        default:
          _sntprintf(SeachBuf,sizeof(SeachBuf),TEXT("Bomb%d_%d"),nrow,ncol);
          break;
      }
      GetPrivateProfileString(m_szGameRoomName, SeachBuf, TEXT("1"), OutBuf, 255, m_szConfigFileName);
      _sntscanf(OutBuf, sizeof(OutBuf), _TEXT("%I64d"), &m_lScoreList[nrow][ncol]);
      if(m_lScoreList[nrow][ncol] == -1)
      {
        return false;
      }
    }
  }
  return true;
}
//////////////////////////////////////////////////////////////////////////
