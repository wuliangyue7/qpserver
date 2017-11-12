#pragma once

//游戏控制基类
class ICheatServer
{
public:
  ICheatServer(void) {};
  virtual ~ICheatServer(void) {};

public:
  //服务器控制
  virtual bool __cdecl CheatServer(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame) = NULL;
  virtual bool __cdecl CheatServer(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem) = NULL;
};
