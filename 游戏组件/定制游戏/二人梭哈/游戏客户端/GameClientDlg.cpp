#include "Stdafx.h"
#include "GameOption.h"
#include "GameClient.h"
#include "GameClientDlg.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME          200               //开始定时器
#define IDI_GIVE_UP           201               //放弃定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
  ON_MESSAGE(IDM_START,OnStart)
  ON_MESSAGE(IDM_FOLLOW,OnFollow)
  ON_MESSAGE(IDM_GIVE_UP,OnGiveUp)
  ON_MESSAGE(IDM_SHOWHAND,OnShowHand)
  ON_MESSAGE(IDM_ADD_GOLD,OnAddGold)
  ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
  ON_MESSAGE(IDM_ADMIN_COMMDN, OnAdminControl)
  ON_MESSAGE(IDM_ADMIN_COMMDN_EX, OnAdminControlEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
  //游戏变量
  m_lTurnMaxGold=0L;
  m_lTurnBasicGold=0L;
  m_lBasicGold = 0L;
  memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
  m_lShowHandScore = 0L;
  ZeroMemory(m_lUserScore,sizeof(m_lUserScore));


  //辅助变量
  m_lGoldShow=0L;
  m_bShowHand=false;
  memset(m_szName,0,sizeof(m_szName));
  ZeroMemory(&m_GameEnd,sizeof(m_GameEnd));

  return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//初始函数
bool CGameClientEngine::OnInitGameEngine()
{
  //全局对象
  CGlobalUnits * m_pGlobalUnits=(CGlobalUnits *)CGlobalUnits::GetInstance();
  ASSERT(m_pGlobalUnits!=NULL);

  //设置图标
  HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
  m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);
  SetIcon(hIcon,TRUE);
  SetIcon(hIcon,FALSE);

  return true;
}

//重置框架
bool CGameClientEngine::OnResetGameEngine()
{
  //游戏变量
  m_lTurnMaxGold=0L;
  m_lTurnBasicGold=0L;
  m_lBasicGold = 0L;
  memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
  m_wCurrentUser = INVALID_CHAIR;
  m_lShowHandScore = 0L;
  ZeroMemory(m_lUserScore,sizeof(m_lUserScore));


  //辅助变量
  m_lGoldShow=0L;
  m_bShowHand=false;
  memset(m_szName,0,sizeof(m_szName));
  ZeroMemory(&m_GameEnd,sizeof(m_GameEnd));

  return true;
}

//游戏设置
void CGameClientEngine::OnGameOptionSet()
{
  //构造数据
  CGameOption GameOption;
  GameOption.m_bEnableSound=m_pGlobalUnits->m_bAllowSound;
  GameOption.m_bAllowLookon=m_pGlobalUnits->m_bAllowLookon;

  //配置数据
  //if (GameOption.DoModal()==IDOK)
  //{
  //  EnableSound(GameOption.m_bEnableSound);
  //  AllowUserLookon(0,GameOption.m_bAllowLookon);
  //}

  //设置配置
  m_pGlobalUnits->m_bAllowSound=GameOption.m_bEnableSound;
  m_pGlobalUnits->m_bAllowLookon=GameOption.m_bAllowLookon;

  return;
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
  return true;
}
//时间消息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
  switch(wClockID)
  {
    case IDI_START_GAME:    //继续游戏定时器
    {
      if(nElapse==0)
      {
        AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
        return false;
      }
      if(nElapse<=10)
      {
        PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
      }

      return true;
    }
    case IDI_GIVE_UP:     //放弃定时器
    {
      WORD wViewChairID=SwitchViewChairID(wChairID);
      if(nElapse==0)
      {
        if((IsLookonMode()==false)&&(wViewChairID==MYSELF_VIEW_ID))
        {
          OnGiveUp(0,0);
        }
        return false;
      }
      if((nElapse<=10)&&(wViewChairID==MYSELF_VIEW_ID)&&(IsLookonMode()==false))
      {
        PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
      }

      return true;
    }
  }
  return false;
}

//旁观状态
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
  return true;
}

//网络消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_START:  //游戏开始
    {
      return OnSubGameStart(pData,wDataSize);
    }
    case SUB_S_ADD_GOLD:  //用户下注
    {
      m_GameClientView.FinishDispatchCard();
      return OnSubAddGold(pData,wDataSize);
    }
    case SUB_S_GIVE_UP:   //用户放弃
    {
      m_GameClientView.FinishDispatchCard();
      return OnSubGiveUp(pData,wDataSize);
    }
    case SUB_S_SEND_CARD: //发牌消息
    {
      m_GameClientView.FinishDispatchCard();
      return OnSubSendCard(pData,wDataSize);
    }
    case SUB_S_GAME_END:  //游戏结束
    {
      return OnSubGameEnd(pData,wDataSize);
    }
    case SUB_S_AMDIN_COMMAND:   //设置
    {
      return OnSubAdminControl(pData,wDataSize);
    }
    case SUB_S_AMDIN_COMMAND_EX:    //设置
    {
      return OnSubAdminControlEx(pData,wDataSize);
    }
    //case SUB_S_ALL_CARD:  //扑克消息
    //  {
    //    return true;
    //  }
  }

  return false;
}

//游戏场景
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
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
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

      //辅助变量
      m_bShowHand=false;

      //设置界面
      m_lGoldShow=0L;
      m_lTurnBasicGold=0L;
      m_lBasicGold = pStatusFree->dwBasicGold;
      m_GameClientView.SetGoldTitleInfo(0,pStatusFree->dwBasicGold);

      //玩家设置
      IClientUserItem * pIClientUserItem=GetMeUserItem();
      if((IsLookonMode()==false)&&(pIClientUserItem->GetUserStatus()!=US_READY))
      {
        //设置按钮
        m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
        m_GameClientView.m_btStart.SetFocus();

        //设置时间
        SetGameClock(GetMeChairID(),IDI_START_GAME,30);
      }
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
        m_GameClientView.m_btOpenAdminEx.ShowWindow(SW_SHOW);
      }

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

      //设置变量
      m_lTurnMaxGold=pStatusPlay->lTurnMaxGold;
      m_lTurnBasicGold=pStatusPlay->lTurnBasicGold;
      m_lBasicGold = pStatusPlay->lBasicGold;
      CopyMemory(m_bPlayStatus,pStatusPlay->bPlayStatus,sizeof(m_bPlayStatus));
      m_wCurrentUser = pStatusPlay->wCurrentUser;
      m_lShowHandScore = pStatusPlay->lShowHandScore;
      m_bShowHand = pStatusPlay->bShowHand?true:false;

      //设置界面
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        WORD wViewStation=SwitchViewChairID(i);
        IClientUserItem * pClientUserItem=GetTableUserItem(i);
        if(pClientUserItem!=NULL)
        {
          m_lUserScore[i] = pClientUserItem->GetUserScore()-pStatusPlay->lServiceScore;
          lstrcpyn(m_szName[i],pClientUserItem->GetNickName(),CountArray(m_szName[i]));
          if(m_bPlayStatus[i]==TRUE)
          {
            m_GameClientView.m_CardControl[wViewStation].SetCardData(pStatusPlay->bTableCardArray[i],pStatusPlay->bTableCardCount[i]);
          }
          m_GameClientView.SetUserGold(wViewStation,pClientUserItem->GetUserScore());
        }
        m_GameClientView.SetUserGoldInfo(wViewStation,false,pStatusPlay->lTableGold[2*i]);
        m_GameClientView.SetUserGoldInfo(wViewStation,true,pStatusPlay->lTableGold[2*i+1]);
      }
      m_GameClientView.SetGoldTitleInfo(pStatusPlay->lTurnMaxGold,pStatusPlay->lBasicGold);

      //判断是否观看
      if((IsLookonMode()==false)||(IsAllowLookon()==true))
      {
        m_GameClientView.m_CardControl[MYSELF_VIEW_ID].AllowPositively(true);
      }

      //判断是否自己下注
      if((IsLookonMode()==false)&&(pStatusPlay->wCurrentUser==GetMeChairID()))
      {
        UpdateScoreControl();
      }
      SetGameClock(pStatusPlay->wCurrentUser,IDI_GIVE_UP,30);

      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight)&&m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);
        m_GameClientView.m_btOpenAdminEx.ShowWindow(SW_SHOW);
      }

      return true;
    }
  }

  return false;
}

//游戏开始
bool CGameClientEngine::OnSubGameStart(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_GameStart))
  {
    return false;
  }
  CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pBuffer;

  //游戏变量
  m_lTurnMaxGold=pGameStart->lTurnMaxGold;
  m_lTurnBasicGold=pGameStart->lTurnBasicGold;
  m_lBasicGold = pGameStart->lBasicGold;
  memset(m_szName,0,sizeof(m_szName));
  memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
  m_wCurrentUser = pGameStart->wCurrentUser;
  m_lShowHandScore = pGameStart->lShowHandScore;

  //辅助变量
  m_lGoldShow=0L;
  m_bShowHand=false;

  //变量定义
  WORD wMeChairID=GetMeChairID();
  bool bLookonMode=IsLookonMode();
  LONGLONG lBaseGold=m_lBasicGold;
  m_GameClientView.SetGoldTitleInfo(pGameStart->lTurnMaxGold,m_lBasicGold);

  //设置状态
  SetGameStatus(GAME_STATUS_PLAY);

  //设置界面
  m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //变量定义
    WORD wViewChairID=SwitchViewChairID(i);
    IClientUserItem * pClientUserItem=GetTableUserItem(i);

    //设置界面
    if(pClientUserItem!=NULL)
    {
      m_bPlayStatus[i]=TRUE;
      m_lUserScore[i] = pClientUserItem->GetUserScore()-pGameStart->lServiceScore;
      m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
      m_GameClientView.SetUserGoldInfo(wViewChairID,true,lBaseGold);
      m_GameClientView.m_GoldView[i*2].SetGoldIndex(1);
      lstrcpyn(m_szName[i],pClientUserItem->GetNickName(),CountArray(m_szName[i]));
      m_GameClientView.SetUserGold(wViewChairID,pClientUserItem->GetUserScore());
    }
    else
    {
      m_bPlayStatus[i]=FALSE;
      m_lUserScore[i] = 0L;
      m_GameClientView.SetUserGoldInfo(wViewChairID,true,0L);
      m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
    }

    //设置控件
    m_GameClientView.m_CardControl[wViewChairID].SetCardData(NULL,0);
    m_GameClientView.m_CardControl[wViewChairID].ShowFirstCard(false);
  }
  if(bLookonMode==false)
  {
    ActiveGameFrame();
    m_GameClientView.m_CardControl[MYSELF_VIEW_ID].AllowPositively(true);
  }

  //派发扑克
  for(BYTE cbIndex=0; cbIndex<2; cbIndex++)
  {
    for(WORD i=0; i<GAME_PLAYER; i++)
    {
      if(m_bPlayStatus[i]==TRUE)
      {
        //变量定义
        WORD wViewChairID=SwitchViewChairID(i);
        BYTE cbCardData[2]= {0,pGameStart->bCardData[i]};

        //派发扑克
        cbCardData[0]=(GetMeChairID()==i)?pGameStart->bFundusCard:0;
        m_GameClientView.DispatchUserCard(wViewChairID,cbCardData[cbIndex]);
      }
    }
  }

  //设置
  PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

  return true;
}

//用户加注
bool CGameClientEngine::OnSubAddGold(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_AddGold))
  {
    return false;
  }
  CMD_S_AddGold * pAddGold=(CMD_S_AddGold *)pBuffer;

  //变量定义
  WORD wMeChairID=GetMeChairID();
  WORD wViewChairID=SwitchViewChairID(pAddGold->wLastChairID);
  m_wCurrentUser = pAddGold->wCurrentUser;

  //处理数据
  m_lTurnBasicGold=pAddGold->lCurrentLessGold;

  m_bShowHand = pAddGold->bShowHand;
  if(m_bShowHand && m_bPlayStatus[pAddGold->wLastChairID])
  {
    m_GameClientView.SetUserShowHand(true);
    m_GameClientView.m_GoldView[wViewChairID*2].SetGoldIndex(2);
  }

  if(pAddGold->lLastAddGold > 0L)
  {
    m_GameClientView.SetUserGoldInfo(wViewChairID,false,pAddGold->lLastAddGold);
  }
  if((IsLookonMode()==false)&&(pAddGold->wCurrentUser==wMeChairID))
  {
    UpdateScoreControl();
  }

  //其他处理
  SetGameClock(pAddGold->wCurrentUser,IDI_GIVE_UP,30);

  if(m_bShowHand && m_bPlayStatus[pAddGold->wLastChairID])
  {
    PlayGameSound(AfxGetInstanceHandle(),TEXT("SHOW_HAND"));
  }
  else if(m_bPlayStatus[pAddGold->wLastChairID]==TRUE)
  {
    PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
  }

  return true;
}

//用户放弃
bool CGameClientEngine::OnSubGiveUp(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_GiveUp))
  {
    return false;
  }
  CMD_S_GiveUp * pGiveUp=(CMD_S_GiveUp *)pBuffer;

  //设置变量
  m_bPlayStatus[pGiveUp->wUserChairID]=false;
  if(pGiveUp->wUserChairID==GetMeChairID())
  {
    SetGameStatus(GAME_STATUS_FREE);
  }

  //设置界面
  BYTE bCard[5]= {0,0,0,0,0};
  WORD wViewStation=SwitchViewChairID(pGiveUp->wUserChairID);
  WORD bCount=m_GameClientView.m_CardControl[wViewStation].GetCardCount();
  m_GameClientView.m_CardControl[wViewStation].AllowPositively(false);
  m_GameClientView.m_CardControl[wViewStation].ShowFirstCard(false);
  m_GameClientView.m_CardControl[wViewStation].SetCardData(bCard,bCount);

  //更新数据
  if(m_lTurnMaxGold != pGiveUp->lTurnMaxGold)
  {
    //设置数据
    m_lTurnMaxGold = pGiveUp->lTurnMaxGold;

    //设置最大下注
    m_GameClientView.SetGoldTitleInfo(m_lTurnMaxGold,m_GameClientView.m_lBasicGold);

    if(!IsLookonMode() && m_wCurrentUser==GetMeChairID() && pGiveUp->wUserChairID != m_wCurrentUser)
    {
      UpdateScoreControl();
    }
  }

  //其他处理
  if(pGiveUp->wUserChairID==GetClockChairID())
  {
    KillGameClock(IDI_GIVE_UP);
  }
  PlayGameSound(AfxGetInstanceHandle(),TEXT("GIVE_UP"));

  return true;
}

//发牌消息
bool CGameClientEngine::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_R_SendCard))
  {
    return false;
  }
  CMD_R_SendCard * pSendCard=(CMD_R_SendCard *)pBuffer;

  //设置变量
  m_lGoldShow=0L;
  m_lTurnBasicGold=0L;
  m_lTurnMaxGold=pSendCard->lMaxGold;
  m_wCurrentUser = pSendCard->wCurrentUser;

  //更新界面
  LONGLONG lUserTableGold=0L;
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取位置
    WORD wViewChairID=SwitchViewChairID(i);

    //设置金币
    lUserTableGold=m_GameClientView.m_GoldView[wViewChairID*2].GetGold();
    lUserTableGold+=m_GameClientView.m_GoldView[wViewChairID*2+1].GetGold();
    m_GameClientView.SetUserGoldInfo(wViewChairID,false,0L);
    m_GameClientView.SetUserGoldInfo(wViewChairID,true,lUserTableGold);
  }

  //派发扑克,从上次最大玩家开始发起
  WORD wLastMostUser = pSendCard->wStartChairId;
  ASSERT(wLastMostUser != INVALID_CHAIR);
  for(BYTE i=0; i<pSendCard->cbSendCardCount; i++)
  {
    for(WORD j=0; j<GAME_PLAYER; j++)
    {
      WORD wChairId = (wLastMostUser+j)%GAME_PLAYER;
      if(m_bPlayStatus[wChairId]==TRUE&&pSendCard->bUserCard[wChairId][i]!=0)
      {
        WORD wViewChairID=SwitchViewChairID(wChairId);
        m_GameClientView.DispatchUserCard(wViewChairID,pSendCard->bUserCard[wChairId][i]);
      }
    }
  }

  //播放声音
  if(!m_bShowHand)
  {
    PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
  }

  return true;
}

//游戏结束
bool CGameClientEngine::OnSubGameEnd(const void * pBuffer, WORD wDataSize)
{
  //效验参数
  if(wDataSize!=sizeof(CMD_S_GameEnd))
  {
    return false;
  }
  CMD_S_GameEnd * pGameEnd=(CMD_S_GameEnd *)pBuffer;

  //播放声音
  if(!IsLookonMode())
  {
    if(pGameEnd->lGameGold[GetMeChairID()]>0L)
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WIN"));
    }
    else
    {
      PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_LOST"));
    }
  }
  else
  {
    PlayGameSound(GetModuleHandle(NULL),TEXT("GAME_END"));
  }

  //设置状态
  SetGameStatus(GAME_STATUS_FREE);

  KillGameClock(IDI_GIVE_UP);

  //复制数据
  CopyMemory(&m_GameEnd,pGameEnd,sizeof(m_GameEnd));

  if(!m_GameClientView.IsDispatchCard())
  {
    PerformGameEnd();
  }

  return true;
}

//开始按钮
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
  //删除时间
  KillGameClock(IDI_START_GAME);

  //设置控件
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    m_GameClientView.m_GoldView[i*2].SetGold(0L);
    m_GameClientView.m_GoldView[i*2+1].SetGold(0L);
    m_GameClientView.m_GoldView[i*2].SetGoldIndex(1);
    m_GameClientView.m_CardControl[i].SetCardData(NULL,0);
    m_GameClientView.m_CardControl[i].ShowFirstCard(false);
  }

  //设置控件
  m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
  HideScoreControl();

  //隐藏控件
  m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

  //更新界面
  m_GameClientView.RefreshGameView();

  //发送消息
  SendUserReady(NULL,0);

  return 0;
}

//放弃按钮
LRESULT CGameClientEngine::OnGiveUp(WPARAM wParam, LPARAM lParam)
{
  //放弃游戏
  KillGameClock(IDI_GIVE_UP);
  //显示按钮
  HideScoreControl();

  m_GameClientView.SetUserGoldInfo(MYSELF_VIEW_ID,false,m_lGoldShow);

  //发送消息
  SendSocketData(SUB_C_GIVE_UP);

  return 0;
}

//跟注按钮
LRESULT CGameClientEngine::OnFollow(WPARAM wParam, LPARAM lParam)
{

  //获取金币
  LONGLONG lGold=m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2].GetGold();

  LONGLONG lShowHandScore = __min(m_lUserScore[GetMeChairID()],m_lShowHandScore);

  if(lGold+m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2+1].GetGold() < lShowHandScore)
  {
    lGold=__max(lGold,m_lTurnBasicGold);
  }

  //设置变量
  if(lShowHandScore == lGold+m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2+1].GetGold())
  {
    if(!m_bShowHand)
    {
      m_GameClientView.SetUserShowHand(true);
    }
    m_bShowHand=true;
    m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2].SetGoldIndex(2);
  }

  //显示按钮
  HideScoreControl();

  m_GameClientView.SetUserGoldInfo(MYSELF_VIEW_ID,false,lGold);

  //删除定时器
  KillGameClock(IDI_GIVE_UP);

  //发送数据
  CMD_C_AddGold AddPoint;
  AddPoint.lGold=lGold;
  SendSocketData(SUB_C_ADD_GOLD,&AddPoint,sizeof(AddPoint));

  return 0;
}

//梭哈按钮
LRESULT CGameClientEngine::OnShowHand(WPARAM wParam, LPARAM lParam)
{
  LONGLONG lShowHandScore = __min(m_lUserScore[GetMeChairID()],m_lShowHandScore);
  LONGLONG lGold=lShowHandScore-m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2+1].GetGold();

  m_GameClientView.SetUserGoldInfo(MYSELF_VIEW_ID,false,lGold);
  OnFollow(0,0);

  m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2].SetGoldIndex(2);

  return 0;
}

//加注消息

LRESULT CGameClientEngine::OnAddGold(WPARAM wParam, LPARAM lParam)
{
  LONGLONG lGold = m_lTurnBasicGold+m_lBasicGold*(LONG)wParam;
  LONGLONG lShowHandScore = __min(m_lUserScore[GetMeChairID()],m_lShowHandScore);

  if(lGold+m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2+1].GetGold() < lShowHandScore)
  {
    lGold=__max(lGold,m_lTurnBasicGold);
  }

  //设置加注
  m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2].SetGold(lGold);

  //播放声音
  PlayGameSound(AfxGetInstanceHandle(),TEXT("CHOOSE_SCORE"));

  //
  OnFollow(0,0);

  return 0;
}

//发牌完成
LRESULT CGameClientEngine::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
  if(m_bPlayStatus[GetMeChairID()] && GetGameStatus() == GAME_STATUS_FREE)
  {
    PerformGameEnd();
    return 0;
  }

  //下注判断
  if((IsLookonMode()==false)&&(m_wCurrentUser==GetMeChairID()))
  {
    UpdateScoreControl();
  }


  //设置定时器
  SetGameClock(m_wCurrentUser,IDI_GIVE_UP,30);

  return 0;
}

//控制
LRESULT CGameClientEngine::OnAdminControl(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_AMDIN_COMMAND,(CMD_C_AdminReq*)wParam,sizeof(CMD_C_AdminReq));
  return true;
}

//控制
bool CGameClientEngine::OnSubAdminControl(const void * pBuffer, WORD wDataSize)
{
  ASSERT(wDataSize==sizeof(CMD_S_CommandResult));
  if(wDataSize!=sizeof(CMD_S_CommandResult))
  {
    return false;
  }

  m_GameClientView.m_pClientControlDlg->ReqResult(pBuffer);
  return true;
}

//控制
LRESULT CGameClientEngine::OnAdminControlEx(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_AMDIN_COMMAND_EX,(CMD_C_AdminReqEx*)wParam,sizeof(CMD_C_AdminReqEx));
  return true;
}

//控制
bool CGameClientEngine::OnSubAdminControlEx(const void * pBuffer, WORD wDataSize)
{
  ASSERT(wDataSize==sizeof(CMD_S_CommandResultEx));
  if(wDataSize!=sizeof(CMD_S_CommandResultEx))
  {
    return false;
  }

  m_GameClientView.m_pClientControlDlgEx->ReqResult(pBuffer);
  return true;
}

//
void CGameClientEngine::UpdateScoreControl()
{
  ActiveGameFrame();

  //显示按钮
  m_GameClientView.ShowScoreControl(true);

  WORD wMeChairID = GetMeChairID();

  //设置数据
  m_lGoldShow=m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2].GetGold();

  //变量定义
  WORD wCardCount=m_GameClientView.m_CardControl[MYSELF_VIEW_ID].GetCardData(NULL,0);
  LONGLONG lLeaveScore=m_lTurnMaxGold-m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2].GetGold()-m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2+1].GetGold();

  //禁止按钮
  m_GameClientView.m_btShowHand.EnableWindow((wCardCount>=3&&(lLeaveScore>0||m_bShowHand))?TRUE:FALSE);
  lLeaveScore=m_lTurnBasicGold-m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2].GetGold();
  LONGLONG lShowHandScore = __min(m_lUserScore[wMeChairID],m_lShowHandScore);
  if(!m_bShowHand &&
     (m_lTurnBasicGold == 0 || lLeaveScore>0L&&lShowHandScore>m_lTurnBasicGold+m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2+1].GetGold())
    )
  {
    m_GameClientView.m_btFollow.EnableWindow(TRUE);
  }
  else
  {
    m_GameClientView.m_btFollow.EnableWindow(FALSE);
  }

  //加注按钮
  lLeaveScore=m_lTurnMaxGold-m_lTurnBasicGold-m_GameClientView.m_GoldView[MYSELF_VIEW_ID*2+1].GetGold();
  m_GameClientView.m_btAddTimes1.EnableWindow((lLeaveScore>=m_lBasicGold)?TRUE:FALSE);
  m_GameClientView.m_btAddTimes2.EnableWindow((lLeaveScore>=2*m_lBasicGold)?TRUE:FALSE);
  m_GameClientView.m_btAddTimes3.EnableWindow((lLeaveScore>=3*m_lBasicGold)?TRUE:FALSE);

  return;
}

//
void CGameClientEngine::HideScoreControl()
{
  m_GameClientView.ShowScoreControl(false);
}

//
void CGameClientEngine::PerformGameEnd()
{
  CMD_S_GameEnd *pGameEnd = &m_GameEnd;

  //设置积分
  CString strTemp ,strEnd = _T("\n");
  for(WORD i = 0; i < GAME_PLAYER; i++)
  {
    if(pGameEnd->lGameGold[i]>0)
    {
      strTemp.Format(_T("%s: %+I64d\n"),m_szName[i],pGameEnd->lGameGold[i]);
    }
    else if(m_szName[i][0] != 0)
    {
      strTemp.Format(_T("%s: %I64d\n"),m_szName[i],pGameEnd->lGameGold[i]);
    }
    else
    {
      strTemp.Empty();
    }
    strEnd += strTemp;
  }
  //消息积分
  if(m_pIStringMessage != NULL)
  {
    m_pIStringMessage->InsertSystemString((LPCTSTR)strEnd);
  }

  //游戏正常结束
  BYTE bCardData[5];
  m_GameClientView.m_ScoreView.ResetScore();
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //设置信息
    if(pGameEnd->lGameGold[i]!=0L)
    {
      if(m_szName[i][0]==0)
      {
        m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),pGameEnd->lGameGold[i]);
      }
      else
      {
        m_GameClientView.m_ScoreView.SetGameScore(i,m_szName[i],pGameEnd->lGameGold[i]);
      }
      if(pGameEnd->bUserCard[i]!=0)
      {
        WORD wViewStation=SwitchViewChairID(i);
        WORD wCardCount=m_GameClientView.m_CardControl[wViewStation].GetCardData(bCardData,CountArray(bCardData));
        bCardData[0]=pGameEnd->bUserCard[i];
        m_GameClientView.m_CardControl[wViewStation].SetCardData(bCardData,wCardCount);
        m_GameClientView.m_CardControl[wViewStation].ShowFirstCard(true);
      }
    }
  }
  m_GameClientView.m_ScoreView.SetTax(pGameEnd->lTax);
  m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

  //设置控件
  m_GameClientView.SetUserShowHand(false);
  HideScoreControl();

  //游戏变量
  m_lTurnMaxGold=0L;
  m_lTurnBasicGold=0L;
  memset(m_bPlayStatus,0,sizeof(m_bPlayStatus));
  m_wCurrentUser = INVALID_CHAIR;
  m_lShowHandScore = 0L;

  //辅助变量
  m_lGoldShow=0L;
  m_bShowHand=false;
  memset(m_szName,0,sizeof(m_szName));

  //设置界面
  if(IsLookonMode()==false)
  {
    m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
    m_GameClientView.m_btStart.SetFocus();
    SetGameClock(GetMeChairID(),IDI_START_GAME,30);
  }
}

//////////////////////////////////////////////////////////////////////////
