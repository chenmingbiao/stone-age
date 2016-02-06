#define __HANDLETIME_C__ 
#include "version.h"
#include <time.h>
#include <sys/time.h>

#include "common.h"
#include "handletime.h"

#define LSTIME_SECONDS_PER_DAY 5400 /* LSTIMEÓò  ÉÙÊÏ¼°òØ  ¼°  ÐÑ */

   
#define LSTIME_HOURS_PER_DAY 1024 /* LSTIMEÓò  ÉÙÊÏ¼°LSTIME¼°ÁÝÃÞÐÑ */
#define LSTIME_DAYS_PER_YEAR 100 /* LSTIMEÓò  ÉÙÊÏ¼°LSTIME¼°  ÐÑ */


/*
// Nuke 0701: localtime

  struct timeval NowTime;
#ifdef localtime
#undef localtime
#endif

struct tm *localtime(const time_t *timep)
{
	static struct tm lt;
	memset(&lt,0,sizeof(lt));
	lt.tm_sec=*timep %60;
	lt.tm_min=(*timep %3600) / 60;
	lt.tm_hour=(*timep % 86400) / 3600;	
	return &lt;
}
*/

/*------------------------------------------------------------
 * Â¦ÐÑ
 *  Ø¦ØÆ
 * ß¯Ô»°À
 ------------------------------------------------------------*/
BOOL setNewTime( void )
{
    if( gettimeofday( &NowTime, (struct timezone*)NULL) != 0 ) {
		NowTime.tv_sec = time(0);
		// Nuke 0701: Localtime down
		print("\n time err !! \n"); 
        return FALSE;
	}
    NowTime.tv_sec += DEBUG_ADJUSTTIME;
    return TRUE;
}


/*******************************************************************
*******************************************************************/
static long era = (long)912766409 + 5400; 
									/* SA¼°¿Ð±åÄÚÈÕØÆÐ× */
void RealTimeToLSTime(long t , LSTIME *lstime)
{
	long lsseconds = t - era; /* LSÝç  ¾®ÈÕ¼°  ÐÑ */
    long lsdays; /* LSÝç  ¾®ÈÕ¼°  ÐÑ */

	lstime->year = (int)( lsseconds/(LSTIME_SECONDS_PER_DAY*LSTIME_DAYS_PER_YEAR) );

    lsdays = lsseconds/LSTIME_SECONDS_PER_DAY;/* ÒýÄÚÝç  ¾®ÈÕ¼°  ÐÑÃ«»ÍÒ£ØÆ»¯ */
	lstime->day  = lsdays % LSTIME_DAYS_PER_YEAR;/*   ñ²Ð×Ô»¼°  ÐÑÆ¥à«ÔÈÐ×Ø¤ÒýÔ»»¥  */


    lstime->hour = (int)(lsseconds % LSTIME_SECONDS_PER_DAY )
        * LSTIME_HOURS_PER_DAY / LSTIME_SECONDS_PER_DAY;

	return;
}

/*******************************************************************
	LSÁÝÃÞ¾®ÈÕÑ¨ÆË¼þÁÝÃÞ±åÔÊÔÂ
*******************************************************************/
void LSTimeToRealTime( LSTIME *lstime, long *t)
{
	*t=(long)(
        ( lstime->hour*LSTIME_DAYS_PER_YEAR+lstime->day) /* ÁÝÃÞ */
               *LSTIME_HOURS_PER_DAY

        +     lstime->year)


        *450;
	return;
}

/*******************************************************************
*******************************************************************/
LSTIME_SECTION getLSTime (LSTIME *lstime)
{
	if (NIGHT_TO_MORNING < lstime->hour
        && lstime->hour <= MORNING_TO_NOON)
		return LS_MORNING;
	else if(NOON_TO_EVENING < lstime->hour
            && lstime->hour <= EVENING_TO_NIGHT)
		return LS_EVENING;
	else if(EVENING_TO_NIGHT < lstime->hour
            && lstime->hour <= NIGHT_TO_MORNING)
		return LS_NIGHT;
	else
		return LS_NOON;
}


#ifdef _ASSESS_SYSEFFICACY
static clock_t TotalClock = 0;
static clock_t StartClock = 0;
//static int EndClock = 0;
//static float SysTime=0.0;
static clock_t EndClock = 0;
static double SysTime=0.0;
static int Cnum = 0;

#ifdef _ASSESS_SYSEFFICACY_SUB
static clock_t Net_TotalClock = 0;
static clock_t NPCGEN_TotalClock = 0;
static clock_t Battle_TotalClock = 0;
static clock_t Char_TotalClock = 0;
static clock_t Petmail_TotalClock = 0;
static clock_t Family_TotalClock = 0;
static clock_t SaveCheck_TotalClock = 0;
static clock_t GMBroadCast_TotalClock = 0;
static double Net_SysTime=0.0;
static double NPCGEN_SysTime=0.0;
static double Battle_SysTime=0.0;
static double Char_SysTime=0.0;
static double Petmail_SysTime=0.0;
static double Family_SysTime=0.0;
static double SaveCheck_SysTime=0.0;
static double GMBroadCast_SysTime=0.0;
static clock_t SubStartClock = 0;
#endif

void Assess_InitSysEfficacy()
{
	TotalClock = 0;
	StartClock = 0;
	EndClock = 0;
#ifdef _ASSESS_SYSEFFICACY_SUB
	Net_TotalClock = 0;
	NPCGEN_TotalClock = 0;
	Battle_TotalClock = 0;
	Char_TotalClock = 0;
	Petmail_TotalClock = 0;
	Family_TotalClock = 0;
	SaveCheck_TotalClock = 0;
	GMBroadCast_TotalClock = 0;

	SubStartClock = 0;
#endif
}

void Assess_SysEfficacy( int flg)
{
	if( flg == 0 ){
		StartClock=clock();
	}else if( flg == 1 ){
		EndClock = clock();
		if( EndClock < StartClock ) return;
		TotalClock += (int)(EndClock-StartClock);
		Cnum++;
		if( Cnum%500 == 0 ){
			SysTime = (float)(TotalClock/Cnum)/CLOCKS_PER_SEC;
			TotalClock = 0;
#ifdef _ASSESS_SYSEFFICACY_SUB
			Net_SysTime = (float)(Net_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Net_TotalClock = 0;
			NPCGEN_SysTime = (float)(NPCGEN_TotalClock/Cnum)/CLOCKS_PER_SEC;
			NPCGEN_TotalClock = 0;
			Battle_SysTime = (float)(Battle_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Battle_TotalClock = 0;
			Char_SysTime = (float)(Char_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Char_TotalClock = 0;
			Petmail_SysTime = (float)(Petmail_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Petmail_TotalClock = 0;
			Family_SysTime = (float)(Family_TotalClock/Cnum)/CLOCKS_PER_SEC;
			Family_TotalClock = 0;
			SaveCheck_SysTime = (float)(SaveCheck_TotalClock/Cnum)/CLOCKS_PER_SEC;
			SaveCheck_TotalClock = 0;
			GMBroadCast_SysTime = (float)(GMBroadCast_TotalClock/Cnum)/CLOCKS_PER_SEC;
			GMBroadCast_TotalClock = 0;
#endif
			Cnum = 0;
		}
	}
	/*
	EndClock = clock();
	if( StartClock != 0 ){
		if( EndClock < StartClock ) return;
		TotalClock += (int)(EndClock-StartClock);
		Cnum++;
		if( Cnum%500 == 0 ){
			SysTime = (float)TotalClock/Cnum;
			TotalClock = 0;
		}
	}
	StartClock = EndClock;
	*/
}

void ASSESS_getSysEfficacy( float *TVsec)
{
	*TVsec = SysTime;
}

#ifdef _ASSESS_SYSEFFICACY_SUB
void Assess_SysEfficacy_sub( int flg, int loop)
{

	if( flg == 0 ){
		SubStartClock = clock();
	}else if( flg == 1 ){
		EndClock = clock();
		if( EndClock < SubStartClock ) return;

		switch( loop) {
		case 1: // Net_TotalClock
			Net_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 2: // NPCGEN_TotalClock
			NPCGEN_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 3: // Battle_TotalClock
			Battle_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 4: // Char_TotalClock
			Char_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 5: // Petmail_TotalClock
			Petmail_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 6: // Family_TotalClock
			Family_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 7: // SaveCheck_TotalClock
			SaveCheck_TotalClock += (int)(EndClock-SubStartClock);
			break;
		case 8: // GMBroadCast_TotalClock
			GMBroadCast_TotalClock += (int)(EndClock-SubStartClock);
			break;
		}
	}
}

void ASSESS_getSysEfficacy_sub( float *TVsec, int loop_index)
{
	switch( loop_index) {
	case 1:
		*TVsec = Net_SysTime;
		break;
	case 2:
		*TVsec = NPCGEN_SysTime;
		break;
	case 3:
		*TVsec = Battle_SysTime;
		break;
	case 4:
		*TVsec = Char_SysTime;
		break;
	case 5:
		*TVsec = Petmail_SysTime;
		break;
	case 6:
		*TVsec = Family_SysTime;
		break;
	case 7:
		*TVsec = SaveCheck_SysTime;
		break;
	case 8:
		*TVsec = GMBroadCast_SysTime;
		break;
	}
}

#endif

#endif

