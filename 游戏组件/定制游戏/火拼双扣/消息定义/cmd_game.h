#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//服务定义
//游戏属性
#ifdef VIDEO_GAME
#define KIND_ID           233                 //游戏 I D
#define GAME_NAME         TEXT("火拼双扣" )         //游戏名字
#define GAME_PLAYER         2                 //游戏人数
#else
#define KIND_ID           233                 //游戏 I D
#define GAME_NAME         TEXT("火拼双扣" )         //游戏名字
#define GAME_PLAYER         2                 //游戏人数
#endif

#define VERSION_SERVER            PROCESS_VERSION(7,0,1)      //程序版本
#define VERSION_CLIENT            PROCESS_VERSION(7,0,1)      //程序版本

//////////////////////////////////////////////////////////////////////////
//数目定义

#define MAX_COUNT         27                  //最大数目
#define FULL_COUNT          108                 //全牌数目
#define TYPE_COUNT          11                  //牌型数目
#define CARD_TYPE_COUNT       15                  //面值类型
#define BOMB_TYPE_COUNT       9                 //炸弹项数
//////////////////////////////////////////////////////////////////////////

//数值掩码
#define MASK_COLOR          0xF0                //花色掩码
#define MASK_VALUE          0x0F                //数值掩码


//////////////////////////////////////////////////////////////////////////
//状态定义

//游戏状态
#define GS_SK_FREE          GAME_STATUS_FREE          //等待开始
#define GS_SK_PLAYING       GAME_STATUS_PLAY          //游戏状态



//分析结构
struct tagAnalyseInitResult
{
  BYTE              cbBlockCount[8];          //扑克数目
  BYTE              cbCardData[8][MAX_COUNT];     //扑克数据
  BYTE                            cbKingCount;            //王的个数
  BYTE                            cbSubKingCount;           //王的个数
};

//分析结构
struct tagAnalyseResult
{
  //原始输入
  BYTE  m_cbCardCount;
  //牌型分类
  struct tagCardStyle
  {
    BYTE  m_cbCard;
    BYTE  m_cbCount;
  } m_CardStyle[CARD_TYPE_COUNT];         //纸牌归类

  int  m_nUnEqualCard;          //异牌面数
  int  m_nJockerCount;          //变牌数量

  //结果类型
  BYTE  m_cbCardType;           //取得牌型

  BYTE  m_cbMaxIndex;           //起始索引
  BYTE  m_cbMinIndex;           //终止索引
#define  NORINDEX_BEGIN 0   //常规牌起始索引
#define  NORINDEX_END 12    //常规牌终止索引
  int   m_nMaxCount;            //最大单种牌数

  //炸弹信息
  BYTE  m_cbBombGrade;          //炸弹等级
};
//////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_S_GAME_START      100                 //游戏函数
#define SUB_S_OUT_CARD        101                 //用户出牌
#define SUB_S_PASS_CARD       102                 //用户放弃
#define SUB_S_GAME_END        103                 //游戏结束
#define SUB_S_TRUSTEE       104                 //用户托管
#define SUB_S_PLAYVOICE             106                                 //播放声音
#define SUB_S_ON_READY        107                 //用户准备
#define SUB_S_ADMIN_COMMDN      110                 //系统控制
#define SUB_S_CHEAT_CARD      111                 //系统控制



//作弊扑克
struct CMD_S_CheatCard
{
  WORD              wCardUser[GAME_PLAYER ];        //作弊玩家
  BYTE              cbUserCount;              //作弊数量
  BYTE              cbCardData[GAME_PLAYER ][MAX_COUNT];  //扑克列表
  BYTE              cbCardCount[GAME_PLAYER ];        //扑克数量
};

struct CMD_S_OnReady
{
  WORD  wChairId;         //玩家ID

};
struct CMD_S_PlayVoice
{
  WORD  wChairId;         //播放玩家
  BYTE  type;           //播放类型
  BYTE  VoiceId;          //声音编号
};

//空闲状态
struct CMD_S_StatusFree
{
  //庄家用户
  WORD              wBankerUser;            //庄家用户
  //历史积分
  LONGLONG                        lAllTurnScore;            //总局得分
  LONGLONG                        lLastTurnScore;           //上局得分
};

//游戏状态
struct CMD_S_StatusPlay
{
  //游戏变量
  LONGLONG            lCellScore[3];            //单元积分
  WORD              wCurrentUser;           //当前玩家

  //历史积分
  LONGLONG                        lAllTurnScore[GAME_PLAYER];     //总局得分
  LONGLONG                        lLastTurnScore[GAME_PLAYER];    //上局得分

  //庄家用户
  WORD              wBankerUser;            //庄家用户

  //响应标志
  bool              bTrustee[GAME_PLAYER];        //是否托管

  //出牌信息
  WORD              wTurnWiner;                //胜利玩家
  BYTE              cbTurnCardCount;             //出牌数目
  BYTE              cbTurnCardData[MAX_COUNT];         //出牌数据
  BYTE              cbOutCardCount[GAME_PLAYER];       //出牌数目
  BYTE              cbOutCardData[GAME_PLAYER][MAX_COUNT]; //出牌列表
  tagAnalyseResult        LastOutCard;               //最后出牌

  //扑克信息
  BYTE              cbHandCardData[MAX_COUNT];      //手上扑克
  BYTE              cbHandCardCount[GAME_PLAYER];   //扑克数目
};



//发送扑克
struct CMD_S_GameStart
{
  WORD              wCurrentUser;           //当前玩家
  LONGLONG            lCellScore[3];            //单元积分
  BYTE              cbCardData[MAX_COUNT];        //扑克列表
  bool              bTrustee[GAME_PLAYER];        //是否托管
  BYTE              cbStartRandCard;          //随机扑克
  WORD              wGetRandCardID;           //拿牌用户
  BYTE              cbGetCardPosition;          //拿牌位置
  BYTE              cbPlayerCardData[MAX_COUNT];    //对方扑克，只发给机器人
};

//用户出牌
struct CMD_S_OutCard
{
  BYTE              cbCardCount;            //出牌数目
  WORD              wCurrentUser;           //当前玩家
  WORD              wOutCardUser;           //出牌玩家
  BYTE              cbCardData[MAX_COUNT];        //扑克列表
  LONGLONG            lBombScore[GAME_PLAYER];      //贡献分数
  BYTE              cbCheatCardData[MAX_COUNT];     //做弊数据
};

//放弃出牌
struct CMD_S_PassCard
{
  BYTE              cbTurnOver;             //一轮结束
  WORD              wCurrentUser;           //当前玩家
  WORD              wPassCardUser;            //放弃玩家
};

//游戏结束
struct CMD_S_GameEnd
{
  //游戏成绩
  LONGLONG            lGameScore[GAME_PLAYER];      //游戏积分
  LONGLONG            lAllTurnScore[GAME_PLAYER];     //总局得分
  LONGLONG            lLastTurnScore[GAME_PLAYER];    //上局得分
  LONGLONG            lGameTax;             //游戏税收
  //得分明细
  LONGLONG            lBombScore[GAME_PLAYER];                    //炸弹得分
  BYTE              cbBombList[GAME_PLAYER][BOMB_TYPE_COUNT];   //炸弹计数

  //扑克信息
  BYTE              cbCardCount[GAME_PLAYER];     //扑克数目
  BYTE              cbCardData[GAME_PLAYER][MAX_COUNT]; //扑克列表
  BYTE                            bHunter;              //用户抄底
  BYTE                            cbResultType;           //结果类型

  int                             nMutile;              //倍数
  int                             nBaseScore;             //基础分
};
//用户托管
struct CMD_S_Trustee
{
  bool              bTrustee;             //是否托管
  WORD              wChairID;             //托管用户
};
//////////////////////////////////////////////////////////////////////////
//命令定义

#define SUB_C_OUT_CARD        1                 //用户出牌
#define SUB_C_PASS_CARD       2                 //用户放弃
#define SUB_C_TRUSTEE       5                 //用户托管
#define SUB_C_PLAYVOICE             6                 //播放声音
#define SUB_C_REQ_HUNTERS     7                 //申请抄底
#define SUB_C_ADMIN_COMMDN      8                 //系统控制


#define IDM_ADMIN_COMMDN WM_USER+1000


struct CMD_C_PlayVoice
{
  BYTE  type;
  BYTE  VoiceId;
};

//用户出牌
struct CMD_C_OutCard
{
  BYTE              cbCardCount;            //出牌数目
  BYTE              cbCardData[MAX_COUNT];        //扑克数据
};
//用户托管
struct CMD_C_Trustee
{
  bool              bTrustee;             //是否托管
};


//服务器控制返回
#define  S_CR_FAILURE       0   //失败
#define  S_CR_UPDATE_SUCCES     1   //更新成功
#define  S_CR_SET_SUCCESS     2   //设置成功
#define  S_CR_CANCEL_SUCCESS    3   //取消成功

struct CMD_S_ControlReturns
{
  BYTE cbReturnsType;       //回复类型
  BYTE cbControlStyle;      //控制方式
  BYTE cbControlTimes;      //控制次数
};


//申请类型
#define  C_CA_UPDATE        1   //更新
#define  C_CA_SET         2   //设置
#define  C_CA_CANCELS       3   //取消

//控制方式
#define   CS_BANKER_LOSE    1
#define   CS_BANKER_WIN   2

struct CMD_C_ControlApplication
{
  BYTE cbControlAppType;      //申请类型
  BYTE cbControlStyle;      //控制方式
  BYTE cbControlTimes;      //控制次数

};

//////////////////////////////////////////////////////////////////////////

#endif