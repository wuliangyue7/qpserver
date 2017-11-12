#include "StdAfx.h"
#include "servercontrolitemsink.h"


CServerControlItemSink::CServerControlItemSink(void)
{
  m_cbControlTimes = 0;
  m_cbControlStyle=0;
}

CServerControlItemSink::~CServerControlItemSink(void)
{

}

//服务器控制
bool  CServerControlItemSink::ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame)
{
  ASSERT(wDataSize==sizeof(CMD_C_ControlApplication));
  if(wDataSize!=sizeof(CMD_C_ControlApplication))
  {
    return false;
  }
  //如果不具有管理员权限 则返回错误
  if(!CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
  {
    return false;
  }

  CMD_C_ControlApplication* pControlApplication = (CMD_C_ControlApplication*)pDataBuffer;

  switch(pControlApplication->cbControlAppType)
  {
    case C_CA_UPDATE: //更新
    {
      CMD_S_ControlReturns ControlReturns;
      ZeroMemory(&ControlReturns,sizeof(ControlReturns));
      ControlReturns.cbReturnsType = S_CR_UPDATE_SUCCES;
      ControlReturns.cbControlTimes = m_cbControlTimes;
      ControlReturns.cbControlStyle=m_cbControlStyle;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
    }
    return true;
    case C_CA_SET:    //设置
    {

      switch(pControlApplication->cbControlStyle)
      {

        case CS_BANKER_LOSE:  //庄家输牌
        {
          m_cbControlStyle=pControlApplication->cbControlStyle;
          m_cbControlTimes=pControlApplication->cbControlTimes;
          CMD_S_ControlReturns ControlReturns;
          ZeroMemory(&ControlReturns,sizeof(ControlReturns));
          ControlReturns.cbReturnsType = S_CR_SET_SUCCESS;
          ControlReturns.cbControlTimes = m_cbControlTimes;
          ControlReturns.cbControlStyle=m_cbControlStyle;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));

          break;
        }
        case CS_BANKER_WIN:   //庄家赢牌
        {
          m_cbControlStyle=pControlApplication->cbControlStyle;
          m_cbControlTimes=pControlApplication->cbControlTimes;
          CMD_S_ControlReturns ControlReturns;
          ZeroMemory(&ControlReturns,sizeof(ControlReturns));
          ControlReturns.cbReturnsType = S_CR_SET_SUCCESS;
          ControlReturns.cbControlTimes = m_cbControlTimes;
          ControlReturns.cbControlStyle=m_cbControlStyle;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
          break;
        }
        default:  //拒绝请求
        {
          //设置失败
          CMD_S_ControlReturns ControlReturns;
          ZeroMemory(&ControlReturns,sizeof(ControlReturns));
          ControlReturns.cbReturnsType = S_CR_FAILURE;
          pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
          break;
        }

      }
      return true;
    }
    case C_CA_CANCELS:  //取消
    {
      m_cbControlTimes = 0;
      CMD_S_ControlReturns ControlReturns;
      ZeroMemory(&ControlReturns,sizeof(ControlReturns));
      ControlReturns.cbReturnsType = S_CR_CANCEL_SUCCESS;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
    }
    return true;
  }
  //设置失败
  CMD_S_ControlReturns ControlReturns;
  ZeroMemory(&ControlReturns,sizeof(ControlReturns));
  ControlReturns.cbReturnsType = S_CR_FAILURE;
  pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
  return true;
}

//需要控制
bool  CServerControlItemSink::NeedControl()
{
  if(m_cbControlStyle > 0 && m_cbControlTimes > 0)
  {
    return true;
  }

  return false;

}

//完成控制
bool  CServerControlItemSink::CompleteControl()
{

  if(m_cbControlTimes > 0)
  {
    m_cbControlTimes--;
  }

  if(m_cbControlTimes == 0)
  {
    m_cbControlTimes = 0;
    m_cbControlStyle = 0;

  }
  return true;
}

void  CServerControlItemSink::ReturnControlType(int & ControlType)
{
  ControlType=m_cbControlStyle;
}

