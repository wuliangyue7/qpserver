#include "stdafx.h"
#include "DlgWebRegister.h"
#include <multimon.h>
//#include "UiInformation.h"
//#include "GameCheck.h"
#include "UiWebBrowserEx.h"
//#include "MD5.h"
#include "MissionLogon.h"

//////////////////////////////////////////////////////////////////////////////////

//圆角大小
#define ROUND_CX          3                 //圆角宽度
#define ROUND_CY          3                 //圆角高度

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgWebRegister, CDialog)
  ON_WM_NCCREATE()
  ON_WM_NCDESTROY()
  ON_WM_NCPAINT()
  ON_WM_NCACTIVATE()
  ON_WM_NCCALCSIZE()
  ON_WM_NCHITTEST()
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_BN_CLICKED(IDCANCEL__, OnCancel)

END_MESSAGE_MAP()


//构造函数
CDlgWebRegister::CDlgWebRegister() : CDialog(IDD_DLG_WEB_REGISTER)
{
  m_uSizeStatus = SIZE_RESTORED;
  m_bWebCreat = false;
  m_enLogonType = en_Account_Logon;

  memset(&m_stLogonAccounts,0, sizeof(m_stLogonAccounts));
  memset(m_szWebRegisterLink,0, sizeof(m_szWebRegisterLink));
  memset(m_szQQLogonLink,0, sizeof(m_szQQLogonLink));

  m_bLogonCreat = false;
  m_bRegtistCreat = false;

  return;
}

//析构函数
CDlgWebRegister::~CDlgWebRegister()
{
}

//控件绑定
VOID CDlgWebRegister::DoDataExchange(CDataExchange * pDX)
{
  __super::DoDataExchange(pDX);

  //按钮控件
  DDX_Control(pDX, IDCANCEL__, m_btCancel);

}

bool CDlgWebRegister::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes)
{
  if(uMsg == WM_MOUSEMOVE)
  {
    int n = 0;
  }
  if(uMsg == WM_KEYDOWN)
  {

  }


  return m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes);
}

void CDlgWebRegister::Notify(TNotifyUI& msg)
{
  //鼠标点击
  if(_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
  {
    //按钮事件
    if(ButtonEvent(msg))
    {
      return;
    }
  }
}

BOOL CDlgWebRegister::OnNcCreate(LPCREATESTRUCT lpCS)
{
  Subclass();

  return __super::OnNcCreate(lpCS);
}

void CDlgWebRegister::OnNcDestroy()
{
  UnSubclass();

  __super::OnNcDestroy();
}

int CDlgWebRegister::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(__super::OnCreate(lpCreateStruct)==-1)
  {
    return -1;
  }

  //ModifyStyle(WS_CAPTION,0,0);
  //ModifyStyleEx(WS_BORDER|WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,0,0);
  //ModifyStyle(0, WS_MINIMIZEBOX);

  // 初始化UI管理器
  m_PaintManager.Init(m_hWnd);


  //设置按钮
  HINSTANCE hInstance=AfxGetInstanceHandle();
  m_btCancel.SetButtonImage(IDB_BT_REGISTER_CLOSE,hInstance,false,false);
  m_btCancel.SetWindowPos(NULL,460,6,0,0,SWP_NOZORDER|SWP_NOCOPYBITS|SWP_NOACTIVATE|SWP_NOSIZE);

  CenterWindow();


  //SetRegisterlogon(true,TEXT("RegisterLogonWeb"));

  //SetQQlogon(true,TEXT("QQLogonWeb"));

  CSize SizeWindow(485,432);
  //获取窗口
  CRect rcWindow;
  GetWindowRect(&rcWindow);

  //设置区域
  CRgn RgnWindow;
  RgnWindow.CreateRoundRectRgn(ROUND_CX,ROUND_CY,SizeWindow.cx-ROUND_CX,SizeWindow.cy-ROUND_CY,ROUND_CX,ROUND_CY);

  //设置区域
  SetWindowRgn(RgnWindow,FALSE);

  ////设置路径
  //TCHAR szDirectory[MAX_PATH]=TEXT("");
  //CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));
  //
  ////配置文件
  //TCHAR szFileName[MAX_PATH]=TEXT("");
  //_snwprintf(szFileName,sizeof(szFileName),TEXT("%s\\LobbyUrl.ini"),szDirectory);

  ////读取地址
  //GetPrivateProfileString(TEXT("URL"), TEXT("WebRegister"), TEXT("http://www.7298.com/lobby/Register.aspx"), m_szWebRegisterLink, sizeof(TCHAR)*MAX_PATH, szFileName);
  //GetPrivateProfileString(TEXT("URL"), TEXT("QQLogon"), TEXT("http://www.7298.com/lobby/QQLogin.aspx"), m_szQQLogonLink, sizeof(TCHAR)*MAX_PATH, szFileName);

  CGlobalWebLink * pGlobalWebLink=CGlobalWebLink::GetInstance();
  if(pGlobalWebLink != NULL)
  {
    _snwprintf(m_szWebRegisterLink,sizeof(m_szWebRegisterLink),TEXT("%s"),pGlobalWebLink->GetWebRegisterLink());
    _snwprintf(m_szQQLogonLink,sizeof(m_szWebRegisterLink),TEXT("%s"),pGlobalWebLink->GetQQLogonLink());
  }
  return 1;
}
void CDlgWebRegister::OnNcPaint()
{
  __super::OnNcPaint();
}

BOOL CDlgWebRegister::OnNcActivate(BOOL bActive)
{
  return (bActive == 0) ? TRUE : FALSE;
}



void CDlgWebRegister::OnNcCalcSize(BOOL bCalc, NCCALCSIZE_PARAMS* pNcCalcParam)
{
  return;
}

LRESULT CDlgWebRegister::OnNcHitTest(CPoint ptMouse)
{
  LRESULT lRet = HitTestNCA(m_hWnd, ptMouse);

  if(lRet == HTNOWHERE)
  {
    return __super::OnNcHitTest(ptMouse);
  }
  return lRet;
}



void CDlgWebRegister::OnSize(UINT nType, int cx, int cy)
{
  //UINT uCmd = (UINT)nType;
  //if( uCmd == SIZE_MINIMIZED ) {
  //  OnSized(SIZE_MINIMIZED);
  //}
  //else if( uCmd == SIZE_RESTORED && m_uSizeStatus == SIZE_MAXIMIZED ) {
  //  OnSized(SIZE_RESTORED);
  //}
  //else if( uCmd == SIZE_MAXIMIZED ) {
  //  OnSized(SIZE_MAXIMIZED);
  //}
  //m_uSizeStatus = uCmd;

  //// 计算窗口RGN区域
  //SIZE szRoundCorner = m_PaintManager.GetRoundCorner();

  //if( ::IsIconic(m_hWnd) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
  //  DuiLib::CDuiRect rcWnd;
  //  ::GetWindowRect(m_hWnd, &rcWnd);
  //  rcWnd.Offset(-rcWnd.left, -rcWnd.top);
  //  rcWnd.right++; rcWnd.bottom++;
  //  HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
  //  ::SetWindowRgn(m_hWnd, hRgn, TRUE);
  //  ::DeleteObject(hRgn);
  //}
  //else {
  //  ::SetWindowRgn(m_hWnd, NULL, TRUE);
  //}

  __super::OnSize(nType, cx, cy);
}

CControlUI* CDlgWebRegister::CreateControl(LPCTSTR pstrClass)
{
  if(_tcsicmp(pstrClass, _T("WebBrowserEx")) == 0)
  {
    CUiWebBrowserEx* pUI = new CUiWebBrowserEx();
    if(pUI)
    {
      pUI->SetIDispWebEvent(this);
    }
    return pUI;
  }
  return NULL;

}

bool CDlgWebRegister::HitControl(CControlUI* pControl)
{
  if(pControl == NULL)
  {
    return false;
  }

  LPCTSTR pstrClass = pControl->GetClass();

  if(_tcsicmp(pstrClass, _T("ButtonUI")) == 0 ||
     _tcsicmp(pstrClass, _T("ProgressUI")) == 0 ||
     _tcsicmp(pstrClass, _T("TextUI")) == 0 ||
     _tcsicmp(pstrClass, _T("EditUI")) == 0 ||
     _tcsicmp(pstrClass, _T("ComboUI")) == 0)
  {
    return true;
  }

  return false;
}

LRESULT CDlgWebRegister::HitTestNCA(HWND hWnd, POINT ptMouse)
{
  // Get the window rectangle.
  RECT rcWindow;
  ::GetWindowRect(hWnd, &rcWindow);

  // Determine if hit test is for resizing, default middle (1,1).
  USHORT uRow = 1;
  USHORT uCol = 1;

  RECT rcSizeBox = m_PaintManager.GetSizeBox();
  RECT rcCaption = m_PaintManager.GetCaptionRect();
  bool bCaption = false;

  if(ptMouse.x >= rcWindow.left + rcCaption.left && ptMouse.x < rcWindow.right - rcCaption.right \
     && ptMouse.y >= rcWindow.top + rcCaption.top && ptMouse.y < rcWindow.top + rcCaption.bottom)
  {
    POINT pt = ptMouse;
    ::ScreenToClient(hWnd, &pt);
    bCaption = !HitControl(m_PaintManager.FindControl(pt));
  }

  if(!::IsZoomed(hWnd))
  {
    // Determine if the point is at the top or bottom of the window.
    if(ptMouse.y >= rcWindow.top - 4 && ptMouse.y < rcWindow.top + rcSizeBox.top)
    {
      bCaption = false;
      uRow = 0;
    }
    else if(ptMouse.y < rcWindow.bottom + 4 && ptMouse.y >= rcWindow.bottom - rcSizeBox.bottom)
    {
      bCaption = false;
      uRow = 2;
    }

    // Determine if the point is at the left or right of the window.
    if(ptMouse.x >= rcWindow.left - 4 && ptMouse.x < rcWindow.left + rcSizeBox.left)
    {
      bCaption = false;
      uCol = 0; //left side
    }
    else if(ptMouse.x < rcWindow.right + 4 && ptMouse.x >= rcWindow.right - rcSizeBox.right)
    {
      bCaption = false;
      uCol = 2; //right side
    }
  }

  if(bCaption)
  {
    uRow = 0;
  }

  // Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
  LRESULT hitTests[3][3] =
  {
    { HTTOPLEFT,  bCaption ? HTCAPTION : HTTOP,   HTTOPRIGHT },
    { HTLEFT,   HTNOWHERE,              HTRIGHT },
    { HTBOTTOMLEFT, HTBOTTOM,             HTBOTTOMRIGHT },
  };

  return hitTests[uRow][uCol];
}

bool CDlgWebRegister::Subclass()
{
  ASSERT(::IsWindow(m_hWnd));
  ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(this));
  m_OldWndProc = (WNDPROC)::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LPARAM)__WndProc);
  if(m_OldWndProc == NULL)
  {
    return false;
  }
  m_bSubclassed = true;

  return true;
}

bool CDlgWebRegister::UnSubclass()
{
  ASSERT(::IsWindow(m_hWnd));
  if(!m_bSubclassed)
  {
    return false;
  }
  ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0);
  ::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LPARAM)m_OldWndProc);
  m_OldWndProc = ::DefWindowProc;
  m_bSubclassed = false;

  return true;
}

LRESULT CALLBACK CDlgWebRegister::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CDlgWebRegister* pThis = reinterpret_cast<CDlgWebRegister*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

  if(pThis != NULL)
  {
    LRESULT lResult = 0L;
    if(!pThis->MessageHandler(uMsg, wParam, lParam, lResult))
    {
      return ::CallWindowProc(pThis->m_OldWndProc, hWnd, uMsg, wParam, lParam);
    }
    return lResult;
  }
  else
  {
    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0L;
}

//按钮事件
bool CDlgWebRegister::ButtonEvent(TNotifyUI& msg)
{
  if(_tcsicmp(msg.pSender->GetName(), _T("ButtonClose")) == 0) //关闭
  {

    //隐藏窗口
    ShowWindow(SW_HIDE);

    //显示登录
    ASSERT(CMissionLogon::GetInstance()!=NULL);
    if(CMissionLogon::GetInstance()!=NULL)
    {
      CMissionLogon::GetInstance()->ShowLogon();
    }

    return true;
  }

  return false;

}

//设置QQ登陆
void CDlgWebRegister::SetQQlogon(bool bCreat, CString strWebBrowser)
{
  //CDialogBuilder builder;
  // CControlUI* pRoot = builder.Create(TEXT("ADImage//QQLogon.xml"), GetResType(), this, &m_PaintManager, NULL);
  //m_PaintManager.AttachDialog(pRoot);
  //m_PaintManager.AddNotifier(this);
  //变量定义
  CDialogBuilder Builder;
  STRINGorID xml(TEXT("DlgWebQQLogon.xml"));

  //创建控件
  CControlUI* pRoot = Builder.Create(xml,0,this,&m_PaintManager);
  if(pRoot==NULL)
  {
    //创建根节点
    pRoot = static_cast<CControlUI *>(new CContainerUI());
    pRoot->SetManager(&m_PaintManager,NULL);
  }
  m_PaintManager.AttachDialog(pRoot);
  m_PaintManager.AddNotifier(this);

  //登陆游戏
  CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("LogonRegister"));
  pTabLayout->SelectItem(2);

  m_enLogonType = en_QQ_Logon;
  m_bWebCreat = true;
  CUiWebBrowserEx * pActiveXUI = (CUiWebBrowserEx*)m_PaintManager.FindControl(strWebBrowser);

  if(pActiveXUI)
  {
    if(bCreat/* && !m_bLogonCreat*/)
    {
      m_bLogonCreat = true;
      static CWebBrowserEventHandlerEx wbehEx1;
      pActiveXUI->SetWebBrowserEventHandler(&wbehEx1);

      pActiveXUI->NavigateUrl(TEXT(""));
      pActiveXUI->Navigate2(TEXT(""));
      pActiveXUI->Navigate2(m_szQQLogonLink);
    }
  }

}


void CDlgWebRegister::SetRegisterlogon(bool bCreat, CString strWebBrowser)
{
  //CDialogBuilder builder;
  // CControlUI* pRoot = builder.Create(TEXT("ADImage//WebRegister.xml"), GetResType(), this, &m_PaintManager, NULL);
  //m_PaintManager.AttachDialog(pRoot);
  //m_PaintManager.AddNotifier(this);
  //m_PaintManager.AddPreMessageFilter(this);
  //m_PaintManager.SetPaintNotify(this);

  //变量定义
  CDialogBuilder Builder;
  STRINGorID xml(TEXT("DlgWebRegister.xml"));

  //创建控件
  CControlUI* pRoot = Builder.Create(xml,0,this,&m_PaintManager);
  if(pRoot==NULL)
  {
    //创建根节点
    pRoot = static_cast<CControlUI *>(new CContainerUI());
    pRoot->SetManager(&m_PaintManager,NULL);
  }
  m_PaintManager.AttachDialog(pRoot);
  m_PaintManager.AddNotifier(this);

  //登陆游戏
  CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("LogonRegister"));
  pTabLayout->SelectItem(1);

  m_bWebCreat = true;
  m_enLogonType = en_Register_Logon;

  CUiWebBrowserEx * pActiveXUI = (CUiWebBrowserEx*)m_PaintManager.FindControl(strWebBrowser);

  if(pActiveXUI)
  {
    if(bCreat/* && !m_bRegtistCreat*/)
    {
      m_bRegtistCreat = true;

      pActiveXUI->NavigateUrl(TEXT(""));
      pActiveXUI->Navigate2(m_szWebRegisterLink);
    }
  }
}

VOID CDlgWebRegister::OnEventQQLogon(CString strAccounts, CString strPassword)
{
  _sntprintf(m_stLogonAccounts.szAccounts,CountArray(m_stLogonAccounts.szAccounts),TEXT("%s"),strAccounts.GetBuffer());
  _sntprintf(m_stLogonAccounts.szPassword,CountArray(m_stLogonAccounts.szPassword),TEXT("%s"),strPassword.GetBuffer());

  CString strm;
  strm.Format(TEXT("OnEventQQLogon %s,%s"),m_stLogonAccounts.szAccounts,m_stLogonAccounts.szPassword);
  AfxMessageBox(strm,0,0);

  //隐藏窗口
  ShowWindow(SW_HIDE);

  //执行注册
  CMissionLogon * pMissionLogon=CMissionLogon::GetInstance();
  if(pMissionLogon!=NULL)
  {
    pMissionLogon->PerformLogonMission(FALSE);
  }
}

VOID CDlgWebRegister::OnEventAccountLogon(CString strAccounts, CString strPassword)
{
  _sntprintf(m_stLogonAccounts.szAccounts,CountArray(m_stLogonAccounts.szAccounts),TEXT("%s"),strAccounts.GetBuffer());
  _sntprintf(m_stLogonAccounts.szPassword,CountArray(m_stLogonAccounts.szPassword),TEXT("%s"),strPassword.GetBuffer());

  CString strm;
  strm.Format(TEXT("OnEventAccountLogon %s,%s"),m_stLogonAccounts.szAccounts,m_stLogonAccounts.szPassword);
  AfxMessageBox(strm,0,0);

  //隐藏窗口
  ShowWindow(SW_HIDE);

  //执行注册
  CMissionLogon * pMissionLogon=CMissionLogon::GetInstance();
  if(pMissionLogon!=NULL)
  {
    pMissionLogon->PerformLogonMission(FALSE);
  }

}
//构造数据
WORD CDlgWebRegister::ConstructWebRegisterPacket(BYTE cbBuffer[], WORD wBufferSize, BYTE cbValidateFlags)
{
  //变量定义
  CMD_GP_LogonAccounts * pLogonAccounts=(CMD_GP_LogonAccounts *)cbBuffer;

  //系统信息
  pLogonAccounts->dwPlazaVersion=VERSION_PLAZA;

  //机器标识
  CWHService::GetMachineIDEx(pLogonAccounts->szMachineID);
  //CWHService::GetMachineIDEx(pLogonAccounts->szMachineID2);
  //登录信息
  lstrcpyn(pLogonAccounts->szPassword,m_stLogonAccounts.szPassword,CountArray(pLogonAccounts->szPassword));
  lstrcpyn(pLogonAccounts->szAccounts,m_stLogonAccounts.szAccounts,CountArray(pLogonAccounts->szAccounts));
  pLogonAccounts->cbValidateFlags=cbValidateFlags;

  CGlobalWebLink * pGlobalWebLink=CGlobalWebLink::GetInstance();
  if(pGlobalWebLink != NULL)
  {
    pGlobalWebLink->GetSessionKey(pLogonAccounts->szSessionKey);
  }

  //保存密码
  CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
  tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
  lstrcpyn(pGlobalUserData->szPassword,pLogonAccounts->szPassword,CountArray(pGlobalUserData->szPassword));


  return sizeof(CMD_GP_LogonAccounts);
}
//取消函数
VOID CDlgWebRegister::OnCancel()
{
  //隐藏窗口
  ShowWindow(SW_HIDE);

  //显示登录
  ASSERT(CMissionLogon::GetInstance()!=NULL);
  if(CMissionLogon::GetInstance()!=NULL)
  {
    CMissionLogon::GetInstance()->ShowLogon();
  }

  return;
}
