#ifndef CMD_SHOWHAND_HEAD_FILE
#define CMD_SHOWHAND_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//公共宏定义

#define KIND_ID             19                  //游戏标识
#define GAME_PLAYER           2                 //游戏人数
#define GAME_NAME           TEXT("二人梭哈")          //游戏名字

#define MAX_COUNT         5                 //最大数目

#define VERSION_SERVER            PROCESS_VERSION(7,0,1)        //程序版本
#define VERSION_CLIENT            PROCESS_VERSION(7,0,1)        //程序版本

//////////////////////////////////////////////////////////////////////////
//命令码定义

#define SUB_C_ADD_GOLD          1                 //用户加注
#define SUB_C_GIVE_UP         2                 //放弃跟注
#define SUB_C_AMDIN_COMMAND       3                 //系统控制
#define SUB_C_AMDIN_COMMAND_EX      4                 //系统控制


#define SUB_S_GAME_START        100                 //游戏开始
#define SUB_S_ADD_GOLD          101                 //加注结果
#define SUB_S_GIVE_UP         102                 //放弃跟注
#define SUB_S_SEND_CARD         103                 //发牌消息
#define SUB_S_GAME_END          104                 //游戏结束
#define SUB_S_ALL_CARD          105                 //发牌消息
#define SUB_S_AMDIN_COMMAND       106                 //系统控制
#define SUB_S_AMDIN_COMMAND_EX      107                 //系统控制


#ifndef _UNICODE
#define myprintf  _snprintf
#define mystrcpy  strcpy
#define mystrlen  strlen
#define myscanf   _snscanf
#define myLPSTR   LPCSTR
#define myatoi      atoi
#define myatoi64    _atoi64
#else
#define myprintf  swprintf
#define mystrcpy  wcscpy
#define mystrlen  wcslen
#define myscanf   _snwscanf
#define myLPSTR   LPWSTR
#define myatoi      _wtoi
#define myatoi64  _wtoi64
#endif

#define IDM_ADMIN_COMMDN        WM_USER+1000
#define IDM_ADMIN_COMMDN_EX       WM_USER+1001

//////////////////////////////////////////////////////////////////////////
//消息结构体

//游戏状态
struct CMD_S_StatusFree
{
  LONGLONG              dwBasicGold;            //基础金币
  TCHAR               szGameRoomName[32];     //房间名称
};

//游戏状态
struct CMD_S_StatusPlay
{
  WORD                wCurrentUser;           //当前玩家
  BYTE                bPlayStatus[GAME_PLAYER];     //游戏状态
  BYTE                bTableCardCount[GAME_PLAYER];   //扑克数目
  BYTE                bTableCardArray[GAME_PLAYER][5];  //扑克数组
  LONGLONG              lBasicGold;             //基础金币
  LONGLONG              lTurnMaxGold;           //最大下注
  LONGLONG              lTurnBasicGold;           //最少下注
  LONGLONG              lTableGold[2*GAME_PLAYER];      //桌面金币
  LONGLONG              lShowHandScore;           //
  BYTE                bShowHand;              //是否梭哈
  LONGLONG              lServiceScore;            //服务费用
  TCHAR               szGameRoomName[32];     //房间名称
};

//游戏开始
struct CMD_S_GameStart
{
  BYTE                bFundusCard;            //底牌扑克
  WORD                wCurrentUser;           //当前玩家
  LONGLONG              lTurnMaxGold;           //最大下注
  LONGLONG              lTurnBasicGold;           //最少下注
  LONGLONG              lBasicGold;             //单元下注
  BYTE                bCardData[GAME_PLAYER];       //用户扑克
  LONGLONG              lShowHandScore;           //
  LONGLONG              lServiceScore;            //服务费用
};

//用户放弃
struct CMD_S_GiveUp
{
  WORD                wUserChairID;           //放弃用户
  LONGLONG              lTurnMaxGold;           //
};

//用户加注
struct CMD_C_AddGold
{
  LONGLONG              lGold;                //加注数目
};

//加注结果
struct CMD_S_AddGold
{
  WORD                wLastChairID;           //上一用户
  WORD                wCurrentUser;           //当前用户
  LONGLONG              lLastAddGold;           //加注数目
  LONGLONG              lCurrentLessGold;         //最少加注
  bool                bShowHand;              //是否梭哈
  bool                bNoGiveUp;              //机器人是否放弃梭哈
  LONGLONG              lShowHandGold;            //机器人梭哈金币
  bool                bAndroidLose;           //机器人输s
  bool                bCanShowHand;           //是否能梭哈
};

//发牌数据包
struct CMD_R_SendCard
{
  LONGLONG              lMaxGold;             //最大下注
  WORD                wCurrentUser;           //当前用户
  WORD                wStartChairId;            //
  BYTE                cbSendCardCount;          //
  BYTE                bUserCard[GAME_PLAYER][2];      //用户扑克
  bool                bNoGiveUp;              //机器人是否放弃
  bool                bAndroidLose;           //机器人输s
  LONGLONG              lShowHandGold;            //机器人梭哈金币
  bool                bCanShowHand;           //是否能梭哈
};

//游戏结束
struct CMD_S_GameEnd
{
  LONGLONG              lTax;               //游戏税收
  LONGLONG              lGameGold[GAME_PLAYER];       //游戏得分
  BYTE                bUserCard[GAME_PLAYER];       //用户扑克
};

//发牌数据包
struct CMD_S_AllCard
{
  bool                bAICount[GAME_PLAYER];
  BYTE                cbCardData[GAME_PLAYER][MAX_COUNT]; //用户扑克
};

//////////////////////////////////////////////////////////////////////////
struct CMD_C_AdminReq
{
  BYTE cbReqType;
#define RQ_SET_WIN_AREA 1
#define RQ_RESET_CONTROL  2
#define RQ_PRINT_SYN    3
  BYTE cbExtendData[20];      //附加数据
  BYTE cbExcuteTimes;       //执行次数
};

//请求回复
struct CMD_S_CommandResult
{
  BYTE cbAckType;         //回复类型
#define ACK_SET_WIN_AREA  1
#define ACK_PRINT_SYN     2
#define ACK_RESET_CONTROL 3
  BYTE cbResult;
#define CR_ACCEPT  2      //接受
#define CR_REFUSAL 3      //拒绝
  BYTE cbExtendData[20];      //附加数据
};

//控制区域信息
struct tagControlInfo
{
  INT  nAreaWin;    //控制区域
};

////////////////////////////////////////////////////////////////
struct CMD_C_AdminReqEx
{
  BYTE cbReqType;
#define RQ_UPDATE_STORAGE 1
#define RQ_SET_STORAGE    2
#define RQ_CLEAR_ALLSTORAGE 3
  WORD    wTableID;       //桌号
  LONGLONG  lStorage;       //库存
};

//请求回复
struct CMD_S_CommandResultEx
{
  BYTE cbAckType;         //回复类型
#define ACK_UPDATE_STORAGE  1
#define ACK_SET_STORAGE     2
#define ACK_CLEAR_ALLSTORAGE     3
  BYTE cbResult;
#define CR_ACCEPT  2      //接受
#define CR_REFUSAL 3      //拒绝
  WORD    wTableID;       //桌号
  LONGLONG  lStorage;       //库存
};

///////////////////////////////////////////////////////////////////
#endif
