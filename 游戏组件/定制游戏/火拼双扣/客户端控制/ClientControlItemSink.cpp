// ClientControlItemSink.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"


// CClientControlItemSinkDlg 对话框

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
  : IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
  IClientControlDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
  ON_WM_CTLCOLOR()
  ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
  ON_BN_CLICKED(IDC_BUTTON_SYN, OnBnClickedButtonSyn)
  ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
  ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
  ON_BN_CLICKED(IDC_RADIO_WIN, OnBnClickedRadioWin)
  ON_BN_CLICKED(IDC_RADIO_LOSE, OnBnClickedRadioLose)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg 消息处理程序

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
  IClientControlDlg::OnInitDialog();

  //设置信息
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));
  SetDlgItemText(IDC_STATIC_TIMES,TEXT("控制局数："));

  //SetDlgItemText(IDC_STATIC_NOTIC,TEXT("控制说明："));
  //SetDlgItemText(IDC_STATIC_TEXT,TEXT("区域输赢控制比游戏库存控制策略优先！"));

  SetDlgItemText(IDC_BUTTON_RESET,TEXT("取消控制"));
  SetDlgItemText(IDC_BUTTON_SYN,TEXT("本局信息"));
  SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
  SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));

  return TRUE;
}

//设置颜色
HBRUSH CClientControlItemSinkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

  if(pWnd->GetDlgCtrlID()==IDC_STATIC_INFO)
  {
    pDC->SetTextColor(RGB(255,10,10));
  }
  return hbr;
}

//取消控制
void CClientControlItemSinkDlg::OnBnClickedButtonReset()
{
  CMD_C_ControlApplication ControlApplication;
  ZeroMemory(&ControlApplication, sizeof(ControlApplication));
  ControlApplication.cbControlAppType = C_CA_CANCELS;
  ReSetAdminWnd();
  CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
  if(pGameFrameEngine!=NULL)
  {
    pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
  }

}

//本局控制
void CClientControlItemSinkDlg::OnBnClickedButtonSyn()
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
void CClientControlItemSinkDlg::OnBnClickedButtonOk()
{

  BYTE cbIndex=0;
  bool bFlags=false;
  m_cbControlStyle=0;
  m_cbExcuteTimes=0;

  //控制对家
  if(((CButton*)GetDlgItem(IDC_RADIO_WIN))->GetCheck())
  {
    m_cbControlStyle=CS_BANKER_WIN;
    bFlags=true;
  }
  else
  {
    if(((CButton*)GetDlgItem(IDC_RADIO_LOSE))->GetCheck())
    {
      m_cbControlStyle=CS_BANKER_LOSE;
      bFlags=true;
    }
  }
  m_cbExcuteTimes=(BYTE)((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel()+1;
  //获取执行次数
  if(m_cbExcuteTimes<=0)
  {
    bFlags=false;
  }
  if(bFlags) //参数有效
  {
    CMD_C_ControlApplication ControlApplication;
    ZeroMemory(&ControlApplication, sizeof(ControlApplication));

    ControlApplication.cbControlAppType = C_CA_SET;
    ControlApplication.cbControlStyle = m_cbControlStyle;
    ControlApplication.cbControlTimes = static_cast<BYTE>(m_cbExcuteTimes);
    CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
    if(pGameFrameEngine!=NULL)
    {
      pGameFrameEngine->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&ControlApplication,0);
    }

  }
  else
  {
    AfxMessageBox(TEXT("请选择受控次数以及受控方式!"));
    return;
  }

}

//取消关闭
void CClientControlItemSinkDlg::OnBnClickedButtonCancel()
{
  ShowWindow(SW_HIDE);
}

//更新控制
void  CClientControlItemSinkDlg::UpdateControl(CMD_S_ControlReturns* pControlReturns)
{
  CString str=TEXT("");
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
      m_cbExcuteTimes=pControlReturns->cbControlTimes;
      m_cbControlStyle=pControlReturns->cbControlStyle;
      myprintf(zsText,CountArray(zsText),TEXT("更新数据成功！\r\n %s"),zsTextTemp);
      SetDlgItemText(IDC_STATIC_INFO,zsText);
      ReSetAdminWnd();
      break;
    }
    case S_CR_SET_SUCCESS:
    {
      TCHAR zsText[256] = TEXT("");
      TCHAR zsTextTemp[256] = TEXT("");
      myprintf(zsText,CountArray(zsText),TEXT("设置数据成功！\r\n %s"),zsTextTemp);
      SetDlgItemText(IDC_STATIC_INFO,zsText);


      switch(m_cbControlStyle)
      {
        case CS_BANKER_WIN:
        {
          str.Format(TEXT("请求已接受，控制方式：对家赢，执行次数：%d"),m_cbExcuteTimes);
          break;
        }
        case CS_BANKER_LOSE:
        {
          str.Format(TEXT("请求已接受，控制方式：对家输，执行次数：%d"),m_cbExcuteTimes);
          break;
        }
        default:
          break;
      }


      break;
    }
    case S_CR_CANCEL_SUCCESS:
    {
      SetDlgItemText(IDC_STATIC_INFO,TEXT("取消设置成功！"));
      break;
    }
  }

  SetWindowText(str);
}



//信息
void CClientControlItemSinkDlg::PrintingInfo(TCHAR* pText, WORD cbCount, BYTE cbArea, BYTE cbTimes)
{
  if(cbArea == 0xff)
  {
    myprintf(pText,cbCount,TEXT("暂时无控制。"));
    return;
  }
  TCHAR szDesc[32] = TEXT("");
  myprintf(szDesc,CountArray(szDesc),TEXT("执行次数：%d。"), cbTimes);
  lstrcat(pText,szDesc);
}

void CClientControlItemSinkDlg::ReSetAdminWnd()
{

  if(m_cbControlStyle!=CS_BANKER_WIN&&m_cbControlStyle!=CS_BANKER_LOSE)
  {
    ((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(0);
    ((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(0);
  }
  else
  {
    ((CButton*)GetDlgItem(IDC_RADIO_WIN))->SetCheck(m_cbControlStyle==CS_BANKER_WIN?1:0);
    ((CButton*)GetDlgItem(IDC_RADIO_LOSE))->SetCheck(m_cbControlStyle==CS_BANKER_LOSE?1:0);
  }

  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes>0?m_cbExcuteTimes:-1);

}

bool CClientControlItemSinkDlg:: InitControl()
{
  return true;
}

void CClientControlItemSinkDlg::OnBnClickedRadioWin()
{
  // TODO: 在此添加控件通知处理程序代码
}

void CClientControlItemSinkDlg::OnBnClickedRadioLose()
{
  // TODO: 在此添加控件通知处理程序代码
}
