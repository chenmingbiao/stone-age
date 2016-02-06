#ifndef __NPC_QUIZ_H__
#define __NPC_QUIZ_H__

typedef struct NPC_Quiz{

	int 	no;				//  Ëø¼°  Ä¯
	int 	type;			//  Ëø¼°ÕıÄÌÃó  ±Ø¡õØ©    £ıµ©ºÌ¡õÖ­)	
	int		level;  		//  Ëø¼°ÒÁÃ¬»ï
	int 	answertype;		//²ÏÒü¼°¼»Ì«¾®Ğ×(¨á  £ı¨â  £ıÇ©»ßÑ¨ÓÀÃñ  
	int 	answerNo;		//  Ëø¼°²ÏÒü
	char	question[512]; 	//  Ëø
	char	select1[128]; 	//¼»  ÷°1
	char	select2[128]; 	//¼»  ÷°2
	char	select3[128]; 	//¼»  ÷°3

}NPC_QUIZ;


void NPC_QuizTalked( int meindex , int talkerindex , char *msg ,
                     int color );
BOOL NPC_QuizInit( int meindex );
void NPC_QuizWindowTalked( int meindex, int talkerindex, int seqno, int select, char *data);

BOOL QUIZ_initQuiz( char *filename);


#endif 

/*__NPC_QUIZ_H__*/
