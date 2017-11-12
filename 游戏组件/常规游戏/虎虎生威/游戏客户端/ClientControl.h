#pragma once

//游戏控制基类
class IClientControlDlg : public CDialog
{
public:
  IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent) {}
  virtual ~IClientControlDlg(void) {}

public:
  //更新控制
  virtual void __cdecl UpdateControl(CMD_S_ControlReturns* pControlReturns) = NULL;
  virtual void UpdateClistCtrl(LONGLONG lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair, WORD wViewChairID,IClientUserItem* pUserData)=NULL;
  virtual void ClearClist()=NULL;
  virtual void SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInof) = NULL;
  virtual void SetStorageInfo(LONGLONG lStartStorage) = NULL;

};
