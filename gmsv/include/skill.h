#ifndef __SKILL_H__
#define __SKILL_H__

#include "common.h"
#include "util.h"

typedef struct tagSKILL_intDataSetting
{
    char    *dumpskill;
}SKILL_intDataSetting;

typedef struct tagSKILL_charDataSetting
{
    char    *dumpskill;
}SKILL_charDataSetting;


typedef enum
{
    SKILL_FIRE,
    SKILL_MAGICIAN,
    SKILL_PRIEST,
    SKILL_ALOTOFTHINGS,
    SKILL_AVOIDRATEUP,
    SKILL_DETERMINEITEM,
    SKILL_DETERMINEOTHERS,
    SKILL_FIRECUT,
    SKILL_THUNDERCUT,
    SKILL_ICECUT,
    SKILL_MERCHANT,
    SKILL_HEALER,
    SKILL_FIST,
    SKILL_SWORD,
    SKILL_AXE,
    SKILL_SPEAR,
    SKILL_BOW,
    SKILL_CANE,
    SKILL_LARGEVOICE,
    SKILL_NUM
}SKILL_ID;


typedef struct tagSkillTable
{
    const int       maxlevel;
    void*           effectfunc;                                          
}SKILL_table;


typedef enum
{
    SKILL_LEVEL,		// 技能等级
    SKILL_IDENTITY,		// 技能编号
    SKILL_DATAINTNUM,
}SKILL_DATAINT;

typedef enum
{
    SKILL_DATACHARNUM,
}SKILL_DATACHAR;


typedef struct tagSkill
{
    int         data[SKILL_DATAINTNUM];
    STRING32    string[SKILL_DATACHARNUM];
}Skill;

char* SKILL_makeStringFromSkillData( Skill* sk );
BOOL  SKILL_makeSkillFromStringToArg( char* src, Skill* sk );

int SKILL_getInt( Skill* skill, int element);
int SKILL_setInt( Skill* skill, int element, int new);

char* SKILL_makeSkillStatusString( Skill* skill );

char* SKILL_makeSkillFalseString( void );

BOOL SKILL_makeSkillData( Skill* sk ,int skid, int lev );

int SKILL_levelup( Skill* sk );
int SKILL_getLevelFromSkillID( int charaindex, SKILL_ID id );
BOOL SKILL_getUpableSkillID( int charaindex,char* buf, int buflen );
void SKILL_skillEffect( int charaindex );

#endif 
/*__SKILL_H__*/


