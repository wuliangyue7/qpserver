#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#pragma pack(push)
#pragma pack(1)


//////////////////////////////////////////////////////////////////////////////////
//服务定义

//游戏属性
#define KIND_ID           16                  //游戏 I D
#define GAME_NAME         TEXT("21 点")           //游戏名字

//组件属性
#define GAME_PLAYER         4                 //游戏人数
#define VERSION_SERVER        PROCESS_VERSION(7,0,1)        //程序版本
#define VERSION_CLIENT        PROCESS_VERSION(7,0,1)        //程序版本

//////////////////////////////////////////////////////////////////////////////////

//结束原因
#define GER_NO_PLAYER       0x10                //没有玩家
#define LEN_NICKNAME        32                  //昵称长度

//状态定义
#define GAME_SCENE_FREE       GAME_STATUS_FREE          //等待开始
#define GAME_SCENE_ADD_SCORE    GAME_STATUS_PLAY          //闲家下注
#define GAME_SCENE_GET_CARD     (GAME_STATUS_PLAY+1)        //庄家操作

//命令消息
#define IDM_ADMIN_UPDATE_STORAGE    WM_USER+1001
#define IDM_ADMIN_MODIFY_STORAGE    WM_USER+1011
#define IDM_REQUEST_QUERY_USER      WM_USER+1012
#define IDM_USER_CONTROL        WM_USER+1013
#define IDM_REQUEST_UPDATE_ROOMINFO   WM_USER+1014
#define IDM_CLEAR_CURRENT_QUERYUSER   WM_USER+1015

//操作记录
#define MAX_OPERATION_RECORD      20                  //操作记录条数
#define RECORD_LENGTH         128                 //每条记录字长

//////////////////////////////////////////////////////////////////////////
//服务器命令结构

#define SUB_S_GAME_START        100                 //游戏开始
#define SUB_S_GAME_END          101                 //游戏结束
#define SUB_S_SEND_CARD         102                 //发牌
#define SUB_S_SPLIT_CARD        103                 //分牌
#define SUB_S_STOP_CARD         104                 //停牌
#define SUB_S_DOUBLE_SCORE        105                 //加倍
#define SUB_S_INSURE          106                 //保险
#define SUB_S_ADD_SCORE         107                 //下注
#define SUB_S_GET_CARD          108                 //要牌
#define SUB_S_CHEAT_CARD        109                 //发送明牌

#define SUB_S_ADMIN_STORAGE_INFO    112                 //刷新控制服务端
#define SUB_S_REQUEST_QUERY_RESULT    113                 //查询用户结果
#define SUB_S_USER_CONTROL        114                 //用户控制
#define SUB_S_USER_CONTROL_COMPLETE   115                 //用户控制完成
#define SUB_S_OPERATION_RECORD        116                 //操作记录
#define SUB_S_REQUEST_UDPATE_ROOMINFO_RESULT 117

//机器人存款取款
struct tagCustomAndroid
{
  LONGLONG                  lRobotScoreMin;
  LONGLONG                  lRobotScoreMax;
  LONGLONG                                  lRobotBankGet;
  LONGLONG                  lRobotBankGetBanker;
  LONGLONG                  lRobotBankStoMul;
};

//控制类型
typedef enum {CONTINUE_WIN, CONTINUE_LOST, CONTINUE_CANCEL} CONTROL_TYPE;

//控制结果      控制成功 、控制失败 、控制取消成功 、控制取消无效
typedef enum {CONTROL_SUCCEED, CONTROL_FAIL, CONTROL_CANCEL_SUCCEED, CONTROL_CANCEL_INVALID} CONTROL_RESULT;

//用户行为
typedef enum {USER_SITDOWN, USER_STANDUP, USER_OFFLINE, USER_RECONNECT} USERACTION;

//控制信息
typedef struct
{
  CONTROL_TYPE            control_type;           //控制类型
  BYTE                cbControlCount;           //控制局数
  bool                  bCancelControl;           //取消标识
} USERCONTROL;

//房间用户信息
typedef struct
{
  WORD                wChairID;             //椅子ID
  WORD                wTableID;             //桌子ID
  DWORD               dwGameID;             //GAMEID
  bool                bAndroid;             //机器人标识
  TCHAR               szNickName[LEN_NICKNAME];     //用户昵称
  BYTE                cbUserStatus;           //用户状态
  BYTE                cbGameStatus;           //游戏状态
} ROOMUSERINFO;

//房间用户控制
typedef struct
{
  ROOMUSERINFO            roomUserInfo;           //房间用户信息
  USERCONTROL             userControl;            //用户控制
} ROOMUSERCONTROL;

//作弊扑克
struct CMD_S_CheatCard
{
  BYTE              cbCardData[GAME_PLAYER*2][11];  //扑克列表
};

struct CMD_S_StatusFree
{
  LONGLONG              lRoomStorageStart;          //房间起始库存
  LONGLONG              lRoomStorageCurrent;        //房间当前库存
  tagCustomAndroid          CustomAndroid;            //机器人配置
};

//游戏状态
struct CMD_S_StatusAddScore
{
  LONGLONG              lCellScore;             //单元注数
  LONGLONG              lMaxScore;              //最大注数

  WORD                wBankerUser;            //庄家

  LONGLONG              lTableScore[GAME_PLAYER];     //桌面下注
  LONGLONG              lRoomStorageStart;          //房间起始库存
  LONGLONG              lRoomStorageCurrent;        //房间当前库存
  tagCustomAndroid          CustomAndroid;            //机器人配置
};

//游戏状态
struct CMD_S_StatusGetCard
{
  SCORE               lCellScore;             //单元注数

  WORD                wBankerUser;            //庄家

  SCORE               lTableScore[GAME_PLAYER];     //桌面下注

  //扑克信息
  BYTE                cbCardCount[GAME_PLAYER*2];     //扑克数目
  BYTE                cbHandCardData[GAME_PLAYER*2][11];  //桌面扑克

  //
  BYTE                bStopCard[GAME_PLAYER*2];     //玩家停牌
  BYTE                bDoubleCard[GAME_PLAYER*2];     //玩家加倍
  BYTE                bInsureCard[GAME_PLAYER*2];     //玩家下保险
  LONGLONG              lRoomStorageStart;          //房间起始库存
  LONGLONG              lRoomStorageCurrent;        //房间当前库存
  tagCustomAndroid          CustomAndroid;            //机器人配置
};

//游戏开始
struct CMD_S_GameStart
{
  //下注信息
  SCORE               lCellScore;             //单元下注
  SCORE               lMaxScore;              //最大下注
  //用户信息
  WORD                wBankerUser;            //当前玩家
};

//下注
struct CMD_S_AddScore
{
  WORD                wAddScoreUser;            //下注玩家

  LONGLONG              lAddScore;              //下注额
};

//要牌
struct CMD_S_GetCard
{
  WORD                wGetCardUser;           //要牌玩家
  BYTE                cbCardData;             //牌数据
};

//发牌
struct CMD_S_SendCard
{
  BYTE                cbHandCardData[GAME_PLAYER][2];   //发牌数据
  bool                bWin;               //是否赢牌
};

//分牌
struct CMD_S_SplitCard
{
  WORD                wSplitUser;             //分牌玩家
  BYTE                bInsured;             //是否之前下了保险

  LONGLONG              lAddScore;              //加注额
  BYTE                cbCardData[2];            //牌数据
};

//停牌
struct CMD_S_StopCard
{
  WORD                wStopCardUser;            //停牌玩家
  BYTE                bTurnBanker;            //是否轮到庄家
};

//加倍
struct CMD_S_DoubleScore
{
  WORD                wDoubleScoreUser;         //加倍玩家

  BYTE                cbCardData;             //牌数据
  LONGLONG              lAddScore;              //加注额
};

//保险
struct CMD_S_Insure
{
  WORD                wInsureUser;            //保险玩家

  LONGLONG              lInsureScore;           //保险金
};

//游戏结束
struct CMD_S_GameEnd
{
  LONGLONG              lGameTax[GAME_PLAYER];        //游戏税收
  LONGLONG              lGameScore[GAME_PLAYER];      //游戏得分
  BYTE                cbCardData[GAME_PLAYER*2];      //用户扑克
};

struct CMD_S_RequestQueryResult
{
  ROOMUSERINFO            userinfo;             //用户信息
  bool                bFind;                //找到标识
};

//用户控制
struct CMD_S_UserControl
{
  DWORD                 dwGameID;             //GAMEID
  TCHAR                 szNickName[LEN_NICKNAME];     //用户昵称
  CONTROL_RESULT              controlResult;            //控制结果
  CONTROL_TYPE              controlType;            //控制类型
  BYTE                  cbControlCount;           //控制局数
};

//用户控制
struct CMD_S_UserControlComplete
{
  DWORD                 dwGameID;             //GAMEID
  TCHAR                 szNickName[LEN_NICKNAME];     //用户昵称
  CONTROL_TYPE              controlType;            //控制类型
  BYTE                  cbRemainControlCount;       //剩余控制局数
};

//控制服务端库存信息
struct CMD_S_ADMIN_STORAGE_INFO
{
  LONGLONG  lRoomStorageStart;            //房间起始库存
  LONGLONG  lRoomStorageCurrent;
  LONGLONG  lRoomStorageDeduct;
  LONGLONG  lMaxRoomStorage[2];
  WORD    wRoomStorageMul[2];
};

//操作记录
struct CMD_S_Operation_Record
{
  TCHAR   szRecord[MAX_OPERATION_RECORD][RECORD_LENGTH];          //记录最新操作的20条记录
};

//请求更新结果
struct CMD_S_RequestUpdateRoomInfo_Result
{
  LONGLONG              lRoomStorageCurrent;        //房间当前库存
  ROOMUSERINFO            currentqueryuserinfo;       //当前查询用户信息
  bool                bExistControl;            //查询用户存在控制标识
  USERCONTROL             currentusercontrol;
};

//////////////////////////////////////////////////////////////////////////
//客户端命令结构

#define SUB_C_ADD_SCORE         1                 //用户加注
#define SUB_C_GET_CARD          2                 //要牌
#define SUB_C_DOUBLE_SCORE        3                 //加倍
#define SUB_C_INSURE          4                 //保险
#define SUB_C_SPLIT_CARD        5                 //分牌
#define SUB_C_STOP_CARD         6                 //放弃跟注


#define SUB_C_STORAGE         10                  //更新库存
#define SUB_C_STORAGEMAXMUL       11                  //设置上限
#define SUB_C_REQUEST_QUERY_USER    12                  //请求查询用户
#define SUB_C_USER_CONTROL        13                  //用户控制

//请求更新命令
#define SUB_C_REQUEST_UDPATE_ROOMINFO 14                  //请求更新房间信息
#define SUB_C_CLEAR_CURRENT_QUERYUSER 15

//用户加注
struct CMD_C_AddScore
{
  LONGLONG                lScore;               //加注数目
};

struct CMD_C_UpdateStorage
{
  LONGLONG            lRoomStorageCurrent;          //库存数值
  LONGLONG            lRoomStorageDeduct;           //库存数值
};

struct CMD_C_ModifyStorage
{
  LONGLONG            lMaxRoomStorage[2];             //库存上限
  WORD              wRoomStorageMul[2];             //赢分概率
};

struct CMD_C_RequestQuery_User
{
  DWORD             dwGameID;               //查询用户GAMEID
  TCHAR             szNickName[LEN_NICKNAME];         //查询用户昵称
};

//用户控制
struct CMD_C_UserControl
{
  DWORD                 dwGameID;             //GAMEID
  TCHAR                 szNickName[LEN_NICKNAME];     //用户昵称
  USERCONTROL               userControlInfo;          //
};

//////////////////////////////////////////////////////////////////////////
#pragma pack(pop)
#endif