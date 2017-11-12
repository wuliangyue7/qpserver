#pragma once
#include "../游戏服务器/CheatServer.h"

//数值掩码
#define LOGIC_MASK_COLOR      0xF0                //花色掩码
#define LOGIC_MASK_VALUE      0x0F                //数值掩码


class CCheatServerItemSink : public ICheatServer
{
public:
  CCheatServerItemSink(void);
  virtual ~CCheatServerItemSink(void);

public:
  //服务器控制
  virtual bool __cdecl CheatServer(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame);
  virtual bool __cdecl CheatServer(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem);
};
