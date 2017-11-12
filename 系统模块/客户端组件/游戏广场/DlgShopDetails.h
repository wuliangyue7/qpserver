#ifndef DLG_DLGSHOP_DETALES_HEAD_FILE
#define DLG_DLGSHOP_DETALES_HEAD_FILE

#pragma once

struct tagBuyGamePropertyInfo
{
  WORD          wPropertyID;            //道具ID
  WORD          dwBuyNumber;            //购买数量
  BYTE          cbScoreType;            //货币类型
  BYTE          cbUse;                //是否使用
};

//使用道具
struct tagUseGamePropertyInfo
{
  DWORD             dwUserID;             //使用者
  DWORD             dwRecvUserID;           //道具对谁使用
  DWORD             dwPropID;             //道具ID
  WORD              wPropCount;             //使用数目
};

enum BUY_MODE
{
  BUY_IMMEDIATELY_USE = 0,  //购买后立即使用
  BUY_INTO_BAG        //购买后放入背包
};

//商城接口
interface IGamePropertyAction
{
  //查询事件
  virtual VOID PerformQueryInfo()=NULL;
  //购买事件
  virtual VOID PerformBuyGameProperty(tagBuyGamePropertyInfo *ptagBuyGamePropertyInfo)=NULL;
  //使用事件
  virtual VOID PerformUseGameProperty(const tagUseGamePropertyInfo *pPropertyUseInfo)=NULL;
};
//////////////////////////////////////////////////////////////////////////////////

//购买窗口
class CDlgShopDetails : public CFGuiDialog, public CMissionItem
{
  friend class CDlgShopMain;
  //函数定义
public:
  //构造函数
  CDlgShopDetails();
  //析构函数
  virtual ~CDlgShopDetails();

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
  //消息提醒
  virtual void Notify(TNotifyUI &  msg);

  //重载函数
public:
  //皮肤名称
  virtual LPCTSTR GetSkinFile();

  //用户函数
public:

  //缓冲数据
  VOID InitData();

  VOID LoadDataFromMemory();

  VOID LoadDataFromUI();

  VOID UpdateData();

  //界面操控
  VOID InitUI();

  VOID UpdateUI();

  //网络事件
private:
  //连接事件
  virtual bool OnEventMissionLink(INT nErrorCode);
  //关闭事件
  virtual bool OnEventMissionShut(BYTE cbShutReason);
  //读取事件
  virtual bool OnEventMissionRead(TCP_Command Command, VOID * pData, WORD wDataSize);

  //功能函数
public:
  //道具信息
  void SetPropertyInfo(tagPropertyInfo *ptagPropertyInfo);
  //
  void SetShopMainSink(IGamePropertyAction* pIGamePropertyAction);

  //设置默认
  void SetDefaultBuy(int nValue);

  //是否可以立即使用
  bool IsImmediatelyUseProp(tagPropertyInfo *ptagPropertyInfo);

  //任务变量
protected:
  CMissionManager         m_MissionManager;           //任务管理

  //道具变量
protected:
  tagPropertyInfo         *m_ptagPropertyInfo;          //道具信息
  IGamePropertyAction       *m_pIGamePropertyAction;
  WORD              m_dwBuyNumber;              //购买数量
  BYTE              m_cbConsumeType;            //消费类型
  BUY_MODE            m_eBuyMode;               //购买类型
public:
  //缓冲数据
  vector<int> m_nProps;
};

//////////////////////////////////////////////////////////////////////////////////

#endif