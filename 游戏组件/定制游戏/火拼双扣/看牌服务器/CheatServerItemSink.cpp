#include "StdAfx.h"
#include "CheatServerItemSink.h"

//
CCheatServerItemSink::CCheatServerItemSink(void)
{

}

CCheatServerItemSink::~CCheatServerItemSink(void)
{

}


//服务器控制
bool __cdecl CCheatServerItemSink::CheatServer(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame)
{
  for(WORD i = 0; i < GAME_PLAYER ; i++)
  {
    IServerUserItem * pIServerUserItem = pITableFrame->GetTableUserItem(i);
    ASSERT(pIServerUserItem);
    if(pIServerUserItem)
    {
      //作弊用户
      if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
      {
        CheatServer(pCheatCard, pITableFrame, pIServerUserItem);
      }
    }
  }

  //WB改
  WORD wEnumIndex=0;
  do
  {
    IServerUserItem * pIServerUserItem = pITableFrame->EnumLookonUserItem(wEnumIndex++);
    if(pIServerUserItem == NULL)
    {
      break;
    }
    //作弊用户
    if(CUserRight::IsGameCheatUser(pIServerUserItem->GetUserRight()))
    {
      CheatServer(pCheatCard, pITableFrame, pIServerUserItem);
    }
  }
  while(TRUE);

  return true;
}
//服务器控制
bool __cdecl CCheatServerItemSink::CheatServer(CMD_S_CheatCard *pCheatCard, ITableFrame * pITableFrame, IServerUserItem * pIServerUserItem)
{
  pITableFrame->SendUserItemData(pIServerUserItem,SUB_S_CHEAT_CARD,pCheatCard,sizeof(CMD_S_CheatCard));
  return true;
}