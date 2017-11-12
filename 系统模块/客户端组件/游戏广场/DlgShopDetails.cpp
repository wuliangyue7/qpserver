#include "Stdafx.h"
#include "Resource.h"
#include "DlgShopDetails.h"
#include "PlatformEvent.h"
#include "DlgInformation.h"
//////////////////////////////////////////////////////////////////////////////////
//按钮控件
const TCHAR* const szButtonDecrease = TEXT("ButtonDecrease");
const TCHAR* const szButtonIncrease = TEXT("ButtonIncrease");
const TCHAR* const szButtonBuyUse = TEXT("ButtonBuyUse");
const TCHAR* const szButtonBuyIntoBag = TEXT("ButtonBuyIntoBag");
//const TCHAR* const sz = TEXT("ButtonBuyUse");
//const TCHAR* const sz = TEXT("ButtonBuyUse");

//道具信息
const TCHAR* const szLabelPrice = TEXT("LabelPrice");

const TCHAR* const szLabelVIPPrice = TEXT("LabelVIPPrice");
const TCHAR* const szLabelBuyScore = TEXT("LabelBuyScore");
const TCHAR* const szLabelInsureScore = TEXT("LabelInsureScore");
const TCHAR* const szLabelItemShopIOC = TEXT("LabelItemShopIOC");
const TCHAR* const szLableItemShopName = TEXT("LableItemShopName");
const TCHAR* const szNumberBuy = TEXT("NumberBuy");

//VIP控件
const TCHAR* const szContainerVipAwardControlName = TEXT("ContainerVip");
const TCHAR* const szLabelVipLevelControlName = TEXT("LabelVipLevel");
const TCHAR* const szLabelAwardMemberControlName = TEXT("LableShopVip");

const TCHAR* const szRadioButtonGold = TEXT("RadioButtonGold");
const TCHAR* const szRadioButtonUserMadel = TEXT("RadioButtonUserMadel");
const TCHAR* const szRadioButtonCash = TEXT("RadioButtonCash");
const TCHAR* const szRadioButtonLoveLiness = TEXT("RadioButtonLoveLiness");

const TCHAR* const szNumberOfTypeScore = TEXT("NumberOfTypeScore");
const TCHAR* const szLabelBuyTypeName = TEXT("LabelBuyTypeName");

//文字控件
const TCHAR* const szTextUseTipControlName = TEXT("TextlUseTip");

//容器控件
const TCHAR* const szHoriLayoutDiscountControlName = TEXT("HoriLayoutDiscount");
const TCHAR* const szHoriLayoutVIPPriceControlName = TEXT("HoriLayoutVIPPrice");
//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgShopDetails::CDlgShopDetails() : CFGuiDialog(IDD_DLG_POPUP)
{
  //设置任务
  m_MissionManager.InsertMissionItem(this);

  m_ptagPropertyInfo = NULL;
  m_dwBuyNumber = 1;
  m_cbConsumeType = INVALID_BYTE;

  m_pIGamePropertyAction = NULL;
  m_eBuyMode = BUY_INTO_BAG;
}

//析构函数
CDlgShopDetails::~CDlgShopDetails()
{
}

//创建函数
BOOL CDlgShopDetails::OnInitDialog()
{
  __super::OnInitDialog();

  //修改样式
  ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX);

  //设置标题
  SetWindowText(TEXT("商城"));

  //居中窗口
  CenterWindow(this);

  InitUI();

  UpdateUI();

  return TRUE;
}

//确定函数
VOID CDlgShopDetails::OnOK()
{

  return __super::OnOK();
}

//取消消息
VOID CDlgShopDetails::OnCancel()
{
  return __super::OnCancel();
}

//消息提醒
void CDlgShopDetails::Notify(TNotifyUI &  msg)
{
  //获取对象
  CControlUI * pControlUI = msg.pSender;

  //点击事件
  if(lstrcmp(msg.sType, TEXT("click")) == 0)
  {
    //关闭按钮
    if(lstrcmp(pControlUI->GetName(), TEXT("ButtonClose"))==0)
    {
      return OnCancel();
    }

    //
    if(lstrcmp(pControlUI->GetName(), szRadioButtonGold)==0
       ||lstrcmp(pControlUI->GetName(), szRadioButtonUserMadel)==0
       ||lstrcmp(pControlUI->GetName(), szRadioButtonCash)==0
       ||lstrcmp(pControlUI->GetName(), szRadioButtonLoveLiness)==0)
    {
      UpdateUI();
      //  return OnCancel();
    }

    if(pControlUI->GetName() == szButtonDecrease)
    {
      CNumberUI* pNumberBuy = static_cast<CNumberUI *>(GetControlByName(szNumberBuy));
      if(pNumberBuy != NULL)
      {
        m_dwBuyNumber = (WORD)(pNumberBuy->GetNumber());
      }

      if(m_dwBuyNumber == 1)
      {
        CButtonUI * pButtonUI = static_cast<CButtonUI *>(GetControlByName(szButtonDecrease));
        if(pButtonUI != NULL)
        {
          pButtonUI->SetEnabled(false);
        }
      }
      return;
    }

    if(pControlUI->GetName() == szButtonIncrease)
    {
      CNumberUI* pNumberBuy = static_cast<CNumberUI *>(GetControlByName(szNumberBuy));
      if(pNumberBuy != NULL)
      {
        m_dwBuyNumber = (WORD)(pNumberBuy->GetNumber());
      }

      CButtonUI * pButtonUI = static_cast<CButtonUI *>(GetControlByName(szButtonDecrease));
      if(pButtonUI != NULL)
      {
        pButtonUI->SetEnabled(true);
      }

      UpdateUI();
      return;
    }

    if(pControlUI->GetName() == szButtonBuyUse || pControlUI->GetName() == szButtonBuyIntoBag)
    {
      m_eBuyMode = BUY_INTO_BAG;
      if(pControlUI->GetName() == szButtonBuyUse)
      {
        m_eBuyMode = BUY_IMMEDIATELY_USE;
      }
      tagBuyGamePropertyInfo BuyGamePropertyInfo;
      BuyGamePropertyInfo.dwBuyNumber = m_dwBuyNumber;
      BuyGamePropertyInfo.cbScoreType = m_cbConsumeType;
      BuyGamePropertyInfo.wPropertyID = m_ptagPropertyInfo->wIndex;
      BuyGamePropertyInfo.cbUse = m_eBuyMode;

      m_pIGamePropertyAction->PerformBuyGameProperty(&BuyGamePropertyInfo);
      return;
    }
  }

  return;
}

LPCTSTR CDlgShopDetails::GetSkinFile()
{
  return TEXT("DlgShopDetails.xml");
}

VOID CDlgShopDetails::InitData()
{

}

VOID CDlgShopDetails::LoadDataFromMemory()
{

}

VOID CDlgShopDetails::LoadDataFromUI()
{
  return;
}

VOID CDlgShopDetails::UpdateData()
{

}

VOID CDlgShopDetails::InitUI()
{
  //获取对象
  CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
  tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

  //定义变量
  CRadioButtonUI *pRadioButtonUIGold, *pRadioButtonUIUserMedal, *pRadioButtonUICash, *pRadioButtonUILoveLiness;

  //获取控件
  pRadioButtonUIGold = static_cast<CRadioButtonUI *>(GetControlByName(szRadioButtonGold));
  pRadioButtonUIUserMedal = static_cast<CRadioButtonUI *>(GetControlByName(szRadioButtonUserMadel));
  pRadioButtonUICash = static_cast<CRadioButtonUI *>(GetControlByName(szRadioButtonCash));
  pRadioButtonUILoveLiness = static_cast<CRadioButtonUI *>(GetControlByName(szRadioButtonLoveLiness));

  //设置按钮
  if(m_ptagPropertyInfo->lPropertyGold == 0 && pRadioButtonUIGold != NULL)
  {
    pRadioButtonUIGold->SetVisible(false);
  }
  if(m_ptagPropertyInfo->lPropertyUserMedal == 0 && pRadioButtonUIUserMedal != NULL)
  {
    pRadioButtonUIUserMedal->SetVisible(false);
  }
  if(m_ptagPropertyInfo->dPropertyCash == 0 && pRadioButtonUICash != NULL)
  {
    pRadioButtonUICash->SetVisible(false);
  }
  if(m_ptagPropertyInfo->lPropertyLoveLiness == 0 && pRadioButtonUILoveLiness != NULL)
  {
    pRadioButtonUILoveLiness->SetVisible(false);
  }

  if(m_cbConsumeType==0)
  {
    if(m_ptagPropertyInfo->lPropertyGold != 0 && pRadioButtonUIGold != NULL && pRadioButtonUIGold->IsEnabled() == true)
    {
      pRadioButtonUIGold->SetCheck(true);
    }
    else if(m_ptagPropertyInfo->lPropertyUserMedal != 0 && pRadioButtonUIUserMedal != NULL && pRadioButtonUIUserMedal->IsEnabled() == true)
    {
      pRadioButtonUIUserMedal->SetCheck(true);
    }
    else if(m_ptagPropertyInfo->dPropertyCash != 0 && pRadioButtonUICash != NULL && pRadioButtonUICash->IsEnabled() == true)
    {
      pRadioButtonUICash->SetCheck(true);
    }
    else if(m_ptagPropertyInfo->lPropertyLoveLiness != 0 && pRadioButtonUILoveLiness != NULL && pRadioButtonUILoveLiness->IsEnabled() == true)
    {
      pRadioButtonUILoveLiness->SetCheck(true);
    }

  }
  else
  {
    if(pRadioButtonUIGold)
    {
      pRadioButtonUIGold->SetCheck(m_cbConsumeType == CONSUME_TYPE_GOLD);
    }
    if(pRadioButtonUIUserMedal)
    {
      pRadioButtonUIUserMedal->SetCheck(m_cbConsumeType == CONSUME_TYPE_USEER_MADEL);
    }
    if(pRadioButtonUICash)
    {
      pRadioButtonUICash->SetCheck(m_cbConsumeType == CONSUME_TYPE_CASH);
    }
    if(pRadioButtonUILoveLiness)
    {
      pRadioButtonUILoveLiness->SetCheck(m_cbConsumeType == CONSUME_TYPE_LOVELINESS);
    }
  }


  //折扣显示
  CHorizontalLayoutUI* pHoriLayoutDiscount = static_cast<CHorizontalLayoutUI*>(GetControlByName(szHoriLayoutDiscountControlName));
  if(pHoriLayoutDiscount)
  {
    pHoriLayoutDiscount->SetVisible(pGlobalUserData->cbMemberOrder>0);
  }

  //折后价格
  CHorizontalLayoutUI* HoriLayoutVIPPrice = static_cast<CHorizontalLayoutUI*>(GetControlByName(szHoriLayoutVIPPriceControlName));
  if(HoriLayoutVIPPrice)
  {
    HoriLayoutVIPPrice->SetVisible(pGlobalUserData->cbMemberOrder>0);
  }

  //设置文本
  TCHAR szText[128];
  _sntprintf(szText, CountArray(szText), TEXT("%I64d游戏币"), m_ptagPropertyInfo->lPropertyGold);
  pRadioButtonUIGold->SetText(szText);
  _sntprintf(szText, CountArray(szText), TEXT("%I64d元宝"), m_ptagPropertyInfo->lPropertyUserMedal);
  pRadioButtonUIUserMedal->SetText(szText);
  _sntprintf(szText, CountArray(szText), TEXT("%0.2f游戏豆"), m_ptagPropertyInfo->dPropertyCash);
  pRadioButtonUICash->SetText(szText);
  _sntprintf(szText, CountArray(szText), TEXT("%I64d魅力值"), m_ptagPropertyInfo->lPropertyLoveLiness);
  pRadioButtonUILoveLiness->SetText(szText);

  //道具图标
  CLabelUI *pLabel = static_cast<CLabelUI *>(GetControlByName(szLabelItemShopIOC));
  if(pLabel != NULL)
  {
    CStdString  csImageKind;
    csImageKind.Format(TEXT("DLG_BOX_SHOP\\GMAE_PROPERTY_%d.png"),m_ptagPropertyInfo->wIndex);
    if(m_PaintManager.GetImageEx(csImageKind,NULL)==NULL)
    {
      csImageKind.Empty();
    }

    if(!csImageKind.IsEmpty())
    {
      pLabel->SetBkImage(csImageKind);
    }
  }

  //道具名称
  pLabel = static_cast<CLabelUI *>(GetControlByName(szLableItemShopName));
  if(pLabel != NULL)
  {
    pLabel->SetText(m_ptagPropertyInfo->szName);
  }


  TCHAR szTaskRewardTotal[64]=TEXT("");
  if(pGlobalUserData->cbMemberOrder > 0)
  {

    //会员任务
    const tagMemberParameterNew* pMemberParameter = CUserItemElement::GetInstance()->GetUserMemberParameter(pGlobalUserData->cbMemberOrder);
    if(pMemberParameter != NULL)
    {
      _sntprintf(szTaskRewardTotal,CountArray(szTaskRewardTotal),TEXT("%d%%折扣"),pMemberParameter->dwMemberShop);
    }

    CControlUI * pContainerVip = GetControlByName(szContainerVipAwardControlName);
    if(pContainerVip!=NULL)
    {
      //会员等级
      for(int i=0; i<6; i++)
      {
        CStdString strLevel;
        strLevel.SmallFormat(TEXT("%s%d"),szLabelVipLevelControlName,i);
        CControlUI * pLabelVipUI = pContainerVip->FindControl(strLevel);
        if(pLabelVipUI)
        {
          if(i==pGlobalUserData->cbMemberOrder)
          {
            pLabelVipUI->SetVisible(true);
          }
          else
          {
            pLabelVipUI->SetVisible(false);
          }
        }
      }

      CControlUI * pControl4 = pContainerVip->FindControl(szLabelAwardMemberControlName);
      if(pControl4 !=NULL)
      {
        pControl4->SetVisible(true);
        pControl4->SetText(szTaskRewardTotal);
      }
    }

  }
  else
  {
    CControlUI * pContainerVip = GetControlByName(szContainerVipAwardControlName);
    if(pContainerVip !=NULL)
    {
      pContainerVip->SetVisible(false);
    }
  }

  ////VIP折扣
  //pLabel = static_cast<CLabelUI *>(GetControlByName(szLableShopVip));
  //if(pGlobalUserData->cbMemberOrder > 0)
  //{
  //  _sntprintf(szText, CountArray(szText), TEXT("VIP %0.2f折"), (double)m_ptagPropertyInfo->wDiscount/10.00);
  //  if(pLabel != NULL) pLabel->SetText(szText);
  //}
  //else
  //{
  //  if(pLabel != NULL) pLabel->SetText(TEXT(""));
  //}

  //设置按钮
  CButtonUI * pButtonUI = static_cast<CButtonUI *>(GetControlByName(szButtonDecrease));
  if(pButtonUI != NULL)
  {
    pButtonUI->SetEnabled(false);
  }

  //按钮可用
  bool bEnbale = IsImmediatelyUseProp(m_ptagPropertyInfo);
  CButtonUI* pUseImmediately = static_cast<CButtonUI*>(GetControlByName(szButtonBuyUse));
  if(pUseImmediately != NULL)
  {
    pUseImmediately->SetEnabled(bEnbale);
  }

  //功能说明
  CTextUI* pLabelUseTip = static_cast<CTextUI*>(GetControlByName(szTextUseTipControlName));
  if(pLabelUseTip != NULL)
  {
    if(_tcslen(m_ptagPropertyInfo->szRegulationsInfo) == 0)
    {
      pLabelUseTip->SetText(TEXT("暂无说明"));
    }
    else
    {
      pLabelUseTip->SetText(m_ptagPropertyInfo->szRegulationsInfo);
    }
  }
  return;
}

VOID CDlgShopDetails::UpdateUI()
{
  if(m_ptagPropertyInfo == NULL)
  {
    return;
  }

  //定义变量
  CLabelUI *pLablePrice, *pLabelVIPPrice, *pLabelBuyScore;
  CRadioButtonUI *pRadioButtonUIGold, *pRadioButtonUIUserMedal, *pRadioButtonUICash, *pRadioButtonUILoveLiness;

  TCHAR szText[128] = {0};

  //获取控件
  pLablePrice = static_cast<CLabelUI *>(GetControlByName(szLabelPrice));
  pLabelVIPPrice = static_cast<CLabelUI *>(GetControlByName(szLabelVIPPrice));
  pLabelBuyScore = static_cast<CLabelUI *>(GetControlByName(szLabelBuyScore));
  CNumberUI* pNumberBuy = static_cast<CNumberUI *>(GetControlByName(szNumberBuy));

  pRadioButtonUIGold = static_cast<CRadioButtonUI *>(GetControlByName(szRadioButtonGold));
  pRadioButtonUIUserMedal = static_cast<CRadioButtonUI *>(GetControlByName(szRadioButtonUserMadel));
  pRadioButtonUICash = static_cast<CRadioButtonUI *>(GetControlByName(szRadioButtonCash));
  pRadioButtonUILoveLiness = static_cast<CRadioButtonUI *>(GetControlByName(szRadioButtonLoveLiness));

  ASSERT(pLablePrice != NULL && pLabelVIPPrice != NULL && pLabelBuyScore != NULL && pNumberBuy != NULL);
  if(pLablePrice == NULL || pLabelVIPPrice == NULL || pLabelBuyScore == NULL || pNumberBuy == NULL)
  {
    return;
  }

  //购买数量
  //_sntprintf(szText, CountArray(szText), TEXT("%d"), m_dwBuyNumber);
  //pLableNum->SetText(szText);
  if(pNumberBuy != NULL)
  {
    pNumberBuy->SetMinNumber(1);
  }

  //获取对象
  CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
  tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

  //会员折扣
  WORD wDiscount = 0;
  const tagMemberParameterNew* pMemberParameter = CUserItemElement::GetInstance()->GetUserMemberParameter(pGlobalUserData->cbMemberOrder);
  if(pMemberParameter != NULL)
  {
    wDiscount = (WORD)pMemberParameter->dwMemberShop;
  }
  //金币购买
  if(pRadioButtonUIGold != NULL && pRadioButtonUIGold->GetCheck() == true)
  {
    //道具原价
    _sntprintf(szText, CountArray(szText), TEXT("%I64d游戏币/个"), m_ptagPropertyInfo->lPropertyGold);
    pLablePrice->SetText(szText);

    //会员价格
    _sntprintf(szText, CountArray(szText), TEXT("%I64d游戏币/个"), m_ptagPropertyInfo->lPropertyGold * wDiscount / 100);
    pLabelVIPPrice->SetText(szText);

    //购买金额
    LONGLONG lBuyScore;
    LONG BuyOneScore = 0;
    if(pGlobalUserData->cbMemberOrder == 0)
    {
      BuyOneScore = (LONG)(m_ptagPropertyInfo->lPropertyGold);
    }
    else
    {
      BuyOneScore = (LONG)(m_ptagPropertyInfo->lPropertyGold * ((double)wDiscount / 100.00));
    }
    lBuyScore = BuyOneScore * m_dwBuyNumber;
    _sntprintf(szText, CountArray(szText), TEXT("%I64d游戏币"), lBuyScore);
    pLabelBuyScore->SetText(szText);

    m_cbConsumeType = CONSUME_TYPE_GOLD;

    CLabelUI* LabelBuyTypeName = static_cast<CLabelUI*>(GetControlByName(szLabelBuyTypeName));
    if(LabelBuyTypeName != NULL)
    {
      LabelBuyTypeName->SetText(TEXT("银行存款："));
    }

    //银行金币
    CNumberUI *pNumberUI = static_cast<CNumberUI *>(GetControlByName(szNumberOfTypeScore));
    if(pNumberUI != NULL)
    {
      //获取对象
      CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
      tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

      float fUserInsure = (float)(pGlobalUserData->lUserInsure);
      pNumberUI->SetNumberDecimal(false);
      pNumberUI->SetNumber(fUserInsure);
    }



    if(pNumberBuy != NULL)
    {
      pNumberBuy->SetMaxNumber((int)(max(pGlobalUserData->lUserInsure/BuyOneScore, 1)));
    }

    CButtonUI* pButtonIncrease = static_cast<CButtonUI*>(GetControlByName(szButtonIncrease));
    if(pButtonIncrease != NULL)
    {
      pButtonIncrease->SetEnabled(pGlobalUserData->lUserInsure/BuyOneScore > 1);
    }
  }

  //元宝购买
  if(pRadioButtonUIUserMedal != NULL && pRadioButtonUIUserMedal->GetCheck() == true)
  {
    //道具原价
    _sntprintf(szText, CountArray(szText), TEXT("%I64d元宝/个"), m_ptagPropertyInfo->lPropertyUserMedal);
    pLablePrice->SetText(szText);

    //会员价格
    _sntprintf(szText, CountArray(szText), TEXT("%I64d元宝/个"), m_ptagPropertyInfo->lPropertyUserMedal * wDiscount / 100);
    pLabelVIPPrice->SetText(szText);

    //购买金额
    LONGLONG lBuyScore;
    LONG BuyOneScore = 0;
    if(pGlobalUserData->cbMemberOrder == 0)
    {
      BuyOneScore = (LONG)(m_ptagPropertyInfo->lPropertyUserMedal);
    }
    else
    {
      BuyOneScore = (LONG)(m_ptagPropertyInfo->lPropertyUserMedal * ((double)wDiscount / 100.00));
    }
    lBuyScore = BuyOneScore * m_dwBuyNumber;
    _sntprintf(szText, CountArray(szText), TEXT("%I64d元宝"), lBuyScore);
    pLabelBuyScore->SetText(szText);

    m_cbConsumeType = CONSUME_TYPE_USEER_MADEL;


    CLabelUI* LabelBuyTypeName = static_cast<CLabelUI*>(GetControlByName(szLabelBuyTypeName));
    if(LabelBuyTypeName != NULL)
    {
      LabelBuyTypeName->SetText(TEXT("元    宝："));
    }

    CNumberUI *pNumberUI = static_cast<CNumberUI *>(GetControlByName(szNumberOfTypeScore));
    if(pNumberUI != NULL)
    {
      float fUserIngot = (float)(pGlobalUserData->lUserIngot);
      pNumberUI->SetNumberDecimal(false);
      pNumberUI->SetNumber(fUserIngot);
    }

    if(pNumberBuy != NULL)
    {
      pNumberBuy->SetMaxNumber((int)(pGlobalUserData->lUserIngot/BuyOneScore));
    }

    CButtonUI* pButtonIncrease = static_cast<CButtonUI*>(GetControlByName(szButtonIncrease));
    if(pButtonIncrease != NULL)
    {
      pButtonIncrease->SetEnabled(pGlobalUserData->lUserIngot/BuyOneScore > 1);
    }
  }

  //游戏豆购买
  if(pRadioButtonUICash != NULL && pRadioButtonUICash->GetCheck() == true)
  {
    //道具原价
    _sntprintf(szText, CountArray(szText), TEXT("%0.2f游戏豆/个"), m_ptagPropertyInfo->dPropertyCash);
    pLablePrice->SetText(szText);

    //会员价格
    _sntprintf(szText, CountArray(szText), TEXT("%0.2f游戏豆/个"), m_ptagPropertyInfo->dPropertyCash * ((double)wDiscount / 100.00));
    pLabelVIPPrice->SetText(szText);

    //购买金额
    double lBuyScore;
    LONG BuyOneScore = 0;
    if(pGlobalUserData->cbMemberOrder == 0)
    {
      BuyOneScore = (LONG)(m_ptagPropertyInfo->dPropertyCash);
    }
    else
    {
      BuyOneScore = (LONG)(m_ptagPropertyInfo->dPropertyCash * ((double)wDiscount / 100.00));
    }
    lBuyScore = BuyOneScore * m_dwBuyNumber;
    _sntprintf(szText, CountArray(szText), TEXT("%0.2f游戏豆"), lBuyScore);
    pLabelBuyScore->SetText(szText);

    m_cbConsumeType = CONSUME_TYPE_CASH;

    CLabelUI* LabelBuyTypeName = static_cast<CLabelUI*>(GetControlByName(szLabelBuyTypeName));
    if(LabelBuyTypeName != NULL)
    {
      LabelBuyTypeName->SetText(TEXT("  游戏豆："));
    }

    CNumberUI *pNumberUI = static_cast<CNumberUI *>(GetControlByName(szNumberOfTypeScore));
    if(pNumberUI != NULL)
    {
      float fUserBeans = (float)(pGlobalUserData->dUserBeans);
      pNumberUI->SetNumberDecimal(true);
      pNumberUI->SetNumber(fUserBeans);

    }

    if(pNumberBuy != NULL)
    {
      pNumberBuy->SetMaxNumber((int)(pGlobalUserData->dUserBeans/BuyOneScore));
    }

    CButtonUI* pButtonIncrease = static_cast<CButtonUI*>(GetControlByName(szButtonIncrease));
    if(pButtonIncrease != NULL)
    {
      pButtonIncrease->SetEnabled(pGlobalUserData->dUserBeans/BuyOneScore > 1);
    }
  }

  //魅力值购买
  if(pRadioButtonUILoveLiness != NULL && pRadioButtonUILoveLiness->GetCheck() == true)
  {
    //道具原价
    _sntprintf(szText, CountArray(szText), TEXT("%I64d魅力值/个"), m_ptagPropertyInfo->lPropertyLoveLiness);
    pLablePrice->SetText(szText);

    //会员价格
    _sntprintf(szText, CountArray(szText), TEXT("%I64d魅力值/个"), m_ptagPropertyInfo->lPropertyLoveLiness * wDiscount / 100);
    pLabelVIPPrice->SetText(szText);

    //购买金额
    LONGLONG lBuyScore;
    LONG BuyOneScore = 0;
    if(pGlobalUserData->cbMemberOrder == 0)
    {
      BuyOneScore = (LONG)(m_ptagPropertyInfo->lPropertyLoveLiness);
    }
    else
    {
      BuyOneScore = (LONG)(m_ptagPropertyInfo->lPropertyLoveLiness * ((double)wDiscount / 100.00));
    }
    lBuyScore = BuyOneScore * m_dwBuyNumber;

    _sntprintf(szText, CountArray(szText), TEXT("%I64d魅力值"), lBuyScore);
    pLabelBuyScore->SetText(szText);

    m_cbConsumeType = CONSUME_TYPE_LOVELINESS;

    CLabelUI* LabelBuyTypeName = static_cast<CLabelUI*>(GetControlByName(szLabelBuyTypeName));
    if(LabelBuyTypeName != NULL)
    {
      LabelBuyTypeName->SetText(TEXT("  魅力值："));
    }

    CNumberUI *pNumberUI = static_cast<CNumberUI *>(GetControlByName(szNumberOfTypeScore));
    if(pNumberUI != NULL)
    {
      float fLoveLiness = (float)(pGlobalUserData->dwLoveLiness);
      pNumberUI->SetNumberDecimal(false);
      pNumberUI->SetNumber(fLoveLiness);
    }

    if(pNumberBuy != NULL)
    {
      pNumberBuy->SetMaxNumber(pGlobalUserData->dwLoveLiness/BuyOneScore);
    }

    CButtonUI* pButtonIncrease = static_cast<CButtonUI*>(GetControlByName(szButtonIncrease));
    if(pButtonIncrease != NULL)
    {
      pButtonIncrease->SetEnabled(pGlobalUserData->dwLoveLiness/BuyOneScore > 1);
    }
  }

  return;
}


//连接事件
bool CDlgShopDetails::OnEventMissionLink(INT nErrorCode)
{
  //连接结果
  if(nErrorCode!=0)
  {
    //重试任务
    if(m_MissionManager.AvtiveMissionItem(this,true)==true)
    {
      return true;
    }

    //错误提示
    CDlgInformation Information(NULL);
    Information.ShowMessageBox(TEXT("连接服务器超时，道具信息获取失败！"),MB_OK|MB_ICONSTOP,30);

    return true;
  }

  //获取对象
  CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
  tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

  //定义变量
  CMD_GP_C_PropertyBuy PropertyBuy;
  PropertyBuy.wItemCount = m_dwBuyNumber;
  PropertyBuy.cbConsumeType = m_cbConsumeType;
  PropertyBuy.wPropertyIndex = m_ptagPropertyInfo->wIndex;
  PropertyBuy.dwUserID = pGlobalUserData->dwUserID;

  m_MissionManager.SendData(MDM_GP_USER_SERVICE, SUB_GP_PROPERTY_BUY, &PropertyBuy, sizeof(CMD_GP_C_PropertyBuy));

  return true;
}

//关闭事件
bool CDlgShopDetails::OnEventMissionShut(BYTE cbShutReason)
{
  //提示消息
  if(cbShutReason!=SHUT_REASON_NORMAL)
  {
    //重试任务
    if(m_MissionManager.AvtiveMissionItem(this,true)==true)
    {
      return true;
    }

    //错误提示
    CDlgInformation Information(NULL);
    Information.ShowMessageBox(TEXT("连接服务器超时，道具信息获取失败！"),MB_OK|MB_ICONSTOP,30);
  }


  return true;
}

//读取事件
bool CDlgShopDetails::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
  if(Command.wMainCmdID == MDM_GP_PROPERTY)
  {
    if(Command.wSubCmdID == SUB_GP_PROPERTY_BUY_RESULT)
    {
      //效验参数
      ASSERT(wDataSize == sizeof(CMD_GP_S_PropertySuccess));
      if(wDataSize != sizeof(CMD_GP_S_PropertySuccess))
      {
        return false;
      }

      //终止任务
      m_MissionManager.ConcludeMissionItem(this,false);

      //定义变量
      CMD_GP_S_PropertySuccess * pPropertySuccess=(CMD_GP_S_PropertySuccess *)pData;

      if(pPropertySuccess->szNotifyContent[0] != 0)
      {
        //错误提示
        CDlgInformation Information(this);
        Information.ShowMessageBox(pPropertySuccess->szNotifyContent,MB_OK|MB_ICONERROR,30);

        return true;
      }
      //获取对象
      CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
      tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

      pGlobalUserData->lUserInsure = pPropertySuccess->lInsureScore;
      pGlobalUserData->lUserIngot = pPropertySuccess->lUserMedal;
      pGlobalUserData->dwLoveLiness = (DWORD)(pPropertySuccess->lLoveLiness);
      pGlobalUserData->dUserBeans = pPropertySuccess->dCash;
      pGlobalUserData->cbMemberOrder = pPropertySuccess->cbCurrMemberOrder;

      //发送事件
      CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
      if(pPlatformEvent!=NULL)
      {
        pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE, 0L);
      }

      //错误提示
      CDlgInformation Information(this);
      Information.ShowMessageBox(TEXT("购买成功！"),MB_OK|MB_ICONINFORMATION,30);
    }
  }
  return true;
}

//道具信息
void CDlgShopDetails::SetPropertyInfo(tagPropertyInfo *ptagPropertyInfo)
{
  m_ptagPropertyInfo = ptagPropertyInfo;
  return;
}

//
void CDlgShopDetails::SetShopMainSink(IGamePropertyAction* pIGamePropertyAction)
{
  m_pIGamePropertyAction = pIGamePropertyAction;
  return;
}

bool CDlgShopDetails::IsImmediatelyUseProp(tagPropertyInfo *ptagPropertyInfo)
{
  if(ptagPropertyInfo == NULL)
  {
    return true;
  }
  //礼物 和 大小喇叭 不可以购买立即使用
  if(ptagPropertyInfo->wKind == PROP_KIND_GIFT ||
     ptagPropertyInfo->wKind == PROP_KIND_TRUMPET ||
     ptagPropertyInfo->wKind == PROP_KIND_TYPHON)
  {
    return false;
  }
  return true;
}

void CDlgShopDetails::SetDefaultBuy(int nValue)
{
  switch(nValue)
  {
    case 1:
    {
      m_cbConsumeType= CONSUME_TYPE_CASH;
      break;
    }
    case 2:
    {
      m_cbConsumeType= CONSUME_TYPE_GOLD;
      break;
    }
    case 3:
    {
      m_cbConsumeType= CONSUME_TYPE_LOVELINESS;
      break;
    }
    case 4:
    {
      m_cbConsumeType= CONSUME_TYPE_USEER_MADEL;
      break;
    }
    default:
      m_cbConsumeType = 0;
  }

}

//////////////////////////////////////////////////////////////////////////////////