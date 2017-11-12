// ClientControlItemSink.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"

////////////////////////////////////////////////////////////////////////////////////////////
// CClientControlItemSinkDlg 对话框
IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
  : IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
  m_cbWinArea=0;        //赢牌区域
  m_cbExcuteTimes=0;      //执行次数
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
  IClientControlDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
  ON_BN_CLICKED(IDC_BUTTON_OK, OnExcute)
  ON_BN_CLICKED(IDC_BUTTON_CANCEL,OnCancel)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg 消息处理程序

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
  IClientControlDlg::OnInitDialog();

  SetDlgItemText(IDC_STATIC_AREA, TEXT("输赢控制："));
  SetDlgItemText(IDC_STATIC_NOTIC,TEXT("控制说明：控制选中的用户输"));
  SetDlgItemText(IDC_STATIC_TEXT,TEXT("用户输赢控制比游戏库存控制策略优先!"));
  SetDlgItemText(IDC_CHECK_LONG,TEXT("对家"));
  SetDlgItemText(IDC_CHECK_HE,TEXT("自已"));
  SetDlgItemText(IDC_CHECK_HU,TEXT("2"));
  SetDlgItemText(IDC_CHECK_SMALL,TEXT("3"));

  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));
  ((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(0);


  SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
  SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));

  return TRUE;
}

void CClientControlItemSinkDlg::OnExcute()
{
  CMD_C_AdminReq adminReq;
  BYTE cbIndex=0;
  int nCheckID=255;
  for(cbIndex=0; cbIndex<GAME_PLAYER; cbIndex++)
  {
    if(((CButton*)GetDlgItem(IDC_CHECK_LONG+cbIndex))->GetCheck())
    {
      nCheckID=cbIndex;
    }
  }

  int nSelect=0;

  if(nSelect>=0 && nCheckID != 255)
  {

    CGameFrameEngine *pFrameEngine = CGameFrameEngine::GetInstance();

    adminReq.cbReqType=RQ_SET_WIN_AREA;
    adminReq.cbExtendData[0]=pFrameEngine->SwitchViewChairID((BYTE)nCheckID+1);
    adminReq.cbExtendData[1]=pFrameEngine->SwitchViewChairID((BYTE)nCheckID);
    adminReq.cbExcuteTimes = GetDlgItemInt(IDC_COMBO_TIMES);
    m_cbWinArea=adminReq.cbExtendData[0];
    m_cbExcuteTimes = adminReq.cbExcuteTimes;

    CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
  }
  else
  {
    AfxMessageBox(TEXT("请选择受控次数以及受控区域!"));
    //OnRefresh();
  }
  // TODO: 在此添加控件通知处理程序代码
}


bool CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{

  const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
  CString str;
  switch(pResult->cbAckType)
  {

    case ACK_SET_WIN_AREA:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str="输赢控制命令已经接受！";
      }
      else
      {
        str.Format(TEXT("请求失败!"));
        m_cbExcuteTimes=0;
        m_cbWinArea=0;
      }
      break;
    }
    case ACK_RESET_CONTROL:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str="重置请求已接受!";
        for(int nIndex=0; nIndex<11; nIndex++)
        {
          ((CButton*)GetDlgItem(IDC_CHECK_LONG+nIndex))->SetCheck(0);
        }
        m_cbWinArea=0;
        m_cbExcuteTimes=0;
      }
      break;
    }
    case ACK_PRINT_SYN:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str="服务器同步请求已接受!";
        for(int nIndex=0; nIndex<11; nIndex++)
        {
          ((CButton*)GetDlgItem(IDC_CHECK_LONG+nIndex))->SetCheck(0);
        }

        m_cbWinArea=pResult->cbExtendData[0];
        m_cbExcuteTimes=pResult->cbExtendData[1];
        if(m_cbWinArea>0&&m_cbWinArea<=11)
        {
          ((CButton*)GetDlgItem(IDC_CHECK_LONG+m_cbWinArea-1))->SetCheck(1);
        }

        //SetTimer(10,2000,0);
      }
      else
      {
        str.Format(TEXT("请求失败!"));
      }
      break;
    }

    default:
      break;
  }

  SetWindowText(str);
  return true;
}


void CClientControlItemSinkDlg::OnCancel()
{
  // TODO: 在此添加专用代码和/或调用基类
  ShowWindow(SW_HIDE);
  //  CDialog::OnCancel();
}

void __cdecl CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
  GetDlgItem(IDC_BUTTON_OK)->EnableWindow(bEnable);
}
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
// CClientControlItemSinkDlg 对话框
IMPLEMENT_DYNAMIC(CClientControlItemSinkDlgEx, IClientControlDlg)

CClientControlItemSinkDlgEx::CClientControlItemSinkDlgEx(CWnd* pParent /*=NULL*/)
  : IClientControlDlg(CClientControlItemSinkDlgEx::IDD, pParent)
{
  m_wTableID = 0;
  m_lStorage = 0;
}

CClientControlItemSinkDlgEx::~CClientControlItemSinkDlgEx()
{
}

void CClientControlItemSinkDlgEx::DoDataExchange(CDataExchange* pDX)
{
  IClientControlDlg::DoDataExchange(pDX);

  DDX_Text(pDX,IDC_EDIT_TABLE_ID,m_wTableID);
  DDX_Text(pDX,IDC_EDIT_STORAGE,m_lStorage);

  //DDV_MinMaxLongLong(pDX,m_wTableID, 1, USHRT_MAX);
  //DDV_MinMaxLongLong(pDX,m_lStorage, LLONG_MIN, LLONG_MAX);
}

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlgEx, IClientControlDlg)
  ON_BN_CLICKED(IDC_BUTTON_UPDATE_STORAGE, OnUpdateStorage)
  ON_BN_CLICKED(IDC_BUTTON_SET_STORAGE,OnSetStorage)
  ON_BN_CLICKED(IDC_BUTTON_CLEAR_ALLSTORAGE,OnClearAllStorage)
  ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

//初始化
BOOL CClientControlItemSinkDlgEx::OnInitDialog()
{
  IClientControlDlg::OnInitDialog();

  SetDlgItemText(IDC_STATIC_STORAGE,TEXT(""));

  return TRUE;
}


void CClientControlItemSinkDlgEx::OnUpdateStorage()
{
  //检测输入值非空
  TCHAR szData[256] = {0};
  GetDlgItemText(IDC_EDIT_TABLE_ID, szData, sizeof(szData));
  if(0 == szData[0])
  {
    if(IDOK  == AfxMessageBox(TEXT("请输入查询桌号！")))
    {
      return;
    }
  }

  //读取变量
  UpdateData(TRUE); //从控件读取数据到变量

  if(m_wTableID < 1 || m_wTableID > USHRT_MAX)
  {
    CString str;
    str.Format(TEXT("[桌号]输入范围为1-%u"),USHRT_MAX);
    if(IDOK  == AfxMessageBox(str))
    {
      return;
    }
  }

  //发送消息
  CGameFrameEngine *pFrameEngine = CGameFrameEngine::GetInstance();

  CMD_C_AdminReqEx adminReq;
  ZeroMemory(&adminReq, sizeof(adminReq));

  adminReq.cbReqType=RQ_UPDATE_STORAGE;
  adminReq.wTableID = m_wTableID-1;

  CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN_EX,(WPARAM)&adminReq,0);

  return;
}

void CClientControlItemSinkDlgEx::OnSetStorage()
{
  //检测输入值非空
  TCHAR szData[256] = {0};
  GetDlgItemText(IDC_EDIT_TABLE_ID, szData, sizeof(szData));
  if(0 == szData[0])
  {
    if(IDOK  == AfxMessageBox(TEXT("请输入设置桌号！")))
    {
      return;
    }
  }
  GetDlgItemText(IDC_EDIT_STORAGE, szData, sizeof(szData));
  if(0 == szData[0])
  {
    if(IDOK  == AfxMessageBox(TEXT("请输入设置库存值！")))
    {
      return;
    }
  }

  //读取变量
  UpdateData(TRUE); //从控件读取数据到变量

  if(m_wTableID < 1 || m_wTableID > USHRT_MAX)
  {
    CString str;
    str.Format(TEXT("[桌号]输入范围为1-%u"),USHRT_MAX);
    if(IDOK  == AfxMessageBox(str))
    {
      return;
    }
  }

  if(m_lStorage < 0 || m_lStorage > LLONG_MAX)
  {
    CString str;
    str.Format(TEXT("[库存]输入范围为%I64d-%I64d"), 0, LLONG_MAX);
    if(IDOK  == AfxMessageBox(str))
    {
      return;
    }
  }

  //发送消息
  CGameFrameEngine *pFrameEngine = CGameFrameEngine::GetInstance();

  CMD_C_AdminReqEx adminReq;
  ZeroMemory(&adminReq, sizeof(adminReq));

  adminReq.cbReqType=RQ_SET_STORAGE;
  adminReq.wTableID = m_wTableID-1;
  adminReq.lStorage = m_lStorage;

  CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN_EX,(WPARAM)&adminReq,0);

  return;
}

void CClientControlItemSinkDlgEx::OnClearAllStorage()
{
  CMD_C_AdminReqEx adminReq;
  ZeroMemory(&adminReq, sizeof(adminReq));

  adminReq.cbReqType=RQ_CLEAR_ALLSTORAGE;
  CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN_EX,(WPARAM)&adminReq,0);
}

bool CClientControlItemSinkDlgEx::ReqResult(const void * pBuffer)
{

  const CMD_S_CommandResultEx*pResult=(CMD_S_CommandResultEx*)pBuffer;
  CString str;
  switch(pResult->cbAckType)
  {
    case ACK_UPDATE_STORAGE:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        m_wTableID = pResult->wTableID+1;
        m_lStorage = pResult->lStorage;

        //变量值写入控件
        UpdateData(FALSE);

        str.Format(TEXT("库存查询成功！\r\n第【%u】桌库存值：%I64d"), m_wTableID, m_lStorage);
      }
      else
      {
        str.Format(TEXT("库存查询失败！"));
      }
      break;
    }
    case ACK_SET_STORAGE:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        m_wTableID = pResult->wTableID+1;
        m_lStorage = pResult->lStorage;

        //变量值写入控件
        UpdateData(FALSE);

        str.Format(TEXT("库存设置成功！\r\n第【%u】桌库存值：%I64d"), m_wTableID, m_lStorage);

      }
      else
      {
        str.Format(TEXT("库存设置失败！"));
      }
      break;
    }
    case ACK_CLEAR_ALLSTORAGE:
    {
      if(pResult->cbResult==CR_ACCEPT)
      {
        str = TEXT("清空所有桌子库存成功！");
      }
      break;
    }

    default:
      break;
  }

  SetDlgItemText(IDC_STATIC_STORAGE, str);

  return true;
}

void __cdecl CClientControlItemSinkDlgEx::OnAllowControl(bool bEnable)
{
  return;
}

HBRUSH CClientControlItemSinkDlgEx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  HBRUSH hbr = IClientControlDlg::OnCtlColor(pDC, pWnd, nCtlColor);

  if(pWnd->GetDlgCtrlID() == IDC_STATIC_STORAGE)
  {
    pDC->SetTextColor(RGB(255,0,0)); //字体颜色
    //pDC->SetBkColor(RGB(0, 0, 255)); //字体背景色
  }

  return hbr;
}

/////////////////////////////////////////////////////////////////////////////////////////////