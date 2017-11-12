#include "StdAfx.h"
#include "Resource.h"
#include "CardControl.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//公共定义
#define POS_SHOOT         5                 //弹起象素
#define POS_SPACE         20                  //分隔间隔
#define ITEM_COUNT          43                  //子项数目
#define INVALID_ITEM        0xFFFF                //无效索引

//扑克大小
#define CARD_WIDTH          39                  //扑克宽度
#define CARD_HEIGHT         64                  //扑克高度

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardListImage::CCardListImage()
{
  //位置变量
  m_nItemWidth=0;
  m_nItemHeight=0;

  return;
}

//析构函数
CCardListImage::~CCardListImage()
{
}

//加载资源
bool CCardListImage::LoadResource(CWnd *pWnd,HINSTANCE hInstance,UINT nIDResource)
{
  //加载资源
  m_CardListImage.LoadFromResource(hInstance,nIDResource);

  //设置变量
  m_nItemHeight=m_CardListImage.GetHeight();
  m_nItemWidth=m_CardListImage.GetWidth()/ITEM_COUNT;

  return true;
}

//释放资源
bool CCardListImage::DestroyResource()
{
  //设置变量
  m_nItemWidth=0;
  m_nItemHeight=0;

  //释放资源
  m_CardListImage.Destroy();

  return true;
}

//获取位置
int CCardListImage::GetImageIndex(BYTE cbCardData)
{
  //背景判断
  if(cbCardData==0)
  {
    return 0;
  }

  //计算位置
  BYTE cbValue=cbCardData&MASK_VALUE;
  BYTE cbColor=(cbCardData&MASK_COLOR)>>4;
  return (cbColor>=0x03)?(cbValue+27):(cbColor*9+cbValue);
}

//绘画扑克
bool CCardListImage::DrawCardItem(CDC * pDestDC, BYTE cbCardData, int xDest, int yDest)
{
  //效验状态
  ASSERT(m_CardListImage.IsNull()==false);
  ASSERT((m_nItemWidth!=0)&&(m_nItemHeight!=0));

  //绘画子项
  if(cbCardData<=0x3F)
  {
    int nImageXPos=GetImageIndex(cbCardData)*m_nItemWidth;

    m_CardListImage.TransDrawImage(pDestDC,xDest,yDest,m_nItemWidth,m_nItemHeight,nImageXPos,0 ,RGB(255,0,255));
  }
  return true;
}

//绘画扑克
bool CCardListImage::DrawCardItem(CDC * pDestDC, BYTE cbCardData, int xDest, int yDest, int nDstW, int nDstH)
{
  //效验状态
  ASSERT(m_CardListImage.IsNull()==false);
  ASSERT((m_nItemWidth!=0)&&(m_nItemHeight!=0));

  //绘画子项
  if(cbCardData<=0x3F)
  {
    int nImageXPos=GetImageIndex(cbCardData)*m_nItemWidth;

    m_CardListImage.TransDrawImage(pDestDC,xDest,yDest,nDstW,nDstH,nImageXPos,0,m_nItemWidth,m_nItemHeight  ,RGB(255,0,255));
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardResource::CCardResource()
{
}

//析构函数
CCardResource::~CCardResource()
{
}

//加载资源
bool CCardResource::LoadResource(CWnd *pWnd)
{
  //变量定义
  HINSTANCE hInstance=AfxGetInstanceHandle();

  //用户扑克
  m_ImageUserTop.LoadFromResource(pWnd,hInstance,IDB_CARD_USER_TOP);
  m_ImageUserLeft.LoadFromResource(pWnd,hInstance,IDB_CARD_USER_LEFT);
  m_ImageUserRight.LoadFromResource(pWnd,hInstance,IDB_CARD_USER_RIGHT);

  m_ImageUserBottom.LoadResource(pWnd,hInstance,IDB_CARD_USER_BOTTOM);
  m_ImageUserBtGrayed.LoadResource(pWnd,hInstance,IDB_CARD_USER_BOTTOM_GRAYED);

  //桌子扑克
  m_ImageTableTop.LoadResource(pWnd,hInstance,IDB_CARD_TABLE_TOP);
  m_ImageTableLeft.LoadResource(pWnd,hInstance,IDB_CARD_TABLE_LEFT);
  m_ImageTableRight.LoadResource(pWnd,hInstance,IDB_CARD_TABLE_RIGHT);
  m_ImageTableBottom.LoadResource(pWnd,hInstance,IDB_CARD_TABLE_BOTTOM);
  m_ImageWeaveCard.LoadResource(pWnd,hInstance,IDB_TABLE_BOTTOM);

  //牌堆扑克
  m_ImageHeapSingleV.LoadFromResource(pWnd,hInstance,IDB_CARD_HEAP_SINGLE_V);
  m_ImageHeapSingleH.LoadFromResource(pWnd,hInstance,IDB_CARD_HEAP_SINGLE_H);
  m_ImageHeapDoubleV.LoadFromResource(pWnd,hInstance,IDB_CARD_HEAP_DOUBLE_V);
  m_ImageHeapDoubleH.LoadFromResource(pWnd,hInstance,IDB_CARD_HEAP_DOUBLE_H);


  return true;
}

//消耗资源
bool CCardResource::DestroyResource()
{
  //用户扑克
  m_ImageUserTop.Destroy();
  m_ImageUserLeft.Destroy();
  m_ImageUserRight.Destroy();
  m_ImageUserBottom.DestroyResource();

  //桌子扑克
  m_ImageTableTop.DestroyResource();
  m_ImageTableLeft.DestroyResource();
  m_ImageTableRight.DestroyResource();
  m_ImageTableBottom.DestroyResource();
  m_ImageUserBottom.DestroyResource();

  //牌堆扑克
  m_ImageHeapSingleV.Destroy();
  m_ImageHeapSingleH.Destroy();
  m_ImageHeapDoubleV.Destroy();
  m_ImageHeapDoubleH.Destroy();


  return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CHeapCard::CHeapCard()
{
  //控制变量
  m_ControlPoint.SetPoint(0,0);
  m_CardDirection=Direction_East;

  //扑克变量
  m_wFullCount=HEAP_FULL_COUNT;
  m_wMinusHeadCount=0;
  m_wMinusLastCount=0;
  ZeroMemory(m_cbCardData,sizeof(m_cbCardData));

  return;
}

//析构函数
CHeapCard::~CHeapCard()
{
}

//绘画扑克
void CHeapCard::DrawCardControl(CDC * pDC)
{
  switch(m_CardDirection)
  {
    case Direction_East:  //东向
    {
      //绘画扑克
      if((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
      {
        //变量定义
        int nXPos=0,nYPos=0;
        int nYExcusion = Y_HEAP_DOUBLE_V_EXCUSION;
        int nCardHeight = g_CardResource.m_ImageHeapDoubleV.GetHeight()-nYExcusion;

        //尾部扑克
        nXPos = m_ControlPoint.x;
        nYPos = m_ControlPoint.y-(m_wFullCount/2*nYExcusion+nCardHeight)+m_wMinusLastCount/2*nYExcusion;
        WORD wCardIndex = (m_wMinusLastCount/2)*2;
        if(m_wMinusLastCount%2!=0)
        {
          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 255)
            {
              g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+6 ,25,21);
            }
          }
          else
          {
            g_CardResource.m_ImageHeapSingleV.TransDrawImage(pDC,nXPos,nYPos+6 ,RGB(255,0,255));
          }
          wCardIndex += 2;
          nYPos += nYExcusion;
        }

        //中间扑克
        WORD wFinallyIndex=(m_wFullCount-m_wMinusHeadCount)/2;
        WORD wDoubleIndex = (m_wMinusLastCount+1)/2;
        for(WORD i=wDoubleIndex; i<wFinallyIndex; i++)
        {
          //if( m_cbCardData[wCardIndex+1] != 255 || m_cbCardData[wCardIndex] != 255 )

          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0 || m_cbCardData[wCardIndex] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 0)
            {
              if(m_cbCardData[wCardIndex+1] != 255)
              {
                g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+6 ,25,21);
              }
            }
            else
            {
              g_CardResource.m_ImageHeapSingleV.TransDrawImage(pDC,nXPos,nYPos+6 ,RGB(255,0,255));
            }
            if(m_cbCardData[wCardIndex] != 0)
            {
              if(m_cbCardData[wCardIndex] != 255)
              {
                g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[wCardIndex],nXPos,nYPos ,25,21);
              }
            }
            else
            {
              g_CardResource.m_ImageHeapSingleV.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
            }
          }
          else
          {
            g_CardResource.m_ImageHeapDoubleV.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
          }
          wCardIndex += 2;
          nYPos += nYExcusion;
        }

        //头部扑克
        if(m_wMinusHeadCount%2!=0)
        {
          nYPos += 10;
          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 255)
            {
              g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos ,25,21);
            }
          }
          else
          {
            g_CardResource.m_ImageHeapSingleV.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
          }
          //if( wDoubleIndex < wFinallyIndex && m_cbCardData[wCardIndex+1] != 255 && m_cbCardData[wCardIndex-2] != 255 )
          {


          }
        }
      }
      break;
    }
    case Direction_South: //南向
    {
      //绘画扑克
      if((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
      {
        //变量定义
        int nXPos=0,nYPos=0;

        //尾部扑克
        int nXExcusion = g_CardResource.m_ImageHeapDoubleH.GetWidth();
        int nYExcusion = 6;
        nYPos = m_ControlPoint.y;
        nXPos = m_ControlPoint.x-m_wFullCount/2*nXExcusion+m_wMinusLastCount/2*nXExcusion;
        WORD wCardIndex = (m_wMinusLastCount/2)*2;
        if(m_wMinusLastCount%2!=0)
        {
          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 255)
            {
              g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+nYExcusion ,18,28);
            }
          }
          else
          {
            g_CardResource.m_ImageHeapSingleH.TransDrawImage(pDC,nXPos,nYPos+nYExcusion ,RGB(255,0,255));
          }
          wCardIndex += 2;
          nXPos += nXExcusion;
        }

        //中间扑克
        WORD wFinallyIndex=(m_wFullCount-m_wMinusHeadCount)/2;
        WORD wDoubleIndex = (m_wMinusLastCount+1)/2;
        for(WORD i=wDoubleIndex; i<wFinallyIndex; i++)
        {
          //if( m_cbCardData[wCardIndex+1] != 255 || m_cbCardData[wCardIndex] != 255 )

          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0 || m_cbCardData[wCardIndex] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 0)
            {
              if(m_cbCardData[wCardIndex+1] != 255)
              {
                g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+nYExcusion ,18,28);
              }
            }
            else
            {
              g_CardResource.m_ImageHeapSingleH.TransDrawImage(pDC,nXPos,nYPos+nYExcusion ,RGB(255,0,255));
            }
            if(m_cbCardData[wCardIndex] != 0)
            {
              if(m_cbCardData[wCardIndex] != 255)
              {
                g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbCardData[wCardIndex],nXPos,nYPos ,18,28);
              }
            }
            else
            {
              g_CardResource.m_ImageHeapSingleH.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
            }
          }
          else
          {
            g_CardResource.m_ImageHeapDoubleH.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
          }
          wCardIndex += 2;
          nXPos += nXExcusion;
        }

        //头部扑克
        if(m_wMinusHeadCount%2!=0)
        {
          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 255)
            {
              g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+nYExcusion ,18,28);
            }
          }
          else
          {
            g_CardResource.m_ImageHeapSingleH.TransDrawImage(pDC,nXPos,nYPos+nYExcusion ,RGB(255,0,255));
          }
          //if( wDoubleIndex < wFinallyIndex && m_cbCardData[wCardIndex+1] != 255 && m_cbCardData[wCardIndex-2] != 255)
          {

          }
        }
      }

      break;
    }
    case Direction_West:  //西向
    {
      //绘画扑克
      if((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
      {
        //变量定义
        int nXPos=0,nYPos=0;

        //头部扑克
        int nYExcusion = Y_HEAP_DOUBLE_V_EXCUSION;
        nXPos = m_ControlPoint.x;
        nYPos = m_ControlPoint.y+m_wMinusHeadCount/2*nYExcusion;
        WORD wCardIndex = m_wFullCount-(m_wMinusHeadCount/2)*2-2;
        if(m_wMinusHeadCount%2!=0)
        {
          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 255)
            {
              g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+6 ,25,21);
            }
          }
          else
          {
            g_CardResource.m_ImageHeapSingleV.TransDrawImage(pDC,nXPos,nYPos+6 ,RGB(255,0,255));
          }
          wCardIndex -= 2;
          nYPos += nYExcusion;
        }

        //中间扑克
        WORD wFinallyIndex=(m_wFullCount-m_wMinusLastCount)/2;
        WORD wDoubleIndex = (m_wMinusHeadCount+1)/2;
        for(WORD i=wDoubleIndex; i<wFinallyIndex; i++)
        {
          //if( m_cbCardData[wCardIndex+1] != 255 || m_cbCardData[wCardIndex] != 255 )

          //如果有翻牌
          if(m_cbCardData[wCardIndex] != 0 || m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 0)
            {
              if(m_cbCardData[wCardIndex+1] != 255)
              {
                g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+6 ,25,21);
              }
            }
            else
            {
              g_CardResource.m_ImageHeapSingleV.TransDrawImage(pDC,nXPos,nYPos+6 ,RGB(255,0,255));
            }
            if(m_cbCardData[wCardIndex] != 0)
            {
              if(m_cbCardData[wCardIndex] != 255)
              {
                g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[wCardIndex],nXPos,nYPos ,25,21);
              }
            }
            else
            {
              g_CardResource.m_ImageHeapSingleV.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
            }
          }
          else
          {
            g_CardResource.m_ImageHeapDoubleV.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
          }
          nYPos += nYExcusion;
          wCardIndex -= 2;
        }

        //尾部扑克
        if(m_wMinusLastCount%2!=0)
        {
          nYPos += 6;
          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 255)
            {
              g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos ,25,21);
            }
          }
          else
          {

            g_CardResource.m_ImageHeapSingleV.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
          }
          //if( wDoubleIndex < wFinallyIndex && m_cbCardData[wCardIndex+1] != 255 && m_cbCardData[wCardIndex+2] != 255 )
          {

          }
        }
      }

      break;
    }
    case Direction_North: //北向
    {
      //绘画扑克
      if((m_wFullCount-m_wMinusHeadCount-m_wMinusLastCount)>0)
      {
        //变量定义
        int nXPos=0,nYPos=0;

        //头部扑克
        int nXExcusion = g_CardResource.m_ImageHeapDoubleH.GetWidth();
        int nYExcusion = 6;
        nYPos = m_ControlPoint.y;
        nXPos = m_ControlPoint.x+m_wMinusHeadCount/2*nXExcusion;
        WORD wCardIndex = m_wFullCount-(m_wMinusHeadCount/2)*2-2;
        if(m_wMinusHeadCount%2!=0)
        {
          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 255)
            {
              g_CardResource.m_ImageTableTop.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+nYExcusion ,18,28);
            }
          }
          else
          {

            g_CardResource.m_ImageHeapSingleH.TransDrawImage(pDC,nXPos,nYPos+nYExcusion ,RGB(255,0,255));
          }
          wCardIndex -= 2;
          nXPos += nXExcusion;
        }

        //中间扑克
        WORD wFinallyIndex=(m_wFullCount-m_wMinusLastCount)/2;
        WORD wDoubleIndex = (m_wMinusHeadCount+1)/2;
        for(WORD i=wDoubleIndex; i<wFinallyIndex; i++)
        {
          //if( m_cbCardData[wCardIndex+1] != 255 || m_cbCardData[wCardIndex] != 255 )

          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0 || m_cbCardData[wCardIndex] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 0)
            {
              if(m_cbCardData[wCardIndex+1] != 255)
              {
                g_CardResource.m_ImageTableTop.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+nYExcusion ,18,28);
              }
            }
            else
            {
              g_CardResource.m_ImageHeapSingleH.TransDrawImage(pDC,nXPos,nYPos+nYExcusion ,RGB(255,0,255));
            }
            if(m_cbCardData[wCardIndex] != 0)
            {
              if(m_cbCardData[wCardIndex] != 255)
              {
                g_CardResource.m_ImageTableTop.DrawCardItem(pDC,m_cbCardData[wCardIndex],nXPos,nYPos ,18,28);
              }
            }
            else
            {
              g_CardResource.m_ImageHeapSingleH.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
            }
          }
          else
          {
            g_CardResource.m_ImageHeapDoubleH.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
          }
          wCardIndex -= 2;
          nXPos += nXExcusion;
        }

        //尾部扑克
        if(m_wMinusLastCount%2!=0)
        {
          //如果有翻牌
          if(m_cbCardData[wCardIndex+1] != 0)
          {
            if(m_cbCardData[wCardIndex+1] != 255)
            {
              g_CardResource.m_ImageTableTop.DrawCardItem(pDC,m_cbCardData[wCardIndex+1],nXPos,nYPos+nYExcusion ,18,28);
            }
          }
          else
          {

            g_CardResource.m_ImageHeapSingleH.TransDrawImage(pDC,nXPos,nYPos+nYExcusion ,RGB(255,0,255));
          }
          //if( wDoubleIndex < wFinallyIndex && m_cbCardData[wCardIndex+1] != 255 && m_cbCardData[wCardIndex+2] != 255 )
          {

          }
        }
      }

      break;
    }
  }

  return;
}

//设置扑克
bool CHeapCard::SetHeapCardInfo(WORD wMinusHeadCount, WORD wMinusLastCount)
{
  ASSERT(wMinusHeadCount+wMinusLastCount<=m_wFullCount);
  if(wMinusHeadCount + wMinusLastCount > m_wFullCount)
  {
    return false;
  }

  //设置变量
  m_wMinusHeadCount=wMinusHeadCount;
  m_wMinusLastCount=wMinusLastCount;

  return true;
}

//设置翻牌,wIndex从牌尾的上面那只牌算起
void CHeapCard::SetCardData(WORD wIndex, BYTE cbCardData)
{
  ASSERT(wIndex < m_wFullCount);
  if(wIndex >= m_wFullCount)
  {
    return;
  }

  m_cbCardData[wIndex] = cbCardData;
}

//重置翻牌
void CHeapCard::ResetCardData()
{
  ZeroMemory(m_cbCardData,sizeof(m_cbCardData));
  return ;
}

//设置牌为空,wIndex从牌尾的上面那只算起
void CHeapCard::SetCardEmpty(WORD wIndex, BYTE cbEmptyCount)
{
  ASSERT(wIndex+1 >= cbEmptyCount);
  if(wIndex+1 < cbEmptyCount)
  {
    return;
  }

  for(int i = wIndex; i >= wIndex+1-cbEmptyCount; i--)
  {
    m_cbCardData[i] = 255;
  }
}

//是否有财神
bool CHeapCard::HaveMagicCard()
{
  ASSERT(m_wMinusHeadCount+m_wMinusLastCount <= m_wFullCount);
  if(m_wMinusHeadCount+m_wMinusLastCount >= m_wFullCount)
  {
    return false;
  }

  for(int i = m_wMinusHeadCount; i < m_wFullCount-m_wMinusLastCount; i++)
  {
    if(m_cbCardData[i] > 0)
    {
      return true;
    }
  }

  return false;
}

//位置是否空的  (从尾部数起)
bool CHeapCard::IsNotEmptyPos(BYTE bPosCount)
{
  if(bPosCount >= m_wMinusLastCount && bPosCount < HEAP_FULL_COUNT-m_wMinusHeadCount)
  {
    return true;
  }

  return false;
}

//获取出牌位置
CPoint CHeapCard::GetDispatchCardPos(bool bTail)
{
  CPoint ptHead = m_ControlPoint;

  switch(m_CardDirection)
  {
    case Direction_East:  //东
    {
      int nCardHeight = g_CardResource.m_ImageHeapDoubleV.GetHeight()-Y_HEAP_DOUBLE_V_EXCUSION;
      int nYExcusion = 0;
      if(!bTail)
      {
        nYExcusion = (m_wMinusHeadCount+2)/2;
        ptHead.y -= nYExcusion*Y_HEAP_DOUBLE_V_EXCUSION+nCardHeight;
        if(m_wMinusHeadCount%2 != 0)
        {
          ptHead.y += 9;
        }
      }
      else
      {
        nYExcusion = (m_wFullCount-m_wMinusLastCount+1)/2;
        ptHead.y -= nYExcusion*Y_HEAP_DOUBLE_V_EXCUSION+nCardHeight;
        if(m_wMinusLastCount%2 != 0)
        {
          ptHead.y += 9;
        }
      }
    }
    break;
    case Direction_South: //南
    {
      int nXExcusion = 0;
      if(!bTail)
      {
        nXExcusion = (m_wMinusHeadCount+2)/2;
        ptHead.x -= nXExcusion*g_CardResource.m_ImageHeapDoubleH.GetWidth();
        if(m_wMinusHeadCount%2 != 0)
        {
          ptHead.y += 9;
        }
      }
      else
      {
        nXExcusion = (m_wFullCount-m_wMinusLastCount+1)/2;
        ptHead.x -= nXExcusion*g_CardResource.m_ImageHeapDoubleH.GetWidth();
        if(m_wMinusLastCount%2 != 0)
        {
          ptHead.y += 9;
        }
      }
    }
    break;
    case Direction_West:  //西
    {
      int nYExcusion = 0;
      if(!bTail)
      {
        nYExcusion = m_wMinusHeadCount/2;
        ptHead.y += nYExcusion*Y_HEAP_DOUBLE_V_EXCUSION;
        if(m_wMinusHeadCount%2 != 0)
        {
          ptHead.y += 10;
        }
      }
      else
      {
        nYExcusion = (m_wFullCount-m_wMinusLastCount)/2;
        ptHead.y += nYExcusion*Y_HEAP_DOUBLE_V_EXCUSION;
        if(m_wMinusLastCount%2 != 0)
        {
          ptHead.y += 10;
        }
      }
    }
    break;
    case Direction_North: //北
    {
      int nXExcusion = 0;
      if(!bTail)
      {
        nXExcusion = m_wMinusHeadCount/2;
        ptHead.x += nXExcusion*g_CardResource.m_ImageHeapDoubleH.GetWidth();
        if(m_wMinusHeadCount%2 != 0)
        {
          ptHead.y += 9;
        }
      }
      else
      {
        nXExcusion = (m_wFullCount-m_wMinusLastCount)/2;
        ptHead.x += nXExcusion*g_CardResource.m_ImageHeapDoubleH.GetWidth();
        if(m_wMinusLastCount%2 != 0)
        {
          ptHead.y += 9;
        }
      }
    }
    break;
  }

  return ptHead;
}

//获取出牌位置,参数wIndex表示从牌尾上面那只牌算起
CPoint CHeapCard::GetDispatchCardPos(WORD wIndex)
{
  ASSERT(wIndex < m_wFullCount);
  CPoint ptHead = m_ControlPoint;

  switch(m_CardDirection)
  {
    case Direction_East:  //东
    {
      int nCardHeight = g_CardResource.m_ImageHeapDoubleV.GetHeight()-Y_HEAP_DOUBLE_V_EXCUSION;
      int nYExcusion = (m_wFullCount-wIndex+1)/2*Y_HEAP_DOUBLE_V_EXCUSION+nCardHeight;
      ptHead.y -= nYExcusion;
      if(wIndex%2 != 0)
      {
        ptHead.y += 9;
      }
    }
    break;
    case Direction_South: //南
    {
      int nXExcusion = (m_wFullCount-wIndex+1)/2*g_CardResource.m_ImageHeapDoubleH.GetWidth();
      ptHead.x -= nXExcusion;
      if(wIndex%2 != 0)
      {
        ptHead.y += 9;
      }
    }
    break;
    case Direction_West:  //西
    {
      int nYExcusion = (m_wFullCount/2-(wIndex+2)/2)*Y_HEAP_DOUBLE_V_EXCUSION;
      ptHead.y += nYExcusion;
      if(wIndex%2 != 0)
      {
        ptHead.y += 10;
      }
    }
    break;
    case Direction_North: //北
    {
      int nXExcusion = (m_wFullCount/2-(wIndex+2)/2)*g_CardResource.m_ImageHeapDoubleH.GetWidth();
      ptHead.x += nXExcusion;
      if(wIndex%2 != 0)
      {
        ptHead.y += 9;
      }
    }
    break;
  }

  return ptHead;
}

//从牌堆移除牌,bTail指是否从牌尾移除
bool CHeapCard::DeductionHeapCard(bool bTail, bool & bCgMagciPos)
{
  if(m_wMinusHeadCount+m_wMinusLastCount >= m_wFullCount)
  {
    return false;
  }

  if(!bTail)
  {
    m_wMinusHeadCount++;
    WORD wCardIndex = 0;
    if((m_wFullCount-m_wMinusHeadCount)%2 != 0)
    {
      wCardIndex = m_wFullCount-m_wMinusHeadCount-1;
    }
    else
    {
      wCardIndex = m_wFullCount-m_wMinusHeadCount+1;
    }
    while(m_cbCardData[wCardIndex] != 0 && m_wMinusLastCount+m_wMinusHeadCount<m_wFullCount)
    {
      m_wMinusHeadCount++;
      if(wCardIndex%2 != 0)
      {
        if(wCardIndex < 3)
        {
          break;
        }
        wCardIndex -= 3;
      }
      else
      {
        wCardIndex++;
      }
    }
  }
  else
  {
    m_wMinusLastCount++;
    if(m_cbCardData[m_wMinusLastCount-1] != 0)
    {
      if(m_wMinusHeadCount + m_wMinusLastCount == m_wFullCount-1 && m_wMinusHeadCount%2 == 1)
      {
        m_cbCardData[HEAP_FULL_COUNT-m_wMinusHeadCount] = m_cbCardData[m_wMinusLastCount-1];
        m_cbCardData[m_wMinusLastCount-1] = 0;
      }
      else
      {
        m_cbCardData[m_wMinusLastCount] = m_cbCardData[m_wMinusLastCount-1];
        m_cbCardData[m_wMinusLastCount-1] = 0;
      }
      if(m_wMinusHeadCount+m_wMinusLastCount >= m_wFullCount)
      {
        bCgMagciPos = true;
      }
    }
    /*while( m_cbCardData[m_wMinusLastCount-1] != 0 && m_wMinusLastCount+m_wMinusHeadCount<m_wFullCount )
      m_wMinusLastCount++;*/
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CWeaveCard::CWeaveCard()
{
  //状态变量
  m_bDisplayItem=false;
  m_ControlPoint.SetPoint(0,0);
  m_CardDirection=Direction_South;
  m_cbDirectionCardPos = 4;

  //扑克数据
  m_wCardCount=0;
  ZeroMemory(&m_cbCardData,sizeof(m_cbCardData));



  return;
}

//析构函数
CWeaveCard::~CWeaveCard()
{
}

bool CWeaveCard::LoadResource(CWnd *pWnd)
{
  m_ImageDirectionPos.LoadImage(pWnd,AfxGetInstanceHandle(),TEXT("DIRECTION_OPERATE"));
  m_sizeImageDirection.cx = m_ImageDirectionPos.GetWidth()/4;
  m_sizeImageDirection.cy = m_ImageDirectionPos.GetHeight();
  return true;
}
//绘画扑克
void CWeaveCard::DrawCardControl(CDC * pDC,bool bEnd)
{
  //显示判断
  if(m_wCardCount==0)
  {
    return;
  }

  //变量定义
  int nXScreenPos=0,nYScreenPos=0;
  CCardListImage *pCardListImg = NULL;
  int nTmpX=0,nTmpY=0;

  //绘画扑克
  switch(m_CardDirection)
  {
    case Direction_East:  //东向
    {
      //变量定义
      int nYExcusionV = Y_TABLE_LEFT_EXCUSION;
      int nYExcusionH = Y_TABLE_BOTTOM_EXCUSION;

      nXScreenPos = m_ControlPoint.x;
      nYScreenPos = m_ControlPoint.y;

      //前三扑克
      for(int i = 0; i < 3; i++)
      {
        if(m_cbDirectionCardPos < 4)
        {
          nTmpY = (i==2?0:nYExcusionV);
          if(m_cbDirectionCardPos == 0 && i == 0)
          {
            pCardListImg = &g_CardResource.m_ImageTableBottom;
            nTmpY = (i==2?0:nYExcusionH);
          }
          else if(m_cbDirectionCardPos == 2 && i == 2)
          {
            pCardListImg = &g_CardResource.m_ImageTableTop;
            nTmpY = (i==2?0:nYExcusionH);
          }
          else
          {
            pCardListImg = &g_CardResource.m_ImageTableLeft;
          }
          pCardListImg->DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos);
          nYScreenPos += nTmpY;
        }
        else
        {
          g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos);
          nYScreenPos += (i==2?0:nYExcusionV);
        }
      }

      //第四扑克
      if(m_wCardCount == 4)
      {
        g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,GetCardData(3),nXScreenPos,nYScreenPos-nYExcusionV-12);
      }

      break;
    }
    case Direction_South: //南向
    {
      //变量定义
      if(bEnd)
      {
        int nXExcusionV = g_CardResource.m_ImageTableBottom.GetViewWidth();
        int nXExcusionH = g_CardResource.m_ImageTableLeft.GetViewWidth();

        nXScreenPos = m_ControlPoint.x;
        nYScreenPos = m_ControlPoint.y;

        //前三扑克
        for(int i = 0; i < 3; i++)
        {
          if(m_cbDirectionCardPos < 4)
          {
            nTmpX = (i==2?0:nXExcusionV);
            nTmpY = 0;
            /*if (m_cbDirectionCardPos == 1 && i == 2)
            {
            pCardListImg = &g_CardResource.m_ImageTableLeft;
            nTmpX = (i==2?0:nXExcusionH);
            nTmpY = 11;
            }
            else if (m_cbDirectionCardPos == 3 && i == 0)
            {
            pCardListImg = &g_CardResource.m_ImageTableRight;
            nTmpX = (i==2?0:nXExcusionH);
            nTmpY = 11;
            }
            else*/
            pCardListImg = &g_CardResource.m_ImageTableBottom;
            pCardListImg->DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos+nTmpY);
            nXScreenPos += nTmpX;
          }
          else
          {
            g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos);
            nXScreenPos += (i==2?0:nXExcusionV);
          }
        }

        //第四扑克
        if(m_wCardCount == 4)
        {
          g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,GetCardData(3),nXScreenPos-nXExcusionV,nYScreenPos-12);
        }
      }
      else
      {
        int nXExcusionV = g_CardResource.m_ImageWeaveCard.GetViewWidth();
        int nXExcusionH = g_CardResource.m_ImageTableLeft.GetViewWidth();

        nXScreenPos = m_ControlPoint.x;
        nYScreenPos = m_ControlPoint.y;

        //前三扑克
        for(int i = 0; i < 3; i++)
        {
          if(m_cbDirectionCardPos < 4)
          {
            nTmpX = (i==2?0:nXExcusionV);
            nTmpY = 0;
            /*if (m_cbDirectionCardPos == 1 && i == 2)
            {
            pCardListImg = &g_CardResource.m_ImageTableLeft;
            nTmpX = (i==2?0:nXExcusionH);
            nTmpY = 11;
            }
            else if (m_cbDirectionCardPos == 3 && i == 0)
            {
            pCardListImg = &g_CardResource.m_ImageTableRight;
            nTmpX = (i==2?0:nXExcusionH);
            nTmpY = 11;
            }
            else*/
            pCardListImg = &g_CardResource.m_ImageWeaveCard;
            pCardListImg->DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos+nTmpY);
            nXScreenPos += nTmpX;
          }
          else
          {
            g_CardResource.m_ImageWeaveCard.DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos);
            nXScreenPos += (i==2?0:nXExcusionV);
          }
        }

        //第四扑克
        if(m_wCardCount == 4)
        {
          g_CardResource.m_ImageWeaveCard.DrawCardItem(pDC,GetCardData(3),nXScreenPos-nXExcusionV,nYScreenPos-12);
        }
      }


      break;
    }
    case Direction_West:  //西向
    {
      //变量定义
      int nYExcusionV = Y_TABLE_RIGHT_EXCUSION;
      int nYExcusionH = Y_TABLE_BOTTOM_EXCUSION;

      nXScreenPos = m_ControlPoint.x;
      nYScreenPos = m_ControlPoint.y-nYExcusionV*2-g_CardResource.m_ImageTableRight.GetViewHeight();

      //前三扑克
      for(int i = 2; i >= 0; i--)
      {
        if(m_cbDirectionCardPos < 4)
        {
          nTmpX = 0;
          nTmpY = (i==0?0:nYExcusionV);
          if(m_cbDirectionCardPos == 0 && i == 2)
          {
            pCardListImg = &g_CardResource.m_ImageTableBottom;
            nTmpX = 8;
            nTmpY = (i==0?0:nYExcusionH);
          }
          else if(m_cbDirectionCardPos == 2 && i == 0)
          {
            pCardListImg = &g_CardResource.m_ImageTableTop;
            nTmpX = 8;
            nTmpY = (i==0?0:nYExcusionH);
          }
          else
          {
            pCardListImg = &g_CardResource.m_ImageTableRight;
          }
          pCardListImg->DrawCardItem(pDC,GetCardData(i),nXScreenPos+nTmpX,nYScreenPos);
          nYScreenPos += nTmpY;
        }
        else
        {
          g_CardResource.m_ImageTableRight.DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos);
          nYScreenPos += (i==0?0:nYExcusionV);
        }
      }

      //第四扑克
      if(m_wCardCount == 4)
      {
        g_CardResource.m_ImageTableRight.DrawCardItem(pDC,GetCardData(3),nXScreenPos,nYScreenPos-nYExcusionV-12);
      }

      break;
    }
    case Direction_North: //北向
    {
      //变量定义
      int nXExcusionV = g_CardResource.m_ImageTableBottom.GetViewWidth();
      int nXExcusionH = g_CardResource.m_ImageTableLeft.GetViewWidth();

      nXScreenPos = m_ControlPoint.x-nXExcusionV*3;
      nYScreenPos = m_ControlPoint.y;

      //前三扑克
      for(int i = 2; i >= 0; i--)
      {
        if(m_cbDirectionCardPos < 4)
        {
          nTmpX = (i==0?0:nXExcusionV);
          /*if (m_cbDirectionCardPos == 1 && i == 0)
          {
            pCardListImg = &g_CardResource.m_ImageTableLeft;
            nTmpX = (i==0?0:nXExcusionH);
          }
          else if (m_cbDirectionCardPos == 3 && i == 2)
          {
            pCardListImg = &g_CardResource.m_ImageTableRight;
            nTmpX = (i==0?0:nXExcusionH);
          }
          else*/
          pCardListImg = &g_CardResource.m_ImageTableTop;
          pCardListImg->DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos);
          nXScreenPos += nTmpX;
        }
        else
        {
          g_CardResource.m_ImageTableTop.DrawCardItem(pDC,GetCardData(i),nXScreenPos,nYScreenPos);
          nXScreenPos += (i==0?0:nXExcusionV);
        }
      }

      //第四扑克
      if(m_wCardCount == 4)
      {
        g_CardResource.m_ImageTableTop.DrawCardItem(pDC,GetCardData(3),nXScreenPos-nXExcusionV,nYScreenPos-12);
      }

      break;
    }
  }

  return;
}

//设置扑克
bool CWeaveCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
  //效验大小
  ASSERT(wCardCount<=CountArray(m_cbCardData));
  if(wCardCount>CountArray(m_cbCardData))
  {
    return false;
  }

  //设置扑克
  m_wCardCount=wCardCount;
  CopyMemory(m_cbCardData,cbCardData,sizeof(BYTE)*wCardCount);

  if(m_wCardCount == 0)
  {
    m_cbDirectionCardPos = 4;
  }

  return true;
}

//获取扑克
BYTE CWeaveCard::GetCardData(WORD wIndex)
{
  ASSERT(wIndex<CountArray(m_cbCardData));
  return (m_bDisplayItem==true)?m_cbCardData[wIndex]:0;
}

//绘画扑克
void CWeaveCard::DrawCardControl(CDC * pDC, int nXPos, int nYPos,bool bEnd)
{
  //设置位置
  SetControlPoint(nXPos,nYPos);

  DrawCardControl(pDC ,bEnd);

  return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CUserCard::CUserCard()
{
  //扑克数据
  m_wCardCount=0;
  m_bCurrentCard=false;

  //控制变量
  m_ControlPoint.SetPoint(0,0);
  m_CardDirection=Direction_East;

  return;
}

//析构函数
CUserCard::~CUserCard()
{
}

//绘画扑克
void CUserCard::DrawCardControl(CDC * pDC)
{
  switch(m_CardDirection)
  {
    case Direction_East:  //东向
    {
      int nYExcusion = Y_USER_LEFT_EXCUSION;

      int nXPos = m_ControlPoint.x;
      int nYPos = m_ControlPoint.y-nYExcusion*m_wCardCount-g_CardResource.m_ImageUserLeft.GetHeight()-8;

      //正常扑克
      for(WORD i=0; i<m_wCardCount; i++)
      {

        g_CardResource.m_ImageUserLeft.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
        nYPos += nYExcusion;
      }

      //当前扑克
      if(m_bCurrentCard)
      {

        g_CardResource.m_ImageUserLeft.TransDrawImage(pDC,nXPos,nYPos+8 ,RGB(255,0,255));
      }

      break;
    }
    case Direction_West:  //西向
    {
      int nYExcusion = Y_USER_RIGHT_EXCUSION;

      int nXPos = m_ControlPoint.x;
      int nYPos = m_ControlPoint.y;

      //当前扑克
      if(m_bCurrentCard==true)
      {

        g_CardResource.m_ImageUserRight.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
      }
      nYPos += nYExcusion+10;

      //正常扑克
      for(WORD i=0; i<m_wCardCount; i++)
      {

        g_CardResource.m_ImageUserRight.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
        nYPos += nYExcusion;
      }

      break;
    }
    case Direction_North: //北向
    {
      int nXExcusion = g_CardResource.m_ImageUserTop.GetWidth();

      int nXPos = m_ControlPoint.x;
      int nYPos = m_ControlPoint.y;

      //当前扑克
      if(m_bCurrentCard==true)
      {

        g_CardResource.m_ImageUserTop.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
      }
      nXPos += nXExcusion+12;

      //正常扑克
      for(WORD i=0; i<m_wCardCount; i++)
      {

        g_CardResource.m_ImageUserTop.TransDrawImage(pDC,nXPos,nYPos ,RGB(255,0,255));
        nXPos += nXExcusion;
      }

      break;
    }
  }

  return;
}

//设置扑克
bool CUserCard::SetCurrentCard(bool bCurrentCard)
{
  //设置变量
  m_bCurrentCard=bCurrentCard;

  return true;
}

//设置扑克
bool CUserCard::SetCardData(WORD wCardCount, bool bCurrentCard)
{
  //设置变量
  m_wCardCount=wCardCount;
  m_bCurrentCard=bCurrentCard;

  return true;
}

//添加扑克
bool CUserCard::AddCardData(bool bCurrentCard)
{
  ASSERT(m_wCardCount+(m_bCurrentCard?1:0) < MAX_COUNT);
  if(m_wCardCount+(m_bCurrentCard?1:0) >= MAX_COUNT)
  {
    return false;
  }

  if(!bCurrentCard)
  {
    m_wCardCount++;
  }
  else
  {
    m_bCurrentCard = true;
  }

  return true;
}

//获取出牌位置
CPoint CUserCard::GetDispatchCardPos()
{
  CPoint ptTail = m_ControlPoint;
  if(m_CardDirection == Direction_East)
  {
    ptTail.y -= g_CardResource.m_ImageUserLeft.GetHeight();
  }
  return ptTail;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDiscardCard::CDiscardCard()
{
  //扑克数据
  m_cbCurrentCount = 0;
  m_cbCurrentMaxCount = 15;

  //控制变量
  m_ControlPoint.SetPoint(0,0);
  m_CardDirection=Direction_East;

  return;
}

//析构函数
CDiscardCard::~CDiscardCard()
{
}

//绘画扑克
void CDiscardCard::DrawCardControl(CDC * pDC)
{
  //绘画控制
  switch(m_CardDirection)
  {
    case Direction_East:  //东向
    {
      //绘画扑克
      for(int i = 0; i < m_ItemArray.GetCount(); i++)
      {
        tagDiscardItem &di = m_ItemArray[i];
        int nXPos=m_ControlPoint.x+di.ptExcusion.x;
        int nYPos=m_ControlPoint.y+di.ptExcusion.y;
        g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,di.cbCardData,nXPos,nYPos);
      }

      break;
    }
    case Direction_South: //南向
    {
      if(m_ItemArray.GetCount() == 0)
      {
        break;
      }

      //绘画扑克
      int nDrawCount = 0;
      int nMaxCount = 15;
      while(nDrawCount+nMaxCount < m_ItemArray.GetCount())
      {
        nDrawCount += nMaxCount;
        nMaxCount -= 2;
      }
      int nBeginIndex = nDrawCount;
      int nEndIndex = (int)m_ItemArray.GetCount()-1;
      while(nDrawCount >= 0)
      {
        //绘画列
        for(; nBeginIndex <= nEndIndex; nBeginIndex++)
        {
          tagDiscardItem &di = m_ItemArray[nBeginIndex];
          int nXPos=m_ControlPoint.x+di.ptExcusion.x;
          int nYPos=m_ControlPoint.y+di.ptExcusion.y;
          g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,di.cbCardData,nXPos,nYPos);
        }
        nEndIndex = nDrawCount-1;
        nMaxCount += 2;
        nBeginIndex = nDrawCount-nMaxCount;
        nDrawCount -= nMaxCount;
      };

      break;
    }
    case Direction_West:  //西向
    {
      //绘画扑克
      for(int i = (int)m_ItemArray.GetCount()-1; i >= 0; i--)
      {
        tagDiscardItem &di = m_ItemArray[i];
        int nXPos=m_ControlPoint.x+di.ptExcusion.x;
        int nYPos=m_ControlPoint.y+di.ptExcusion.y;
        g_CardResource.m_ImageTableRight.DrawCardItem(pDC,di.cbCardData,nXPos,nYPos);
      }

      break;
    }
    case Direction_North: //北向
    {
      if(m_ItemArray.GetCount() == 0)
      {
        break;
      }

      //绘画扑克
      int nMaxCount = 15;
      if(nMaxCount > m_ItemArray.GetCount())
      {
        nMaxCount = (int)m_ItemArray.GetCount();
      }
      int nBeginIndex = nMaxCount-1;
      int nEndIndex = 0;
      int nDrawCount = 0;
      while(nDrawCount < m_ItemArray.GetCount())
      {
        nDrawCount += nBeginIndex-nEndIndex+1;
        //绘画列
        for(; nBeginIndex >= nEndIndex; nBeginIndex--)
        {
          tagDiscardItem &di = m_ItemArray[nBeginIndex];
          int nXPos=m_ControlPoint.x+di.ptExcusion.x;
          int nYPos=m_ControlPoint.y+di.ptExcusion.y;
          g_CardResource.m_ImageTableTop.DrawCardItem(pDC,di.cbCardData,nXPos,nYPos);
        }
        nEndIndex = nDrawCount;
        nMaxCount -= 2;
        nBeginIndex = nDrawCount+nMaxCount-1;
        if(nDrawCount + nMaxCount > m_ItemArray.GetCount())
        {
          nBeginIndex = (int)m_ItemArray.GetCount()-1;
        }
      }
      break;
    }
  }

  return;
}

//增加扑克
bool CDiscardCard::AddCardItem(BYTE cbCardData)
{
  //定义变量
  tagDiscardItem di;
  di.cbCardData = cbCardData;

  //设置坐标
  //如果超过当前列最大数
  if(++m_cbCurrentCount > m_cbCurrentMaxCount)
  {
    //如果已经排到最小列
    if(m_cbCurrentMaxCount == 2)
    {
      //替换最后一个牌
      m_ItemArray[m_ItemArray.GetCount()-1].cbCardData = cbCardData;
      return true;
    }
    //重新开始一列
    m_cbCurrentMaxCount -= 2;
    m_cbCurrentCount = 1;
  }

  //计算偏移
  CPoint pt;
  switch(m_CardDirection)
  {
    case Direction_East:  //东
    {
      int nItemWidth = g_CardResource.m_ImageTableLeft.GetViewWidth();
      int nItemHeight = Y_TABLE_LEFT_EXCUSION;
      int nRowIndex = (10-m_cbCurrentMaxCount)/2;
      pt.x = nItemWidth*nRowIndex;
      pt.y = (m_cbCurrentCount-1+nRowIndex)*nItemHeight;
    }
    break;
    case Direction_South: //南
    {
      int nItemWidth = g_CardResource.m_ImageTableBottom.GetViewWidth();
      int nItemHeight = Y_TABLE_BOTTOM_EXCUSION;
      int nRowIndex = (15-m_cbCurrentMaxCount)/2;
      pt.x = (m_cbCurrentCount-1+nRowIndex)*nItemWidth;
      pt.y = -nItemHeight*nRowIndex;
    }
    break;
    case Direction_West:  //西
    {
      int nItemWidth = g_CardResource.m_ImageTableRight.GetViewWidth();
      int nItemHeight = Y_TABLE_RIGHT_EXCUSION;
      int nRowIndex = (10-m_cbCurrentMaxCount)/2;
      pt.x = -nItemWidth*nRowIndex;
      pt.y = -(m_cbCurrentCount-1+nRowIndex)*nItemHeight - g_CardResource.m_ImageTableRight.GetViewHeight();
    }
    break;
    case Direction_North: //北
    {
      int nItemWidth = g_CardResource.m_ImageTableTop.GetViewWidth();
      int nItemHeight = Y_TABLE_TOP_EXCUSION;
      int nRowIndex = (15-m_cbCurrentMaxCount)/2;
      pt.x = -(m_cbCurrentCount+nRowIndex)*nItemWidth;
      pt.y = nItemHeight*nRowIndex;
    }
    break;
  }

  //设置偏移
  di.ptExcusion = pt;

  //添加
  m_ItemArray.Add(di);

  return true;
}

//设置方向
void CDiscardCard::SetDirection(enDirection Direction)
{
  m_CardDirection = Direction;

  //设置当前列最大显示数目,水平方向第一列为15,往后逐步减2,垂直方向第一列为10,往后逐步减2
  if(m_CardDirection == Direction_East || m_CardDirection == Direction_West)
  {
    m_cbCurrentMaxCount = 10;
  }
  else
  {
    m_cbCurrentMaxCount = 15;
  }
}

//设置扑克
bool CDiscardCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
  m_cbCurrentCount = 0;

  //设置当前列最大显示数目,水平方向第一列为15,往后逐步减2,垂直方向第一列为10,往后逐步减2
  if(m_CardDirection == Direction_East || m_CardDirection == Direction_West)
  {
    m_cbCurrentMaxCount = 10;
  }
  else
  {
    m_cbCurrentMaxCount = 15;
  }

  m_ItemArray.RemoveAll();
  for(WORD i = 0; i < wCardCount; i++)
  {
    AddCardItem(cbCardData[i]);
  }

  return true;
}

//获取位置
CPoint CDiscardCard::GetLastCardPosition()
{
  if(m_ItemArray.GetCount() == 0)
  {
    return CPoint(0,0);
  }

  CPoint pt = m_ItemArray[m_ItemArray.GetCount()-1].ptExcusion;
  pt.x += m_ControlPoint.x;
  pt.y += m_ControlPoint.y;

  //居中
  switch(m_CardDirection)
  {
    case Direction_East:
    case Direction_West:
    {
      pt.x += g_CardResource.m_ImageTableLeft.GetViewWidth()/2;
      pt.y += Y_TABLE_LEFT_EXCUSION/2;
    }
    break;
    case Direction_North:
    case Direction_South:
    {
      pt.x += g_CardResource.m_ImageTableTop.GetViewWidth()/2;
      pt.y += Y_TABLE_TOP_EXCUSION/2;
    }
  }

  return pt;
}

//获取出牌位置
CPoint CDiscardCard::GetDispatchCardPos()
{
  CPoint pt;

  //如果超过当前列最大数
  BYTE cbCurrentCount = m_cbCurrentCount;
  BYTE cbCurrentMaxCount = m_cbCurrentMaxCount;
  if(++cbCurrentCount > cbCurrentMaxCount)
  {
    //如果已经排到最小列
    if(cbCurrentMaxCount == 2)
    {
      //替换最后一个牌
      pt = m_ItemArray[m_ItemArray.GetCount()-1].ptExcusion;
      pt += m_ControlPoint;
      return pt;
    }
    //重新开始一列
    cbCurrentMaxCount -= 2;
    cbCurrentCount = 1;
  }

  //计算偏移
  switch(m_CardDirection)
  {
    case Direction_East:  //东
    {
      int nItemWidth = g_CardResource.m_ImageTableLeft.GetViewWidth();
      int nItemHeight = Y_TABLE_LEFT_EXCUSION;
      int nRowIndex = (10-cbCurrentMaxCount)/2;
      pt.x = nItemWidth*nRowIndex;
      pt.y = (cbCurrentCount-1+nRowIndex)*nItemHeight;
    }
    break;
    case Direction_South: //南
    {
      int nItemWidth = g_CardResource.m_ImageTableBottom.GetViewWidth();
      int nItemHeight = Y_TABLE_BOTTOM_EXCUSION;
      int nRowIndex = (15-cbCurrentMaxCount)/2;
      pt.x = (cbCurrentCount-1+nRowIndex)*nItemWidth;
      pt.y = -nItemHeight*nRowIndex;
    }
    break;
    case Direction_West:  //西
    {
      int nItemWidth = g_CardResource.m_ImageTableRight.GetViewWidth();
      int nItemHeight = Y_TABLE_RIGHT_EXCUSION;
      int nRowIndex = (10-cbCurrentMaxCount)/2;
      pt.x = -nItemWidth*nRowIndex;
      pt.y = -(cbCurrentCount-1+nRowIndex)*nItemHeight - g_CardResource.m_ImageTableRight.GetViewHeight();
    }
    break;
    case Direction_North: //北
    {
      int nItemWidth = g_CardResource.m_ImageTableTop.GetViewWidth();
      int nItemHeight = Y_TABLE_TOP_EXCUSION;
      int nRowIndex = (15-cbCurrentMaxCount)/2;
      pt.x = -(cbCurrentCount+nRowIndex)*nItemWidth;
      pt.y = nItemHeight*nRowIndex;
    }
    break;
  }

  //加基准位置
  pt += m_ControlPoint;

  return pt;
}

//移除牌
void CDiscardCard::RemoveCard(BYTE cbRemoveCount)
{
  ASSERT(cbRemoveCount <= m_ItemArray.GetCount());
  if(cbRemoveCount > m_ItemArray.GetCount())
  {
    return;
  }

  INT nCount = (INT)m_ItemArray.GetCount();
  m_ItemArray.RemoveAt(nCount-cbRemoveCount,cbRemoveCount);

  //设置变量
  if(m_cbCurrentCount < cbRemoveCount)
  {
    cbRemoveCount -= m_cbCurrentCount;
    if(m_CardDirection == Direction_East || m_CardDirection == Direction_West)
    {
      m_cbCurrentMaxCount = __min(10,m_cbCurrentMaxCount+2);
    }
    else
    {
      m_cbCurrentMaxCount = __min(15,m_cbCurrentMaxCount+2);
    }
    while(cbRemoveCount > m_cbCurrentMaxCount)
    {
      cbRemoveCount -= m_cbCurrentMaxCount;
      if(m_CardDirection == Direction_East || m_CardDirection == Direction_West)
      {
        m_cbCurrentMaxCount = __min(10,m_cbCurrentMaxCount+2);
      }
      else
      {
        m_cbCurrentMaxCount = __min(15,m_cbCurrentMaxCount+2);
      }
    }
    m_cbCurrentCount = m_cbCurrentMaxCount-cbRemoveCount;
  }
  else
  {
    m_cbCurrentCount -= cbRemoveCount;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableCard::CTableCard()
{
  //扑克数据
  m_wCardCount=0;
  ZeroMemory(m_cbCardData,sizeof(m_cbCardData));

  //控制变量
  m_ControlPoint.SetPoint(0,0);
  m_CardDirection=Direction_East;

  return;
}

//析构函数
CTableCard::~CTableCard()
{
}

//绘画扑克
void CTableCard::DrawCardControl(CDC * pDC)
{
  //绘画控制
  switch(m_CardDirection)
  {
    case Direction_East:  //东向
    {
      int nYExcusion = Y_TABLE_LEFT_EXCUSION;
      int nXPos = m_ControlPoint.x;
      int nYPos = m_ControlPoint.y+(CountArray(m_cbCardData)-m_wCardCount)*nYExcusion;

      //绘画扑克
      for(WORD i=0; i<m_wCardCount; i++)
      {
        g_CardResource.m_ImageTableLeft.DrawCardItem(pDC,m_cbCardData[i],nXPos,nYPos);
        nYPos += nYExcusion;
      }

      break;
    }
    case Direction_South: //南向
    {
      int nXExcusion = g_CardResource.m_ImageTableBottom.GetViewWidth();
      int nXPos = m_ControlPoint.x+(CountArray(m_cbCardData)-m_wCardCount)*nXExcusion;
      int nYPos = m_ControlPoint.y;

      //绘画扑克
      for(WORD i=0; i<m_wCardCount; i++)
      {
        g_CardResource.m_ImageTableBottom.DrawCardItem(pDC,m_cbCardData[i],nXPos,nYPos);
        nXPos += nXExcusion;
      }

      break;
    }
    case Direction_West:  //西向
    {
      int nYExcusion = Y_TABLE_RIGHT_EXCUSION;
      int nXPos = m_ControlPoint.x;
      int nYPos = m_ControlPoint.y-(CountArray(m_cbCardData)-1)*nYExcusion-g_CardResource.m_ImageTableRight.GetViewHeight();

      //绘画扑克
      for(WORD i=0; i<m_wCardCount; i++)
      {
        g_CardResource.m_ImageTableRight.DrawCardItem(pDC,m_cbCardData[m_wCardCount-i-1],nXPos,nYPos);
        nYPos += nYExcusion;
      }

      break;
    }
    case Direction_North: //北向
    {
      int nItemWidth = g_CardResource.m_ImageTableTop.GetViewWidth();
      int nXPos = m_ControlPoint.x-CountArray(m_cbCardData)*nItemWidth;
      int nYPos = m_ControlPoint.y;

      //绘画扑克
      for(WORD i=0; i<m_wCardCount; i++)
      {
        g_CardResource.m_ImageTableTop.DrawCardItem(pDC,m_cbCardData[m_wCardCount-i-1],nXPos,nYPos);
        nXPos += nItemWidth;
      }

      break;
    }
  }

  return;
}

//设置扑克
bool CTableCard::SetCardData(const BYTE cbCardData[], WORD wCardCount)
{
  //效验大小
  ASSERT(wCardCount<=CountArray(m_cbCardData));
  if(wCardCount>CountArray(m_cbCardData))
  {
    return false;
  }

  //设置扑克
  m_wCardCount=wCardCount;
  CopyMemory(m_cbCardData,cbCardData,sizeof(m_cbCardData[0])*wCardCount);

  return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
  //状态变量
  m_bPositively=false;
  m_bDisplayItem=false;
  m_bSelectMode = false;
  m_cbSelectInfoCount = 0;
  m_cbCurSelectIndex = CountArray(m_SelectCardInfo);
  m_cbMagicCard=0;
  ZeroMemory(m_SelectCardInfo,sizeof(m_SelectCardInfo));
  for(BYTE i = 0; i < CountArray(m_rcSelectCard); i++)
  {
    m_rcSelectCard[i].SetRectEmpty();
  }

  //位置变量
  m_XCollocateMode=enXCenter;
  m_YCollocateMode=enYCenter;
  m_BenchmarkPos.SetPoint(0,0);

  //扑克数据
  m_wCardCount=0;
  m_wHoverItem=INVALID_ITEM;
  ZeroMemory(&m_CurrentCard,sizeof(m_CurrentCard));
  ZeroMemory(&m_CardItemArray,sizeof(m_CardItemArray));

  //加载设置
  m_ControlPoint.SetPoint(0,0);
  m_ControlSize.cy = CARD_HEIGHT+POS_SHOOT;
  m_ControlSize.cx=(CountArray(m_CardItemArray)+1)*CARD_WIDTH+POS_SPACE;



  return;
}

//析构函数
CCardControl::~CCardControl()
{
}

bool CCardControl::LoadResource(CWnd *pWnd)
{
  //百搭图片
  m_ImageBaida.LoadFromResource(pWnd, AfxGetInstanceHandle() ,IDB_BAIDA);

  //麻将背景
  m_ImageCurrentCardBack.LoadFromResource(pWnd, AfxGetInstanceHandle() ,IDB_CURRENT_CARD_BACK);

  return true;

}
//基准位置
void CCardControl::SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
  //设置变量
  m_BenchmarkPos.x=nXPos;
  m_BenchmarkPos.y=nYPos;
  m_XCollocateMode=XCollocateMode;
  m_YCollocateMode=YCollocateMode;

  //横向位置
  switch(m_XCollocateMode)
  {
    case enXLeft:
    {
      m_ControlPoint.x=m_BenchmarkPos.x;
      break;
    }
    case enXCenter:
    {
      m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx/2;
      break;
    }
    case enXRight:
    {
      m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx;
      break;
    }
  }

  //竖向位置
  switch(m_YCollocateMode)
  {
    case enYTop:
    {
      m_ControlPoint.y=m_BenchmarkPos.y;
      break;
    }
    case enYCenter:
    {
      m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy/2;
      break;
    }
    case enYBottom:
    {
      m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy;
      break;
    }
  }

  return;
}

//基准位置
void CCardControl::SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
  //设置变量
  m_BenchmarkPos=BenchmarkPos;
  m_XCollocateMode=XCollocateMode;
  m_YCollocateMode=YCollocateMode;

  //横向位置
  switch(m_XCollocateMode)
  {
    case enXLeft:
    {
      m_ControlPoint.x=m_BenchmarkPos.x;
      break;
    }
    case enXCenter:
    {
      m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx/2;
      break;
    }
    case enXRight:
    {
      m_ControlPoint.x=m_BenchmarkPos.x-m_ControlSize.cx;
      break;
    }
  }

  //竖向位置
  switch(m_YCollocateMode)
  {
    case enYTop:
    {
      m_ControlPoint.y=m_BenchmarkPos.y;
      break;
    }
    case enYCenter:
    {
      m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy/2;
      break;
    }
    case enYBottom:
    {
      m_ControlPoint.y=m_BenchmarkPos.y-m_ControlSize.cy;
      break;
    }
  }

  return;
}

//获取弹起扑克
BYTE CCardControl::GetHoverCard()
{
  //获取扑克
  if(m_wHoverItem!=INVALID_ITEM)
  {
    if(m_wHoverItem==CountArray(m_CardItemArray))
    {
      return m_CurrentCard.cbCardData;
    }
    else
    {
      return m_CardItemArray[m_wHoverItem].cbCardData;
    }
  }

  return 0;
}

//设置扑克
bool CCardControl::SetCurrentCard(BYTE cbCardData)
{
  //设置变量
  m_CurrentCard.bShoot=false;
  m_CurrentCard.bGrayed = false;
  m_CurrentCard.cbCardData=cbCardData;

  return true;
}

//设置扑克
bool CCardControl::SetCurrentCard(tagCardItem CardItem)
{
  //设置变量
  m_CurrentCard = CardItem;

  return true;
}

//设置扑克
bool CCardControl::SetCardData(const BYTE cbCardData[], WORD wCardCount, BYTE cbCurrentCard)
{
  //效验大小
  ASSERT(wCardCount<=CountArray(m_CardItemArray));
  if(wCardCount>CountArray(m_CardItemArray))
  {
    return false;
  }

  //当前扑克
  m_CurrentCard.bShoot=false;
  m_CurrentCard.bGrayed = false;
  m_CurrentCard.cbCardData=cbCurrentCard;

  //设置扑克
  m_wCardCount=wCardCount;
  for(WORD i=0; i<m_wCardCount; i++)
  {
    m_CardItemArray[i].bShoot=false;
    m_CardItemArray[i].bGrayed = false;
    m_CardItemArray[i].cbCardData=cbCardData[i];
  }

  return true;
}

//添加扑克
bool CCardControl::AddCardData(const BYTE cbCardData[], WORD wCardCount)
{
  ASSERT(m_wCardCount+(m_CurrentCard.cbCardData!=0?1:0)+wCardCount <= MAX_COUNT);
  if(m_wCardCount+(m_CurrentCard.cbCardData!=0?1:0)+wCardCount > MAX_COUNT)
  {
    return false;
  }

  for(WORD i = 0; i < wCardCount; i++)
  {
    if(m_wCardCount < CountArray(m_CardItemArray))
    {
      m_CardItemArray[m_wCardCount].bShoot = false;
      m_CardItemArray[m_wCardCount].bGrayed = false;
      m_CardItemArray[m_wCardCount++].cbCardData = cbCardData[i];
    }
    else
    {
      ASSERT(i == wCardCount-1);
      m_CurrentCard.bShoot = false;
      m_CurrentCard.bGrayed = false;
      m_CurrentCard.cbCardData = cbCardData[i];
    }
  }
  return true;
}

//是否财神
bool CCardControl::IsMagicCard(const BYTE cbMagicCard)
{
  if(m_cbMagicCard != 0 && cbMagicCard != 0)
  {
    if(m_cbMagicCard >= 0x38 && m_cbMagicCard <= 0x3B)
    {
      return ((cbMagicCard >= 0x38) && (cbMagicCard <= 0x3B));
    }
    else if(m_cbMagicCard >= 0x3C && m_cbMagicCard <= 0x3F)
    {
      return ((cbMagicCard >= 0x3C) && (cbMagicCard <= 0x3F));
    }
    else
    {
      return (cbMagicCard==m_cbMagicCard);
    }
  }
  return false;
}

//设置扑克
bool CCardControl::SetCardItem(const tagCardItem CardItemArray[], WORD wCardCount)
{
  //效验大小
  ASSERT(wCardCount<=CountArray(m_CardItemArray));
  if(wCardCount>CountArray(m_CardItemArray))
  {
    return false;
  }

  //设置扑克
  m_wCardCount=wCardCount;
  for(WORD i=0; i<m_wCardCount; i++)
  {
    m_CardItemArray[i] = CardItemArray[i];
  }

  return true;
}

//绘画扑克
void CCardControl::DrawCardControl(CDC * pDC)
{
  //百搭扑克

  //绘画准备
  int nXExcursion=m_ControlPoint.x+(CountArray(m_CardItemArray)-m_wCardCount)*CARD_WIDTH;

  //绘画扑克
  for(WORD i=0; i<m_wCardCount; i++)
  {
    //计算位置
    int nXScreenPos=nXExcursion+CARD_WIDTH*i;
    int nYScreenPos=m_ControlPoint.y+(m_CardItemArray[i].bShoot==false?POS_SHOOT:0);

    //绘画扑克
    BYTE cbCardData=(m_bDisplayItem==true)?m_CardItemArray[i].cbCardData:0;
    if(!m_CardItemArray[i].bGrayed)
    {
      g_CardResource.m_ImageUserBottom.DrawCardItem(pDC,cbCardData,nXScreenPos,nYScreenPos);
    }
    else
    {
      g_CardResource.m_ImageUserBtGrayed.DrawCardItem(pDC,cbCardData,nXScreenPos,nYScreenPos);
    }

    if(m_cbMagicCard != 0 && IsMagicCard(cbCardData))
    {
      m_ImageBaida.TransDrawImage(pDC, nXScreenPos, nYScreenPos+0,RGB(255,0,255));
    }
  }

  //当前扑克
  if(m_CurrentCard.cbCardData!=0)
  {
    //计算位置
    int nXScreenPos=m_ControlPoint.x+CountArray(m_CardItemArray)*CARD_WIDTH+POS_SPACE;
    int nYScreenPos=m_ControlPoint.y+(m_CurrentCard.bShoot==false?POS_SHOOT:0);

    //绘画背景
    if(m_bDisplayItem)
    {
      INT nXPos = nXScreenPos-8;
      INT nYPos = m_ControlPoint.y + 5 - 8;
      m_ImageCurrentCardBack.TransDrawImage(pDC, nXPos, nYPos,RGB(255,0,255));
    }

    //绘画扑克
    BYTE cbCardData=(m_bDisplayItem==true)?m_CurrentCard.cbCardData:0;
    if(!m_CurrentCard.bGrayed)
    {
      g_CardResource.m_ImageUserBottom.DrawCardItem(pDC,cbCardData,nXScreenPos,nYScreenPos);
    }
    else
    {
      g_CardResource.m_ImageUserBtGrayed.DrawCardItem(pDC,cbCardData,nXScreenPos,nYScreenPos);
    }

    if(m_cbMagicCard != 0 && IsMagicCard(cbCardData))
    {
      m_ImageBaida.TransDrawImage(pDC, nXScreenPos, nYScreenPos+0,RGB(255,0,255));
    }
  }

  return;
}

//索引切换
WORD CCardControl::SwitchCardPoint(CPoint & MousePoint)
{
  //基准位置
  int nXPos=MousePoint.x-m_ControlPoint.x;
  int nYPos=MousePoint.y-m_ControlPoint.y;

  //范围判断
  if((nXPos<0)||(nXPos>m_ControlSize.cx))
  {
    return INVALID_ITEM;
  }
  if((nYPos<POS_SHOOT)||(nYPos>m_ControlSize.cy))
  {
    return INVALID_ITEM;
  }

  //牌列子项
  if(nXPos<CARD_WIDTH*(int)CountArray(m_CardItemArray))
  {
    WORD wViewIndex=(WORD)(nXPos/CARD_WIDTH)+m_wCardCount;
    if(wViewIndex>=CountArray(m_CardItemArray))
    {
      wViewIndex -= CountArray(m_CardItemArray);
      //如果该牌不是灰色状态,则返回该索引
      if(!m_CardItemArray[wViewIndex].bGrayed)
      {
        return wViewIndex;
      }
    }
    return INVALID_ITEM;
  }

  //当前子项
  if((m_CurrentCard.cbCardData!=0)&&(nXPos>=(m_ControlSize.cx-CARD_WIDTH))&&!m_CurrentCard.bGrayed)
  {
    return CountArray(m_CardItemArray);
  }

  return INVALID_ITEM;
}

//光标消息
bool CCardControl::OnEventSetCursor(CPoint Point, CRect &rcRePaint)
{
  rcRePaint.SetRectEmpty();

  if(!m_bSelectMode && m_bPositively==true)
  {
    //获取索引
    WORD wHoverItem=SwitchCardPoint(Point);
    //响应判断
    if((m_bPositively==false)&&(m_wHoverItem!=INVALID_ITEM))
    {
      m_wHoverItem=INVALID_ITEM;
    }
    if(wHoverItem != m_wHoverItem)
    {
      //计算区域
      CRect rcOld = GetCardRect(m_wHoverItem);
      CRect rcNew = GetCardRect(wHoverItem);
      rcRePaint.UnionRect(rcOld,rcNew);
      rcRePaint.InflateRect(0,0,40,0);
      rcRePaint.OffsetRect(m_ControlPoint.x,m_ControlPoint.y);

      if(m_wHoverItem != INVALID_ITEM)
      {
        if(m_wHoverItem == CountArray(m_CardItemArray))
        {
          m_CurrentCard.bShoot = false;
        }
        else
        {
          m_CardItemArray[m_wHoverItem].bShoot = false;
        }
      }
      m_wHoverItem=wHoverItem;
      if(m_wHoverItem != INVALID_ITEM)
      {
        if(m_wHoverItem == CountArray(m_CardItemArray))
        {
          m_CurrentCard.bShoot = true;
        }
        else
        {
          m_CardItemArray[m_wHoverItem].bShoot = true;
        }
      }
    }

    if(wHoverItem!=INVALID_ITEM)
    {
      SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
      return true;
    }
    else
    {
      return false;
    }
  }
  //如果是选择模式下,则弹起当前选择的牌
  else if(m_bSelectMode && m_bPositively)
  {
    //设置重绘区
    if(m_cbCurSelectIndex < CountArray(m_SelectCardInfo))
    {
      CRect rc = m_rcSelectCard[m_cbCurSelectIndex];
      rc.OffsetRect(m_ControlPoint.x,m_ControlPoint.y);
      rcRePaint = rc;
    }

    //获取当前选择位置
    BYTE cbCurIndex = CountArray(m_SelectCardInfo);
    for(BYTE i = 0; i < m_cbSelectInfoCount; i++)
    {
      CRect rcValid = m_rcSelectCard[i];
      rcValid.OffsetRect(m_ControlPoint.x,m_ControlPoint.y);
      if(rcValid.PtInRect(Point))
      {
        cbCurIndex = i;
        break;
      }
    }

    //设置当前弹起
    if(m_cbCurSelectIndex != cbCurIndex)
    {
      //设置所有牌非弹起
      m_CurrentCard.bShoot = false;
      for(WORD i = 0; i < m_wCardCount; i++)
      {
        m_CardItemArray[i].bShoot = false;
      }
      //设置当前选择位置
      m_cbCurSelectIndex = cbCurIndex;
      //设置当前弹起
      if(m_cbCurSelectIndex < CountArray(m_SelectCardInfo))
      {
        for(BYTE i = 0; i < m_SelectCardInfo[cbCurIndex].cbCardCount; i++)
        {
          for(WORD j = 0; j < m_wCardCount; j++)
          {
            if(m_CardItemArray[j].cbCardData == m_SelectCardInfo[cbCurIndex].cbCardData[i] &&
               !m_CardItemArray[j].bShoot)
            {
              m_CardItemArray[j].bShoot = true;
              break;
            }
          }
          if(j == m_wCardCount &&
             m_CurrentCard.cbCardData == m_SelectCardInfo[cbCurIndex].cbCardData[i])
          {
            m_CurrentCard.bShoot = true;
          }
        }
        //加入重绘区域
        CRect rc = m_rcSelectCard[cbCurIndex];
        rc.OffsetRect(m_ControlPoint.x,m_ControlPoint.y);
        rcRePaint |= rc;
      }
    }

    if(m_cbCurSelectIndex!=CountArray(m_SelectCardInfo))
    {
      SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
      return true;
    }
    else
    {
      return false;
    }
  }

  return false;
}

//设置所有牌是否灰色
void CCardControl::SetAllCardGray(bool bGray)
{
  for(WORD i = 0; i < m_wCardCount; i++)
  {
    m_CardItemArray[i].bGrayed = bGray;
  }

  if(m_CurrentCard.cbCardData != 0)
  {
    m_CurrentCard.bGrayed = bGray;
  }
}

//设置牌灰色
void CCardControl::SetCardGray(BYTE cbCardData[], BYTE cbCardCount, bool bGray)
{
  ASSERT(cbCardCount <= m_wCardCount+1);
  if(cbCardCount > m_wCardCount+1)
  {
    return;
  }

  for(WORD i = 0; i < cbCardCount; i++)
  {
    //查找相应牌
    for(WORD j = 0; j < m_wCardCount; j++)
    {
      if(m_CardItemArray[j].cbCardData == cbCardData[i])
      {
        m_CardItemArray[j].bGrayed = bGray;
        break;
      }
    }
    if(j == m_wCardCount && m_CurrentCard.cbCardData == cbCardData[i])
    {
      m_CurrentCard.bGrayed = bGray;
    }
  }
}

//获取灰色牌
BYTE CCardControl::GetGrayCard(BYTE cbCardData[], BYTE cbMaxCount, bool bGetGray)
{
  //清0
  ZeroMemory(cbCardData,sizeof(BYTE)*cbMaxCount);

  BYTE cbGetCount = 0;
  //查找符合条件的牌
  for(WORD i = 0; i < m_wCardCount; i++)
  {
    if(m_CardItemArray[i].bGrayed == bGetGray)
    {
      cbCardData[cbGetCount++] = m_CardItemArray[i].cbCardData;
      if(cbGetCount == cbMaxCount)
      {
        return cbGetCount;
      }
    }
  }
  if(m_CurrentCard.bGrayed == bGetGray)
  {
    cbCardData[cbGetCount++] = m_CurrentCard.cbCardData;
  }

  return cbGetCount;
}

//设置操作模式
void CCardControl::SetSelectMode(bool bSelectMode)
{
  if(m_bSelectMode != bSelectMode)
  {
    m_bSelectMode = bSelectMode;
    if(!m_bSelectMode)
    {
      SetAllCardGray(false);
    }
  }
}

//获取出牌位置
CPoint CCardControl::GetDispatchCardPos()
{
  CPoint pt = m_ControlPoint;
  pt.x += CountArray(m_CardItemArray)*CARD_WIDTH+POS_SPACE;
  return pt;
}

//鼠标按下
bool CCardControl::OnEventLeftHitCard()
{
  if(!m_bSelectMode)
  {
    BYTE cbHoverCard = GetHoverCard();
    if(cbHoverCard != 0)
    {
      CGameFrameView *pWnd=CGameFrameView::GetInstance();
      if(pWnd)
      {
        pWnd->PostEngineMessage(IDM_OUT_CARD,cbHoverCard,0);
      }
    }
    return cbHoverCard!=0;
  }
  //选择模式下
  else
  {
    //当前选择
    if(m_cbCurSelectIndex < CountArray(m_SelectCardInfo))
    {
      CGameFrameView *pWnd=CGameFrameView::GetInstance();
      if(pWnd)
      {
        pWnd->PostEngineMessage(IDM_CARD_OPERATE,0,0);
      }
    }
    else
    {
      return false;
    }
  }
  return false;
}

//获取区域
CRect CCardControl::GetCardRect(WORD wItemIndex)
{
  //变量定义
  CRect rc;
  rc.SetRectEmpty();

  if(wItemIndex == INVALID_ITEM)
  {
    return rc;
  }

  //当前牌
  if(wItemIndex == CountArray(m_CardItemArray))
  {
    CPoint ptTopLeft(CountArray(m_CardItemArray)*CARD_WIDTH+POS_SPACE,0);
    CPoint ptRightBottom(ptTopLeft.x+CARD_WIDTH,ptTopLeft.y+CARD_HEIGHT);
    rc.SetRect(ptTopLeft,ptRightBottom);
  }
  else
  {
    WORD wViewIndex = CountArray(m_CardItemArray)-m_wCardCount+wItemIndex;
    CPoint ptTopLeft(wViewIndex*CARD_WIDTH,0);
    CPoint ptRightBottom(ptTopLeft.x+CARD_WIDTH,ptTopLeft.y+CARD_HEIGHT);
    rc.SetRect(ptTopLeft,ptRightBottom);
  }

  //扩大像素
  rc.InflateRect(0,0,0,POS_SHOOT);
  return rc;
}

//玩家吃牌
bool CCardControl::OnEventUserAction(const tagSelectCardInfo SelectInfo[MAX_COUNT], BYTE cbInfoCount)
{
  //初始化
  m_cbSelectInfoCount = cbInfoCount;
  CopyMemory(m_SelectCardInfo,SelectInfo,sizeof(tagSelectCardInfo)*cbInfoCount);

  //设置所有牌灰色
  SetAllCardGray(true);

  //设置变量
  m_bSelectMode = true;
  m_cbCurSelectIndex = CountArray(m_SelectCardInfo);

  //根据组合信息，搜索并设置牌
  for(BYTE i = 0; i < m_cbSelectInfoCount; i++)
  {
    //设置区域
    m_rcSelectCard[i].SetRectEmpty();

    for(BYTE j = 0; j < m_SelectCardInfo[i].cbCardCount; j++)
    {
      for(WORD k = 0; k < m_wCardCount; k++)
      {
        if(m_CardItemArray[k].cbCardData == m_SelectCardInfo[i].cbCardData[j])
        {
          CRect rcCard = GetCardRect(k);

          if(rcCard != (m_rcSelectCard[i]&rcCard))
          {
            //设置不灰
            m_CardItemArray[k].bGrayed = false;

            //增加区域
            m_rcSelectCard[i] |= rcCard;

            break;
          }
        }
      }
      if(k == m_wCardCount && m_CurrentCard.cbCardData == m_SelectCardInfo[i].cbCardData[j])
      {
        //设置不灰
        m_CurrentCard.bGrayed = false;

        //增加区域
        m_rcSelectCard[i] |= GetCardRect(CountArray(m_CardItemArray));
      }
    }
  }

  return m_cbSelectInfoCount==1;
}

//获取玩家操作结果
void CCardControl::GetUserSelectResult(tagSelectCardInfo &SelectInfo)
{
  if(m_bSelectMode)
  {
    ASSERT(m_cbSelectInfoCount > 0);
    if(m_cbSelectInfoCount == 0)
    {
      return;
    }
    if(m_cbCurSelectIndex < CountArray(m_SelectCardInfo))
    {
      SelectInfo = m_SelectCardInfo[m_cbCurSelectIndex];
    }
    else
    {
      SelectInfo = m_SelectCardInfo[0];
    }
  }
  else
  {
    SelectInfo.cbCardCount = 0;
  }
  return;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//变量声明
CCardResource           g_CardResource;           //扑克资源

//////////////////////////////////////////////////////////////////////////
