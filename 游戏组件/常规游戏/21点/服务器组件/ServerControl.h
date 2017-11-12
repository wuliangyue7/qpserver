#pragma once

#define MAX_COUNT         11                  //最大数目

//游戏控制基类
class IServerControl
{
public:
  IServerControl(void) {};
  virtual ~IServerControl(void) {};

public:
  //服务器控制
  virtual bool __cdecl ServerControl(BYTE cbHandCardData[GAME_PLAYER*2][11], ITableFrame * pITableFrame) = NULL;
  virtual bool __cdecl ServerControl(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame) = NULL;

  //返回控制区域
  virtual bool __cdecl ControlResult(BYTE cbControlCardData[GAME_PLAYER * 2][MAX_COUNT], BYTE cbCardCount[GAME_PLAYER*2], ROOMUSERCONTROL Keyroomusercontrol, ITableFrame * pITableFrame, WORD wBankerUser, BYTE cbPlayStatus[GAME_PLAYER]) = NULL;
};
