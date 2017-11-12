#ifndef GAME_CLIENT_HEAD_FILE
#define GAME_CLIENT_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"
#include "JettonControl.h"
#include "CControlWnd.h"
#include "GameLogic.h"
#include "ClientControl.h"

//////////////////////////////////////////////////////////////////////////////////

//消息定义
#define IDM_START         WM_USER+100             //开始消息
#define IDM_ADD_SCORE       WM_USER+101             //加注消息
#define IDM_GET_CARD        WM_USER+102             //要牌消息  
#define IDM_STOP_CARD       WM_USER+103             //停牌消息
#define IDM_DOUBLE_CARD       WM_USER+104             //加倍消息
#define IDM_SPLIT_CARD        WM_USER+105             //分牌消息
#define IDM_INSURE_CARD       WM_USER+106             //保险消息
#define IDM_SEND_CARD_FINISH    WM_USER+107             //发牌完成
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

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
protected:
  LONGLONG            m_lCellScore;           //单元注

  BYTE              m_cbUserCardType[GAME_PLAYER*2];  //
  BYTE              m_bStopCard[GAME_PLAYER*2];     //是否停牌
  BYTE              m_bDoubleCard[GAME_PLAYER*2];   //是否加倍
  BYTE              m_bInsureCard[GAME_PLAYER*2];   //是否保险

  BYTE              m_cbWaitStatus;           //等待标识

  WORD              m_wBankerUser;            //庄家

  //
  CPoint              m_ptBanker[GAME_PLAYER];      //

  //动画变量
protected:
  CPoint              m_SendCardPos;            //发牌位置
  CPoint              m_SendCardCurPos;         //发牌当前位置
  INT               m_nStepCount;           //步数
  INT               m_nXStep;             //步长
  INT               m_nYStep;             //步长
  CSendCardItemArray        m_SendCardItemArray;        //发牌数组

  //按钮控件
public:
  CSkinButton           m_btStart;              //开始按钮
  CSkinButton           m_btGetCard;            //要牌按钮
  CSkinButton           m_btStopCard;           //停牌按钮
  CSkinButton           m_btDoubleCard;           //加倍按钮
  CSkinButton           m_btSplitCard;            //分牌按钮
  CSkinButton           m_btInsureCard;           //保险按钮

  //控件变量
public:
  CScoreView            m_ScoreView;            //成绩窗口
  CCardControl          m_CardControl[GAME_PLAYER*2];   //用户扑克
  CJettonControl          m_PlayerJeton[GAME_PLAYER];     //玩家筹码
  CControlWnd           m_GoldControl;            //下注框
  CGameLogic            m_GameLogic;            //

  //控制
public:
  HINSTANCE           m_hInst;
  IClientControlDlg*        m_pClientControlDlg;
  CButton             m_btOpenAdmin;            //系统控制

  //界面变量
protected:
  CBitImageEx           m_ImageCard;            //扑克资源
  CBitImageEx           m_ImageViewBack;          //背景资源
  CBitImageEx           m_ImageViewFill;          //填充资源
  CBitImageEx           m_ImageCellScore;         //单元积分
  CBitImageEx           m_ImageBanker;            //
  CBitImageEx           m_ImageWaitBanker;          //
  CBitImageEx           m_ImageWaitOthers;          //
  CBitImageEx           m_ImageWaitScore;         //
  CPngImageEx           m_PngNumber;            //数字位图
  CPngImageEx           m_PngCellNumber;          //单元数字
  CPngImageEx           m_PngCardStatus;          //
  CPngImageEx           m_PngCardStatusBack;        //
  CPngImageEx           m_PngCardValue;           //
  CPngImageEx           m_PngCardValueTip;          //
  CPngImageEx           m_PngCardArrow;           //
  //函数定义
public:
  //构造函数
  CGameClientView();
  //析构函数
  virtual ~CGameClientView();
protected:
  //命令函数
  virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
  //重载函数
private:
  //重置界面
  virtual void ResetGameView();
  //调整控件
  virtual VOID RectifyControl(INT nWidth, INT nHeight);
  //绘画界面
  virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);

  virtual bool RealizeWIN7()
  {
    return true;
  }

  //功能函数
public:
  //动画判断
  bool IsDispatchCard();
  //完成发牌
  void FinishDispatchCard();
  //发送扑克
  void DispatchUserCard(WORD wChairID, BYTE cbCardData);
  //设置单元注
  void SetCellScore(LONGLONG lCellScore);
  //玩家停牌
  void SetUserStopCard(WORD wChairID);
  //玩家加倍
  void SetUserDoubleCard(WORD wChairID);
  //玩家下保险
  void SetUserInsureCard(WORD wChairID, WORD wOperIndex);
  //设置等待
  void SetWaitStatus(BYTE cbWaitStatus);
  //设置庄家
  void SetBankerUser(WORD wChairId);
  //
  void SetUserCardType(WORD wChairId, WORD wOperIndex, BYTE cbCardType);

  //辅助函数
protected:
  ////艺术字体
  //void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
  ////艺术字体
  //void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect);
  //画数字
  void DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, CPngImageEx &PngNumber, bool bHasSign);

  //消息映射
protected:
  //建立函数
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  //定时器消息
  afx_msg void OnTimer(UINT nIDEvent);
  //管理员控制
  afx_msg void OpenAdminWnd();

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif