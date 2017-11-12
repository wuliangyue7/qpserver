#ifndef SCORE_VIEW_HEAD_FILE
#define SCORE_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////////

//积分视图类
class CScoreView
{
  //变量定义
protected:
  LONGLONG            m_lGameTax;             //游戏税收
  LONGLONG            m_lScore[GAME_PLAYER];        //游戏得分

  int                             m_nMutile;              //倍数
  int                             m_nBaseScore;           //基础分
  LONGLONG            m_lGivingScore[GAME_PLAYER];    //贡献分



  TCHAR             m_szUserName[GAME_PLAYER][LEN_ACCOUNTS];//用户名字
  CString                         m_strUserName[GAME_PLAYER];     //用户名字

  //资源变量
protected:
  CPngImage           m_ImageBack;            //背景图案
  CPngImage           m_PngWinLose;           //结果信息

  //函数定义
public:
  //构造函数
  CScoreView(void);
  //析构函数
  virtual ~CScoreView(void);

  //功能函数
public:
  //重置积分
  void ResetScore();
  //设置税收
  void SetGameTax(LONG lGameTax);
  //设置积分
  void SetBaseScore(int nMultle,int nBaseScore);
  //设置积分
  void SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore,LONGLONG lGivingScore);
  //绘画视图
  VOID DrawScoreView(CDC * pDC,int xBegin,int yBegin,CWnd * Pwnd);
  //绘画字符
  void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect,int nWidth, int nWeight,UINT nFormat,CWnd * Pwnd);


};

//////////////////////////////////////////////////////////////////////////

#endif