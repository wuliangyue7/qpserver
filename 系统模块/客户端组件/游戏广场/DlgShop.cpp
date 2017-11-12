#include "Stdafx.h"
#include "Resource.h"
#include "DlgShop.h"
#include "PlatformEvent.h"
#include "DlgShopDetails.h"
#include "PlatformFrame.h"
#include "PlatformEvent.h"
#include "DlgInformation.h"
#include "GlobalUnits.h"
//////////////////////////////////////////////////////////////////////////////////
//按钮控件
const TCHAR* const szButtonCloseControlName = TEXT("ButtonClose");
const TCHAR* const szButtonButtonHelpControlName = TEXT("ButtonButtonHelp");

//道具控件
const TCHAR* const szPageLayoutAllProperty = TEXT("PageLayoutAllProperty");
const TCHAR* const szPageLayoutCashProperty = TEXT("PageLayoutCashProperty");
const TCHAR* const szPageLayoutGoldProperty = TEXT("PageLayoutGoldProperty");
const TCHAR* const szPageLayoutLoveLinessProperty = TEXT("PageLayoutLoveLinessProperty");
const TCHAR* const szPageLayoutUserMedalProperty = TEXT("PageLayoutUserMedalProperty");

//道具属性控件
const TCHAR* const szLableItemShopIOC = TEXT("ItemShopIOC");
const TCHAR* const szLableItemShopName = TEXT("ItemShopName");
const TCHAR* const szLableItemShopPrice = TEXT("ItemShopPrice");
//VIP控件
const TCHAR* const szLableItemShopVip = TEXT("ItemShopVip");
const TCHAR* const szItemContainerVipControlName = TEXT("ItemContainerVip");
const TCHAR* const szLabelVipLevelControlName = TEXT("LabelVipLevel");
//按钮控件
const TCHAR* const szButtonMyBag = TEXT("ButtonMyBag");
const TCHAR* const szButtonRecharge = TEXT("ButtonRecharge");
const TCHAR* const szButtonMyInsure = TEXT("ButtonMyInsure");
//文本控件
const TCHAR* const szTextShopLink = TEXT("TextShopLink");

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgShopMain::CDlgShopMain() : CFGuiDialog(IDD_DLG_POPUP)
{
  m_pDlgShopDetails = NULL;
  //设置任务
  m_bQueryProperty = false;
  m_bBuyProperty = false;
  m_bUseProperty = false;
  ZeroMemory(&m_PropertyUseInfo, sizeof(m_PropertyUseInfo));

}

//析构函数
CDlgShopMain::~CDlgShopMain()
{
}

void CDlgShopMain::EndDialog()
{
  if(m_pDlgShopDetails!=NULL)
  {
    m_pDlgShopDetails->EndDialog(IDOK);
  }
  __super::EndDialog(IDOK);
}

//创建函数
BOOL CDlgShopMain::OnInitDialog()
{
  //修改样式
  ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX);

  //设置标题
  SetWindowText(TEXT("商城中心"));

  //居中窗口
  /*CenterWindow(this);*/



  return TRUE;
}

//确定函数
VOID CDlgShopMain::OnOK()
{
  //return __super::OnOK();
}

//取消消息
VOID CDlgShopMain::OnCancel()
{
  return __super::OnCancel();
}


//消息提醒
void CDlgShopMain::Notify(TNotifyUI &  msg)
{
  //获取对象
  CControlUI * pControlUI = msg.pSender;

  //点击事件
  if(lstrcmp(msg.sType, TEXT("click")) == 0)
  {
    //关闭按钮
    if(lstrcmp(pControlUI->GetName(), szButtonCloseControlName)==0)
    {
      return OnCancel();
    }

    //背包按钮
    if(lstrcmp(pControlUI->GetName(), szButtonMyBag)==0)
    {
      CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
      if(pPlatformEvent!=NULL)
      {
        pPlatformEvent->PostPlatformEvent(EVENT_USER_SHOW_BAG,0L);
      }

      OnCancel();
    }

    //银行按钮
    if(lstrcmp(pControlUI->GetName(), szButtonMyInsure)==0)
    {
      //发送事件
      CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
      if(pPlatformEvent!=NULL)
      {
        pPlatformEvent->PostPlatformEvent(EVENT_USER_SHOW_MY_INSURE,0L);
      }

      OnCancel();
    }

    //兑换按钮
    if(lstrcmp(pControlUI->GetName(), szButtonRecharge)==0)
    {
      CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
      if(pPlatformEvent!=NULL)
      {
        pPlatformEvent->PostPlatformEvent(EVENT_USER_SHOW_RECHARGE,0L);
      }

      OnCancel();
    }

    //道具响应
    if(lstrcmp(pControlUI->GetName(), TEXT("ItemGameSelect"))==0)
    {
      CDlgShopDetails dlgShopDetails;
      m_pDlgShopDetails = &dlgShopDetails;
      dlgShopDetails.SetPropertyInfo((tagPropertyInfo *)pControlUI->GetTag());
      dlgShopDetails.SetShopMainSink((IGamePropertyAction*)this);
      CTabLayoutUI* pControl = static_cast<CTabLayoutUI*>(GetControlByName(TEXT("TabLayoutShop")));
      if(pControl)
      {
        dlgShopDetails.SetDefaultBuy(pControl->GetCurSel());
      }
      dlgShopDetails.DoModal();
      m_pDlgShopDetails = NULL;
      return;
    }

  }
  else if(msg.sType == TEXT("tabselect"))
  {
    if(pControlUI->GetName() == TEXT("TabLayoutShop"))
    {
      CTextUI* pTextShopLink = static_cast<CTextUI*>(GetControlByName(szTextShopLink));
      if(pTextShopLink != NULL)
      {
        if(msg.wParam >= 0 && msg.wParam < 5)
        {
          TCHAR szLinkName[][16]= {TEXT("游戏豆"), TEXT("游戏豆"), TEXT("游戏币"), TEXT("魅力值"), TEXT("元宝")};
          CStdString sLinkText;
          sLinkText.Format(TEXT("<a name=\'GetBeanLink\' linkcolor=\'#FF25A5FF\' linkhovercolor=\'#FF0585CF\'>如何获取%s？</a>"), szLinkName[msg.wParam]);
          pTextShopLink->SetText(sLinkText);
        }
      }
    }
  }
  else if(msg.sType == TEXT("link"))
  {
    CStdString sLinkName = (LPCTSTR)msg.lParam;
    if(sLinkName == TEXT("GetBeanLink"))
    {
      //获取对象
      ASSERT(CGlobalWebLink::GetInstance()!=NULL);
      CGlobalWebLink * pGlobalWebLink=CGlobalWebLink::GetInstance();

      //构造地址
      TCHAR szLinkInfo[256]=TEXT("");
      _sntprintf(szLinkInfo,CountArray(szLinkInfo),TEXT("%s/%sUserService/UserService4.aspx"),pGlobalWebLink->GetPlatformLink(),pGlobalWebLink->GetValidateLink());
      //打开页面
      ShellExecute(NULL,TEXT("OPEN"),szLinkInfo,NULL,NULL,SW_NORMAL);
    }
  }

  return;
}

LPCTSTR CDlgShopMain::GetSkinFile()
{
  return TEXT("DlgShop.xml");
}

VOID CDlgShopMain::InitData()
{

}

VOID CDlgShopMain::LoadDataFromMemory()
{

}

VOID CDlgShopMain::LoadDataFromUI()
{
  m_pPageLayoutUI[0] = static_cast<CPageLayoutUI *>(GetControlByName(szPageLayoutAllProperty));
  m_pPageLayoutUI[1] = static_cast<CPageLayoutUI *>(GetControlByName(szPageLayoutCashProperty));
  m_pPageLayoutUI[2] = static_cast<CPageLayoutUI *>(GetControlByName(szPageLayoutGoldProperty));
  m_pPageLayoutUI[3] = static_cast<CPageLayoutUI *>(GetControlByName(szPageLayoutLoveLinessProperty));
  m_pPageLayoutUI[4] = static_cast<CPageLayoutUI *>(GetControlByName(szPageLayoutUserMedalProperty));

  for(int i = 0; i < 5; ++i)
  {
    if(m_pPageLayoutUI[i] != NULL)
    {
      m_pPageLayoutUI[i]->RemoveAll();
    }
  }

  return;
}

VOID CDlgShopMain::UpdateData()
{

}

VOID CDlgShopMain::InitUI()
{

}

VOID CDlgShopMain::UpdateUI()
{

}

//查询事件
VOID CDlgShopMain::PerformQueryInfo()
{
  return;
}

//查询事件
VOID CDlgShopMain::PerformBuyGameProperty(tagBuyGamePropertyInfo *ptagBuyGamePropertyInfo)
{
  return;
}

VOID CDlgShopMain::PerformUseGameProperty(const tagUseGamePropertyInfo *pPropertyUseInfo)
{
  return;
}
//道具属性
VOID CDlgShopMain::SetPropertyInfo(CPageLayoutUI* pPageLayoutUI, tagPropertyInfo *ptagPropertyInfo, BYTE cbIndex)
{

  CDialogBuilder builder;
  CVerticalLayoutUI* pVerticalLayoutUI =NULL;

  try
  {
    //创建子项
    pVerticalLayoutUI = static_cast<CVerticalLayoutUI *>(builder.Create(_T("DlgShopItem.xml"), (UINT)0, NULL, &m_PaintManager));
    if(pVerticalLayoutUI==NULL)
    {
      throw TEXT("内存不足！");
    }

  }
  catch(...)
  {
    ASSERT(FALSE);
  }
  //设置属性

  //道具图标
  CLabelUI *pLabelUI = (CLabelUI *)pVerticalLayoutUI->FindControl(szLableItemShopIOC);
  if(pLabelUI != NULL)
  {
    CStdString  csImageKind;
    csImageKind.Format(TEXT("DLG_BOX_SHOP\\GMAE_PROPERTY_%d.png"),ptagPropertyInfo->wIndex);
    if(m_PaintManager.GetImageEx(csImageKind,NULL)==NULL)
    {
      csImageKind.Empty();
    }

    if(!csImageKind.IsEmpty())
    {
      pLabelUI->SetBkImage(csImageKind);
    }
  }

  //绑定数据
  CButtonUI *pButtonUI = (CButtonUI *)pVerticalLayoutUI->FindControl(TEXT("ItemGameSelect"));
  if(pButtonUI != NULL)
  {
    pButtonUI->SetTag((UINT_PTR)ptagPropertyInfo);
  }

  //道具名称
  pLabelUI = (CLabelUI *)pVerticalLayoutUI->FindControl(szLableItemShopName);
  if(pLabelUI != NULL)
  {
    pLabelUI->SetText(ptagPropertyInfo->szName);
  }

  //VIP折扣
  //用户信息
  CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
  tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
  TCHAR szTaskRewardTotal[64]=TEXT("");
  if(pGlobalUserData->cbMemberOrder > 0)
  {

    //会员任务
    const tagMemberParameterNew* pMemberParameter = CUserItemElement::GetInstance()->GetUserMemberParameter(pGlobalUserData->cbMemberOrder);
    if(pMemberParameter != NULL)
    {
      _sntprintf(szTaskRewardTotal,CountArray(szTaskRewardTotal),TEXT("%d%%折扣"),pMemberParameter->dwMemberShop);
    }

    CControlUI * pContainerVip = pVerticalLayoutUI->FindControl(szItemContainerVipControlName);
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

      CControlUI * pControl4 = pContainerVip->FindControl(szLableItemShopVip);
      if(pControl4 !=NULL)
      {
        pControl4->SetVisible(true);
        pControl4->SetText(szTaskRewardTotal);
      }
    }

  }
  else
  {
    CControlUI * pContainerVip = pVerticalLayoutUI->FindControl(szItemContainerVipControlName);
    if(pContainerVip !=NULL)
    {
      pContainerVip->SetVisible(false);
    }
  }

  pLabelUI = (CLabelUI *)pVerticalLayoutUI->FindControl(szLableItemShopVip);
  if(pLabelUI != NULL)
  {
    if(ptagPropertyInfo->wDiscount == 0)
    {
      pLabelUI->SetText(TEXT(""));
    }
    else
    {
		TCHAR szText[20] = { 0 };
      _sntprintf(szText,CountArray(szText), TEXT("VIP%0.1f折起"), double(ptagPropertyInfo->wDiscount)/10.0);
      pLabelUI->SetText(szText);
    }
  }

  //道具价格
  TCHAR szPrice[64];
  if(ptagPropertyInfo->lPropertyGold != 0 && (cbIndex == 0 || cbIndex == 2))
  {
    _sntprintf(szPrice,CountArray(szPrice), TEXT("%I64d游戏币"), ptagPropertyInfo->lPropertyGold);
    m_pPageLayoutUI[2]->Add((CControlUI*)pVerticalLayoutUI);
  }
  else if(ptagPropertyInfo->lPropertyUserMedal != 0 && (cbIndex == 0 || cbIndex == 4))
  {
    _sntprintf(szPrice,CountArray(szPrice), TEXT("%I64d元宝"), ptagPropertyInfo->lPropertyUserMedal);
    m_pPageLayoutUI[4]->Add((CControlUI*)pVerticalLayoutUI);
  }

  else if(ptagPropertyInfo->lPropertyLoveLiness != 0 && (cbIndex == 0 || cbIndex == 3))
  {
    _sntprintf(szPrice,CountArray(szPrice), TEXT("%I64d魅力值"), ptagPropertyInfo->lPropertyLoveLiness);
    m_pPageLayoutUI[3]->Add((CControlUI*)pVerticalLayoutUI);
  }
  else if(ptagPropertyInfo->dPropertyCash != 0 && (cbIndex == 0 || cbIndex == 1))
  {
    _sntprintf(szPrice,CountArray(szPrice), TEXT("%0.2f游戏豆"), ptagPropertyInfo->dPropertyCash);
    m_pPageLayoutUI[1]->Add((CControlUI*)pVerticalLayoutUI);
  }


  //道具价格
  pLabelUI = (CLabelUI *)pVerticalLayoutUI->FindControl(szLableItemShopPrice);
  if(pLabelUI != NULL)
  {
    pLabelUI->SetText(szPrice);
  }

  pPageLayoutUI->Add(pVerticalLayoutUI);
  return;
}

void CDlgShopMain::ShowSuccessMessage(const CMD_GP_S_PropertyUse* pPropertyUse)
{
  if(pPropertyUse == NULL)
  {
    return;
  }
  CPlatformFrame * pPlatformFrame =  CPlatformFrame::GetInstance();
  if(pPlatformFrame->IsEnteredServerItem())
  {
    return;
  }

  CStdString tMessage = TEXT("道具使用成功！");
  if(pPropertyUse->dwPropKind == PROP_KIND_GEM)
  {
    tMessage.Format(TEXT("%s道具使用成功！恭喜您获得%d游戏币！"), pPropertyUse->szName, pPropertyUse->lUseResultsGold);
  }

  CDlgInformation Information(NULL);
  Information.ShowMessageBox(tMessage.GetData(), MB_ICONINFORMATION);
}

//////////////////////////////////////////////////////////////////////

//构造函数
CDlgShopPlaza::CDlgShopPlaza()
{
  //设置任务
  m_MissionManager.InsertMissionItem(this);
  m_bQueryProperty = false;
  m_bBuyProperty = false;
  ZeroMemory(&m_ConfigProperty, sizeof(m_ConfigProperty));
}

//析构函数
CDlgShopPlaza::~CDlgShopPlaza()
{
}

//创建函数
BOOL CDlgShopPlaza::OnInitDialog()
{
  __super::OnInitDialog();

  //修改样式
  ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX);

  //设置标题
  SetWindowText(TEXT("商城"));

  //居中窗口
  CenterWindow(this);

  //下移距离
  CRect rc;
  GetWindowRect(&rc);
  rc.top += 30;
  rc.bottom += 30;
  //移动窗口
  MoveWindow(rc);


  m_bQueryProperty = true;
  //发起连接
  PerformQueryInfo();

  LoadDataFromUI();

  return TRUE;
}

//查询事件
VOID CDlgShopPlaza::PerformQueryInfo()
{
  m_bBuyProperty = false;
  m_bQueryProperty = true;

  //发起连接
  if(m_MissionManager.AvtiveMissionItem(this,false)==false)
  {
    MessageBox(TEXT("连接失败"), TEXT(""), 0);
    return ;
  }

  return;
}

//查询事件
VOID CDlgShopPlaza::PerformBuyGameProperty(tagBuyGamePropertyInfo *ptagBuyGamePropertyInfo)
{
  m_bBuyProperty = true;
  m_bQueryProperty = false;

  CopyMemory(&m_tagBuyGamePropertyInfo,ptagBuyGamePropertyInfo,sizeof(tagBuyGamePropertyInfo));

  //发起连接
  if(m_MissionManager.AvtiveMissionItem(this,false)==false)
  {
    MessageBox(TEXT("连接失败"), TEXT(""), 0);
    return ;
  }
  return;
}

//使用事件
VOID CDlgShopPlaza::PerformUseGameProperty(const tagUseGamePropertyInfo *pPropertyUseInfo)
{
  m_bBuyProperty = false;
  m_bQueryProperty = false;
  m_bUseProperty = true;

  CopyMemory(&m_PropertyUseInfo,pPropertyUseInfo,sizeof(tagUseGamePropertyInfo));

  //发起连接
  if(m_MissionManager.AvtiveMissionItem(this,false)==false)
  {
    MessageBox(TEXT("连接失败"), TEXT(""), 0);
    return ;
  }
  return;
}

//连接事件
bool CDlgShopPlaza::OnEventMissionLink(INT nErrorCode)
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
  //发送数据
  if(m_bQueryProperty == true)
  {
    //获取对象
    CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
    tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

    m_MissionManager.SendData(MDM_GP_PROPERTY, SUB_GP_QUERY_PROPERTY);
  }
  else if(m_bBuyProperty == true)
  {
    //获取对象
    CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
    tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

    //定义变量
    CMD_GP_C_PropertyBuy PropertyBuy;

    PropertyBuy.wItemCount = m_tagBuyGamePropertyInfo.dwBuyNumber;
    PropertyBuy.cbConsumeType = m_tagBuyGamePropertyInfo.cbScoreType;
    PropertyBuy.wPropertyIndex = m_tagBuyGamePropertyInfo.wPropertyID;
    PropertyBuy.dwUserID = pGlobalUserData->dwUserID;
    m_MissionManager.SendData(MDM_GP_PROPERTY, SUB_GP_PROPERTY_BUY, &PropertyBuy, sizeof(CMD_GP_C_PropertyBuy));

  }
  else if(m_bUseProperty == true)
  {
    //获取对象
    CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
    tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

    //使用背包物品
    CMD_GP_C_PropertyUse PropertyUse;
    PropertyUse.dwUserID = m_PropertyUseInfo.dwUserID;
    PropertyUse.dwRecvUserID = m_PropertyUseInfo.dwRecvUserID;
    PropertyUse.dwPropID = m_PropertyUseInfo.dwPropID;
    PropertyUse.wPropCount = m_PropertyUseInfo.wPropCount;
    m_MissionManager.SendData(MDM_GP_PROPERTY, SUB_GP_PROPERTY_USE, &PropertyUse, sizeof(PropertyUse));
  }

  return true;
}

//关闭事件
bool CDlgShopPlaza::OnEventMissionShut(BYTE cbShutReason)
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
    Information.ShowMessageBox(TEXT("连接服务器超时，道具信息获取失败！"),MB_OK|MB_ICONSTOP);
  }

  return true;
}

//读取事件
bool CDlgShopPlaza::OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize)
{
  //命令处理
  if(Command.wMainCmdID==MDM_GP_PROPERTY)
  {
    if(Command.wSubCmdID == SUB_GP_QUERY_PROPERTY_RESULT)
    {
      //效验参数
      ASSERT(wDataSize%sizeof(tagPropertyInfo)==0);
      if(wDataSize%sizeof(tagPropertyInfo)!=0)
      {
        return false;
      }

      //变量定义
      WORD wItemCount=wDataSize/sizeof(tagPropertyInfo);
      tagPropertyInfo * pGamePropertyItem=(tagPropertyInfo *)pData;

      //保存道具
      memcpy(m_ConfigProperty.PropertyInfo+m_ConfigProperty.cbPropertyCount, pGamePropertyItem, sizeof(tagPropertyInfo)*wItemCount);
      m_ConfigProperty.cbPropertyCount += wItemCount;
    }
    if(Command.wSubCmdID == SUB_GP_QUERY_PROPERTY_RESULT_FINISH)
    {
      //重置道具
      CGlobalUnits::GetInstance()->ClearPropertyConfigInfo();
      CGlobalUnits::GetInstance()->SetPropertyConfigInfo(m_ConfigProperty.PropertyInfo, m_ConfigProperty.cbPropertyCount);

      //添加道具
      for(int i = 0; i < m_ConfigProperty.cbPropertyCount; ++i)
      {
        //添加道具
        if(m_ConfigProperty.PropertyInfo[i].wRecommend != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[0], &m_ConfigProperty.PropertyInfo[i], 0);
        }
        if(m_ConfigProperty.PropertyInfo[i].dPropertyCash != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[1], &m_ConfigProperty.PropertyInfo[i], 1);
        }
        if(m_ConfigProperty.PropertyInfo[i].lPropertyGold != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[2], &m_ConfigProperty.PropertyInfo[i], 2);
        }
        if(m_ConfigProperty.PropertyInfo[i].lPropertyLoveLiness != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[3], &m_ConfigProperty.PropertyInfo[i], 3);
        }
        if(m_ConfigProperty.PropertyInfo[i].lPropertyUserMedal != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[4], &m_ConfigProperty.PropertyInfo[i], 4);
        }
      }
      //终止任务
      m_MissionManager.ConcludeMissionItem(this,false);
    }

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
      if(pPropertySuccess->cbSuccessed==0)
      {
        if(pPropertySuccess->szNotifyContent[0] != 0)
        {
          //错误提示
          CDlgInformation Information(this);
          Information.ShowMessageBox(pPropertySuccess->szNotifyContent,MB_OK|MB_ICONERROR,30);

          return true;
        }
      }
      else
      {
        //获取对象
        CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
        tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

        pGlobalUserData->lUserInsure = pPropertySuccess->lInsureScore;
        pGlobalUserData->lUserIngot = pPropertySuccess->lUserMedal;
        pGlobalUserData->dwLoveLiness = (DWORD)(pPropertySuccess->lLoveLiness);
        pGlobalUserData->dUserBeans = pPropertySuccess->dCash;
        pGlobalUserData->cbMemberOrder = pPropertySuccess->cbCurrMemberOrder;

        if(m_pDlgShopDetails)
        {
          m_pDlgShopDetails->UpdateUI();
        }

        //发送事件
        CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
        if(pPlatformEvent!=NULL)
        {
          pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE, 0L);
        }

        if(static_cast<BUY_MODE>(m_tagBuyGamePropertyInfo.cbUse) == BUY_IMMEDIATELY_USE)
        {
          tagUseGamePropertyInfo  UsePropertyInfo;
          UsePropertyInfo.dwUserID = pGlobalUserData->dwUserID;
          UsePropertyInfo.dwRecvUserID = pGlobalUserData->dwUserID;
          UsePropertyInfo.dwPropID = m_tagBuyGamePropertyInfo.wPropertyID;
          UsePropertyInfo.wPropCount = m_tagBuyGamePropertyInfo.dwBuyNumber;
          PerformUseGameProperty(&UsePropertyInfo);
        }
        else
        {
          if(pPropertySuccess->szNotifyContent[0] != 0)
          {
            //错误提示
            CDlgInformation Information(this);
            Information.ShowMessageBox(pPropertySuccess->szNotifyContent,MB_OK|MB_ICONINFORMATION,30);
            return true;
          }
        }
      }
    }
    else if(Command.wSubCmdID == SUB_GP_PROPERTY_USE_RESULT)
    {
      CMD_GP_S_PropertyUse* pPropertyUse = (CMD_GP_S_PropertyUse*)pData;
      CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
      tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
      if(pPropertyUse->dwHandleCode != 0)
      {
        //提示消息
        CDlgInformation Information(NULL);
        Information.ShowMessageBox(pPropertyUse->szNotifyContent,MB_OK|MB_ICONSTOP,30);
        return false;
      }

      //设置变量
      pGlobalUserData->lUserScore=pPropertyUse->Score;
      pGlobalUserData->cbMemberOrder=pPropertyUse->cbMemberOrder;

      //发送事件
      CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
      if(pPlatformEvent!=NULL)
      {
        pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE,0L);
      }
      //更新BUFF
      tagPropertyBuff Buff= {0};
      Buff.dwKind = pPropertyUse->dwPropKind;
      Buff.tUseTime = pPropertyUse->tUseTime;
      Buff.UseResultsValidTime = pPropertyUse->UseResultsValidTime;
      Buff.dwScoreMultiple = pPropertyUse->dwScoreMultiple;
      //发送事件
      if(pPlatformEvent!=NULL)
      {
        pPlatformEvent->SendPlatformEvent(EVENT_USER_SHOW_UPDATE_BUFF,(LPARAM)&Buff);
      }

      //显示消息
      ShowSuccessMessage(pPropertyUse);
    }
    return true;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgShopServer::CDlgShopServer()
{
  //设置任务
  m_bQueryProperty = false;
  m_bBuyProperty = false;

  ZeroMemory(&m_ConfigProperty, sizeof(m_ConfigProperty));
}

//析构函数
CDlgShopServer::~CDlgShopServer()
{
}

//创建函数
BOOL CDlgShopServer::OnInitDialog()
{
  __super::OnInitDialog();

  //修改样式
  ModifyStyle(0, WS_MINIMIZEBOX|WS_MAXIMIZEBOX);

  //设置标题
  SetWindowText(TEXT("商城"));

  //居中窗口
  CenterWindow(this);

  //下移距离
  CRect rc;
  GetWindowRect(&rc);
  rc.top += 30;
  rc.bottom += 30;
  //移动窗口
  MoveWindow(rc);

  m_bQueryProperty = true;

  PerformQueryInfo();
  LoadDataFromUI();

  return TRUE;
}

//查询事件
VOID CDlgShopServer::PerformQueryInfo()
{
  m_pITCPSocket->SendData(MDM_GR_PROPERTY, SUB_GR_QUERY_PROPERTY);
  return;
}

//查询事件
VOID CDlgShopServer::PerformBuyGameProperty(tagBuyGamePropertyInfo *ptagBuyGamePropertyInfo)
{
  CopyMemory(&m_tagBuyGamePropertyInfo,ptagBuyGamePropertyInfo,sizeof(tagBuyGamePropertyInfo));
  //获取对象
  CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
  tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

  //定义变量
  CMD_GR_C_GamePropertyBuy PropertyBuy;
  PropertyBuy.wItemCount = m_tagBuyGamePropertyInfo.dwBuyNumber;
  PropertyBuy.cbConsumeType = m_tagBuyGamePropertyInfo.cbScoreType;
  PropertyBuy.wPropertyIndex = m_tagBuyGamePropertyInfo.wPropertyID;
  PropertyBuy.dwUserID = pGlobalUserData->dwUserID;
  m_pITCPSocket->SendData(MDM_GR_PROPERTY, SUB_GR_GAME_PROPERTY_BUY, &PropertyBuy, sizeof(CMD_GR_C_GamePropertyBuy));

  return;
}

VOID CDlgShopServer::PerformUseGameProperty(const tagUseGamePropertyInfo *pPropertyUseInfo)
{
  CopyMemory(&m_PropertyUseInfo,pPropertyUseInfo,sizeof(tagUseGamePropertyInfo));

  CMD_GR_C_PropertyUse PropertyUse;
  PropertyUse.dwUserID = m_PropertyUseInfo.dwUserID;
  PropertyUse.dwRecvUserID = m_PropertyUseInfo.dwRecvUserID;
  PropertyUse.dwPropID = m_PropertyUseInfo.dwPropID;
  PropertyUse.wPropCount = m_PropertyUseInfo.wPropCount;
  m_pITCPSocket->SendData(MDM_GR_PROPERTY, SUB_GR_PROPERTY_USE, &PropertyUse, sizeof(PropertyUse));
}

//配置银行
VOID CDlgShopServer::Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem)
{
  //设置变量
  m_pITCPSocket=pITCPSocket;
  m_pIMySelfUserItem=pIMySelfUserItem;

  return;
}
//银行消息
bool CDlgShopServer::OnServerShopMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
  if(wMainCmdID == MDM_GR_PROPERTY)
  {
    if(wSubCmdID == SUB_GR_QUERY_PROPERTY_RESULT)
    {
      //效验参数
      ASSERT(wDataSize%sizeof(tagPropertyInfo)==0);
      if(wDataSize%sizeof(tagPropertyInfo)!=0)
      {
        return false;
      }

      //变量定义
      WORD wItemCount=wDataSize/sizeof(tagPropertyInfo);
      tagPropertyInfo * pGamePropertyItem=(tagPropertyInfo *)pData;

      //保存道具
      memcpy(m_ConfigProperty.PropertyInfo+m_ConfigProperty.cbPropertyCount, pGamePropertyItem, sizeof(tagPropertyInfo)*wItemCount);
      m_ConfigProperty.cbPropertyCount += wItemCount;
    }
    if(wSubCmdID== SUB_GR_QUERY_PROPERTY_RESULT_FINISH)
    {
      ////重置道具
      //CGlobalUnits::GetInstance()->ClearPropertyConfigInfo();
      //CGlobalUnits::GetInstance()->SetPropertyConfigInfo(m_ConfigProperty.PropertyInfo, m_ConfigProperty.cbPropertyCount);

      //获取对象
      CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
      tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
      //会员折扣
      const tagMemberParameterNew* pMemberParameter = CUserItemElement::GetInstance()->GetUserMemberParameter(pGlobalUserData->cbMemberOrder);
      if(pMemberParameter != NULL)
      {
        for(int i = 0; i < m_ConfigProperty.cbPropertyCount; i++)
        {
          m_ConfigProperty.PropertyInfo[i].wDiscount = (WORD)pMemberParameter->dwMemberShop;
        }
      }

      for(int i = 0; i < m_ConfigProperty.cbPropertyCount; ++i)
      {
        //添加道具

        if(m_ConfigProperty.PropertyInfo[i].wRecommend != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[0], &m_ConfigProperty.PropertyInfo[i], 0);
        }
        if(m_ConfigProperty.PropertyInfo[i].dPropertyCash != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[1], &m_ConfigProperty.PropertyInfo[i], 1);
        }
        if(m_ConfigProperty.PropertyInfo[i].lPropertyGold != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[2], &m_ConfigProperty.PropertyInfo[i], 2);
        }
        if(m_ConfigProperty.PropertyInfo[i].lPropertyLoveLiness != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[3], &m_ConfigProperty.PropertyInfo[i], 3);
        }
        if(m_ConfigProperty.PropertyInfo[i].lPropertyUserMedal != 0)
        {
          SetPropertyInfo(m_pPageLayoutUI[4], &m_ConfigProperty.PropertyInfo[i], 4);
        }
      }

    }
    else if(wSubCmdID == SUB_GR_GAME_PROPERTY_BUY_RESULT)
    {
      //效验参数
      ASSERT(wDataSize <= sizeof(CMD_GR_S_PropertyBuyResult));
      if(wDataSize > sizeof(CMD_GR_S_PropertyBuyResult))
      {
        return false;
      }

      //定义变量
      CMD_GR_S_PropertyBuyResult * pPropertySuccess=(CMD_GR_S_PropertyBuyResult *)pData;

      if(pPropertySuccess->cbSuccessed == 0)
      {
        if(pPropertySuccess->szNotifyContent[0] != 0)
        {
          //错误提示
          CDlgInformation Information(this);
          Information.ShowMessageBox(pPropertySuccess->szNotifyContent,MB_OK|MB_ICONERROR,30);

          return true;
        }
      }
      else
      {
        //获取对象
        CGlobalUserInfo * pGlobalUserInfo = CGlobalUserInfo::GetInstance();
        tagGlobalUserData * pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

        pGlobalUserData->lUserInsure = pPropertySuccess->lInsureScore;
        pGlobalUserData->lUserIngot = pPropertySuccess->lUserMedal;
        pGlobalUserData->dwLoveLiness = (DWORD)pPropertySuccess->lLoveLiness;
        pGlobalUserData->dUserBeans = pPropertySuccess->dCash;
        pGlobalUserData->cbMemberOrder = pPropertySuccess->cbCurrMemberOrder;

        if(m_pDlgShopDetails)
        {
          m_pDlgShopDetails->UpdateUI();
        }

        //发送事件
        CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
        if(pPlatformEvent!=NULL)
        {
          pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE, 0L);
        }

        if(static_cast<BUY_MODE>(m_tagBuyGamePropertyInfo.cbUse) == BUY_IMMEDIATELY_USE)
        {
          tagUseGamePropertyInfo  UsePropertyInfo;
          UsePropertyInfo.dwUserID = pGlobalUserData->dwUserID;
          UsePropertyInfo.dwRecvUserID = pGlobalUserData->dwUserID;
          UsePropertyInfo.dwPropID = m_tagBuyGamePropertyInfo.wPropertyID;
          UsePropertyInfo.wPropCount = m_tagBuyGamePropertyInfo.dwBuyNumber;
          PerformUseGameProperty(&UsePropertyInfo);
        }
        else
        {
          //房间喇叭
          if(pPropertySuccess->dwPropID == PROPERTY_ID_TRUMPET)
          {
            int nTrumpetCount = CPlatformFrame::GetInstance()->GetPlazaViewServerCtrl()->GetTrumpetCount();
            nTrumpetCount += pPropertySuccess->dwPropCount;
            CPlatformFrame::GetInstance()->GetPlazaViewServerCtrl()->SetTrumpetCount(nTrumpetCount);
          }
          else if(pPropertySuccess->dwPropID == PROPERTY_ID_TYPHON)
          {
            int nTyphonCount = CPlatformFrame::GetInstance()->GetPlazaViewServerCtrl()->GetTyphonCount();
            nTyphonCount += pPropertySuccess->dwPropCount;
            CPlatformFrame::GetInstance()->GetPlazaViewServerCtrl()->SetTyphonCount(nTyphonCount);
          }
          if(pPropertySuccess->szNotifyContent[0] != 0)
          {
            //错误提示
            CDlgInformation Information(this);
            Information.ShowMessageBox(pPropertySuccess->szNotifyContent,MB_OK|MB_ICONINFORMATION,30);

            return true;
          }
        }
      }




    }
    else if(wSubCmdID == SUB_GR_PROPERTY_USE_RESULT)
    {
      CMD_GR_S_PropertyUse* pPropertyUse = (CMD_GR_S_PropertyUse*)pData;
      if(pPropertyUse->dwUserID != m_pIMySelfUserItem->GetUserID())
      {
        return false;
      }
      if(pPropertyUse->dwHandleCode != 0)
      {
        return false;
      }

      CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
      tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();
      //设置变量
      pGlobalUserData->lUserScore=pPropertyUse->Score;
      pGlobalUserData->cbMemberOrder=pPropertyUse->cbMemberOrder;

      //发送事件
      CPlatformEvent * pPlatformEvent=CPlatformEvent::GetInstance();
      if(pPlatformEvent!=NULL)
      {
        pPlatformEvent->SendPlatformEvent(EVENT_USER_INFO_UPDATE,0L);
      }

    }
  }
  return true;
}


//////////////////////////////////////////////////////////////////////////////////