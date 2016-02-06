#ifndef __NPC_SCHEDULEMAN_H__
#define __NPC_SCHEDULEMAN_H__

#define CHARNAME_MAX	32

// 最大 scheduleman 数量
#ifdef _FAMILY_MANORNUM_CHANGE
#define MAX_SCHEDULEMAN	20
#else
#define MAX_SCHEDULEMAN	12
#endif

// 每一个 scheduleman 控管的 schedule 数量
#define MAX_SCHEDULE	24

// schedule 列表的一页所能列出的最大数量
#define MAXSCHEDULEINONEWINDOW	10

// family 列表的一页所能列出的最大数量
#define MAXFAMILYINONEWINDOW	10

// 每一个 scheduleman 所使用的 schedule 储存位置
// 对应在 fmpks 的 ID*MAX_SCHEDULE 上 (ID=0~MAX_SCHEDULEMAN-1)
// ID 是写在 data/npc 中指定给 "id:" 的值

// dueltime = (隔天?10000:0) + (小时*100)

typedef struct tagFamilyPKSchedule {
	int	dueltime;	// 比赛时刻
	int	host_index;	// 主队家族
	char host_name[CHARNAME_MAX];	// 主队家族名称
	int	guest_index;	// 客队家族
	char guest_name[CHARNAME_MAX];	// 客队家族名称
	int	prepare_time;	// 准备时间 (1~40 分钟)
	int	max_player;	// 最大出场人数 (1~家族人数上限)
	int	flag;		// 状态
	int	win;		// 胜利条件设定
	int	challenge_timeout;	// 挑战时限
	int	setting_timeout;	// 设定状态的 timeout
	char gmsv_name[256];
} FamilyPKSchedule;

// 纪录在 FamilyPKSchedule (fmpks) 的 flag 值
#define FMPKS_FLAG_NONE		-1	// 没有任何排程
#define FMPKS_FLAG_CHALLENGE	0	// 等待客队同意中
#define FMPKS_FLAG_SETTING	1	// 主队正在设定排程 (取消时变成 NONE)
#define FMPKS_FLAG_CONFIRMING	2	// 客队正在同意中
#define FMPKS_FLAG_SCHEDULED	3	// 已经排好排程，尚未开打
#define FMPKS_FLAG_DUEL		4	// 开打中
#define FMPKS_FLAG_HOSTWIN	5	// 主队胜
#define FMPKS_FLAG_GUESTWIN	6	// 客队胜
#define FMPKS_FLAG_MANOR_BATTLEBEGIN	7	// 庄园挑战 战斗中
#define FMPKS_FLAG_MANOR_PREPARE	8	// 庄园挑战 准备中
#define FMPKS_FLAG_MANOR_PEACE	9	// 庄园挑战 休战中
#define FMPKS_FLAG_MANOR_OTHERPLANET	10	// 庄园挑战 在别的星球决战
#define FMPKS_FLAG_MANOR_BATTLEEND	11	// 庄园挑战 战斗结束
#define FMPKS_FLAG_MANOR_PEACE_SAVE	12	// 将庄园挑战存档
#define FMPKS_FLAG_MANOR_READYTOFIGHT	13	// (GM: manorpk) 全星系变成可约战状态
#define FMPKS_FLAG_MANOR_CLEANFLAG	14	// (GM: manorpk) 本星球清除状态

// 传给 client 的 flag
#define FLAG_NONE	-1
#define FLAG_MODIFY	0
#define FLAG_ACCEPT	1
#define FLAG_SETTING	2
#define FLAG_CONFIRM	3
#define FLAG_SCHEDULED	4

extern FamilyPKSchedule fmpks[MAX_SCHEDULE*MAX_SCHEDULEMAN];

BOOL NPC_SchedulemanInit(int meindex);
void NPC_SchedulemanTalked(int meindex, int talkerindex, char *msg, int color);
void NPC_SchedulemanWindowTalked(int meindex, int talkerindex,
				 int seqno, int select, char *data);
void NPC_SchedulemanLoop(int meindex);

#endif 

/*__NPC_SCHEDULEMAN_H__*/
