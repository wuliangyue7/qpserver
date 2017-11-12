#include "Stdafx.h"
#include "Resource.h"
#include "DlgMemberGift.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////////////
//按钮控件
const TCHAR* const szButtonCloseControlName = TEXT("ButtonClose");
const TCHAR* const szButtonTakeControlName = TEXT("ButtonTake");
const TCHAR* const szPageLayoutGiftControlName = TEXT("PageLayoutGift");
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgMemberGift::CDlgMemberGift() : CFGuiDialog(IDD_DLG_POPUP)
{
  m_bMember = false;
  ZeroMemory(m_GiftSub,sizeof(m_GiftSub));
  m_wGiftSubCount = 0;
}

//析构函数
CDlgMemberGift::~CDlgMemberGift()
{
}

//创建函数
BOOL CDlgMemberGift::OnInitDialog()
{
  __super::OnInitDialog();

  //修改样式
  ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX);

  //设置标题
  SetWindowText(TEXT("会员礼包"));

  //居中窗口
  CenterWindow(this);


  return TRUE;
}

//确定函数
VOID CDlgMemberGift::OnOK()
{

  return __super::OnOK();
}

//取消消息
VOID CDlgMemberGift::OnCancel()
{
  return __super::OnCancel();
}

//消息提醒
void CDlgMemberGift::Notify(TNotifyUI &  msg)
{
  //获取对象
  CControlUI * pControlUI = msg.pSender;

  //点击事件
  if(lstrcmp(msg.sType, TEXT("click")) == 0)
  {
    //取前几位
    if(lstrcmp(pControlUI->GetName(), szButtonCloseControlName)==0)
    {
      return OnCancel();
    }
    else if(lstrcmp(pControlUI->GetName(), szButtonTakeControlName)==0)
    {
      return OnOK();
    }
  }

  return;
}

LPCTSTR CDlgMemberGift::GetSkinFile()
{
  return TEXT("DlgMemberGift.xml");
}

VOID CDlgMemberGift::SetMember(bool bMember)
{
  m_bMember =bMember;
}

void CDlgMemberGift::InitControlUI()
{
  UpdateUI();

}

VOID CDlgMemberGift::UpdateUI()
{
  CControlUI * pContorl1 = (CControlUI*)GetControlByName(szButtonTakeControlName);
  pContorl1->SetEnabled(m_bMember);

  int nGiftcount = (int)m_wGiftSubCount;


  const ConfigProperty *pPropConfig = CGlobalUnits::GetInstance()->GetPropertyConfigInfo();

  CPageLayoutUI * pPageLayoutUI = static_cast<CPageLayoutUI *>(GetControlByName(szPageLayoutGiftControlName));
  if(pPageLayoutUI)
  {
    for(int i =0; i<pPropConfig->cbPropertyCount; i++)
    {
      for(int j=0; j<nGiftcount; j++)
      {
        if(pPropConfig->PropertyInfo[i].wIndex == m_GiftSub[j].wPropertyID)
        {
          //创建UI
          CDialogBuilder builder;
          CVerticalLayoutUI* pVerticalLayoutUI =NULL;
          pVerticalLayoutUI = static_cast<CVerticalLayoutUI *>(builder.Create(_T("DlgMemberGiftItem.xml"), (UINT)0, NULL, &m_PaintManager));
          pPageLayoutUI->Add(pVerticalLayoutUI);

          //道具图标
          CLabelUI* pPropertyImage = (CLabelUI*)pVerticalLayoutUI->FindControl(TEXT("ItemPropIOC"));
          if(pPropertyImage != NULL)
          {
            CStdString  csImageKind;
            csImageKind.Format(TEXT("DLG_BOX_SHOP\\GMAE_PROPERTY_%d.png"),pPropConfig->PropertyInfo[i].wIndex);
            pPropertyImage->SetBkImage(csImageKind);
          }

          //道具数量
          CNumberUI* pPropertyNum = (CNumberUI*)pVerticalLayoutUI->FindControl(TEXT("ItemPropNum"));
          if(pPropertyNum != NULL)
          {
            pPropertyNum->SetNumber(m_GiftSub[j].wPropertyCount);
          }
          //道具名字
          CLabelUI* pPropertyName = (CLabelUI*)pVerticalLayoutUI->FindControl(TEXT("ItemPropName"));
          if(pPropertyName != NULL)
          {
            pPropertyName->SetText(pPropConfig->PropertyInfo[i].szName);
          }
        }
      }
    }
  }

  CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
  tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
  CUserItemElement * pUserItemElement=CUserItemElement::GetInstance();
  int nMemberOrder = pGlobalUserData->cbMemberOrder;
  if(nMemberOrder == 0)
  {
    nMemberOrder = 1;
  }
  const tagMemberParameterNew*  pMemberPara = pUserItemElement->GetUserMemberParameter(nMemberOrder);
  if(pPageLayoutUI)
  {
    //创建UI
    CDialogBuilder builder;
    CVerticalLayoutUI* pVerticalLayoutUI =NULL;
    pVerticalLayoutUI = static_cast<CVerticalLayoutUI *>(builder.Create(_T("DlgMemberGiftItem.xml"), (UINT)0, NULL, &m_PaintManager));
    pPageLayoutUI->Add(pVerticalLayoutUI);

    //道具图标
    CLabelUI* pPropertyImage = (CLabelUI*)pVerticalLayoutUI->FindControl(TEXT("ItemPropIOC"));
    if(pPropertyImage != NULL)
    {
      CStdString  csImageKind;
      csImageKind.Format(TEXT("DLG_BOX_CHECKINNEW\\LABEL_MONEY.png"));
      pPropertyImage->SetBkImage(csImageKind);
    }

    //道具数量
    CNumberUI* pPropertyNum = (CNumberUI*)pVerticalLayoutUI->FindControl(TEXT("ItemPropNum"));
    if(pPropertyNum != NULL)
    {
      pPropertyNum->SetVisible(false);
    }

    TCHAR szText[128]= {0};
    _sntprintf(szText, CountArray(szText), TEXT("%d"), pMemberPara->dwMemberDayPresent);

    //道具名字
    CLabelUI* pPropertyName = (CLabelUI*)pVerticalLayoutUI->FindControl(TEXT("ItemPropName"));
    if(pPropertyName != NULL)
    {
      pPropertyName->SetText(szText);
    }
  }

}


VOID CDlgMemberGift::SetData(const  tagGiftPropertyInfo *pGiftSub,DWORD nCount)
{
  m_wGiftSubCount = nCount;
  memcpy(m_GiftSub, pGiftSub, sizeof(tagGiftPropertyInfo)*nCount);
}

//////////////////////////////////////////////////////////////////////////////////