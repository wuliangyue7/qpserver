#pragma once
#include "../游戏服务器/ServerControl.h"
#include "GameLogic.h"
//数值掩码
#define LOGIC_MASK_COLOR      0xF0                //花色掩码
#define LOGIC_MASK_VALUE      0x0F                //数值掩码


class CServerControlItemSink : public IServerControl
{
  //控制变量
protected:
  BYTE              m_cbWinSideControl;         //控制输赢
  BYTE              m_cbExcuteTimes;          //执行次数
  int               m_nSendCardCount;         //发送次数

  CGameLogic            m_GameLogic;              //游戏逻辑
public:
  CServerControlItemSink(void);
  virtual ~CServerControlItemSink(void);

public:
  //服务器控制
  virtual bool __cdecl ServerControl(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem, ITableFrame * pITableFrame);

  //需要控制
  virtual bool __cdecl NeedControl();

  //满足控制
  virtual bool __cdecl MeetControl(tagControlInfo ControlInfo);

  //完成控制
  virtual bool __cdecl CompleteControl();

  //返回控制区域
  virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER][MAX_COUNT]);
  //服务器控制
  virtual bool __cdecl ShowCheatCard(BYTE cbHandCardData[GAME_PLAYER][5], ITableFrame * pITableFrame);

  //获取牌信息
  CString GetCradInfo(BYTE cbCardData);
};
