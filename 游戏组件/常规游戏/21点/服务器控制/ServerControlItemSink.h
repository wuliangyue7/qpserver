#pragma once
#include "../服务器组件/ServerControl.h"
#include "GameLogic.h"

//数值掩码
#define LOGIC_MASK_COLOR      0xF0                //花色掩码
#define LOGIC_MASK_VALUE      0x0F                //数值掩码


class CServerControlItemSink : public IServerControl
{
public:
  CServerControlItemSink(void);
  virtual ~CServerControlItemSink(void);

  CGameLogic            m_GameLogic;              //游戏逻辑

public:
  //服务器控制
  virtual bool __cdecl ServerControl(BYTE cbHandCardData[GAME_PLAYER*2][11], ITableFrame * pITableFrame);
  //服务器控制
  virtual bool __cdecl ServerControl(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame);

  //获取牌信息
  CString GetCradInfo(BYTE cbCardData);

  //返回控制区域
  virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER * 2][MAX_COUNT], BYTE cbCardCount[GAME_PLAYER*2], ROOMUSERCONTROL Keyroomusercontrol, ITableFrame * pITableFrame, WORD wBankerUser, BYTE cbPlayStatus[GAME_PLAYER]);
};
