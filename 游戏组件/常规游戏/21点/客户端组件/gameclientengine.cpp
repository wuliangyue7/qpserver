#include "StdAfx.h"

#include "GameClient.h"
#include "GameClientEngine.h"

//////////////////////////////////////////////////////////////////////////

//定时器标识
#define IDI_START_GAME        200                 //开始定时器
#define IDI_USER_OPERATE      201                 //加注定时器

//时间标识
#define TIME_START_GAME       30                  //开始定时器
#define TIME_USER_OPERATE     30                  //放弃定时器

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameClientEngine, CGameFrameEngine)
  ON_MESSAGE(IDM_START,OnStart)
  ON_MESSAGE(IDM_ADD_SCORE,OnAddScore)
  ON_MESSAGE(IDM_GET_CARD,OnGetCard)
  ON_MESSAGE(IDM_STOP_CARD,OnStopCard)
  ON_MESSAGE(IDM_DOUBLE_CARD,OnDoubleCard)
  ON_MESSAGE(IDM_SPLIT_CARD,OnSplitCard)
  ON_MESSAGE(IDM_INSURE_CARD,OnInsureCard)
  ON_MESSAGE(IDM_SEND_CARD_FINISH,OnSendCardFinish)
  ON_MESSAGE(IDM_ADMIN_UPDATE_STORAGE, OnStorage)
  ON_MESSAGE(IDM_ADMIN_MODIFY_STORAGE, OnStorageInfo)
  ON_MESSAGE(IDM_REQUEST_QUERY_USER, OnRequestQueryUser)
  ON_MESSAGE(IDM_USER_CONTROL, OnUserControl)
  ON_MESSAGE(IDM_REQUEST_UPDATE_ROOMINFO, OnRequestUpdateRoomInfo)
  ON_MESSAGE(IDM_CLEAR_CURRENT_QUERYUSER, OnClearCurrentQueryUser)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameClientEngine::CGameClientEngine()
{
  //配置变量
  m_dwCardHSpace=16;//DEFAULT_PELS;

  m_wBankerUser = INVALID_CHAIR;

  ZeroMemory(m_bStopCard,sizeof(m_bStopCard));
  ZeroMemory(m_bInsureCard,sizeof(m_bInsureCard));
  ZeroMemory(m_bDoubleCard,sizeof(m_bDoubleCard));
  ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

  m_bTurnBanker = FALSE;
  m_bSpliting = FALSE;
  m_wSplitingUser = INVALID_CHAIR;

  //加注信息
  m_lMaxScore=0L;
  m_lCellScore=0L;

  //状态变量
  ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
  ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

  return;
}

//析构函数
CGameClientEngine::~CGameClientEngine()
{
}

//创建引擎
bool CGameClientEngine::OnInitGameEngine()
{
  //设置属性
  HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
  m_pIClientKernel->SetGameAttribute(KIND_ID,GAME_PLAYER,VERSION_CLIENT,hIcon,GAME_NAME);

  for(int i=0; i<GAME_PLAYER; i++)
  {
    m_GameClientView.m_CardControl[i].SetCardSpace(m_dwCardHSpace);
  }
  return true;
}

//重置函数
bool CGameClientEngine::OnResetGameEngine()
{
  m_wBankerUser = INVALID_CHAIR;

  ZeroMemory(m_bStopCard,sizeof(m_bStopCard));
  ZeroMemory(m_bInsureCard,sizeof(m_bInsureCard));
  ZeroMemory(m_bDoubleCard,sizeof(m_bDoubleCard));
  ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

  m_bTurnBanker = FALSE;
  m_bSpliting = FALSE;
  m_wSplitingUser = INVALID_CHAIR;

  //加注信息
  m_lMaxScore=0L;
  m_lCellScore=0L;

  //状态变量
  ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
  ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
  return true;
}

//时钟删除
bool CGameClientEngine::OnEventGameClockKill(WORD wChairID)
{
  return true;
}

//时钟信息
bool CGameClientEngine::OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID)
{
  switch(wClockID)
  {
    case IDI_START_GAME:    //开始定时器
    {
      //中止判断
      if(nElapse==0)
      {

        AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);
        return false;
      }

      //警告通知
      if(nElapse<=5)
      {
        PlayGameSound(AfxGetInstanceHandle(),TEXT("START_WARN"));
      }

      return true;
    }
    case IDI_USER_OPERATE:  //加注定时器
    {
      //获取位置
      WORD wViewChairID=SwitchViewChairID(wChairID);

      //中止判断
      if(nElapse==0)
      {
        if((IsLookonMode()==false)&&(wViewChairID==2))
        {
          if(m_cbCardCount[0] > 0)
          {
            OnStopCard(0,0);
          }
          else
          {
            OnAddScore(1,1);
          }
        }
        return false;
      }

      //警告通知
      if((nElapse<=3)&&(wViewChairID==2)&&(IsLookonMode()==false))
      {
        PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_WARN"));
        return true;
      }

      return true;
    }
  }

  return false;
}

//旁观消息
bool CGameClientEngine::OnEventLookonMode(VOID * pData, WORD wDataSize)
{
  return true;
}

//游戏消息
bool CGameClientEngine::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  switch(wSubCmdID)
  {
    case SUB_S_GAME_START:  //游戏开始
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      //消息处理
      return OnSubGameStart(pData,wDataSize);
    }
    case SUB_S_ADD_SCORE: //加注
    {
      return OnSubAddScore(pData,wDataSize);
    }
    case SUB_S_STOP_CARD: //停牌
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      //消息处理
      return OnSubStopCard(pData,wDataSize);
    }
    case SUB_S_DOUBLE_SCORE:    //加倍
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      //消息处理
      return OnSubDoubleCard(pData,wDataSize);
    }
    case SUB_S_SPLIT_CARD:  //分牌
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      //消息处理
      return OnSubSplitCard(pData,wDataSize);
    }
    case SUB_S_INSURE:    //保险
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      //消息处理
      return OnSubInsureCard(pData,wDataSize);
    }
    case SUB_S_GET_CARD:  //要牌
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      //消息处理
      return OnSubGetCard(pData,wDataSize);
    }
    case SUB_S_SEND_CARD: //发牌消息
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      //消息处理
      return OnSubSendCard(pData,wDataSize);
    }
    case SUB_S_GAME_END:  //游戏结束
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      //消息处理
      return OnSubGameEnd(pData,wDataSize);
    }
    case SUB_S_CHEAT_CARD:
    {
      //结束动画
      m_GameClientView.FinishDispatchCard();

      return OnSubGMCard(pData,wDataSize);
    }
    case SUB_S_ADMIN_STORAGE_INFO:  //特殊客户端信息
    {
      ASSERT(wDataSize == sizeof(CMD_S_ADMIN_STORAGE_INFO));
      if(wDataSize != sizeof(CMD_S_ADMIN_STORAGE_INFO))
      {
        return false;
      }
      //判断权限
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
         ||NULL == m_GameClientView.m_hInst)
      {
        return true;
      }

      CMD_S_ADMIN_STORAGE_INFO *pStorage = (CMD_S_ADMIN_STORAGE_INFO *)pData;
      m_GameClientView.m_pClientControlDlg->SetStorageInfo(pStorage);
      return true;
    }
    case SUB_S_REQUEST_QUERY_RESULT:
    {
      ASSERT(wDataSize == sizeof(CMD_S_RequestQueryResult));
      if(wDataSize != sizeof(CMD_S_RequestQueryResult))
      {
        return false;
      }
      //判断权限
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
         ||NULL == m_GameClientView.m_hInst)
      {
        return true;
      }

      CMD_S_RequestQueryResult *pQueryResult = (CMD_S_RequestQueryResult *)pData;
      m_GameClientView.m_pClientControlDlg->RequestQueryResult(pQueryResult);

      return true;
    }
    case SUB_S_USER_CONTROL:
    {
      ASSERT(wDataSize == sizeof(CMD_S_UserControl));
      if(wDataSize != sizeof(CMD_S_UserControl))
      {
        return false;
      }
      //判断权限
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
         ||NULL == m_GameClientView.m_hInst)
      {
        return true;
      }

      CMD_S_UserControl *pUserControl = (CMD_S_UserControl *)pData;
      m_GameClientView.m_pClientControlDlg->RoomUserControlResult(pUserControl);

      return true;
    }
    case SUB_S_USER_CONTROL_COMPLETE:
    {
      ASSERT(wDataSize == sizeof(CMD_S_UserControlComplete));
      if(wDataSize != sizeof(CMD_S_UserControlComplete))
      {
        return false;
      }
      //判断权限
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
         ||NULL == m_GameClientView.m_hInst)
      {
        return true;
      }

      CMD_S_UserControlComplete *pUserControlComplete = (CMD_S_UserControlComplete *)pData;
      m_GameClientView.m_pClientControlDlg->UserControlComplete(pUserControlComplete);

      return true;
    }
    case SUB_S_OPERATION_RECORD:
    {
      ASSERT(wDataSize == sizeof(CMD_S_Operation_Record));
      if(wDataSize != sizeof(CMD_S_Operation_Record))
      {
        return false;
      }
      //判断权限
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
         ||NULL == m_GameClientView.m_hInst)
      {
        return true;
      }

      CMD_S_Operation_Record *pOperation_Record = (CMD_S_Operation_Record *)pData;
      m_GameClientView.m_pClientControlDlg->UpdateOperationRecord(pOperation_Record);

      return true;
    }
    case SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT:
    {
      ASSERT(wDataSize == sizeof(CMD_S_RequestUpdateRoomInfo_Result));
      if(wDataSize != sizeof(CMD_S_RequestUpdateRoomInfo_Result))
      {
        return false;
      }
      //判断权限
      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) == false
         ||NULL == m_GameClientView.m_hInst)
      {
        return true;
      }

      CMD_S_RequestUpdateRoomInfo_Result *pRoomInfo_Result = (CMD_S_RequestUpdateRoomInfo_Result *)pData;
      m_GameClientView.m_pClientControlDlg->UpdateRoomInfoResult(pRoomInfo_Result);

      return true;
    }
  }
  ASSERT(FALSE);
  return false;
}

//场景消息
bool CGameClientEngine::OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize)
{
  switch(cbGameStatus)
  {
    case GAME_SCENE_FREE:   //空闲状态
    {
      //效验数据
      if(wDataSize!=sizeof(CMD_S_StatusFree))
      {
        return false;
      }
      CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;

      //设置控件
      if(IsLookonMode()==false&&GetMeUserItem()->GetUserStatus()!=US_READY)
      {
        m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
        m_GameClientView.m_btStart.SetFocus();
        //设置时间
        SetGameClock(GetMeChairID(),IDI_START_GAME,30);
      }
      else
      {
        m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
      }

      if(IsLookonMode()==false)
      {
        //设置牌
        m_GameClientView.m_CardControl[4].SetPositively(true);
        m_GameClientView.m_CardControl[5].SetPositively(true);
      }

      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

        //设置更新定时器
        m_GameClientView.m_pClientControlDlg->SetUpdateIDI();

        //设置房间库存
        m_GameClientView.m_pClientControlDlg->SetRoomStorage(pStatusFree->lRoomStorageStart, pStatusFree->lRoomStorageCurrent);
      }

      return true;
    }
    case GAME_SCENE_ADD_SCORE:  //游戏状态
    {
      //效验数据
      if(wDataSize != sizeof(CMD_S_StatusAddScore))
      {
        return false;
      }
      CMD_S_StatusAddScore *pAddScoreStatus = (CMD_S_StatusAddScore *)pData;

      //设置变量
      m_lCellScore = pAddScoreStatus->lCellScore;
      m_lMaxScore = pAddScoreStatus->lMaxScore;
      m_wBankerUser = pAddScoreStatus->wBankerUser;

      SetGameStatus(GAME_SCENE_ADD_SCORE);

      //设置等待状态
      m_GameClientView.SetWaitStatus(1);

      //设置单元积分
      m_GameClientView.SetCellScore(m_lCellScore);

      m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

      for(WORD i = 0; i < GAME_PLAYER; i++)
      {
        WORD wViewId = SwitchViewChairID(i);
        //设置筹码
        m_GameClientView.m_PlayerJeton[wViewId].SetScore(pAddScoreStatus->lTableScore[i]);
      }

      //设置牌
      if(!IsLookonMode())
      {
        m_GameClientView.m_CardControl[4].SetPositively(true);
        m_GameClientView.m_CardControl[5].SetPositively(true);
      }

      IClientUserItem *pUserItem=NULL;
      tagUserInfo * pUserData=NULL;
      //帐号名字
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        pUserItem=GetTableUserItem(i);
        if(pUserItem == NULL)
        {
          continue;
        }
        pUserData=pUserItem->GetUserInfo();
        if(pUserData != NULL)
        {
          m_cbPlayStatus[i] = TRUE;
        }
        if(pUserData!=NULL)
        {
          lstrcpyn(m_szAccounts[i],pUserData->szNickName,CountArray(m_szAccounts[i]));
        }
      }

      //更新操作控件
      if(!IsLookonMode() && pAddScoreStatus->lTableScore[GetMeChairID()] == 0L && m_wBankerUser != GetMeChairID())
      {
        m_GameClientView.m_GoldControl.SetScoreControl(m_lMaxScore/m_lCellScore,1,1);
        SetGameClock(GetMeChairID(),IDI_USER_OPERATE,TIME_USER_OPERATE);
      }

      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

        //设置更新定时器
        m_GameClientView.m_pClientControlDlg->SetUpdateIDI();

        //设置房间库存
        m_GameClientView.m_pClientControlDlg->SetRoomStorage(pAddScoreStatus->lRoomStorageStart, pAddScoreStatus->lRoomStorageCurrent);
      }

      return true;
    }
    case GAME_SCENE_GET_CARD:
    {
      //效验数据
      if(wDataSize != sizeof(CMD_S_StatusGetCard))
      {
        return false;
      }
      CMD_S_StatusGetCard *pGetCardStatus = (CMD_S_StatusGetCard *)pData;

      WORD wMeChairId = GetMeChairID();

      //设置变量
      m_lCellScore = pGetCardStatus->lCellScore;
      m_wBankerUser = pGetCardStatus->wBankerUser;
      CopyMemory(m_bStopCard,pGetCardStatus->bStopCard,sizeof(pGetCardStatus->bStopCard));
      m_bInsureCard[0] = pGetCardStatus->bInsureCard[wMeChairId*2];
      m_bInsureCard[1] = pGetCardStatus->bInsureCard[wMeChairId*2+1];
      m_bDoubleCard[0] = pGetCardStatus->bDoubleCard[wMeChairId*2];
      m_bDoubleCard[1] = pGetCardStatus->bDoubleCard[wMeChairId*2+1];
      m_cbCardCount[0] = pGetCardStatus->cbCardCount[wMeChairId*2];
      m_cbCardCount[1] = pGetCardStatus->cbCardCount[wMeChairId*2+1];
      CopyMemory(m_cbHandCardData[0],pGetCardStatus->cbHandCardData[wMeChairId*2],sizeof(m_cbHandCardData[0]));
      CopyMemory(m_cbHandCardData[1],pGetCardStatus->cbHandCardData[wMeChairId*2+1],sizeof(m_cbHandCardData[0]));

      SetGameStatus(GAME_SCENE_GET_CARD);
      IClientUserItem *pUserItem = NULL ;
      tagUserInfo * pUserData = NULL;
      //帐号名字
      for(WORD i=0; i<GAME_PLAYER; i++)
      {
        pUserItem = GetTableUserItem(i);
        if(pUserItem == NULL)
        {
          continue;
        }
        pUserData=pUserItem->GetUserInfo();
        if(pUserData != NULL)
        {
          m_cbPlayStatus[i] = TRUE;
        }
        if(pUserData!=NULL)
        {
          lstrcpyn(m_szAccounts[i],pUserData->szNickName,CountArray(m_szAccounts[i]));
        }
      }

      //设置等待状态
      bool bOperCardComplete = true;
      for(WORD i = 0; i < GAME_PLAYER; i ++)
      {
        if(!m_cbPlayStatus[i] || i == m_wBankerUser)
        {
          continue;
        }
        if(!m_bStopCard[i*2] || (pGetCardStatus->cbCardCount[i*2+1]>0&&!m_bStopCard[i*2+1]))
        {
          bOperCardComplete = false;
          break;
        }
      }
      if(bOperCardComplete)
      {
        m_bTurnBanker = TRUE;
      }
      m_GameClientView.SetWaitStatus(bOperCardComplete?3:2);

      //设置单元积分
      m_GameClientView.SetCellScore(m_lCellScore);

      m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));

      //设置扑克控件
      if(!IsLookonMode())
      {
        m_GameClientView.m_CardControl[2*2].SetPositively(true);
        m_GameClientView.m_CardControl[2*2+1].SetPositively(true);
      }

      for(WORD i = 0; i < GAME_PLAYER; i++)
      {
        if(!m_cbPlayStatus[i])
        {
          continue;
        }
        WORD wViewId = SwitchViewChairID(i);
        //设置扑克
        m_GameClientView.m_CardControl[wViewId*2].SetCardData(pGetCardStatus->cbHandCardData[i*2],pGetCardStatus->cbCardCount[i*2]);
        m_GameClientView.m_CardControl[wViewId*2+1].SetCardData(pGetCardStatus->cbHandCardData[i*2+1],pGetCardStatus->cbCardCount[i*2+1]);
        //设置筹码
        m_GameClientView.m_PlayerJeton[wViewId].SetScore(pGetCardStatus->lTableScore[i]);
        //设置牌面值
        BYTE cbCardType = 0;
        if(i == wMeChairId && !IsLookonMode())
        {
          cbCardType = m_GameLogic.GetCardType(pGetCardStatus->cbHandCardData[i*2],pGetCardStatus->cbCardCount[i*2],m_cbCardCount[1]>0);
        }
        else
        {
          cbCardType = m_GameLogic.GetCardType(&pGetCardStatus->cbHandCardData[i*2][1],pGetCardStatus->cbCardCount[i*2]-1,true);
        }
        m_GameClientView.SetUserCardType(wViewId,0,cbCardType);
        if(pGetCardStatus->cbCardCount[i*2+1] > 0)
        {
          if(i == wMeChairId && !IsLookonMode())
          {
            cbCardType = m_GameLogic.GetCardType(pGetCardStatus->cbHandCardData[i*2+1],pGetCardStatus->cbCardCount[i*2+1],true);
          }
          else
          {
            cbCardType = m_GameLogic.GetCardType(&pGetCardStatus->cbHandCardData[i*2+1][1],pGetCardStatus->cbCardCount[i*2+1]-1,true);
          }
          m_GameClientView.SetUserCardType(wViewId,0,cbCardType);
        }
        //设置加倍,保险,停牌
        if(pGetCardStatus->bDoubleCard[i*2])
        {
          m_GameClientView.SetUserDoubleCard(wViewId);
        }
        if(pGetCardStatus->bInsureCard[i*2])
        {
          m_GameClientView.SetUserInsureCard(wViewId,0);
        }
        if(pGetCardStatus->bStopCard[i*2])
        {
          m_GameClientView.SetUserStopCard(wViewId);
        }
        if(pGetCardStatus->bDoubleCard[i*2+1])
        {
          m_GameClientView.SetUserDoubleCard(wViewId);
        }
        if(pGetCardStatus->bInsureCard[i*2+1])
        {
          m_GameClientView.SetUserInsureCard(wViewId,1);
        }
        if(pGetCardStatus->bStopCard[i*2+1])
        {
          m_GameClientView.SetUserStopCard(wViewId);
        }
      }

      //更新操作控件
      if(!IsLookonMode() && ((bOperCardComplete&&wMeChairId==m_wBankerUser) ||
                             (!m_bStopCard[wMeChairId*2] || (m_cbCardCount[1]>0&&!m_bStopCard[wMeChairId*2+1])))
        )
      {
        UpdateScoreControl();
        SetGameClock(GetMeChairID(),IDI_USER_OPERATE,TIME_USER_OPERATE);
      }

      if(CUserRight::IsGameCheatUser(m_pIClientKernel->GetUserAttribute()->dwUserRight) && m_GameClientView.m_hInst)
      {
        m_GameClientView.m_btOpenAdmin.ShowWindow(SW_SHOW);

        //设置更新定时器
        m_GameClientView.m_pClientControlDlg->SetUpdateIDI();

        //设置房间库存
        m_GameClientView.m_pClientControlDlg->SetRoomStorage(pGetCardStatus->lRoomStorageStart, pGetCardStatus->lRoomStorageCurrent);
      }

      return true;
    }
  }

  return false;
}

//要牌
bool CGameClientEngine::OnSubGetCard(const void *pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize != sizeof(CMD_S_GetCard))
  {
    return false;
  }
  CMD_S_GetCard *pGetCard = (CMD_S_GetCard *)pBuffer;

  //若是自己要牌
  WORD wMeChairId = GetMeChairID();
  if(wMeChairId == pGetCard->wGetCardUser)
  {
    WORD wIndex = m_bStopCard[wMeChairId*2]?1:0;
    m_cbHandCardData[wIndex][m_cbCardCount[wIndex]++] = pGetCard->cbCardData;
  }

  //发牌
  BYTE cbCardData = pGetCard->cbCardData;
  WORD wDispatchId = SwitchViewChairID(pGetCard->wGetCardUser)*2;
  if(m_bStopCard[pGetCard->wGetCardUser*2])
  {
    wDispatchId++;
  }

  BYTE cbCard[MAX_COUNT],cbCardCount;
  cbCardCount = (BYTE)m_GameClientView.m_CardControl[wDispatchId].GetCardData(cbCard,MAX_COUNT);

  m_GameClientView.DispatchUserCard(wDispatchId,cbCardData);

  //设置牌面值
  cbCard[cbCardCount++] = pGetCard->cbCardData;
  BYTE cbCardType = 0;
  if(pGetCard->wGetCardUser == wMeChairId && !IsLookonMode())
  {
    cbCardType = m_GameLogic.GetCardType(cbCard,cbCardCount,m_cbCardCount[1]>0);
  }
  else
  {
    cbCardType = m_GameLogic.GetCardType(&cbCard[1],cbCardCount-1,true);
  }
  m_GameClientView.SetUserCardType(SwitchViewChairID(pGetCard->wGetCardUser),m_bStopCard[pGetCard->wGetCardUser*2]?1:0,cbCardType);

  return true;
}

//保险消息
bool CGameClientEngine::OnSubInsureCard(const void *pBuffer, WORD wDataSize)
{
  //效验
  if(wDataSize != sizeof(CMD_S_Insure))
  {
    return false;
  }
  CMD_S_Insure *pInsureCard = (CMD_S_Insure *)pBuffer;

  //设置保险
  WORD wMeChairId = GetMeChairID();
  if(wMeChairId == pInsureCard->wInsureUser)
  {
    if(!m_bStopCard[wMeChairId*2])
    {
      m_bInsureCard[0] = TRUE;
    }
    else
    {
      m_bInsureCard[1] = TRUE;
    }
  }

  //设置筹码
  WORD wViewId = SwitchViewChairID(pInsureCard->wInsureUser);
  m_GameClientView.m_PlayerJeton[wViewId].AddScore(pInsureCard->lInsureScore);

  //播放声音
  if(IsLookonMode() || GetMeChairID() != pInsureCard->wInsureUser)
  {
    PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
  }

  //设置界面
  m_GameClientView.SetUserInsureCard(wViewId,m_bStopCard[pInsureCard->wInsureUser*2]?1:0);

  //更新操作控件
  if(GetMeChairID() == pInsureCard->wInsureUser && !IsLookonMode())
  {
    UpdateScoreControl();
    //设置定时器
    SetGameClock(GetMeChairID(),IDI_USER_OPERATE,TIME_USER_OPERATE);
  }

  //更新界面
  m_GameClientView.InvalidGameView(0,0,0,0);

  return true;
}

//分牌消息
bool CGameClientEngine::OnSubSplitCard(const void *pBuffer, WORD wDataSize)
{
  //效验
  if(wDataSize != sizeof(CMD_S_SplitCard))
  {
    return false;
  }
  CMD_S_SplitCard *pSplitCard = (CMD_S_SplitCard *)pBuffer;

  //设置筹码
  WORD wViewId = SwitchViewChairID(pSplitCard->wSplitUser);
  m_GameClientView.m_PlayerJeton[wViewId].AddScore(pSplitCard->lAddScore);

  //播放声音
  if(IsLookonMode() || GetMeChairID() != pSplitCard->wSplitUser)
  {
    PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
  }

  //若是自己分牌
  WORD wMeChairId = GetMeChairID();
  if(pSplitCard->wSplitUser == wMeChairId)
  {
    ASSERT(m_cbCardCount[0] == 2 && m_cbCardCount[1] == 0);
    m_cbHandCardData[1][1] = m_cbHandCardData[0][1];
    m_cbHandCardData[0][0] = pSplitCard->cbCardData[0];
    m_cbHandCardData[1][0] = pSplitCard->cbCardData[1];
    m_cbCardCount[0] = m_cbCardCount[1] = 2;
  }

  //设置正在分牌
  m_bSpliting = TRUE;
  m_wSplitingUser = pSplitCard->wSplitUser;

  //设置扑克
  BYTE cbCardData[MAX_COUNT],cbCardCount;
  cbCardCount = (BYTE)m_GameClientView.m_CardControl[wViewId*2].GetCardData(cbCardData,MAX_COUNT);
  m_GameClientView.m_CardControl[wViewId*2].SetCardData(&cbCardData[1],1);
  m_GameClientView.m_CardControl[wViewId*2+1].SetCardData(&cbCardData[1],1);
  m_GameClientView.m_CardControl[wViewId*2].SetDisplayHead(true);
  m_GameClientView.m_CardControl[wViewId*2+1].SetDisplayHead(true);
  if(wMeChairId == pSplitCard->wSplitUser)
  {
    m_GameClientView.m_CardControl[wViewId*2].SetPositively(false);
    m_GameClientView.m_CardControl[wViewId*2+1].SetPositively(false);
  }
  //发牌
  m_GameClientView.DispatchUserCard(wViewId*2,cbCardData[0]);
  m_GameClientView.DispatchUserCard(wViewId*2+1,cbCardData[0]);

  //设置牌面值
  if(wViewId == 2 && !IsLookonMode())
  {
    BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[0],2,true);
    m_GameClientView.SetUserCardType(wViewId,0,cbCardType);
    cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[1],2,true);
    m_GameClientView.SetUserCardType(wViewId,1,cbCardType);
  }
  else
  {
    BYTE cbCardType = m_GameLogic.GetCardType(&cbCardData[1],1,true);
    m_GameClientView.SetUserCardType(wViewId,1,cbCardType);
  }

  //如果已下保险,则第二手牌也跟着保险
  if(pSplitCard->bInsured)
  {
    m_GameClientView.SetUserInsureCard(wViewId,1);
  }

  //更新界面
  m_GameClientView.InvalidGameView(0,0,0,0);

  return true;
}

//加倍消息
bool CGameClientEngine::OnSubDoubleCard(const void *pBuffer, WORD wDataSize)
{
  //效验
  if(wDataSize != sizeof(CMD_S_DoubleScore))
  {
    return false;
  }
  CMD_S_DoubleScore *pDoubleScore = (CMD_S_DoubleScore *)pBuffer;

  //设置筹码
  WORD wViewId = SwitchViewChairID(pDoubleScore->wDoubleScoreUser);
  m_GameClientView.m_PlayerJeton[wViewId].AddScore(pDoubleScore->lAddScore);

  //播放声音
  if(IsLookonMode() || GetMeChairID() != pDoubleScore->wDoubleScoreUser)
  {
    PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
  }

  //若是自己加倍
  WORD wMeChairId = GetMeChairID();
  if(wMeChairId == pDoubleScore->wDoubleScoreUser)
  {
    WORD wIndex = m_bStopCard[wMeChairId*2]?1:0;
    m_cbHandCardData[wIndex][m_cbCardCount[wIndex]++] = pDoubleScore->cbCardData;
  }

  //设置加倍
  if(wMeChairId == pDoubleScore->wDoubleScoreUser)
  {
    if(!m_bDoubleCard[0])
    {
      m_bDoubleCard[0] = TRUE;
    }
    else
    {
      m_bDoubleCard[1] = TRUE;
    }
  }
  m_GameClientView.SetUserDoubleCard(wViewId);

  //发送扑克
  WORD wOperIndex = 0;
  if(m_bStopCard[pDoubleScore->wDoubleScoreUser*2])
  {
    wOperIndex++;
  }
  ASSERT(!m_bStopCard[pDoubleScore->wDoubleScoreUser*2+wOperIndex]);
  BYTE cbCardData[MAX_COUNT],cbCardCount;
  cbCardCount = (BYTE)m_GameClientView.m_CardControl[wViewId*2+wOperIndex].GetCardData(cbCardData,MAX_COUNT);
  cbCardData[cbCardCount++] = pDoubleScore->cbCardData;
  m_GameClientView.DispatchUserCard(wViewId*2+wOperIndex,cbCardData[cbCardCount-1]);

  //设置牌面值
  BYTE cbCardType = 0;
  if(wViewId == 2 && !IsLookonMode())
  {
    cbCardType = m_GameLogic.GetCardType(cbCardData,cbCardCount,wOperIndex>0);
  }
  else
  {
    cbCardType = m_GameLogic.GetCardType(&cbCardData[1],cbCardCount-1,true);
  }
  m_GameClientView.SetUserCardType(wViewId,wOperIndex,cbCardType);

  //更新界面
  m_GameClientView.InvalidGameView(0,0,0,0);

  return true;
}

//加注消息
bool CGameClientEngine::OnSubAddScore(const void *pBuffer, WORD wDataSize)
{
  //效验
  if(wDataSize != sizeof(CMD_S_AddScore))
  {
    return false;
  }
  CMD_S_AddScore *pAddScore = (CMD_S_AddScore *)pBuffer;

  //设置筹码
  WORD wViewId = SwitchViewChairID(pAddScore->wAddScoreUser);
  m_GameClientView.m_PlayerJeton[wViewId].SetScore(pAddScore->lAddScore);

  //播放声音
  if(IsLookonMode() || GetMeChairID() != pAddScore->wAddScoreUser)
  {
    PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));
  }

  m_GameClientView.InvalidGameView(0,0,0,0);

  return true;
}

//停牌消息
bool CGameClientEngine::OnSubStopCard(const void *pBuffer, WORD wDataSize)
{
  //效验
  if(wDataSize != sizeof(CMD_S_StopCard))
  {
    return false;
  }
  CMD_S_StopCard *pStopCard = (CMD_S_StopCard *)pBuffer;

  //隐藏操作控件
  if(pStopCard->wStopCardUser == GetMeChairID())
  {
    HideScoreControl();
  }

  //设置停牌变量
  WORD wOperIndex = pStopCard->wStopCardUser*2;
  if(m_bStopCard[wOperIndex])
  {
    m_bStopCard[++wOperIndex] = TRUE;
  }
  else
  {
    m_bStopCard[wOperIndex] = TRUE;
  }

  //设置界面
  WORD wViewId = SwitchViewChairID(pStopCard->wStopCardUser);
  m_GameClientView.SetUserStopCard(wViewId);

  //是否手上牌已全停
  WORD wMeChairId = GetMeChairID();
  if(!IsLookonMode() && wMeChairId == pStopCard->wStopCardUser)
  {
    if((wOperIndex%2 == 0) && m_cbCardCount[1] > 0)
    {
      //更新操作控件
      UpdateScoreControl();
      //设置定时器
      SetGameClock(wMeChairId,IDI_USER_OPERATE,TIME_USER_OPERATE);
    }
  }

  //是否轮到庄家操作
  if(pStopCard->bTurnBanker)
  {
    m_bTurnBanker = TRUE;
    m_GameClientView.SetWaitStatus(3);
    if(wMeChairId == m_wBankerUser)
    {
      ActiveGameFrame();
      UpdateScoreControl();
      SetGameClock(wMeChairId,IDI_USER_OPERATE,TIME_USER_OPERATE);
    }
  }

  //更新界面
  m_GameClientView.InvalidGameView(0,0,0,0);

  return true;
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

  //设置状态
  SetGameStatus(GAME_SCENE_ADD_SCORE);

  //下注变量
  m_lMaxScore=pGameStart->lMaxScore;
  m_lCellScore=pGameStart->lCellScore;
  m_wBankerUser = pGameStart->wBankerUser;

  //设置变量
  LONGLONG lTableScore=0L;
  IClientUserItem *pUserItem=NULL;
  tagUserInfo * pUserData =NULL;
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //获取用户
    pUserItem=GetTableUserItem(i);
    if(pUserItem == NULL)
    {
      continue;
    }
    pUserData = pUserItem->GetUserInfo();
    if(pUserData==NULL)
    {
      continue;
    }

    //游戏信息
    m_cbPlayStatus[i]=TRUE;

    //用户名字
    lstrcpyn(m_szAccounts[i],pUserData->szNickName,CountArray(m_szAccounts[i]));
  }

  //调整最大下注
  if(!IsLookonMode() && GetMeChairID() != m_wBankerUser)
  {
    ASSERT(GetMeUserItem()!= NULL);
    const tagUserInfo *pUserData = GetMeUserItem()->GetUserInfo();
    ASSERT(pUserData != NULL);
    m_lMaxScore = __min(m_lMaxScore,pUserData->lScore/3L);
  }

  //设置界面
  m_GameClientView.SetWaitStatus(1);
  m_GameClientView.SetBankerUser(SwitchViewChairID(m_wBankerUser));
  m_GameClientView.SetCellScore(m_lCellScore);
  if(IsLookonMode())
  {
    m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);
    m_GameClientView.SetUserStopCard(INVALID_CHAIR);
    m_GameClientView.SetUserDoubleCard(INVALID_CHAIR);
    m_GameClientView.SetUserInsureCard(INVALID_CHAIR,0);
    for(WORD i = 0; i < GAME_PLAYER; i++)
    {
      //设置扑克
      m_GameClientView.m_CardControl[i*2].SetDisplayHead(false);
      m_GameClientView.m_CardControl[i*2+1].SetDisplayHead(false);
      m_GameClientView.m_CardControl[i*2].SetCardData(NULL,0);
      m_GameClientView.m_CardControl[i*2+1].SetCardData(NULL,0);
    }
  }

  //环境设置
  PlayGameSound(AfxGetInstanceHandle(),TEXT("GAME_START"));

  //激活框架
  if(IsLookonMode()==false&&m_wBankerUser!=GetMeChairID())
  {
    ActiveGameFrame();
    m_GameClientView.m_GoldControl.SetScoreControl(m_lMaxScore/m_lCellScore,1,1);
    SetGameClock(GetMeChairID(),IDI_USER_OPERATE,TIME_USER_OPERATE);
  }

  return true;
}

//发牌消息
bool CGameClientEngine::OnSubSendCard(const void * pBuffer, WORD wDataSize)
{
  //效验数据
  if(wDataSize!=sizeof(CMD_S_SendCard))
  {
    return false;
  }
  CMD_S_SendCard * pSendCard=(CMD_S_SendCard *)pBuffer;

  //设置游戏状态
  SetGameStatus(GAME_SCENE_ADD_SCORE);

  //设置变量
  WORD wMeChairId = GetMeChairID();
  m_cbCardCount[0] = 2;
  CopyMemory(m_cbHandCardData[0],pSendCard->cbHandCardData[wMeChairId],sizeof(BYTE)*2);

  //设置界面
  m_GameClientView.SetWaitStatus(2);

  //发牌
  for(BYTE j = 0; j < CountArray(pSendCard->cbHandCardData[0]); j++)
  {
    WORD wId = m_wBankerUser;
    for(BYTE i = 0; i < GAME_PLAYER; i++)
    {
      wId = (wId+1)%GAME_PLAYER;
      if(!m_cbPlayStatus[wId])
      {
        continue;
      }
      m_GameClientView.DispatchUserCard(SwitchViewChairID(wId)*2,pSendCard->cbHandCardData[wId][j]);
    }
  }

  //设置牌面值
  for(WORD i = 0; i < GAME_PLAYER; i++)
  {
    if(!m_cbPlayStatus[i])
    {
      continue;
    }
    BYTE cbCardType = 0;
    if(i == GetMeChairID() && !IsLookonMode())
    {
      cbCardType = m_GameLogic.GetCardType(pSendCard->cbHandCardData[i],2,false);
    }
    else
    {
      cbCardType = m_GameLogic.GetCardType(&pSendCard->cbHandCardData[i][1],1,false);
    }
    m_GameClientView.SetUserCardType(SwitchViewChairID(i),0,cbCardType);
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

  //状态设置
  HideScoreControl();
  SetGameStatus(GAME_SCENE_FREE);
  KillGameClock(IDI_USER_OPERATE);

  //播放声音
  if(IsLookonMode()==false)
  {
    if(pGameEnd->lGameScore[GetMeChairID()]>0L)
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

  //正常结束
  m_GameClientView.m_ScoreView.ResetScore();
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //设置筹码
    m_GameClientView.m_PlayerJeton[i].SetScore(0L);
    //设置信息
    if(pGameEnd->lGameScore[i]!=0L||m_szAccounts[i][0]!=0)
    {
      if(m_szAccounts[i][0]==0)
      {
        m_GameClientView.m_ScoreView.SetGameScore(i,TEXT("已离开"),pGameEnd->lGameScore[i]);
      }
      else
      {
        m_GameClientView.m_ScoreView.SetGameScore(i,m_szAccounts[i],pGameEnd->lGameScore[i]);
      }
      //扣税
      m_GameClientView.m_ScoreView.SetGameTax(i,pGameEnd->lGameTax[i]);
      if(pGameEnd->cbCardData[i*2]!=0)
      {
        //获取扑克
        BYTE cbCardData[MAX_COUNT];
        WORD wViewChairID=SwitchViewChairID(i);
        BYTE cbCardCount=(BYTE)m_GameClientView.m_CardControl[wViewChairID*2].GetCardData(cbCardData,CountArray(cbCardData));
        //设置扑克
        cbCardData[0]=pGameEnd->cbCardData[i*2];
        m_GameClientView.m_CardControl[wViewChairID*2].SetDisplayHead(true);
        m_GameClientView.m_CardControl[wViewChairID*2].SetCardData(cbCardData,cbCardCount);
        BYTE cbCardType = m_GameLogic.GetCardType(cbCardData,cbCardCount,
                          m_GameClientView.m_CardControl[wViewChairID*2+1].GetCardCount()>0);
        m_GameClientView.SetUserCardType(wViewChairID,0,cbCardType);

        cbCardCount = (BYTE)m_GameClientView.m_CardControl[wViewChairID*2+1].GetCardData(cbCardData,CountArray(cbCardData));
        if(cbCardCount > 0)
        {
          //设置扑克
          cbCardData[0]=pGameEnd->cbCardData[i*2+1];
          m_GameClientView.m_CardControl[wViewChairID*2+1].SetDisplayHead(true);
          m_GameClientView.m_CardControl[wViewChairID*2+1].SetCardData(cbCardData,cbCardCount);
          BYTE cbCardType = m_GameLogic.GetCardType(cbCardData,cbCardCount,true);
          m_GameClientView.SetUserCardType(wViewChairID,1,cbCardType);
        }
      }
    }
  }
  m_GameClientView.m_ScoreView.ShowWindow(SW_SHOW);

  //设置积分

  //成绩显示在即时聊天对话框
  TCHAR szBuffer[256]=TEXT("");
  _sntprintf(szBuffer,CountArray(szBuffer),TEXT("本局结束,成绩统计:"));
  m_pIStringMessage->InsertNormalString(szBuffer);
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    if(pGameEnd->lGameScore[i]==0)
    {
      continue;
    }

    //成绩输出
    //IClientUserItem * pUserData=GetTableUserItem(i);
    _sntprintf(szBuffer,CountArray(szBuffer),TEXT("\n%s：%+I64d"),m_szAccounts[i],pGameEnd->lGameScore[i]);
    m_pIStringMessage->InsertNormalString(szBuffer);
  }
  m_pIStringMessage->InsertNormalString(TEXT("\n"));

  //庄家
  m_wBankerUser = INVALID_CHAIR;

  ZeroMemory(m_bStopCard,sizeof(m_bStopCard));
  ZeroMemory(m_bInsureCard,sizeof(m_bInsureCard));
  ZeroMemory(m_bDoubleCard,sizeof(m_bDoubleCard));
  ZeroMemory(m_cbCardCount,sizeof(m_cbCardCount));
  ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

  m_bTurnBanker = FALSE;
  m_bSpliting = FALSE;
  m_wSplitingUser = INVALID_CHAIR;

  //加注信息
  m_lMaxScore=0L;
  m_lCellScore=0L;

  //状态变量
  ZeroMemory(m_szAccounts,sizeof(m_szAccounts));
  ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));

  //开始按钮
  if(IsLookonMode()==false)
  {
    m_GameClientView.m_btStart.ShowWindow(SW_SHOW);
    SetGameClock(GetMeChairID(),IDI_START_GAME,TIME_START_GAME);
  }

  return true;
}

bool CGameClientEngine::OnSubGMCard(const void * pData, WORD wDataSize)
{
  //效验数据
  ASSERT(wDataSize==sizeof(CMD_S_CheatCard));
  if(wDataSize!=sizeof(CMD_S_CheatCard))
  {
    return false;
  }

  //变量定义
  CMD_S_CheatCard * pCheatCard=(CMD_S_CheatCard *)pData;

  for(WORD i = 0; i < GAME_PLAYER ; i++)
  {
    //显示扑克
    if(i==GetMeChairID() || m_cbPlayStatus[i] == FALSE)
    {
      continue;
    }

    //设置扑克
    m_GameClientView.m_CardControl[SwitchViewChairID(i)*2].SetCardData(pCheatCard->cbCardData[i*2],2);
    m_GameClientView.m_CardControl[SwitchViewChairID(i)*2].SetDisplayHead(true);
  }

  for(WORD j = 0; j < GAME_PLAYER ; j++)
  {
    if(i==GetMeChairID() || m_cbPlayStatus[i] == FALSE)
    {
      continue;
    }
    if(pCheatCard->cbCardData[j*2+1][0]!=0)
    {
      m_GameClientView.m_CardControl[SwitchViewChairID(j)*2+1].SetCardData(pCheatCard->cbCardData[j*2+1],2);
      m_GameClientView.m_CardControl[SwitchViewChairID(j)*2+1].SetDisplayHead(true);
    }
  }

  m_GameClientView.InvalidGameView(0,0,0,0);
  return true;
}

//隐藏控制
void CGameClientEngine::HideScoreControl()
{
  m_GameClientView.m_GoldControl.ShowWindow(SW_HIDE);
  //控制按钮
  m_GameClientView.m_btGetCard.ShowWindow(SW_HIDE);
  m_GameClientView.m_btStopCard.ShowWindow(SW_HIDE);
  m_GameClientView.m_btDoubleCard.ShowWindow(SW_HIDE);
  m_GameClientView.m_btSplitCard.ShowWindow(SW_HIDE);
  m_GameClientView.m_btInsureCard.ShowWindow(SW_HIDE);
  return;
}

//更新控制
BOOL CGameClientEngine::UpdateScoreControl()
{
  //效验
  if(IsLookonMode())
  {
    return FALSE;
  }

  WORD wMeChairId = GetMeChairID();
  if((!m_bTurnBanker && wMeChairId==m_wBankerUser) || (m_bTurnBanker && wMeChairId!=m_wBankerUser))
  {
    return FALSE;
  }

  if(m_bStopCard[wMeChairId*2] && m_bStopCard[wMeChairId*2+1])
  {
    return FALSE;
  }

  if(m_bDoubleCard[0] && (m_cbCardCount[1]==0||m_bDoubleCard[1]))
  {
    return FALSE;
  }

  WORD wIndex = m_bStopCard[wMeChairId*2]?1:0;
  if(m_cbCardCount[wIndex] == 0)
  {
    return FALSE;
  }
  BYTE cbCardType = m_GameLogic.GetCardType(&m_cbHandCardData[wIndex][1],m_cbCardCount[wIndex]-1,true);
  if(cbCardType == CT_BAOPAI || cbCardType == CT_BJ-1)
  {
    m_GameClientView.m_btStopCard.ShowWindow(SW_HIDE);
    return FALSE;
  }
  if(m_GameClientView.m_btGetCard.IsWindowVisible())
  {
    return FALSE;
  }

  m_GameClientView.m_btStopCard.ShowWindow(SW_SHOW);
  m_GameClientView.m_btStopCard.EnableWindow();

  cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[wIndex],m_cbCardCount[wIndex],false);
  if(cbCardType > CT_BAOPAI && cbCardType < CT_BJ-1 || cbCardType > CT_BJ)
  {
    m_GameClientView.m_btGetCard.ShowWindow(SW_SHOW);
    m_GameClientView.m_btGetCard.EnableWindow();
  }

  if(wMeChairId != m_wBankerUser)
  {
    if(m_cbCardCount[wIndex]==2)
    {
      m_GameClientView.m_btDoubleCard.ShowWindow(SW_SHOW);
      m_GameClientView.m_btDoubleCard.EnableWindow();
    }
    if(m_cbCardCount[1] == 0 && m_cbCardCount[0] == 2 &&
       m_GameLogic.GetCardValue(m_cbHandCardData[0][1]) == m_GameLogic.GetCardValue(m_cbHandCardData[0][0]))
    {
      m_GameClientView.m_btSplitCard.ShowWindow(SW_SHOW);
      m_GameClientView.m_btSplitCard.EnableWindow();
    }
    if(m_cbCardCount[wIndex]==2 && !m_bInsureCard[wIndex])
    {
      BYTE cbCardData[MAX_COUNT],cbCardCount;
      cbCardCount = (BYTE)m_GameClientView.m_CardControl[SwitchViewChairID(m_wBankerUser)*2].GetCardData(cbCardData,MAX_COUNT);
      if(cbCardCount == 2 && m_GameLogic.GetCardValue(cbCardData[1])==1)
      {
        m_GameClientView.m_btInsureCard.ShowWindow(SW_SHOW);
        m_GameClientView.m_btInsureCard.EnableWindow(TRUE);
      }
    }
  }

  return TRUE;
}

//开始按钮
LRESULT CGameClientEngine::OnStart(WPARAM wParam, LPARAM lParam)
{
  //删除定时器
  KillGameClock(IDI_START_GAME);

  //设置界面
  m_GameClientView.m_btStart.ShowWindow(SW_HIDE);
  m_GameClientView.m_ScoreView.ShowWindow(SW_HIDE);

  //设置界面
  m_GameClientView.SetWaitStatus(0);
  m_GameClientView.SetBankerUser(INVALID_CHAIR);
  m_GameClientView.SetUserStopCard(INVALID_CHAIR);
  m_GameClientView.SetUserDoubleCard(INVALID_CHAIR);
  m_GameClientView.SetUserInsureCard(INVALID_CHAIR,0);
  for(WORD i = 0; i < GAME_PLAYER; i++)
  {
    //设置扑克
    m_GameClientView.m_CardControl[i*2].SetDisplayHead(false);
    m_GameClientView.m_CardControl[i*2+1].SetDisplayHead(false);
    m_GameClientView.m_CardControl[i*2].SetCardData(NULL,0);
    m_GameClientView.m_CardControl[i*2+1].SetCardData(NULL,0);
  }

  //发送消息
  SendUserReady(NULL,0);

  return 0;
}

//停牌按钮
LRESULT CGameClientEngine::OnStopCard(WPARAM wParam, LPARAM lParam)
{
  //隐藏控件
  HideScoreControl();

  //删除定时器
  KillGameClock(IDI_USER_OPERATE);

  //发送数据
  SendSocketData(SUB_C_STOP_CARD);

  return 0;
}

//加倍按钮
LRESULT CGameClientEngine::OnDoubleCard(WPARAM wParam, LPARAM lParam)
{
  //隐藏控件
  HideScoreControl();

  //删除定时器
  KillGameClock(IDI_USER_OPERATE);

  //发送数据
  SendSocketData(SUB_C_DOUBLE_SCORE);

  //播放声音
  PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

  return 0;
}

//保险按钮
LRESULT CGameClientEngine::OnInsureCard(WPARAM wParam, LPARAM lParam)
{
  //隐藏控件
  HideScoreControl();

  //删除定时器
  KillGameClock(IDI_USER_OPERATE);

  //发送数据
  SendSocketData(SUB_C_INSURE);

  //播放声音
  PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

  return 0;
}

//分牌按钮
LRESULT CGameClientEngine::OnSplitCard(WPARAM wParam, LPARAM lParam)
{
  //隐藏控件
  HideScoreControl();

  //删除定时器
  KillGameClock(IDI_USER_OPERATE);

  //发送数据
  SendSocketData(SUB_C_SPLIT_CARD);

  //播放声音
  PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

  return 0;
}

//要牌按钮
LRESULT CGameClientEngine::OnGetCard(WPARAM wParam, LPARAM lParam)
{
  //隐藏控件
  HideScoreControl();

  //删除定时器
  KillGameClock(IDI_USER_OPERATE);

  //发送数据
  SendSocketData(SUB_C_GET_CARD);

  return 0;
}

//下注按钮
LRESULT CGameClientEngine::OnAddScore(WPARAM wParam, LPARAM lParam)
{
  //隐藏控件
  HideScoreControl();

  //删除定时器
  KillGameClock(IDI_USER_OPERATE);

  LONGLONG pTempValue = (LONGLONG)lParam;
  LONGLONG lValue = pTempValue;

  //发送数据
  CMD_C_AddScore AddScore;
  AddScore.lScore = lValue*m_lCellScore;
  SendSocketData(SUB_C_ADD_SCORE,&AddScore,sizeof(AddScore));

  //播放声音
  PlayGameSound(AfxGetInstanceHandle(),TEXT("ADD_SCORE"));

  return 0;
}

//发牌完成
LRESULT CGameClientEngine::OnSendCardFinish(WPARAM wParam, LPARAM lParam)
{
  //分牌处理
  if(m_bSpliting)
  {
    m_bSpliting = FALSE;
    WORD wViewId = SwitchViewChairID(m_wSplitingUser)*2;
    m_GameClientView.m_CardControl[wViewId].SetDisplayHead(false);
    m_GameClientView.m_CardControl[wViewId+1].SetDisplayHead(false);
    if(m_wSplitingUser == GetMeChairID())
    {
      m_GameClientView.m_CardControl[wViewId].SetCardData(m_cbHandCardData[0],m_cbCardCount[0]);
      m_GameClientView.m_CardControl[wViewId+1].SetCardData(m_cbHandCardData[1],m_cbCardCount[1]);
      if(!IsLookonMode())
      {
        m_GameClientView.m_CardControl[wViewId].SetPositively(true);
        m_GameClientView.m_CardControl[wViewId+1].SetPositively(true);
      }
    }
    else
    {
      BYTE cbCardData[2];
      m_GameClientView.m_CardControl[wViewId].GetCardData(cbCardData,2);
      cbCardData[1] = cbCardData[0];
      cbCardData[0] = 0;
      m_GameClientView.m_CardControl[wViewId].SetCardData(cbCardData,2);
      m_GameClientView.m_CardControl[wViewId+1].SetCardData(cbCardData,2);
    }
    m_wSplitingUser = INVALID_CHAIR;
  }

  //操作状态
  if(IsLookonMode()==false)
  {
    //界面设置
    ActiveGameFrame();

    WORD wMeChairId = GetMeChairID();
    //庄家一旦爆牌或BJ，21点，则自动停牌
    if(wMeChairId == m_wBankerUser && m_bTurnBanker)
    {
      BYTE cbCardType = m_GameLogic.GetCardType(m_cbHandCardData[0],m_cbCardCount[0],false);
      if(cbCardType == CT_BAOPAI || cbCardType == CT_BJ || cbCardType == CT_BJ-1)
      {
        OnStopCard(0,0);
        return 0;
      }
    }

    //更新控制，设置时间
    if(UpdateScoreControl())
    {
      SetGameClock(wMeChairId,IDI_USER_OPERATE,TIME_USER_OPERATE);
    }
    ////设置时间
    //if( (!m_bDoubleCard[0] || (m_cbCardCount[1]>0&&!m_bDoubleCard[1])) && m_GameClientView.m_btStopCard.IsWindowVisible() &&
    //  ( (m_bTurnBanker&&wMeChairId==m_wBankerUser) || (!m_bTurnBanker&&wMeChairId!=m_wBankerUser) ) )
    //  SetGameTimer(wMeChairId,IDI_USER_OPERATE,TIME_USER_OPERATE);
  }

  return 0;
}

LRESULT CGameClientEngine::OnStorage(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_STORAGE, (CMD_C_UpdateStorage*)wParam, sizeof(CMD_C_UpdateStorage));
  return true;
}

LRESULT CGameClientEngine::OnStorageInfo(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_STORAGEMAXMUL, (CMD_C_ModifyStorage*)wParam, sizeof(CMD_C_ModifyStorage));
  return true;
}

LRESULT CGameClientEngine::OnRequestQueryUser(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_REQUEST_QUERY_USER, (CMD_C_RequestQuery_User*)wParam, sizeof(CMD_C_RequestQuery_User));
  return true;
}

//用户控制
LRESULT CGameClientEngine::OnUserControl(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_USER_CONTROL, (CMD_C_UserControl*)wParam, sizeof(CMD_C_UserControl));
  return true;
}

//请求更新房间信息
LRESULT CGameClientEngine::OnRequestUpdateRoomInfo(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_REQUEST_UDPATE_ROOMINFO);
  return true;
}

//清空当前查询用户
LRESULT CGameClientEngine::OnClearCurrentQueryUser(WPARAM wParam, LPARAM lParam)
{
  SendSocketData(SUB_C_CLEAR_CURRENT_QUERYUSER);
  return true;
}

//////////////////////////////////////////////////////////////////////////////////
