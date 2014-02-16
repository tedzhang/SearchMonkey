/****************************************************************************
 *
 * Copyright (c) 2000, 2001 
 *     Machine Group
 *     Software Research Lab.
 *     Institute of Computing Tech.
 *     Chinese Academy of Sciences
 *     All rights reserved.
 *
 * This file is the confidential and proprietary property of 
 * Institute of Computing Tech. and the posession or use of this file requires 
 * a written license from the author.
 * Filename: Dictionary.h
 * Abstract:
 *           dictionary class definition
 *			 interface for the CDictionary class.
 * Author:   Kevin Zhang 
 *          (zhanghp@software.ict.ac.cn)
 * Date:     2002-1-8
 *
 * Notes:
 *                
 * 
 ****************************************************************************/
#pragma  once 

#include "Global.h"

#include <stdlib.h>
#include <string.h>

#define CC_NUM  6768
//The number of Chinese Char,including 5 empty position between 3756-3761
#define WT_DELIMITER 0
#define WT_CHINESE   1
#define WT_OTHER     2
#define CC_ID(c1,c2) ((unsigned char)(c1)-176)*94+((unsigned char)(c2)-161)
//The ID equation of Chinese Char 
#define CC_CHAR1(id) (id)/94+176
//The first char computed by the Chinese Char ID
#define CC_CHAR2(id) (id)%94+161
//The second char computed by the Chinese Char ID 

/*data structure for word segmentation and tag result*/
//Add in 2002-6-20
struct tagWordResult{
	char sWord[MAX_WORD_SIZE];
	//The word 
	int nHandle;
	//the POS of the word
	double  dValue;
	//The -log(frequency/MAX)
};
typedef struct tagWordResult WORD_RESULT,*PWORD_RESULT;


struct tagWordItem{
	int		nWordLen;
	char	*sWord;
	int		nHandle;
	int		nFrequency;
};
typedef struct tagWordItem WORD_ITEM,*PWORD_ITEM;

struct tagIndexTable{
    int nCount;
    PWORD_ITEM pWordItemHead;
};
typedef struct tagIndexTable INDEX_TABLE;

struct tagWordChain
{
	WORD_ITEM data;
	struct tagWordChain *next;
};
typedef struct tagWordChain WORD_CHAIN,*PWORD_CHAIN;


struct tagModifyTable{
    int nCount;
	int nDelete;
	PWORD_CHAIN pWordItemHead;
};
typedef struct tagModifyTable MODIFY_TABLE,*PMODIFY_TABLE;

class CDictionary  
{
public:
	CDictionary();
    ~CDictionary();
public:
	bool Output(char *sFilename);
	int  GetFrequency(char *sWord,  int nHandle);
	static int  GetPOSValue(const char *sPOS);
	bool GetPOSString(int nPOS,char *sPOSRet);
	bool GetMaxMatch(char *sWord, char *sWordRet, int *npHandleRet);
	bool GetHandle(char *sWord,int *pnCount,int *pnHandle,int *pnFrequency);
	bool IsExist(char *sWord,int nHandle);
	bool Load(char *sFilename,bool bReset=false);
	int  GetWordType(char *sWord);
	bool PreProcessing(char *sWord,int *nId,char *sWordRet );

	//
	// Add a new word to the dictionary
	//@param nHandle词性 
	// 名词:28160
	// 动词:30208
	// 成语：26880
	// 形容词：24832
	// 感叹词：25856
	// 
	bool AddItem(char *sWord,int nHandle,int nFrequency=0);
	bool Save(char *sFilename);
	bool DelItem(char *sWord,int nHandle);



	INDEX_TABLE   m_IndexTable[CC_NUM];
    PMODIFY_TABLE m_pModifyTable;
protected:
	bool DelModified();
	bool FindInOriginalTable(int nInnerCode,char *sWord,int nHandle,int *nPosRet=0);
	bool FindInModifyTable(int nInnerCode,char *sWord,int nHandle,PWORD_CHAIN *pFindRet=0);
};

inline int CDictionary::GetPOSValue(const char *sPOS)
{
	int nPOS;
	const char *sPlusPos;
	char sTemp[4];
	if(strlen(sPOS)<3)
	{
		nPOS=sPOS[0]*256+sPOS[1];
	}
	else
	{
		sPlusPos=strchr(sPOS,'+');
		strncpy(sTemp,sPOS,sPlusPos-sPOS);
		sTemp[sPlusPos-sPOS]=0;
		nPOS=100*GetPOSValue(sTemp);
		strncpy(sTemp,sPlusPos+1,4);
		nPOS+=atoi(sTemp);
	}
	return nPOS;
}
