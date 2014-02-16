
#include "Dictionary.h"
#include "Utility.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDictionary::CDictionary()
{
  //initilization
  memset(m_IndexTable,0,sizeof(m_IndexTable));
  m_pModifyTable=NULL;
}

CDictionary::~CDictionary()
{
	for(int i=0;i<CC_NUM;i++)
	{
		for(int j=0;j<m_IndexTable[i].nCount;j++)
			delete[] m_IndexTable[i].pWordItemHead[j].sWord;
		delete [] m_IndexTable[i].pWordItemHead;
	}
    DelModified();
}
/*********************************************************************
 *
 *  Func Name  : Load
 *
 *  Description: Load the dictionary from the file .dct
 *              
 *
 *  Parameters : sFilename: the file name
 *
 *  Returns    : success or fail
 *  Author     : Kevin Zhang  
 *  History    : 
 *              1.create 2002-1-9
 *********************************************************************/
bool CDictionary::Load(char *sFilename,bool bReset)
{
	FILE *fp;
	int i,j,nBuffer[3];
	if((fp=fopen(sFilename,"rb"))==NULL)
	{
		return false;//fail while opening the file
	}
	
	//Release the memory for new files
	for( i=0;i<CC_NUM;i++)
	{
		//delete the memory of word item array in the dictionary
		for( j=0;j<m_IndexTable[i].nCount;j++)
		{
			delete[] m_IndexTable[i].pWordItemHead[j].sWord;
		}
		delete [] m_IndexTable[i].pWordItemHead;
	}
	DelModified();
	
	
	for(i=0; i < CC_NUM; i++)
	{
		fread(&(m_IndexTable[i].nCount),sizeof(int),1,fp);
		if(m_IndexTable[i].nCount>0)
		{
			m_IndexTable[i].pWordItemHead=new WORD_ITEM[m_IndexTable[i].nCount];
		}
		else 
		{
			m_IndexTable[i].pWordItemHead=0;
			continue;
		}
		j=0;
		while( j < m_IndexTable[i].nCount )
		{
			fread(nBuffer,sizeof(int), 3, fp);
			m_IndexTable[i].pWordItemHead[j].sWord=new char[nBuffer[1]+1];
			if(nBuffer[1])//String length is more than 0
			{
				fread(m_IndexTable[i].pWordItemHead[j].sWord,sizeof(char),nBuffer[1],fp);
			}
			m_IndexTable[i].pWordItemHead[j].sWord[nBuffer[1]]=0;

			if( bReset )//Reset the frequency
			{
				m_IndexTable[i].pWordItemHead[j].nFrequency=0;
			}
			else
			{
				m_IndexTable[i].pWordItemHead[j].nFrequency=nBuffer[0];
			}
			m_IndexTable[i].pWordItemHead[j].nWordLen=nBuffer[1];
			m_IndexTable[i].pWordItemHead[j].nHandle=nBuffer[2];

			j+=1;//Get next item in the original table.
		}
	}
	fclose(fp);
	return true;
}


bool CDictionary::DelModified()
{
  PWORD_CHAIN pTemp,pCur;
  if(!m_pModifyTable)
	  return true;
  for(int i=0;i<CC_NUM;i++)
  {
      pCur=m_pModifyTable[i].pWordItemHead;
	  while(pCur!=NULL)
	  {
		  pTemp=pCur;
		  pCur=pCur->next;
		  delete[] pTemp->data.sWord;
		  delete pTemp;
	  }
  }
  delete [] m_pModifyTable;
  m_pModifyTable=NULL;
  return true;
}


bool CDictionary::IsExist(char *sWord,  int nHandle)
{
   char sWordFind[MAX_WORD_SIZE-2];
   int nPos;
   if(!PreProcessing(sWord, &nPos,sWordFind))
   {
	   return false;
   }
   return ( FindInOriginalTable(nPos,sWordFind,nHandle)||FindInModifyTable(nPos,sWordFind,nHandle) );
}


bool CDictionary::GetHandle(char *sWord,int *pnCount,int *pnHandle,int *pnFrequency)
{
   char sWordGet[MAX_WORD_SIZE-2];
   int nPos,nFoundPos,nTemp;
   PWORD_CHAIN pPre,pCur;
   *pnCount=0;
   if(!PreProcessing(sWord, &nPos,sWordGet))
	   return false;
   if(FindInOriginalTable(nPos,sWordGet,-1,&nFoundPos))
   {
       pnHandle[*pnCount]=m_IndexTable[nPos].pWordItemHead[nFoundPos].nHandle;
	   pnFrequency[*pnCount]=m_IndexTable[nPos].pWordItemHead[nFoundPos].nFrequency;
	   *pnCount+=1;

	   nTemp=nFoundPos+1;//Check its previous position
	   while(nTemp<m_IndexTable[nPos].nCount&&strcmp(m_IndexTable[nPos].pWordItemHead[nTemp].sWord,sWordGet)==0)
	   {
		   pnHandle[*pnCount]=m_IndexTable[nPos].pWordItemHead[nTemp].nHandle;
		   pnFrequency[*pnCount]=m_IndexTable[nPos].pWordItemHead[nTemp].nFrequency;
		   *pnCount+=1;
		   nTemp+=1;
	   }
	   return true;
   }
   //Operation in the index table and its items 
   if(FindInModifyTable(nPos,sWordGet,-1,&pPre))
   {
	     pCur=m_pModifyTable[nPos].pWordItemHead;
	     if(pPre!=NULL)
			 pCur=pPre->next;
         while(pCur!=NULL && _stricmp(pCur->data.sWord,sWordGet)==0)
		 {
			 pnHandle[*pnCount]=pCur->data.nHandle;
			 pnFrequency[*pnCount]=pCur->data.nFrequency;
			 *pnCount+=1;
 			 pCur=pCur->next;
		 }
	   return true;
   }
   return false;
}



/*
 *	再表中用二分法查找
 */
bool CDictionary::FindInOriginalTable(int nInnerCode,char *sWord,int nHandle,int *nPosRet)
{
    PWORD_ITEM pItems=m_IndexTable[nInnerCode].pWordItemHead;
	int nStart=0;
	int	nEnd=m_IndexTable[nInnerCode].nCount-1;
	int nMid=(nStart+nEnd)/2;
	int	nCount=0;
	int	nCmpValue=0;

	while(nStart<=nEnd)//Binary search
	{
		nCmpValue=strcmp(pItems[nMid].sWord,sWord);
		if(nCmpValue==0 && (pItems[nMid].nHandle == nHandle||nHandle==-1) )
		{
			if(nPosRet)
			{
				if(nHandle==-1)//Not very strict match
				{//Add in 2002-1-28
					nMid--;
					//Get the first item which match the current word
					while(nMid >= 0&&strcmp(pItems[nMid].sWord,sWord)==0)
					{
						nMid--;
					}
					if(nMid<0 || strcmp(pItems[nMid].sWord,sWord) != 0)
					{
						nMid++;
					}
				}
				*nPosRet=nMid;
			}
			return true;//find it
		}
		else if( nCmpValue<0 || ( nCmpValue==0 && pItems[nMid].nHandle < nHandle && nHandle!=-1) )
		{
			nStart=nMid+1;
		}
		else if( nCmpValue>0 || (nCmpValue==0 && pItems[nMid].nHandle > nHandle&&nHandle!=-1))
		{
			nEnd=nMid-1;
		}
		nMid=(nStart+nEnd)/2;
	}

	if(nPosRet)
	{
		//Get the previous position
		*nPosRet=nMid-1;
	}
	return false;
}


/*
 *	在链表中顺序查找
 */
bool CDictionary::FindInModifyTable(int nInnerCode,char *sWord,int nHandle,PWORD_CHAIN *pFindRet)
{
	PWORD_CHAIN pCur=NULL;
	PWORD_CHAIN pPre=NULL;
	
	if(m_pModifyTable==NULL)
	{
		return false;
	}
	
	pCur=m_pModifyTable[nInnerCode].pWordItemHead;
	
	while( pCur!=NULL
			&&( _stricmp(pCur->data.sWord,sWord)<0 
				|| ( _stricmp(pCur->data.sWord,sWord) == 0 && pCur->data.nHandle < nHandle ) ) )
		//sort the link chain as alphabet
	{
		pPre=pCur;
		pCur=pCur->next;
	}
	if( pFindRet )
	{
		*pFindRet=pPre;
	}
	
	return (pCur!=NULL && _stricmp(pCur->data.sWord,sWord)==0 && (pCur->data.nHandle==nHandle || nHandle < 0) );
}


int CDictionary::GetWordType(char *sWord)
{
	int nType=charType((unsigned char *)sWord);
	int	nLen=strlen(sWord);
	
	if(nLen>0 && nType == CT_CHINESE && IsAllChinese((unsigned char *)sWord))
	{
		return WT_CHINESE;//Chinese word
	}
	else if(nLen>0&&nType==CT_DELIMITER)
	{
		return WT_DELIMITER;//Delimiter
	}
	
	return WT_OTHER;//other invalid
}

/*
 *	将词的前面后面的空格除掉,计算一个ID,并保存SWordRet
 */

inline bool CDictionary::PreProcessing(char *sWord, int *nId, char *sWordRet )
{
	
	if(!TrimStringSpaces(sWord))
		return false;
	int nType = charType((unsigned char *)sWord);
	
	if( nType==CT_CHINESE ) 
	{
		//Chinese word
		*nId=CC_ID(sWord[0],sWord[1]);
		//Get the inner code of the first Chinese Char
		strcpy(sWordRet,&sWord[2]);//store the word,not store the first Chinese Char
		return true;
	}
	if(nType==CT_DELIMITER)
	{
		//Delimiter
		*nId=3755;
		//Get the inner code of the first Chinese Char
		strcpy(sWordRet,sWord);//store the word,not store the first Chinese Char
		return true;
	}
	return false;//other invalid
}


bool CDictionary::GetMaxMatch(char *sWord, char *sWordRet,int *npHandleRet)
{
	char sWordGet[MAX_WORD_SIZE-2],sFirstChar[3];
	int nPos,nFoundPos,nTemp;
	PWORD_CHAIN pCur;
	*npHandleRet=-1;
	if(!PreProcessing(sWord, &nPos,sWordGet))
	{
		return false;
	}
	sWordRet[0]=0;

	size_t wordLen = strlen(sWord);
	size_t getWordLen = strlen(sWordGet);

	strncpy(sFirstChar, sWord, wordLen - getWordLen );//Get the first char

	sFirstChar[wordLen - getWordLen]=0;//Set the end flag
	
	FindInOriginalTable(nPos,sWordGet,-1,&nFoundPos);

	nTemp=nFoundPos;//Check its previous position
	if(nFoundPos==-1)
		nTemp=0;
	while(nTemp<m_IndexTable[nPos].nCount&&CC_Find(m_IndexTable[nPos].pWordItemHead[nTemp].sWord,sWordGet)!=m_IndexTable[nPos].pWordItemHead[nTemp].sWord)
	{//Get the next
		nTemp+=1;
	}
	if(nTemp<m_IndexTable[nPos].nCount&&CC_Find(m_IndexTable[nPos].pWordItemHead[nTemp].sWord,sWordGet)==m_IndexTable[nPos].pWordItemHead[nTemp].sWord)
	{
		strcpy(sWordRet,sFirstChar);
		strcat(sWordRet,m_IndexTable[nPos].pWordItemHead[nTemp].sWord);
		*npHandleRet=m_IndexTable[nPos].pWordItemHead[nTemp].nHandle;
		return true;
	}//Cannot get the item and retrieve the modified data if exists
	//Operation in the index table and its items 
	if(m_pModifyTable&&m_pModifyTable[nPos].pWordItemHead)//Exists 
		pCur=m_pModifyTable[nPos].pWordItemHead;
	else
		pCur=NULL;
	while(pCur!=NULL&&strcmp(pCur->data.sWord,sWordGet)<=0&&CC_Find(pCur->data.sWord,sWordGet)!=pCur->data.sWord)//
	{
		pCur=pCur->next;
	}
	if(pCur!=NULL&&CC_Find(pCur->data.sWord,sWordGet)!=pCur->data.sWord)
	{//Get it
		strcpy(sWordRet,sFirstChar);
		strcat(sWordRet,pCur->data.sWord);
		*npHandleRet=pCur->data.nHandle;
		return true;
	}
	return false;
}


int CDictionary::GetFrequency(char *sWord, int nHandle)
{
	char sWordFind[MAX_WORD_SIZE-2];
	int nPos,nIndex;
	PWORD_CHAIN pFound;
	if(!PreProcessing(sWord, &nPos,sWordFind))
		return 0;
	if(FindInOriginalTable(nPos,sWordFind,nHandle,&nIndex))
	{
		return m_IndexTable[nPos].pWordItemHead[nIndex].nFrequency;
	}
	if(FindInModifyTable(nPos,sWordFind,nHandle,&pFound))
	{
		return pFound->data.nFrequency;
	}
	return 0;
}




bool CDictionary::Output(char *sFilename)
{
	FILE *fp;
	int i,j;
	PWORD_CHAIN pCur;
	char sPrevWord[MAX_WORD_SIZE]={0}, sCurWord[MAX_WORD_SIZE]={0},sPOS[10]={0};
	
	if(m_pModifyTable)
	{
		return false;
	}   
	
	//fail while opening the file
	if((fp=fopen(sFilename,"wb"))==NULL)
	{
		return false;
	}
	
	for(i=0;i<CC_NUM;i++)
	{
		pCur=NULL;
		j=0;  
		while(j < m_IndexTable[i].nCount)
		{
			GetPOSString(m_IndexTable[i].pWordItemHead[j].nHandle,sPOS);
			//Get the POS string
			sprintf(sCurWord,"%c%c%s",CC_CHAR1(i),CC_CHAR2(i),m_IndexTable[i].pWordItemHead[j].sWord);
			if(strcmp(sPrevWord,sCurWord)!=0)
				fprintf(fp,"\n%s %s",sCurWord,sPOS);
			else
				fprintf(fp," %s",sPOS);
			strcpy(sPrevWord,sCurWord);
			j+=1;//Get next item in the original table.
		}
	}
	fclose(fp);
	return true;
}


bool CDictionary::GetPOSString(int nPOS, char *sPOSRet)
{
    if(nPOS>'a'*25600) 
	{
		if((nPOS/100)%256!=0)
 			sprintf(sPOSRet,"%c%c+%d",nPOS/25600,(nPOS/100)%256,nPOS%100);
		else
			sprintf(sPOSRet,"%c+%d",nPOS/25600,nPOS%100);
	}
	else
	{
	  if(nPOS>256)
		sprintf(sPOSRet,"%c%c",nPOS/256,nPOS%256);
	  else
		sprintf(sPOSRet,"%c",nPOS%256);
	}
	return true;
}


bool CDictionary::Save(char *sFilename)
{
	FILE *fp;
	int i,j,nCount,nBuffer[3];
	PWORD_CHAIN pCur;
	if((fp=fopen(sFilename,"wb"))==NULL)
		return false;//fail while opening the file
	for(i=0;i<CC_NUM;i++)
	{
		pCur=NULL;
		if(m_pModifyTable)
		{//Modification made
			nCount=m_IndexTable[i].nCount+m_pModifyTable[i].nCount-m_pModifyTable[i].nDelete;
			fwrite(&nCount,sizeof(int),1,fp);
			pCur=m_pModifyTable[i].pWordItemHead;
			j=0;
			while(pCur!=NULL&&j<m_IndexTable[i].nCount)
			{//Output to the file after comparision
				if(strcmp(pCur->data.sWord,m_IndexTable[i].pWordItemHead[j].sWord)<0||(strcmp(pCur->data.sWord,m_IndexTable[i].pWordItemHead[j].sWord)==0&&pCur->data.nHandle<m_IndexTable[i].pWordItemHead[j].nHandle))
				{//Output the modified data to the file
					nBuffer[0]=pCur->data.nFrequency;
					nBuffer[1]=pCur->data.nWordLen;
					nBuffer[2]=pCur->data.nHandle;
					fwrite(nBuffer,sizeof(int),3,fp);
					if(nBuffer[1])//String length is more than 0
						fwrite(pCur->data.sWord,sizeof(char),nBuffer[1],fp);
					pCur=pCur->next;//Get next item in the modify table.
				}
				else if(m_IndexTable[i].pWordItemHead[j].nFrequency==-1)
				{//The item has been removed,so skip it
					j+=1;
				}
				else if(strcmp(pCur->data.sWord,m_IndexTable[i].pWordItemHead[j].sWord)>0||(strcmp(pCur->data.sWord,m_IndexTable[i].pWordItemHead[j].sWord)==0&&pCur->data.nHandle>m_IndexTable[i].pWordItemHead[j].nHandle))
				{//Output the index table data to the file
					nBuffer[0]=m_IndexTable[i].pWordItemHead[j].nFrequency;
					nBuffer[1]=m_IndexTable[i].pWordItemHead[j].nWordLen;
					nBuffer[2]=m_IndexTable[i].pWordItemHead[j].nHandle;
					fwrite(nBuffer,sizeof(int),3,fp);
					if(nBuffer[1])//String length is more than 0
						fwrite(m_IndexTable[i].pWordItemHead[j].sWord,sizeof(char),nBuffer[1],fp);
					j+=1;//Get next item in the original table.
				}
			}
			if(j<m_IndexTable[i].nCount)
			{
				while(j<m_IndexTable[i].nCount)
				{
					if(m_IndexTable[i].pWordItemHead[j].nFrequency!=-1)
					{//Has been deleted
						nBuffer[0]=m_IndexTable[i].pWordItemHead[j].nFrequency;
						nBuffer[1]=m_IndexTable[i].pWordItemHead[j].nWordLen;
						nBuffer[2]=m_IndexTable[i].pWordItemHead[j].nHandle;
						fwrite(nBuffer,sizeof(int),3,fp);
						if(nBuffer[1])//String length is more than 0
							fwrite(m_IndexTable[i].pWordItemHead[j].sWord,sizeof(char),nBuffer[1],fp);
					}
					j+=1;//Get next item in the original table.
				}
			}
			else////No Modification
				while(pCur!=NULL)//Add the rest data to the file.
				{
					nBuffer[0]=pCur->data.nFrequency;
					nBuffer[1]=pCur->data.nWordLen;
					nBuffer[2]=pCur->data.nHandle;
					fwrite(nBuffer,sizeof(int),3,fp);
					if(nBuffer[1])//String length is more than 0
						fwrite(pCur->data.sWord,sizeof(char),nBuffer[1],fp);
					pCur=pCur->next;//Get next item in the modify table.
				}
		}
		else
		{
			fwrite(&m_IndexTable[i].nCount,sizeof(int),1,fp);
			//write to the file
			j=0;  
			while(j<m_IndexTable[i].nCount)
			{
				nBuffer[0]=m_IndexTable[i].pWordItemHead[j].nFrequency;
				nBuffer[1]=m_IndexTable[i].pWordItemHead[j].nWordLen;
				nBuffer[2]=m_IndexTable[i].pWordItemHead[j].nHandle;
				fwrite(nBuffer,sizeof(int),3,fp);
				if(nBuffer[1])//String length is more than 0
					fwrite(m_IndexTable[i].pWordItemHead[j].sWord,sizeof(char),nBuffer[1],fp);
				j+=1;//Get next item in the original table.
			}
		}
	}
	fclose(fp);
	return true;
}
/*********************************************************************
*
*  Func Name  : AddItem
*
*  Description: Add a word item to the dictionary
*              
*
*  Parameters : sWord: the word
*               nHandle:the handle number
*               nFrequency: the frequency
*  Returns    : success or fail
*  Author     : Kevin Zhang  
*  History    : 
*              1.create 2002-1-9
*********************************************************************/

//
//when nFrequency = MAX_FREQUENCY 
//we only want to add a word does not exist in the dictionary
//but if it exist,the original code will modify its frequency
//now we use nFrequecy as judge condition
bool CDictionary::AddItem(char *sWord, int nHandle,int nFrequency)
{

	char sWordAdd[MAX_WORD_SIZE-2];
	int nPos,nFoundPos;
	PWORD_CHAIN pRet,pTemp,pNext;
	int i=0;
	if(!PreProcessing(sWord,&nPos,sWordAdd))
		return false;
	if(FindInOriginalTable(nPos,sWordAdd,nHandle,&nFoundPos) )
	{
		 //
		 //added by yong zhang  2007/4/22
		 //exist donot modify the word's frequency
		 // because we only want to add new word for this dictionary
		 //
		 //if(nFrequency == MAX_FREQUENCE) return true; 

		//The word exists in the original table, so add the frequency
		//Operation in the index table and its items 
		if(m_IndexTable[nPos].pWordItemHead[nFoundPos].nFrequency==-1)
		{//The word item has been removed
			m_IndexTable[nPos].pWordItemHead[nFoundPos].nFrequency=nFrequency;
			if(!m_pModifyTable)//Not prepare the buffer
			{
				m_pModifyTable=new MODIFY_TABLE[CC_NUM];
				memset(m_pModifyTable,0,CC_NUM*sizeof(MODIFY_TABLE));
			}
			m_pModifyTable[nPos].nDelete-=1;
		}
		else
			m_IndexTable[nPos].pWordItemHead[nFoundPos].nFrequency+=nFrequency;
		return true;
	}

	//The items not exists in the index table.
	//As following, we have to find the item whether exists in the modify data region
	//If exists, change the frequency .or else add a item
	if(!m_pModifyTable)//Not prepare the buffer
	{
		m_pModifyTable=new MODIFY_TABLE[CC_NUM];
		memset(m_pModifyTable,0,CC_NUM*sizeof(MODIFY_TABLE));
	}
	if(FindInModifyTable(nPos,sWordAdd,nHandle,&pRet))
	{
		if(pRet!=NULL)
			pRet=pRet->next;
		else
			pRet=m_pModifyTable[nPos].pWordItemHead;
		pRet->data.nFrequency+=nFrequency;
		return true;
	}
	//find the proper position to add the word to the modify data table and link
	pTemp= new WORD_CHAIN;//Allocate the word chain node
	if(pTemp==NULL)//Allocate memory failure
		return false;
	memset(pTemp,0,sizeof(WORD_CHAIN));//init it with 0
	pTemp->data.nHandle=nHandle;//store the handle 
	pTemp->data.nWordLen=strlen(sWordAdd);
	pTemp->data.sWord=new char[1+pTemp->data.nWordLen];
	strcpy(pTemp->data.sWord,sWordAdd);
	pTemp->data.nFrequency=nFrequency;
	pTemp->next=NULL;   
	if(pRet!=NULL)
	{
		pNext=pRet->next;//Get the next item before the current item
		pRet->next=pTemp;//link the node to the chain
	}
	else
	{
		pNext=m_pModifyTable[nPos].pWordItemHead;
		m_pModifyTable[nPos].pWordItemHead=pTemp;//Set the pAdd as the head node
	}
	pTemp->next=pNext;//Very important!!!! or else it will lose some node
	//Modify in 2001-10-29
	m_pModifyTable[nPos].nCount++;//the number increase by one
	return true;
}

bool CDictionary::DelItem(char *sWord,int nHandle)
{
	char sWordDel[MAX_WORD_SIZE-2];
	int nPos,nFoundPos,nTemp;
	PWORD_CHAIN pPre,pTemp,pCur;
	if(!PreProcessing(sWord, &nPos,sWordDel))
		return false;
	if(FindInOriginalTable(nPos,sWordDel,nHandle,&nFoundPos))
	{
		if(!m_pModifyTable)//Not prepare the buffer
		{
			m_pModifyTable=new MODIFY_TABLE[CC_NUM];
			memset(m_pModifyTable,0,CC_NUM*sizeof(MODIFY_TABLE));
		}
		m_IndexTable[nPos].pWordItemHead[nFoundPos].nFrequency=-1;
		m_pModifyTable[nPos].nDelete+=1;
		if(nHandle==-1)//Remove all items which word is sWordDel,ignoring the handle
		{	
			nTemp=nFoundPos+1;//Check its previous position
			while(nTemp<m_IndexTable[nPos].nCount&&strcmp(m_IndexTable[nPos].pWordItemHead[nFoundPos].sWord,sWordDel)==0)
			{
				m_IndexTable[nPos].pWordItemHead[nTemp].nFrequency=-1;
				m_pModifyTable[nPos].nDelete+=1;
				nTemp+=1;
			}
		}
		return true;
	}
	//Operation in the modify table and its items 
	if(FindInModifyTable(nPos,sWordDel,nHandle,&pPre))
	{
		pCur=m_pModifyTable[nPos].pWordItemHead;
		if(pPre!=NULL)
			pCur=pPre->next;
		while(pCur!=NULL && _stricmp(pCur->data.sWord,sWordDel)==0&&(pCur->data.nHandle==nHandle||nHandle<0))
		{
			pTemp=pCur;
			if(pPre!=NULL)//pCur is the first item
				pPre->next=pCur->next;
			else
				m_pModifyTable[nPos].pWordItemHead=pCur->next;
			pCur=pCur->next;
			delete pTemp->data.sWord;//Delete the word
			delete pTemp;
		}
		return true;
	}
	return false;
}
