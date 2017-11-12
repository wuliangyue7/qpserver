#include "StdAfx.h"
#include "ScoreView.h"
#include ".\scoreview.h"

//////////////////////////////////////////////////////////////////////////


//构造函数
CScoreView::CScoreView(void)
{
  //设置数据
  memset(m_lScore,0,sizeof(m_lScore));
  memset(m_szUserName,0,sizeof(m_szUserName));
  m_nMutile=1;
  m_nBaseScore=1;
  memset(m_lGivingScore,0,sizeof(m_lGivingScore));

  //加载资源
  m_ImageBack.LoadImage(AfxGetInstanceHandle(),TEXT("GAME_END"));
  m_PngWinLose.LoadImage(AfxGetInstanceHandle(),TEXT("WIN_LOSE"));
}

//析构函数
CScoreView::~CScoreView(void)
{
}

//重画函数
void CScoreView::DrawScoreView(CDC * pDC,int xBegin,int yBegin,CWnd *Pwnd)
{

  //设置 DC
  pDC->SetBkMode(TRANSPARENT);
  pDC->SetTextColor(RGB(250,250,250));

  //绘画背景
  m_ImageBack.DrawImage(pDC,xBegin,yBegin);

  //显示分数
  CRect rcDraw;
  TCHAR szBuffer[64]=TEXT("");
  for(WORD i=0; i<GAME_PLAYER; i++)
  {
    //用户名字
    rcDraw.SetRect(xBegin+15,yBegin+i*25+73+5,xBegin+135,yBegin+i*25+73+5+14);

    DrawTextString(pDC,m_strUserName[i],RGB(255,255,255),RGB(0,0,0),&rcDraw,14,400,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS,Pwnd);


    //用户积分
    rcDraw.left=xBegin+137;
    rcDraw.right=xBegin+232;
    _sntprintf(szBuffer,sizeof(szBuffer),TEXT("%ld"),m_lScore[i]);

    pDC->SetTextColor(RGB(168,255,0));

    //pDC->DrawText(szBuffer,lstrlen(szBuffer),&rcDraw,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);
    //CDFontEx::DrawText(Pwnd,pDC,  12, 400, szBuffer, rcDraw, RGB(168,255,0), DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS);

    DrawTextString(pDC,szBuffer,RGB(168,255,0),RGB(0,0,0),&rcDraw,14,400,DT_VCENTER|DT_CENTER|DT_END_ELLIPSIS,Pwnd);

    INT nWinLoseWidth = m_PngWinLose.GetWidth()/3;
    INT nWinLostHeight = m_PngWinLose.GetHeight();
    //胜负标志
    BYTE index=0;
    if(m_lScore[i]>0)
    {
      index=0;
    }
    if(m_lScore[i]<0)
    {
      index=1;
    }
    if(m_lScore[i]==0)
    {
      index=2;
    }
    m_PngWinLose.DrawImage(pDC,xBegin+270-nWinLoseWidth/2,yBegin+i*25+73+12-nWinLostHeight/2,nWinLoseWidth,nWinLostHeight,
                           index*nWinLoseWidth,0);
  }

  TCHAR szBuffer1[256]=TEXT("");
  _sntprintf(szBuffer1,CountArray(szBuffer1),TEXT("底分：%d ，倍率：%d"),m_nBaseScore,m_nMutile);

  rcDraw.left=xBegin+20;
  rcDraw.right=xBegin+325;
  rcDraw.top=yBegin+135;
  rcDraw.bottom=rcDraw.top+14;

  DrawTextString(pDC,szBuffer1,RGB(255,223,44),RGB(0,0,0),&rcDraw,14,400,DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS,Pwnd);


  TCHAR szBuffer2[512]=TEXT("");
  _sntprintf(szBuffer2,CountArray(szBuffer2),TEXT("贡献分：[%s] %ld，[%s] %ld"),m_strUserName[0],m_lGivingScore[0],m_strUserName[1],m_lGivingScore[1]);

  rcDraw.left=xBegin+15;
  rcDraw.right=xBegin+520;
  rcDraw.top=yBegin+138;
  rcDraw.bottom=rcDraw.top+14;

  //DrawTextString(pDC,szBuffer2,RGB(255,223,44),RGB(0,0,0),&rcDraw,14,400,DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS,Pwnd);

  return;
}

//重置积分
void CScoreView::ResetScore()
{
  //设置数据
  memset(m_lScore,0,sizeof(m_lScore));
  memset(m_szUserName,0,sizeof(m_szUserName));
  return;
}

//设置税收
void CScoreView::SetGameTax(LONG lGameTax)
{
  //设置变量
  //if (m_lGameTax!=lGameTax)
  //{
  //  m_lGameTax=lGameTax;
  //  Invalidate(FALSE);
  //}
  return;
}

//设置积分
void CScoreView::SetGameScore(WORD wChairID, LPCTSTR pszUserName, LONGLONG lScore,LONGLONG lGivingScore)
{
  //设置变量
  if(wChairID<CountArray(m_lScore))
  {
    m_lScore[wChairID]=lScore;
    m_lGivingScore[wChairID]=lGivingScore;
    lstrcpyn(m_szUserName[wChairID],pszUserName,CountArray(m_szUserName[wChairID]));

    m_strUserName[wChairID]=pszUserName;
  }

  return;
}

//设置积分
void CScoreView::SetBaseScore(int nMultle,int nBaseScore)
{
  m_nMutile=nMultle;
  m_nBaseScore=nBaseScore;

  return;
}

//艺术字体
void CScoreView::DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, LPRECT lpRect,int nWidth, int nWeight,UINT nFormat,CWnd * Pwnd)
{
  //变量定义
  int nStringLength=lstrlen(pszString);
  int nXExcursion[8]= {1,1,1,0,-1,-1,-1,0};
  int nYExcursion[8]= {-1,0,1,1,1,0,-1,-1};

  //绘画边框
  //pDC->SetTextColor(crFrame);
  CRect rcDraw;
  for(int i=0; i<CountArray(nXExcursion); i++)
  {
    //rcDraw.CopyRect(lpRect);
    //rcDraw.OffsetRect(nXExcursion[i],nYExcursion[i]);
    //CDFontEx::DrawText(Pwnd, pDC, nWidth, nWeight,pszString,&rcDraw, crFrame, nFormat);

  }

  //绘画字体
  rcDraw.CopyRect(lpRect);
  CDFontEx::DrawText(Pwnd, pDC, nWidth, nWeight,pszString,&rcDraw, crText, nFormat);
  return;
}

//////////////////////////////////////////////////////////////////////////


