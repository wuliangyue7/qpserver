#ifndef DLG_MEMBER_GIFT_HEAD_FILE
#define DLG_MEMBER_GIFT_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////////////

//会员窗口
class CDlgMemberGift : public CFGuiDialog
{
  //函数定义
public:
  //构造函数
  CDlgMemberGift();
  //析构函数
  virtual ~CDlgMemberGift();

  //重载函数
protected:
  //创建函数
  virtual BOOL OnInitDialog();
  //确定函数
  virtual VOID OnOK();
  //取消消息
  virtual VOID OnCancel();

  //接口函数
public:
  //初始控件
  virtual void InitControlUI();
  //消息提醒
  virtual void Notify(TNotifyUI &  msg);

public:
  //  设置会员
  VOID SetMember(bool bMember);
  //  设置数据
  VOID SetData(const tagGiftPropertyInfo *pGiftSub,DWORD nCount);

  //重载函数
public:
  //皮肤名称
  virtual LPCTSTR GetSkinFile();

public:
  VOID UpdateUI();
public:
  bool m_bMember;
  tagGiftPropertyInfo       m_GiftSub[50];            //包含道具
  DWORD             m_wGiftSubCount;          //道具数量
};

//////////////////////////////////////////////////////////////////////////////////

#endif