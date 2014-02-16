#pragma  once 

#ifdef _DEBUG
#include <assert.h>
#endif

typedef double	ELEMENT_TYPE;	//the type of element

#define MAX_SENTENCE_SIZE	512 //how many bytes one sentence can have
#define MAX_WORD_SIZE		64

#define MAX_FREQUENCE      2079997//1993123+86874
#define LOG_MAX_FRQUENCE   14.547877009368769 //log(MAX_FREQUENCE)

static const int MAX_WORDS = 100;//max words number in one sentence
static const int MAX_SEGMENT_NUM = 3;




#define MAX_SENTENCE_LEN	MAX_SENTENCE_SIZE
#define WORD_MAXLENGTH		MAX_WORD_SIZE


#ifndef NULL
#define NULL 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif