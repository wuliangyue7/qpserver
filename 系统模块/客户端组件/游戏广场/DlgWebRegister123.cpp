#include "StdAfx.h"
#include "GamePlaza.h"
#include "DlgWebRegister.h"
#include "MissionLogon.h"
#include "DlgInformation.h"
//////////////////////////////////////////////////////////////////////////////////

//屏幕位置
#define LAYERED_SIZE        5                 //分层大小

//////////////////////////////////////////////////////////////////////////////////
#define LAST_VERIFY_NONE      0
#define LAST_VERIFY_LOCAL     1
#define LAST_VERIFY_RESULT_FAIL   2
#define LAST_VERIFY_RESULT_SUCESS 3

#define PERFORM_VERIFY_ACCOUNTS   0
#define PERFORM_VERIFY_NICKNAME   1
#define PERFORM_VERIFY_PASSWORD   2
#define PERFORM_VERIFY_TWICE    3


//////////////////////////////////////////////////////////////////////////////////
//响应控件
const TCHAR * szButtonCloseControlName3 = TEXT("ButtonClose");

//////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgWebRegister, CFGuiDialog)
  ON_WM_NCCREATE()
  ON_WM_NCDESTROY()
  //ON_WM_CREATE()


END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgWebRegister::CDlgWebRegister() : CFGuiDialog(IDD_DLG_WEB_REGISTER)
{
  m_enLogonType = en_Account_Logon;

  memset(&m_stLogonAccounts,0, sizeof(m_stLogonAccounts));
  memset(m_szWebRegisterLink,0, sizeof(m_szWebRegisterLink));
  memset(m_szQQLogonLink,0, sizeof(m_szQQLogonLink));


  return;
}

//析构函数
CDlgWebRegister::~CDlgWebRegister()
{
  DestroyWindow();
}

BOOL CDlgWebRegister::PreTranslateMessage(MSG * pMsg)
{
  //按键过虑
  if((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
  {
    return TRUE;
  }

  return CFGuiDialog::PreTranslateMessage(pMsg);
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
  return 1;
}
//配置函数
BOOL CDlgWebRegister::OnInitDialog()
{
  CFGuiDialog::OnInitDialog();

  //设置标题
  SetWindowText(TEXT("用户注册"));

  //设置路径
  TCHAR szDirectory[MAX_PATH]=TEXT("");
  CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

  //配置文件
  TCHAR szFileName[MAX_PATH]=TEXT("");
  _snwprintf(szFileName,sizeof(szFileName),TEXT("%s\\LobbyUrl.ini"),szDirectory);

  //读取地址
  GetPrivateProfileString(TEXT("URL"), TEXT("WebRegister"), TEXT("http://www.7298.com/lobby/Register.aspx"), m_szWebRegisterLink, sizeof(TCHAR)*MAX_PATH, szFileName);
  GetPrivateProfileString(TEXT("URL"), TEXT("QQLogon"), TEXT("http://www.7298.com/lobby/QQLogin.aspx"), m_szQQLogonLink, sizeof(TCHAR)*MAX_PATH, szFileName);

  return TRUE;
}

//取消函数
VOID CDlgWebRegister::OnCancel()
{
  //关闭窗口
  EndDialog(IDCANCEL);

  //显示登录
  ASSERT(CMissionLogon::GetInstance()!=NULL);
  if(CMissionLogon::GetInstance()!=NULL)
  {
    CMissionLogon::GetInstance()->ShowLogon();
  }

  DestroyWindow();

  return;
}

//消息提醒
void CDlgWebRegister::Notify(TNotifyUI &  msg)
{
  //获取对象
  CControlUI * pControlUI = msg.pSender;

  //点击事件
  if(lstrcmp(msg.sType, TEXT("click")) == 0)
  {
    if(lstrcmp(pControlUI->GetName(), szButtonCloseControlName3)==0)
    {
      return OnCancel();
    }

  }

}

LPCTSTR CDlgWebRegister::GetSkinFile()
{
  return TEXT("DlgWebRegister.xml");
}

//初始控件
void CDlgWebRegister::InitControlUI()
{

}


//设置QQ登陆
void CDlgWebRegister::SetQQlogon(bool bCreat, CString strWebBrowser)
{
  //登陆游戏
  CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("LogonRegister"));
  pTabLayout->SelectItem(2);

  m_enLogonType = en_QQ_Logon;

  CUiWebBrowserEx * pActiveXUI = (CUiWebBrowserEx*)m_PaintManager.FindControl(strWebBrowser);

  if(pActiveXUI)
  {
    if(bCreat/* && !m_bLogonCreat*/)
    {
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
  // CControlUI* pRoot = builder.Create(TEXT("Resource//DlgWebRegister.xml"), 0, this, &m_PaintManager, NULL);
  //m_PaintManager.AttachDialog(pRoot);
  //m_PaintManager.AddNotifier(this);


  //登陆游戏
  CTabLayoutUI* pTabLayout = (CTabLayoutUI*)m_PaintManager.FindControl(_T("LogonRegister"));
  pTabLayout->SelectItem(1);
  m_enLogonType = en_Register_Logon;

  CUiWebBrowserEx * pActiveXUI = (CUiWebBrowserEx*)m_PaintManager.FindControl(strWebBrowser);

  if(pActiveXUI)
  {
    if(bCreat/* && !m_bRegtistCreat*/)
    {

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

  //计算大小
  WORD wDataSize = sizeof(CMD_GP_LogonAccounts)-sizeof(pLogonAccounts->szPassPortID);
  wDataSize += CountStringBuffer(pLogonAccounts->szPassPortID);

  return wDataSize;
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
LRESULT CDlgWebRegister::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  return CFGuiDialog::HandleMessage(uMsg,wParam,lParam);
}
CControlUI* CDlgWebRegister::CreateControl(LPCTSTR pstrClass)
{
  AfxMessageBox(pstrClass,0,0);
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
BOOL CDlgWebRegister::OnNcCreate(LPCREATESTRUCT lpCS)
{
  Subclass();

  return CFGuiDialog::OnNcCreate(lpCS);
}

void CDlgWebRegister::OnNcDestroy()
{
  UnSubclass();

  CFGuiDialog::OnNcDestroy();
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
    LRESULT lResult=TRUE;
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
//////////////////////////////////////////////////////////////////////////////////
