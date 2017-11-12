#ifndef CONTROL_WND_HEAD_FILE
#define CONTROL_WND_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//控制窗口
class CControlWnd : public CWnd
{
  //变量定义
protected:
  LONGLONG            m_lScoreMax;            //最大数目
  LONGLONG            m_lScoreLess;           //最少数目

  //状态变量
public:
  WORD              m_wItemCount;           //子项数目
  LONGLONG            m_lScoreCurrent;          //当前数目
  WORD              m_wCurrentItem;           //当前焦点

  //辅助尺寸
protected:
  INT               m_ControlHead;            //头部宽度
  INT               m_ControlTail;            //尾部宽度

  //控件位置
protected:
  CSize             m_SizeItem;             //子项尺寸
  CSize             m_SizeControl;            //控件大小
  CPoint              m_BenchmarkPos;           //基准位置

  //控件变量
protected:
  CSkinButton           m_btMaxScore;           //最大积分
  CSkinButton           m_btLessScore;            //最小积分
  CSkinButton           m_btSendScore;            //放出积分

  //资源变量
protected:
  CBitImage           m_ImageControlL;          //背景资源
  CBitImage           m_ImageControlM;          //背景资源
  CBitImage           m_ImageControlR;          //背景资源
  CBitImage           m_ImageArrow;           //焦点箭头
  CBitImage           m_ImageNumber;            //数字

protected:
  CGameFrameViewGDI         *pGameClientView;
  //函数定义
public:
  //构造函数
  CControlWnd();
  //析构函数
  virtual ~CControlWnd();

  //功能函数
public:
  //设置位置
  VOID SetBenchmarkPos(INT nXPos, INT nYPos);
  //获取数目
  LONGLONG GetScoreCurrent()
  {
    return m_lScoreCurrent;
  }
  //设置信息
  VOID SetScoreControl(LONGLONG lScoreMax, LONGLONG lScoreLess, LONGLONG lScoreCurrent);

  VOID SetGameClientHandler(CGameFrameViewGDI *pClientView)
  {
    pGameClientView = pClientView;
  }

  //内部函数
protected:
  //播放声音
  VOID PlayGoldSound();
  //转换索引
  WORD SwitchToIndex(CPoint MousePoint);

  //按钮消息
protected:
  //最大按钮
  VOID OnBnClickedScoreMax();
  //最小按钮
  VOID OnBnClickedScoreLess();
  //放出按钮
  VOID OnBnClickedSendScore();

  //消息映射
protected:
  //绘画消息
  VOID OnPaint();
  //位置消息
  VOID OnSize(UINT nType, INT cx, INT cy);
  //创建消息
  INT OnCreate(LPCREATESTRUCT lpCreateStruct);
  //鼠标消息
  VOID OnLButtonUp(UINT nFlags, CPoint Point);
  //鼠标消息
  VOID OnRButtonUp(UINT nFlags, CPoint Point);
  //光标消息
  BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

  DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif