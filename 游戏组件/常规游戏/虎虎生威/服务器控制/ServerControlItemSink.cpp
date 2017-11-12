#include "StdAfx.h"
#include "servercontrolitemsink.h"


CServerControlItemSink::CServerControlItemSink(void)
{
  //控制变量
  m_cbControlArea=0xff;           //控制区域
  m_cbControlTimes=0;           //控制次数
}

CServerControlItemSink::~CServerControlItemSink(void)
{

}


bool  CServerControlItemSink::ExcuteControl(BYTE *cbCard)
{
  //控制
  if(m_cbControlArea != 0xff && m_cbControlTimes > 0)
  {
    BYTE cbControlArea[8][4] = { 1, 9, 17, 25,
                                 3, 11, 19, 27,
                                 5, 13, 21, 29,
                                 7, 15, 23, 31,

                                 2, 10, 18, 26,
                                 4, 12, 20, 28,
                                 6, 14, 22, 30,
                                 8, 16, 24, 32
                               };
    *cbCard= cbControlArea[m_cbControlArea][rand()%4];
    m_cbControlTimes--;
    if(m_cbControlTimes == 0)
    {
      m_cbControlArea = 0xff;
    }
    return true;
  }

  return false;
}

//服务器控制
bool  CServerControlItemSink::RecvControlReq(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame)
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

  CMD_C_ControlApplication* pData = (CMD_C_ControlApplication*)pDataBuffer;

  switch(pData->cbControlAppType)
  {
    case C_CA_UPDATE: //更新
    {
      CMD_S_ControlReturns ControlReturns;
      ZeroMemory(&ControlReturns,sizeof(ControlReturns));
      ControlReturns.cbReturnsType = S_CR_UPDATE_SUCCES;
      ControlReturns.cbControlArea = m_cbControlArea;
      ControlReturns.cbControlTimes = m_cbControlTimes;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
    }
    return true;
    case C_CA_SET:    //设置
    {
      if(pData->cbControlArea == 0xff || pData->cbControlArea  > 7)
      {
        //设置失败
        CMD_S_ControlReturns ControlReturns;
        ZeroMemory(&ControlReturns,sizeof(ControlReturns));
        ControlReturns.cbReturnsType = S_CR_FAILURE;
        ControlReturns.cbControlArea = 0xff;
        pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
        return true;
      }
      m_cbControlArea = pData->cbControlArea;
      m_cbControlTimes = pData->cbControlTimes;
      CMD_S_ControlReturns ControlReturns;
      ZeroMemory(&ControlReturns,sizeof(ControlReturns));
      ControlReturns.cbReturnsType = S_CR_SET_SUCCESS;
      ControlReturns.cbControlArea = m_cbControlArea;
      ControlReturns.cbControlTimes = m_cbControlTimes;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
    }
    return true;
    case C_CA_CANCELS:  //取消
    {
      m_cbControlArea = 0xff;
      m_cbControlTimes = 0;
      CMD_S_ControlReturns ControlReturns;
      ZeroMemory(&ControlReturns,sizeof(ControlReturns));
      ControlReturns.cbReturnsType = S_CR_CANCEL_SUCCESS;
      ControlReturns.cbControlArea = 0xff;
      pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
    }
    return true;
  }
  //设置失败
  CMD_S_ControlReturns ControlReturns;
  ZeroMemory(&ControlReturns,sizeof(ControlReturns));
  ControlReturns.cbReturnsType = S_CR_FAILURE;
  ControlReturns.cbControlArea = 0xff;
  pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_ADMIN_COMMDN,&ControlReturns,sizeof(ControlReturns));
  return true;
}


