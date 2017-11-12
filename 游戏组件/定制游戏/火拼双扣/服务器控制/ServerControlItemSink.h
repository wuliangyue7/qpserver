#pragma once
#include "../游戏服务器/ServerControl.h"

class CServerControlItemSink : public IServerControl
{
  //控制操作
protected:

  BYTE              m_cbControlTimes;           //控制次数
  BYTE              m_cbControlStyle;           //控制方式

public:
  CServerControlItemSink(void);
  virtual ~CServerControlItemSink(void);

public:
  //服务器控制
  virtual bool  ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);

  //需要控制
  virtual bool  NeedControl();

  //完成控制
  virtual bool  CompleteControl();

  //返回控制类型
  virtual void  ReturnControlType(int & ControlType);


};
