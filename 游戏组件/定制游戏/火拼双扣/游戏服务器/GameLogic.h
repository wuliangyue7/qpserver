#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//排序类型
#define ST_ORDER          0                 //大小排序
#define ST_COUNT          1                 //数目排序
#define ST_VALUE          2                 //数值排序

//数值掩码
#define MASK_COLOR          0xF0                //花色掩码
#define MASK_VALUE          0x0F                //数值掩码

//扑克类型
#define CT_ERROR          0                 //错误类型
#define CT_SINGLE         1                 //单牌类型
#define CT_DOUBLE         2                 //对子类型
#define CT_THREE          3                 //三条类型
#define CT_SINGLE_LINK        4                 //单连类型
#define CT_DOUBLE_LINK        5                 //姐妹对型
#define CT_THREE_LINK       6                 //姐妹对型
#define CT_BOMB           7                 //炸弹类型
#define CT_BOMB_3W          8                 //3王炸弹
#define CT_BOMB_TW          9                 //天王炸弹
#define CT_BOMB_LINK        10                  //排炸类型

/////////////////////////////////////

static CString GetFileDialogPath()
{
  CString strFileDlgPath;
  TCHAR szModuleDirectory[MAX_PATH];  //模块目录
  GetModuleFileName(AfxGetInstanceHandle(),szModuleDirectory,sizeof(szModuleDirectory));
  int nModuleLen=lstrlen(szModuleDirectory);
  int nProcessLen=lstrlen(AfxGetApp()->m_pszExeName)+lstrlen(TEXT(".EXE")) + 1;
  if(nModuleLen<=nProcessLen)
  {
    return TEXT("");
  }
  szModuleDirectory[nModuleLen-nProcessLen]=0;
  strFileDlgPath = szModuleDirectory;
  return strFileDlgPath;
}

static void NcaTextOut(CString strInfo)
{
#ifdef _DEBUG


  CString strName = GetFileDialogPath()+  "\\ShuangKouHP.log";
  CTime time = CTime::GetCurrentTime() ;
  CString strTime ;
  strTime.Format(TEXT("%d:%d:%d") ,
                 time.GetHour() ,
                 time.GetMinute() ,
                 time.GetSecond()
                );
  CString strMsg;
  strMsg.Format(TEXT("%s,%s\r\n"),strTime,strInfo);

  CFile fLog;
  if(fLog.Open(strName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite))
  {

    fLog.SeekToEnd();
    int strLength=strMsg.GetLength();
#ifdef _UNICODE
    BYTE bom[2] = {0xff, 0xfe};
    fLog.Write(bom,sizeof(BYTE)*2);
    strLength*=2;
#endif
    fLog.Write((LPCTSTR)strMsg,strLength);
    fLog.Close();
  }


#endif


}


//出牌结果
struct tagOutCardResult
{
  BYTE              cbCardCount;            //扑克数目
  BYTE              cbResultCard[MAX_COUNT];      //结果扑克
};

//出牌结果
struct tagOutCountResult
{
  BYTE              cbBombCount;            //炸弹数目
  BYTE              cbOutOverCount;           //出完手数
};

//分布信息
struct tagDistributing
{
  BYTE              cbCardCount;            //扑克数目
  BYTE              cbDistributing[15][13];       //分布信息
};

//////////////////////////////////////////////////////////////////////////



//游戏逻辑类
class CGameLogic
{
  //变量定义
public:
  static const BYTE       m_cbCardData[FULL_COUNT];             //扑克数据
  static const BYTE       m_cbCardGrade[TYPE_COUNT];              //牌型等级
  static const BYTE       m_cbTransTable[BOMB_TYPE_COUNT][BOMB_TYPE_COUNT]; //跃迁列表

  static BYTE                     m_cbLowCardData[13];
  static BYTE           m_cbCardColor[8];
  static BYTE           m_cbKing[4];
  BYTE                            m_cbArrayIndex;
  BYTE                            m_cbColorIndex;
  BYTE                            m_cbKingCount;
  //函数定义
public:
  //构造函数
  CGameLogic();
  //析构函数
  virtual ~CGameLogic();

  //属性函数
public:
  //获取数值
  BYTE GetCardValue(BYTE cbCardData)
  {
    return cbCardData&MASK_VALUE;
  }
  //获取花色
  BYTE GetCardColor(BYTE cbCardData)
  {
    return cbCardData&MASK_COLOR;
  }
  //控制函数
public:
  //排列扑克
  VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount,BYTE cbSortType = ST_ORDER);
  //混乱扑克
  VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
  //混乱扑克（带标记）
  BYTE RandCardList(BYTE cbCardList[],BYTE cbBufferCount,BYTE cbMainCard,WORD cbOutPut[]);
  //删除扑克
  bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
  //删除扑克
  bool RemoveCardEx(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
  //混乱数组
  void RandArray(INT nArrayOne[], INT nCountOne, INT nArrayTwo[], INT nCountTwo);
  //获取扑克
  VOID GetRandCardList(BYTE cbInitCardList[], BYTE cbInitCardCount,BYTE cbCardBuffer[], BYTE cbBufferCount);
  //设置王数
  VOID SetKingCount(BYTE cbKingCount)
  {
    m_cbKingCount=cbKingCount;
  };
  //逻辑函数
public:
  //预处理炸弹
  bool PreDealBombList(BYTE cbBombList[]);
  //炸弹转换
  bool TransBombGrade(BYTE cbBombInfo[]);
  //获得最高翻倍
  BYTE GetMaxBombGrade(BYTE *cbBomblist);
  //得到组合的最大相
  BYTE GetMaxBombGrade(BYTE cbBombList[],BYTE cbListCount);
  //逻辑数值
  BYTE GetCardLogicValue(BYTE cbCardData);
  //逻辑转牌面
  BYTE TransForLogicValue(BYTE cbCardData);
  //逻辑转牌面
  BYTE GetCardLogicValueEx(BYTE cbCardData);
  //对比扑克
  bool CompareCard(tagAnalyseResult& PerOutCard,tagAnalyseResult& CurOutCard);
  //出牌搜索
  bool SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount,BYTE cbPlayerCount, tagOutCardResult & OutCardResult);
  //出牌搜索
  bool AiSearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount,const BYTE cbPlayerCardData[],BYTE cbPlayerCount, tagOutCardResult & OutCardResult);
  //相同连张
  bool SearchSameLinkCard(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE bCardCount,BYTE bSearchLineCount,tagAnalyseInitResult &AnalyseResult);
  //分析扑克(大于搜索张数的被截断)
  VOID AnalyseSameCardData(BYTE cbCardData[], BYTE cbCardCount,BYTE bSearchCount, tagAnalyseInitResult & AnalyseResult);
  //对比手牌扑克
  bool CompareHandCard(const BYTE cbFirstHandCardData[], BYTE cbFirstHandCardCount, const BYTE cbNextHandCardData[],BYTE cbNextHandCardCount,BYTE bBombList[BOMB_TYPE_COUNT]);
  //内部函数
public:
  //炸弹分析
  bool IsBombCard(tagAnalyseResult& AnalyseResult);
  //是否相连
  bool IsLinkCardData(BYTE cbCardBuffer[], BYTE cbBufferCount);
  //查找目标牌型
  bool FindValidCard(tagAnalyseResult& CurOutCard,tagAnalyseResult& cbPerOutCard);
  //补充牌数
  bool FillCardCount(tagAnalyseResult&AnalyseResultEx,int nMinCount);
  //填充牌链
  bool FillCardSeque(tagAnalyseResult&AnalyseResult,int cbMinCount,int cbMinLink,bool bAllowCycle);
  //分析扑克
  VOID AnalysebCardData(const BYTE cbCardData[],int cbCardCount,tagAnalyseResult& AnalyseResult);
  //分析扑克
  VOID AnalyseInitCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseInitResult & AnalyseResult);
  //查找连牌
  bool FindLinkCard(tagAnalyseResult& AnalyseResult,BYTE cbMinCardCount,BYTE cbMinLinkLong,bool bLockCount,bool bAllowCycle);
  //构造扑克
  BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
  //分析分布
  VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing);
  //搜索顺子
  bool SearchLinkCard(const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbFirstCardData,const BYTE cbLastCardData,tagOutCardResult &OutCardResult);
  //搜索同点
  bool SearchSamePointCard(const BYTE cbHandCardData[],BYTE cbHandCardCount,const BYTE cbCardData,tagOutCardResult &OutCardResult);
  //是否无炸弹(没有四张相同的牌 不包括大小王)
  bool HasNotBombCard(BYTE cbHandCardData[],BYTE cbHandCardCount);
  //是否有nSameCount个以上的牌
  bool HasNSameCountCard(BYTE cbHandCardData[],BYTE cbHandCardCount,BYTE nSameCount);
  //剩几手可出完
  bool AnalyseTakeOverCount(const BYTE cbCardData[], BYTE cbCardCount,tagOutCountResult & OutCountResult);


  //列表乱序
  VOID RandCardListData(BYTE cbCardBuffer[], BYTE cbBufferCount);
  //列表乱序不能相连
  VOID RandCardListNoLink(BYTE cbCardBuffer[], BYTE cbBufferCount,BYTE cbNeedCount);
  //获取炸弹数据
  bool GetBombCardData(BYTE cbCardBuffer[], BYTE  & cbBufferCount,BYTE cbBombCount,BYTE cbSameCount,
                       BYTE cbMaxSameCount,BYTE cbMaxSameCountLimit,bool bLimitMaxBombCount=false);
  //获取0炸牌型
  VOID GetZeroBombCardData(BYTE cbCardBuffer[], BYTE  & cbBufferCount);
  //获取3炸以下牌型
  VOID GetLessThreeBombCardData(BYTE cbCardBuffer[], BYTE  & cbBufferCount,BYTE cbBombCount);
  //获取六项牌型
  VOID GetSixXiangCardData(BYTE cbCardBuffer[], BYTE & cbBufferCount);
  //获取七项牌型
  VOID GetSevenXiangCardData(BYTE cbCardBuffer[], BYTE  & cbBufferCount);
  //获取八项牌型
  VOID GetEightXiangCardData(BYTE cbCardBuffer[], BYTE  & cbBufferCount);
  //获取九项牌型
  VOID GetNineXiangCardData(BYTE cbCardBuffer[], BYTE  & cbBufferCount);
  //获取十项牌型
  VOID GetTenXiangCardData(BYTE cbCardBuffer[], BYTE  & cbBufferCount);
  //获取十一项牌型
  VOID GetElevenXiangCardData(BYTE cbCardBuffer[], BYTE &  cbBufferCount);
  //获取十二项牌型
  VOID GetTwelveXiangCardData(BYTE cbCardBuffer[], BYTE & cbBufferCount);
  //获取n连炸数据
  VOID GetLinkBombCardData(BYTE cbCardBuffer[], BYTE  & cbBufferCount,BYTE cbCardCount,BYTE cbLinkCount);
  //随机王牌
  VOID RandomSetKingCard(BYTE cbCardBuffer[], BYTE cbBufferCount);
  //生成其它牌
  VOID GetOtherCardData(BYTE cbCardBuffer[],BYTE  & cbBufferCount,BYTE cbSameCount);
  //获取相牌型
  VOID GetXiangCardDataByIndex(BYTE cbCardBuffer[], BYTE  & cbBufferCount,BYTE cbXiangIndex);

};

//////////////////////////////////////////////////////////////////////////

#endif