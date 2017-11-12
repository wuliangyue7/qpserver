// AdminControlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "AdminControlDlg.h"
#include ".\admincontroldlg.h"


// CAdminControlDlg 对话框

IMPLEMENT_DYNAMIC(CAdminControlDlg, IClientControlDlg)
CAdminControlDlg::CAdminControlDlg(CWnd* pParent /*=NULL*/)
  : IClientControlDlg(CAdminControlDlg::IDD, pParent)
{
}

CAdminControlDlg::~CAdminControlDlg()
{
}

void CAdminControlDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_USERINF_LIST, m_SkinListCtrl);
  DDX_Control(pDX, IDC_LIST_ALL_BET, m_ListTotalBet);
  DDX_Control(pDX, IDC_SEARCH_ID, m_EditSerach);
}


BEGIN_MESSAGE_MAP(CAdminControlDlg, IClientControlDlg)
  ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
  ON_BN_CLICKED(IDC_BUTTON_SYN, OnBnClickedButtonSyn)
  ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
  ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
  ON_BN_CLICKED(IDC_FRESHSTO,OnFreshStorage)
  ON_WM_CTLCOLOR()
  ON_BN_CLICKED(IDC_BT_DEDUCT2, OnBnClickedBtDeduct2)
  ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
  ON_EN_KILLFOCUS(IDC_SEARCH_ID, OnEnKillfocusSearchId)
  ON_EN_SETFOCUS(IDC_SEARCH_ID, OnEnSetfocusSearchId)
END_MESSAGE_MAP()


// CAdminControlDlg 消息处理程序

BOOL CAdminControlDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  SetWindowText(TEXT("控制管理器"));
  //设置信息
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));
  SetDlgItemText(IDC_STATIC_TIMES,TEXT("控制局数："));
  SetDlgItemText(IDC_STATIC_AREA, TEXT("区域控制："));
  SetDlgItemText(IDC_STATIC_NOTIC,TEXT("控制说明："));
  SetDlgItemText(IDC_STATIC_TEXT,TEXT("区域输赢控制比游戏库存控制策略优先！"));
  SetDlgItemText(IDC_RADIO_BEN_40,TEXT("大虎"));
  SetDlgItemText(IDC_RADIO_BAO_30,TEXT("大狗"));
  SetDlgItemText(IDC_RADIO_AO_20,TEXT("大马"));
  SetDlgItemText(IDC_RADIO_DA_10,TEXT("大蛇"));
  SetDlgItemText(IDC_RADIO_BEN_5,TEXT("小虎"));
  SetDlgItemText(IDC_RADIO_BAO_5,TEXT("小狗"));
  SetDlgItemText(IDC_RADIO_AO_5,TEXT("小马"));
  SetDlgItemText(IDC_RADIO_DA_5,TEXT("小蛇"));
  SetDlgItemText(IDC_BUTTON_RESET,TEXT("取消控制"));
  SetDlgItemText(IDC_BUTTON_SYN,TEXT("本局"));
  SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
  SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));
  m_lStorageStart=0;
  m_lStorageDeduct=0;

  ZeroMemory(m_AreaTotalBet,sizeof(m_AreaTotalBet));
  ZeroMemory(m_lUserTotalScore,sizeof(m_lUserTotalScore));
  //列表
  m_SkinListCtrl.ModifyStyle(0,LVS_REPORT);

  DWORD dwStyle = GetWindowLong(m_SkinListCtrl.m_hWnd, GWL_STYLE);
  SetWindowLong(m_SkinListCtrl.m_hWnd, GWL_STYLE, dwStyle|LVS_REPORT);

  DWORD dwStyles = m_SkinListCtrl.GetExStyle();
  dwStyles &= ~LVS_EX_CHECKBOXES;

  m_SkinListCtrl.SetExtendedStyle(dwStyles|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_SkinListCtrl.GetHeaderCtrl()->EnableWindow(FALSE);

  m_SkinListCtrl.InsertColumn(0,TEXT(""),LVCFMT_CENTER,0);
  m_SkinListCtrl.InsertColumn(1,TEXT("序号"),LVCFMT_CENTER,38);
  m_SkinListCtrl.InsertColumn(2,TEXT("玩家昵称"),LVCFMT_CENTER,80);
  m_SkinListCtrl.InsertColumn(3,TEXT("玩家ID"),LVCFMT_CENTER,70);
  m_SkinListCtrl.InsertColumn(4,TEXT("玩家金币"),LVCFMT_CENTER,70);
  m_SkinListCtrl.InsertColumn(5,TEXT("总输赢"),LVCFMT_CENTER,70);
  m_SkinListCtrl.InsertColumn(6,TEXT("大虎"),LVCFMT_CENTER,50);
  m_SkinListCtrl.InsertColumn(7,TEXT("大狗"),LVCFMT_CENTER,50);
  m_SkinListCtrl.InsertColumn(8,TEXT("大马"),LVCFMT_CENTER,50);
  m_SkinListCtrl.InsertColumn(9,TEXT("大蛇"),LVCFMT_CENTER,50);
  m_SkinListCtrl.InsertColumn(10,TEXT("小虎"),LVCFMT_CENTER,50);
  m_SkinListCtrl.InsertColumn(11,TEXT("小狗"),LVCFMT_CENTER,50);
  m_SkinListCtrl.InsertColumn(12,TEXT("小马"),LVCFMT_CENTER,50);
  m_SkinListCtrl.InsertColumn(13,TEXT("小蛇"),LVCFMT_CENTER,50);

  m_ListTotalBet.SetExtendedStyle(m_ListTotalBet.GetExtendedStyle()|LVS_EX_GRIDLINES);
  m_ListTotalBet.InsertColumn(0,TEXT(""),LVCFMT_CENTER,0);
  m_ListTotalBet.InsertColumn(1,TEXT(""),LVCFMT_CENTER,328);//38+80+70+70+70
  m_ListTotalBet.InsertColumn(2,TEXT(""),LVCFMT_CENTER,50);
  m_ListTotalBet.InsertColumn(3,TEXT(""),LVCFMT_CENTER,50);
  m_ListTotalBet.InsertColumn(4,TEXT(""),LVCFMT_CENTER,50);
  m_ListTotalBet.InsertColumn(5,TEXT(""),LVCFMT_CENTER,50);
  m_ListTotalBet.InsertColumn(6,TEXT(""),LVCFMT_CENTER,50);
  m_ListTotalBet.InsertColumn(7,TEXT(""),LVCFMT_CENTER,50);
  m_ListTotalBet.InsertColumn(8,TEXT(""),LVCFMT_CENTER,50);
  m_ListTotalBet.InsertColumn(9,TEXT(""),LVCFMT_CENTER,50);
  m_ListTotalBet.InsertItem(0, TEXT(""));
  m_ListTotalBet.SetItemText(0,1,TEXT("真实玩家下注统计"));
  for(BYTE cbSubItem=2; cbSubItem <=9; cbSubItem++)
  {
    m_ListTotalBet.SetItemText(0, cbSubItem, TEXT("0"));
  }

  m_EditSerach.SetWindowText(TEXT("请输入查找玩家ID"));

  return TRUE;
}

//设置颜色
HBRUSH CAdminControlDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

  if(pWnd->GetDlgCtrlID()==IDC_STATIC_INFO  ||
     pWnd->GetDlgCtrlID()==IDC_STATIC_INFO2 ||
     pWnd->GetDlgCtrlID()==IDC_STATIC_SERACHINFO)
  {
    pDC->SetTextColor(RGB(255,10,10));
  }
  if(pWnd->GetDlgCtrlID() == IDC_SEARCH_ID)
  {
    CString str;
    m_EditSerach.GetWindowText(str);
    if(str.Compare(TEXT("请输入查找玩家ID")) == 0)
    {
      pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
    }
  }
  return hbr;
}

//更新库存
void CAdminControlDlg::OnFreshStorage()
{
  CString str;
  CMD_C_FreshStorage FreshStorage;
  m_lStorageStart=(SCORE)GetDlgItemInt(IDC_STORAGE);
  m_lStorageDeduct=(SCORE)GetDlgItemInt(IDC_DEDUCT);
  FreshStorage.lStorageStart=m_lStorageStart;
  FreshStorage.lStorageDeuct=m_lStorageDeduct;

  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_STORAGE,(WPARAM)&FreshStorage,0);
  }
  str=TEXT("库存已更新！");

  SetDlgItemText(IDC_STATIC_INFO2,str);
}

//取消控制
void CAdminControlDlg::OnBnClickedButtonReset()
{
  CMD_C_ControlApplication ControlApplication;
  ZeroMemory(&ControlApplication, sizeof(ControlApplication));
  ControlApplication.cbControlAppType = C_CA_CANCELS;
  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
  }
}

//本局控制
void CAdminControlDlg::OnBnClickedButtonSyn()
{
  CMD_C_ControlApplication ControlApplication;
  ZeroMemory(&ControlApplication, sizeof(ControlApplication));
  ControlApplication.cbControlAppType = C_CA_UPDATE;
  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
  }
}

//开启控制
void CAdminControlDlg::OnBnClickedButtonOk()
{
  CMD_C_ControlApplication ControlApplication;
  ZeroMemory(&ControlApplication, sizeof(ControlApplication));

  BYTE cbSelectedIndex = 0xff;
  if(((CButton*)GetDlgItem(IDC_RADIO_BEN_40))->GetCheck())
  {
    cbSelectedIndex = 0;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_BAO_30))->GetCheck())
  {
    cbSelectedIndex = 1;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_AO_20))->GetCheck())
  {
    cbSelectedIndex = 2;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_DA_10))->GetCheck())
  {
    cbSelectedIndex = 3;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_BEN_5))->GetCheck())
  {
    cbSelectedIndex = 4;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_BAO_5))->GetCheck())
  {
    cbSelectedIndex = 5;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_AO_5))->GetCheck())
  {
    cbSelectedIndex = 6;
  }
  else if(((CButton*)GetDlgItem(IDC_RADIO_DA_5))->GetCheck())
  {
    cbSelectedIndex = 7;
  }

  int nSelectTimes = ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel();

  if(cbSelectedIndex != 0xff && nSelectTimes >= 0 && nSelectTimes != 0xffffffff)
  {
    ControlApplication.cbControlAppType = C_CA_SET;
    ControlApplication.cbControlArea = cbSelectedIndex;
    ControlApplication.cbControlTimes = static_cast<BYTE>(nSelectTimes + 1);
    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
    }
  }
  else
  {
    SetDlgItemText(IDC_STATIC_INFO,TEXT("请选择受控次数以及受控区域！"));
  }

}

//取消关闭
void CAdminControlDlg::OnBnClickedButtonCancel()
{
  ShowWindow(SW_HIDE);
}


//更新控制
void CAdminControlDlg::UpdateControl(CMD_S_ControlReturns* pControlReturns)
{
  switch(pControlReturns->cbReturnsType)
  {
    case S_CR_FAILURE:
    {
      SetDlgItemText(IDC_STATIC_INFO,TEXT("操作失败！"));
      break;
    }
    case S_CR_UPDATE_SUCCES:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
      myprintf(zsText,CountArray(zsText),TEXT("更新数据成功！\r\n %s"),zsTextTemp);
      SetDlgItemText(IDC_STATIC_INFO,zsText);
      break;
    }
    case S_CR_SET_SUCCESS:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      PrintingInfo(zsTextTemp,256,pControlReturns->cbControlArea,pControlReturns->cbControlTimes);
      myprintf(zsText,CountArray(zsText),TEXT("设置数据成功！\r\n %s"),zsTextTemp);
      SetDlgItemText(IDC_STATIC_INFO,zsText);
      break;
    }
    case S_CR_CANCEL_SUCCESS:
    {
      SetDlgItemText(IDC_STATIC_INFO,TEXT("取消设置成功！"));
      break;
    }
  }
}

//信息
void CAdminControlDlg::PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes)
{
  if(cbArea == 0xff)
  {
    myprintf(pText,cbCount,TEXT("暂时无控制。"));
    return;
  }
  TCHAR szDesc[32] = TEXT("");
  myprintf(pText,cbCount,TEXT("胜利区域："));
  if(cbArea == 0)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("大虎"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 1)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("大狗"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 2)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("大马"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 3)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("大蛇"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 4)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("小虎"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 5)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("小狗"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 6)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("小马"));
    lstrcat(pText,szDesc);
  }
  else if(cbArea == 7)
  {
    _sntprintf(szDesc,CountArray(szDesc),TEXT("小蛇"));
    lstrcat(pText,szDesc);
  }
  myprintf(szDesc,CountArray(szDesc),TEXT("执行次数：%d。"), cbTimes);
  lstrcat(pText,szDesc);
}



void CAdminControlDlg::OnBnClickedBtDeduct2()
{
  // TODO: 在此添加控件通知处理程序代码
  CString strBuf;
  CMD_C_StorageInfo StorageInfo;
  ZeroMemory(&StorageInfo,sizeof(StorageInfo));
  StorageInfo.lMaxStorage[0] = GetDlgItemInt(IDC_MAXSTORAGE1);
  StorageInfo.lMaxStorage[1] = GetDlgItemInt(IDC_MAXSTORAGE2);
  StorageInfo.wStorageMul[0] = GetDlgItemInt(IDC_StorageMul1);
  StorageInfo.wStorageMul[1] = GetDlgItemInt(IDC_StorageMul2);

  if(StorageInfo.lMaxStorage[0]<=0 || StorageInfo.lMaxStorage[0]>=StorageInfo.lMaxStorage[1] ||
     StorageInfo.wStorageMul[0]<=0 || StorageInfo.wStorageMul[0]>100 ||
     StorageInfo.wStorageMul[1]<=0 || StorageInfo.wStorageMul[1]>100)
  {
    strBuf += TEXT("修改条件有误，执行失败！");
  }
  else
  {
    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_STORAGEINFO,(WPARAM)&StorageInfo,0);
    }

    strBuf += TEXT("执行成功!");
  }
  SetDlgItemText(IDC_STATIC_INFO2,strBuf);
}

void CAdminControlDlg::OnBnClickedButton2()
{
  // TODO: 在此添加控件通知处理程序代码
  // TODO: 在此添加控件通知处理程序代码
  int numLine = 0;
  CString strlist;
  GetDlgItem(IDC_SEARCH_ID)->GetWindowText(strlist);
  if(strlist.Compare(TEXT("请输入查找玩家ID")) == 0)
  {
    SetDlgItemText(IDC_STATIC_SERACHINFO,TEXT("请输入查找玩家ID"));
    return;
  }
  numLine = m_SkinListCtrl.GetItemCount();
  int nItem = -1;
  for(int i = 0; i< numLine; i++)
  {
    if(strlist.Compare(m_SkinListCtrl.GetItemText(i,3)) == 0)
    {
      nItem=i;
      break;
    }

  }
  ////查找子项
  if(nItem == -1)
  {
    SetDlgItemText(IDC_STATIC_SERACHINFO,TEXT("你查找的玩家不在游戏中或者该玩家没有下注!"));
  }
  else if(nItem == 0)
  {
    //m_SkinListCtrl.SetItemColor(nItem,RGB(255,0,0),RGB(255,255,255));
    m_SkinListCtrl.SetItemState(nItem, LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);
    m_SkinListCtrl.SetSelectionMark(0);
    m_SkinListCtrl.SetFocus();
  }
  else
  {
    //先保存第一项数据
    CString strBuf[13];
    for(int i=0; i<13; i++)
    {
      strBuf[i]=m_SkinListCtrl.GetItemText(0,i+1);
    }

    for(int j=0; j<13; j++)
    {
      m_SkinListCtrl.SetItemText(0,j+1,m_SkinListCtrl.GetItemText(nItem,j+1));
      m_SkinListCtrl.SetItemText(nItem,j+1,strBuf[j]);
    }
    //m_SkinListCtrl.SetItemColor(0,RGB(255,0,0),RGB(255,255,255));
    m_SkinListCtrl.SetItemState(0, LVIS_FOCUSED | LVIS_SELECTED,LVIS_FOCUSED | LVIS_SELECTED);
    m_SkinListCtrl.SetSelectionMark(0);
    m_SkinListCtrl.SetFocus();
  }
}

void CAdminControlDlg::OnEnKillfocusSearchId()
{
  // TODO: 在此添加控件通知处理程序代码
  CString str;
  m_EditSerach.GetWindowText(str);

  if(str.IsEmpty())
  {
    m_EditSerach.SetWindowText(TEXT("请输入查找玩家ID"));
  }
}

void CAdminControlDlg::OnEnSetfocusSearchId()
{
  // TODO: 在此添加控件通知处理程序代码
  CString str;
  m_EditSerach.GetWindowText(str);
  if(str.Compare(TEXT("请输入查找玩家ID")) == 0)
  {
    m_EditSerach.SetWindowText(TEXT(""));
  }
}

//更新列表
void CAdminControlDlg::UpdateClistCtrl(LONGLONG lAllPlayBet[GAME_PLAYER][AREA_COUNT],BYTE cbViewIndex, LONGLONG lScoreCount, WORD wChair, WORD wViewChairID,IClientUserItem* pUserData)
{
  int nListIndex = cbViewIndex-1;

  //构造变量
  LVFINDINFO lvFindInfo;
  ZeroMemory(&lvFindInfo, sizeof(lvFindInfo));
  lvFindInfo.flags =  LVFI_STRING | LVFI_PARAM;
  lvFindInfo.psz = (LPCTSTR)pUserData->GetNickName();

  //查找第2项的
  int numLine = 0;
  CString strlist=lvFindInfo.psz ;
  numLine = m_SkinListCtrl.GetItemCount();
  int nItem = -1;
  for(int i = 0; i< numLine; i++)
  {
    if(strlist.Compare(m_SkinListCtrl.GetItemText(i,2)) == 0)
    {
      nItem=i;
      break;
    }

  }
  ////查找子项
  //nItem = m_SkinListCtrl.FindItem( &lvFindInfo );
  if(nItem == -1)
  {

    int line=m_SkinListCtrl.GetItemCount();
    if(line>=2)
    {
      line-=1;
    }
    CString strNum;
    strNum.Format(L"%d",line+1);
    nItem = m_SkinListCtrl.InsertItem(line, TEXT("0"));//插入行
    m_SkinListCtrl.SetItemText(line,1,strNum);
  }

  CString strScore;
  int index=0;
  for(int i=0; i<8; i++)
  {
    strScore.Format(TEXT("%I64d"), lAllPlayBet[wViewChairID][i]);
    index=i+6;
    m_SkinListCtrl.SetItemText(nItem, index, strScore);   //设置积分
  }


  strScore.Format(TEXT("%s"),pUserData->GetNickName());
  m_SkinListCtrl.SetItemText(nItem, 2, strScore);   //设置昵称

  strScore.Format(TEXT("%d"),pUserData->GetGameID());
  m_SkinListCtrl.SetItemText(nItem, 3, strScore);   //设置ID

  strScore.Format(TEXT("%d"),pUserData->GetUserScore());
  m_SkinListCtrl.SetItemText(nItem, 4, strScore);   //设置金币

  strScore.Format(TEXT("%d"),m_lUserTotalScore[wChair]);
  m_SkinListCtrl.SetItemText(nItem,5,strScore);   //设置总输赢

  //设置区域总金额
  m_AreaTotalBet[cbViewIndex-1]+=lScoreCount;
  for(int i=0; i<8; i++)
  {
    strScore.Format(TEXT("%I64d"), m_AreaTotalBet[i]);
    index=i+2;
    m_ListTotalBet.SetItemText(0, index, strScore);   //设置数据
  }
}
//清除列表
void CAdminControlDlg::ClearClist()
{
  m_SkinListCtrl.DeleteAllItems();
  for(BYTE cbSubItem=2; cbSubItem <=9; cbSubItem++)
  {
    m_ListTotalBet.SetItemText(0, cbSubItem, TEXT("0"));
  }
  ZeroMemory(m_AreaTotalBet,sizeof(m_AreaTotalBet));
}
//设置库存信息
void CAdminControlDlg::SetStorageInfo(CMD_S_ADMIN_STORAGE_INFO *pStorageInof)
{
  CString strBuf;
  //当前库存
  strBuf.Format(TEXT("%I64d"),pStorageInof->lCurrentStorage);
  GetDlgItem(IDC_STORAGE)->SetWindowText(strBuf);
  //当前衰减
  strBuf.Format(TEXT("%I64d"),pStorageInof->lCurrentDeduct);
  GetDlgItem(IDC_DEDUCT)->SetWindowText(strBuf);
  //库存上限1
  strBuf.Format(TEXT("%I64d"),pStorageInof->lMaxStorage[0]);
  GetDlgItem(IDC_MAXSTORAGE1)->SetWindowText(strBuf);
  //吐分概率
  strBuf.Format(TEXT("%d"),pStorageInof->wStorageMul[0]);
  GetDlgItem(IDC_StorageMul1)->SetWindowText(strBuf);
  //库存上限2
  strBuf.Format(TEXT("%I64d"),pStorageInof->lMaxStorage[1]);
  GetDlgItem(IDC_MAXSTORAGE2)->SetWindowText(strBuf);
  //吐分概率
  strBuf.Format(TEXT("%d"),pStorageInof->wStorageMul[1]);
  GetDlgItem(IDC_StorageMul2)->SetWindowText(strBuf);

  CopyMemory(m_lUserTotalScore,pStorageInof->lUserTotalScore,sizeof(m_lUserTotalScore));
}
//设置起始库存
void CAdminControlDlg::SetStorageInfo(LONGLONG lStartStorage)
{
  CString str;
  str.Format(TEXT("%I64d"),lStartStorage);
  SetDlgItemText(IDC_STARTSTORAGE,str);
}
