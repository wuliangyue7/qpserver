#pragma once

//游戏控制基类
class IClientControlDlg : public CDialog
{
public:
  IClientControlDlg(UINT UID, CWnd* pParent) : CDialog(UID, pParent) {}
  virtual ~IClientControlDlg(void) {}

public:
  //更新控制
  virtual void  UpdateControl(CMD_S_ControlReturns* pControlReturns) = NULL;
  //初始控件
  virtual bool  InitControl()=NULL;
};
