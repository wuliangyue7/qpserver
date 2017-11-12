#pragma once

#include "Stdafx.h"
#include "GoldView.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "ClientControl.h"
//////////////////////////////////////////////////////////////////////////

//消息定义
#define IDM_START           WM_USER+100           //开始消息
#define IDM_ADD_GOLD          WM_USER+101           //加注消息
#define IDM_FOLLOW            WM_USER+102           //跟注消息
#define IDM_GIVE_UP           WM_USER+103           //放弃消息
#define IDM_SHOWHAND          WM_USER+104           //梭哈消息
#define IDM_SEND_CARD_FINISH      WM_USER+106           //发牌完成

#define MYSELF_VIEW_ID          1               //自己座位

//////////////////////////////////////////////////////////////////////////
//结构定义

//发牌子项
struct tagSendCardItem
{
  WORD              wChairID;             //发牌用户
  BYTE              cbCardData;             //发牌数据
};

//数组说明
typedef CWHArray<tagSendCardItem,tagSendCardItem &> CSendCardItemArray;

//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
  //友元定义
  friend class CGameClientDlg;

  //视图变量
protected:
  LONGLONG            m_lUserGold[GAME_PLAYER];     //
  LONGLONG            m_lMaxGold;             //最高下注
  CPoint              m_GoldStation[GAME_PLAYER*2];   //筹码位置
  bool              m_bShowScoreControl;        //
  bool              m_bUserShowHand;          //
public:
  LONGLONG            m_lBasicGold;           //最少分数

  //动画变量
protected:
  CPoint              m_SendCardPos;            //发牌位置
  CPoint              m_SendCardCurPos;         //发牌当前位置
  INT               m_nStepCount;           //步数
  INT               m_nXStep;             //步长
  INT               m_nYStep;             //步长
  CSendCardItemArray        m_SendCardItemArray;        //发牌数组

  INT               m_nXFace;
  INT               m_nYFace;
  INT               m_nXTimer;
  INT               m_nYTimer;
  INT               m_nXBorder;
  INT               m_nYBorder;

  //界面变量
protected:
  //CPngImage           m_ImageCardBack;          //扑克资源
  CBitImage           m_ImageCardBack;          //扑克资源
  CBitImage           m_ImageTitle;           //标题位图
  CBitImage           m_ImageViewFill;          //背景位图
  CBitImage           m_ImageViewCenter;          //背景位图
  CPngImage           m_ImageScoreControlBack;      //
  CBitImage           m_ImageScoreTable;          //
  CPngImage           m_PngShowHand;            //
  CPngImage           m_ImageTimeBack;          //
  CPngImage           m_ImageTimeNumber;          //
  CPngImage           m_ImageNickBack;


  CDFontEx            m_DFontEx;              //字体

  //控件变量
public:
  CSkinButton           m_btStart;              //开始按钮
  CSkinButton           m_btFollow;             //跟注按钮
  CSkinButton           m_btGiveUp;             //放弃按钮
  CSkinButton           m_btShowHand;           //梭哈按钮
  CSkinButton           m_btAddTimes3;            //
  CSkinButton           m_btAddTimes2;            //
  CSkinButton           m_btAddTimes1;            //
  CGoldView           m_GoldView[GAME_PLAYER*2];      //用户筹码
  CGoldView           m_TotalGoldView;          //
  CScoreView            m_ScoreView;            //积分视图
  CCardControl          m_CardControl[GAME_PLAYER];     //扑克控件

  //控件位置
protected:
  CPoint              m_ptScoreControl;         //
  //控制
public:
  CButton             m_btOpenAdmin;            //系统控制
  IClientControlDlg*        m_pClientControlDlg;
  HINSTANCE           m_hInst;

  CButton             m_btOpenAdminEx;            //系统控制
  IClientControlDlg*        m_pClientControlDlgEx;

  //函数定义
public:
  //构造函数
  CGameClientView();
  //析构函数
  virtual ~CGameClientView();

  //继承函数
private:
  //重置界面
  virtual VOID ResetGameView();
  //调整控件
  virtual VOID RectifyControl(INT nWidth, INT nHeight);
  //绘画界面
  virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
  //WIN7支持
  virtual bool RealizeWIN7()
  {
    return true;
  }

  //功能函数
public:
  //金币标题
  void SetGoldTitleInfo(LONGLONG lMaxGold, LONGLONG lBasicGold);
  //用户筹码
  void SetUserGoldInfo(WORD wViewChairID, bool bTableGold, LONGLONG dwGold);
  //动画判断
  bool IsDispatchCard();
  //完成发牌
  void FinishDispatchCard();
  //发送扑克
  void DispatchUserCard(WORD wChairID, BYTE cbCardData);
  //
  VOID ShowScoreControl(bool bShow);
  //
  void SetUserShowHand(bool bShowHand);
  //
  void SetUserGold(WORD wChairId, LONGLONG lGold);
  //更新视图
  void RefreshGameView();
  //管理员控制
  void OpenAdminWnd();
  //管理员控制
  void OpenAdminWndEx();

  //绘画扩充
public:
  // 绘画数字
  void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, LONGLONG lOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
  // 绘画数字
  void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, CString szOutNum, INT nXPos, INT nYPos, UINT uFormat = DT_LEFT);
  // 绘画数字
  void DrawNumber(CDC * pDC, CPngImage* ImageNumber, TCHAR * szImageNum, TCHAR* szOutNum ,INT nXPos, INT nYPos,  UINT uFormat = DT_LEFT);
  // 绘画时钟
  void DrawTime(CDC * pDC, WORD wUserTime,INT nXPos, INT nYPos);


  //消息映射
protected:
  //开始按钮
  afx_msg void OnStart();
  //跟注按钮
  afx_msg void OnFollow();
  //放弃按钮
  afx_msg void OnGiveUp();
  //加注按钮
  afx_msg void OnAddTimes1();
  //加注按钮
  afx_msg void OnAddTimes2();
  //加注按钮
  afx_msg void OnAddTimes3();
  //梭哈按钮
  afx_msg void OnShowHand();
  //建立函数
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //定时器消息
  afx_msg void OnTimer(UINT nIDEvent);
  //光标消息
  BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
  //鼠标消息
  VOID OnLButtonDown(UINT nFlags, CPoint Point);
  //
  VOID OnLButtonUp(UINT nFlags, CPoint Point);

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
