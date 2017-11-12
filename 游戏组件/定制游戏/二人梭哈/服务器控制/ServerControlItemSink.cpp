#include "StdAfx.h"
#include "servercontrolitemsink.h"

//
CServerControlItemSink::CServerControlItemSink(void)
{
  //控制变量
  m_cbWinSideControl=0;
  m_nSendCardCount=0;
  m_cbExcuteTimes = 0;
}

CServerControlItemSink::~CServerControlItemSink(void)
{

}

//服务器控制
bool __cdecl CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame)
{
  //如果不具有管理员权限 则返回错误
  if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
  {
    return false;
  }

  const CMD_C_AdminReq* AdminReq=static_cast<const CMD_C_AdminReq*>(pDataBuffer);
  switch(AdminReq->cbReqType)
  {
    case RQ_RESET_CONTROL:
    {
      m_cbWinSideControl=0;
      m_cbExcuteTimes=0;
      CMD_S_CommandResult cResult;
      cResult.cbAckType=ACK_RESET_CONTROL;
      cResult.cbResult=CR_ACCEPT;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
      break;
    }
    case RQ_SET_WIN_AREA:
    {
      if(AdminReq->cbExtendData[0]<4)
      {
        m_cbWinSideControl=AdminReq->cbExtendData[0];
        m_cbExcuteTimes=AdminReq->cbExcuteTimes;
        CMD_S_CommandResult cResult;
        cResult.cbAckType=ACK_SET_WIN_AREA;
        cResult.cbResult=CR_ACCEPT;
        pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
      }
      else
      {
        CMD_S_CommandResult cResult;
        cResult.cbAckType=ACK_SET_WIN_AREA;
        cResult.cbResult=CR_REFUSAL;
        pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));

      }
      break;
    }
    case RQ_PRINT_SYN:
    {
      CMD_S_CommandResult cResult;
      cResult.cbAckType=ACK_PRINT_SYN;
      cResult.cbResult=CR_ACCEPT;
      cResult.cbExtendData[0]=m_cbWinSideControl;
      cResult.cbExtendData[1]=m_cbExcuteTimes;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_AMDIN_COMMAND,&cResult,sizeof(CMD_S_CommandResult));
      break;
    }

  }

  return true;
}

//需要控制
bool __cdecl CServerControlItemSink::NeedControl()
{
  if(m_cbWinSideControl <4 && m_cbExcuteTimes > 0)
  {
    return true;
  }
  return false;

}

//满足控制
bool __cdecl CServerControlItemSink::MeetControl(tagControlInfo ControlInfo)
{
  return true;
}

//完成控制
bool __cdecl CServerControlItemSink::CompleteControl()
{
  if(m_cbExcuteTimes > 0)
  {
    m_cbExcuteTimes--;
  }

  if(m_cbExcuteTimes == 0)
  {
    m_cbExcuteTimes = 0;
    m_cbWinSideControl=0;
    m_nSendCardCount=0;
  }

  return true;
}

//返回控制区域
bool __cdecl CServerControlItemSink::ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT])
{
  BYTE bCardData[MAX_COUNT];
  BYTE bHandCardData[GAME_PLAYER][MAX_COUNT];
  CopyMemory(bCardData,cbControlCardData[0],sizeof(bCardData));

  for(BYTE i=0; i<GAME_PLAYER; i++)
  {
    CopyMemory(bHandCardData[i],cbControlCardData[i],sizeof(BYTE)*MAX_COUNT);
  }

  //最大牌型
  BYTE bMaxUser=0;
  for(BYTE i=1; i<GAME_PLAYER; i++)
  {
    if(cbControlCardData[i][0] == 0)
    {
      continue;
    }

    if(m_GameLogic.CompareCard(bCardData,cbControlCardData[i],MAX_COUNT) == false)
    {
      CopyMemory(bCardData,cbControlCardData[i],sizeof(bCardData));
      bMaxUser=i;
    }
  }

  CopyMemory(cbControlCardData[m_cbWinSideControl],bCardData,sizeof(bCardData));

  if(m_cbWinSideControl != bMaxUser)
  {
    CopyMemory(cbControlCardData[bMaxUser],bHandCardData[m_cbWinSideControl],sizeof(bHandCardData[i]));
  }

  return true;
}

//服务器控制
bool __cdecl CServerControlItemSink::ShowCheatCard(BYTE cbHandCardData[GAME_PLAYER][5], ITableFrame * pITableFrame)
{
  CString strInfo = TEXT("\n");
  for(int i = 0; i < GAME_PLAYER; ++i)
  {
    IServerUserItem* pTableUserItem = pITableFrame->GetTableUserItem(i);
    if(pTableUserItem == NULL)
    {
      continue;
    }

    strInfo += pTableUserItem->GetNickName();
    strInfo += TEXT("\n");

    for(int j = 0; j < 5; ++j)
    {
      strInfo += GetCradInfo(cbHandCardData[i][j]);
    }
    strInfo += TEXT("\n");
  }

  for(int i = 0; i < GAME_PLAYER; ++i)
  {
    IServerUserItem*  pTableUserItem = pITableFrame->GetTableUserItem(i);
    if(pTableUserItem == NULL)
    {
      continue;
    }

    if(CUserRight::IsGameCheatUser(pTableUserItem->GetUserRight()))
    {
      pITableFrame->SendGameMessage(pTableUserItem, strInfo, SMT_CHAT);
    }
  }

  int nLookonCount = 0;
  IServerUserItem* pLookonUserItem = pITableFrame->EnumLookonUserItem(nLookonCount);
  while(pLookonUserItem)
  {
    if(CUserRight::IsGameCheatUser(pLookonUserItem->GetUserRight()))
    {
      pITableFrame->SendGameMessage(pLookonUserItem, strInfo, SMT_CHAT);
    }

    nLookonCount++;
    pLookonUserItem = pITableFrame->EnumLookonUserItem(nLookonCount);
  }

  return true;
}

//获取牌信息
CString CServerControlItemSink::GetCradInfo(BYTE cbCardData)
{
  CString strInfo;
  if((cbCardData&LOGIC_MASK_COLOR) == 0x00)
  {
    strInfo += TEXT("[方块 ");
  }
  else if((cbCardData&LOGIC_MASK_COLOR) == 0x10)
  {
    strInfo += TEXT("[梅花 ");
  }
  else if((cbCardData&LOGIC_MASK_COLOR) == 0x20)
  {
    strInfo += TEXT("[红桃 ");
  }
  else if((cbCardData&LOGIC_MASK_COLOR) == 0x30)
  {
    strInfo += TEXT("[黑桃 ");
  }

  if((cbCardData&LOGIC_MASK_VALUE) == 0x01)
  {
    strInfo += TEXT("A] ");
  }
  else if((cbCardData&LOGIC_MASK_VALUE) == 0x08)
  {
    strInfo += TEXT("8] ");
  }
  else if((cbCardData&LOGIC_MASK_VALUE) == 0x09)
  {
    strInfo += TEXT("9] ");
  }
  else if((cbCardData&LOGIC_MASK_VALUE) == 0x0A)
  {
    strInfo += TEXT("10] ");
  }
  else if((cbCardData&LOGIC_MASK_VALUE) == 0x0B)
  {
    strInfo += TEXT("J] ");
  }
  else if((cbCardData&LOGIC_MASK_VALUE) == 0x0C)
  {
    strInfo += TEXT("Q] ");
  }
  else if((cbCardData&LOGIC_MASK_VALUE) == 0x0D)
  {
    strInfo += TEXT("K] ");
  }

  return strInfo;
}
