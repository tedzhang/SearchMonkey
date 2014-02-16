
#pragma  once 
#include <string.h>
#include <stdio.h>


#define  CT_SENTENCE_BEGIN 0//Sentence begin 
#define  CT_SENTENCE_END   1//Sentence ending

#define  CT_SINGLE		3//SINGLE byte
#define  CT_DELIMITER	CT_SINGLE+1//delimiter
#define  CT_CHINESE		CT_SINGLE+2//Chinese Char
#define  CT_LETTER		CT_SINGLE+3//HanYu Pinyin
#define  CT_NUM			CT_SINGLE+4//HanYu Pinyin
#define  CT_INDEX		CT_SINGLE+5//HanYu Pinyin
#define  CT_OTHER		CT_SINGLE+12//Other


#define POSTFIX_SINGLE "�Ӱ���ǳش嵥�����̵궴�ɶӷ��帮�Ը۸󹬹������źӺ������������ǽ־����ӿڿ�����¥·������Ū����������������Ȫ��ɽʡ��ˮ����̨̲̳����ͤ��������ϪϿ������������ҤӪ����԰ԷԺբկվ����ׯ�������"
#define POSTFIX_MUTIPLE {"�뵺","��ԭ","����","���","�󹫹�","����","����","�۹�","�ɲ�","�ۿ�","���ٹ�·","��ԭ","��·","��԰","���͹�","�ȵ�","�㳡","����","��Ͽ","��ͬ","����","����","����","�ֵ�","�ڰ�","��ͷ","ú��","����","ũ��","���","ƽԭ","����","Ⱥ��","ɳĮ","ɳ��","ɽ��","ɽ��","ˮ��","���","����","��·","�´�","ѩ��","�γ�","�κ�","�泡","ֱϽ��","������","������","������",""}

#define  TRANS_ENGLISH   "�������������������°İʰŰͰװݰ������������ȱϱ˱𲨲��������������Ųɲֲ��񳹴��Ĵȴδ����������������µõĵǵϵҵٵ۶����Ŷض����������������Ʒҷѷ�򸣸������ǸɸԸ���������ŹϹ��������������ϺӺպ����������������Ӽּ��ܽ𾩾þӾ��������������¿ƿɿ˿Ͽ����������������������������������������������������������¡¬²³·��������������������éï÷����������������ĦĪīĬķľ������������������������ŦŬŵŷ��������������Ƥƽ��������ǡǿ��������Ȫ��������������������ɣɪɭɯɳɽ������ʥʩʫʯʲʷʿ��˹˾˿��������̩̹����������͡ͼ������������������Τάκ��������������������ϣϲ������Ъл������������ҢҶ��������������ӢӺ����Լ������ղ������������׿������٤��������üν�����������Ľ����������������ɺ����ѷ��������ܽ���������������"
#define  TRANS_RUSSIAN   "�������°ͱȱ˲�����Ĵ�µö��Ŷ���������Ǹ�����Ӽ�ݽ𿨿ƿɿ˿���������������������¬³������÷����ķ������ŵ������������������ɫɽ��ʲ˹����̹������ά������ϣл��ҮҶ�������������ǵٸ�����ջ������������������������������ɣɳ��̩ͼ������׿��"
#define  TRANS_JAPANESE  "���°˰װٰ�������ȱ��������ʲ˲ֲ������سന�����δ����������µص�ɶ������縣�Ը߹����Źȹع���úƺͺϺӺں���󻧻Ļ漪�ͼѼӼ�������������������þƾտ����ɿ˿�����������������������������¡¹������������ľ��������������Ƭƽ����ǧǰǳ����������������Ȫ������������ɭɴɼɽ��������ʥʯʵʸ������ˮ˳˾��̩��������������βδ����������ϸ������СТ����������������������ңҰҲҶһ����������ӣ��������������ԨԪԫԭԶ����������������լ����������ֲ֦֪֮��������������׵��������ܥݶ��޹������"
//Translation type
#define  TT_ENGLISH  0
#define  TT_RUSSIAN  1
#define  TT_JAPANESE  2
//Seperator type
#define  SEPERATOR_C_SENTENCE "������������"
#define  SEPERATOR_C_SUB_SENTENCE "����������������"
#define  SEPERATOR_E_SENTENCE "!?:;"
#define  SEPERATOR_E_SUB_SENTENCE ",()\042'"
#define  SEPERATOR_LINK "\n\r ��"
//Sentence begin and ending string
#define SENTENCE_BEGIN	"ʼ##ʼ"
#define SENTENCE_END	"ĩ##ĩ"


//Find a Chinese sub-string in the Chinese String 
inline const char *CC_Find(const char *string, const char *strCharSet)
{
	const char *cp=strstr(string,strCharSet);
	if(cp!=NULL && (cp-string)%2 == 1)
	{
		return NULL;
	}
	return cp;
};


//Get the count of char which is in sWord and in sCharSet
inline int GetCharCount(char *sCharSet,char *sWord)
{
	unsigned int  k; 
	char tchar[3];
	int nCount=0,nCharCount=0;
	unsigned int word_size = strlen(sWord);
	for (k = 0; k < word_size; k+=2)
	{
		while(k < word_size && sWord[k]>0)
			k++;
		strncpy(tchar,sWord+k,2) ;
		tchar[2]='\0';
		nCharCount++;
		if(CC_Find(sCharSet, tchar))
			nCount++;
	}
	return nCount;
}

//Decide whether the word is all  non-foreign translation
inline int GetForeignCharCount(char *sWord)
{
	unsigned int nForeignCount,nCount;
	nForeignCount=GetCharCount(TRANS_ENGLISH,sWord);//English char counnts
	nCount=GetCharCount(TRANS_JAPANESE,sWord);//Japan char counnts
	if(nForeignCount<=nCount)
		nForeignCount=nCount;
	nCount=GetCharCount(TRANS_RUSSIAN,sWord);//Russian char counnts
	if(nForeignCount<=nCount)
		nForeignCount=nCount;
	return nForeignCount;
}


//Judge the type of sChar or (sChar,sChar+1)
inline int charType(unsigned char *sChar)
{
	if(*sChar<128)
	{
		//
		//white space in english is 32
		if(strchr("*!,.?()[]{}+=",(int)*sChar) || *sChar == 32)
			return CT_DELIMITER;
		return CT_SINGLE;
	}
	else if(*sChar==162)
		return CT_INDEX;
	else if(*sChar==163&&*(sChar+1)>175&&*(sChar+1)<186)
		return CT_NUM;
	else if(*sChar==163&&(*(sChar+1)>=193&&*(sChar+1)<=218||*(sChar+1)>=225&&*(sChar+1)<=250))
		return CT_LETTER;
	else if(*sChar==161||*sChar==163)
		return CT_DELIMITER;
	else if(*sChar>=176&&*sChar<=247)
		return CT_CHINESE;
	else
		return CT_OTHER;
};

//Generate the GB2312 List file
inline bool GB2312_Generate(char *sFileName)
{
	FILE *fp;
	unsigned int i,j;
	
	if((fp=fopen(sFileName,"wt"))==NULL)
		return false;//fail while opening the file
	for(i=161;i<255;i++)
		for(j=161;j<255;j++)
			fprintf(fp,"%c%c,%d,%d\n",i,j,i,j);
		fclose(fp);
		return true;
};

//Generate the Chinese Char List file
inline bool CC_Generate(char *sFileName)
{
	FILE *fp;
	unsigned int i,j;
	if((fp=fopen(sFileName,"wt"))==NULL)
		return false;//fail while opening the file
	for(i=176;i<255;i++)
		for(j=161;j<255;j++)
			fprintf(fp,"%c%c,%d,%d\n",i,j,i,j);
		fclose(fp);
		return true;
};



//Get the max Prefix string made up of Chinese Char
inline unsigned int  GetCCPrefix(unsigned char *sSentence)
{
	unsigned int nLen=strlen((const char *)sSentence),nCurPos=0;
	while(nCurPos<nLen&&sSentence[nCurPos]>175&&sSentence[nCurPos]<248)
	{
		nCurPos+=2;//Get next Chinese Char
	}
	return nCurPos;
}


//Judge the string is all made up of Chinese Char
inline bool IsAllChinese(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen-1&&sString[i]<248&&sString[i]>175)
	{
		i+=2;
	}
	if(i<nLen)
		return false;
	return true;
}


//Judge the string is all made up of non-Chinese Char
inline bool IsAllNonChinese(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen)
	{
		if(sString[i]<248&&sString[i]>175)
			return false;
		if(sString[i]>128)
			i+=2;
		else
			i+=1;
	}
    return true;
}


//Judge the string is all made up of Single Byte Char
inline bool IsAllSingleByte(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen&&sString[i]<128)
	{
		i++;
	}
	if(i<nLen)
		return false;
	return true;
}


//Judge the string is all made up of Num Char
inline bool IsAllNum(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString);
	unsigned int i=0;


	char sChar[3];
	sChar[2]=0;
	if(i<nLen)//Get prefix such as + -
	{
		sChar[0]=sString[i++];
		if(sChar[0]<0)//Get first char
			sChar[1]=sString[i++];
		else
			sChar[1]=0;
		if(!strstr("��+��-��",sChar))
		{
			i=0;
		}
	}
	while(i<nLen-1&&sString[i]==163&&sString[i+1]>175&&sString[i+1]<186)
	{
		i+=2;
	}
	if(i<nLen)//Get middle delimiter such as .
	{
		sChar[0]=sString[i++];
		if(sChar[0]<0)//Get first char
			sChar[1]=sString[i++];
		else
			sChar[1]=0;
		if(CC_Find("�á���",sChar)||sChar[0]=='.')
		{//98��1��
			while(i<nLen-1&&sString[i]==163&&sString[i+1]>175&&sString[i+1]<186)
			{
				i+=2;
			}
		}	
		else
		{
			i-=strlen(sChar);
		}
	}
	
	if(i>=nLen)
		return true;
	while(i<nLen&&sString[i]>'0'-1&&sString[i]<'9'+1)
	{//single byte number char
		i+=1;
	}
	if(i<nLen)//Get middle delimiter such as .
	{
		sChar[0]=sString[i++];
		if(sChar[0]<0)//Get first char
			sChar[1]=sString[i++];
		else
			sChar[1]=0;
		if(CC_Find("�á���",sChar)||sChar[0]=='.')
		{//98��1��
			while(i<nLen&&sString[i]>'0'-1&&sString[i]<'9'+1)
			{
				i+=1;
			}
		}	
		else
		{
			i-=strlen(sChar);
		}
	}
	if(i < nLen)//Get middle delimiter such as .
	{
		sChar[0]=sString[i++];
		if(sChar[0]<0)//Get first char
			sChar[1]=sString[i++];
		else
			sChar[1]=0;
		if(!CC_Find("��ǧ���ڰ�Ǫ����",sChar)&&sChar[0]!='%')
			i-=strlen(sChar);
	}
	if(i >= nLen)
		return true;
	return false;
}


//Judge the string is all made up of Index Num Char
inline bool IsAllIndex(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	unsigned char ch = sString[i];
	while(i< nLen-1 && ch==162)
	{
		i+=2;
	}
	if(i >= nLen)
		return true;
	while(i< nLen&&(ch >'A'-1 && ch <'Z'+1)||(ch >'a'-1 && ch <'z'+1))
	{//single byte number char
		i+=1;
	}
	
	if(i < nLen)
		return false;
	return true;
	
}


//Judge the string is all made up of Letter Char
inline bool IsAllLetter(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen-1&&sString[i]==163&&((sString[i+1]>=193&&sString[i+1]<=218)||(sString[i+1]>=225&&sString[i+1]<=250)))
	{
		i+=2;
	}
	if(i<nLen)
		return false;
	
	return true;
}


//Judge the string is all made up of Delimiter
inline bool IsAllDelimiter(unsigned char *sString)
{
	unsigned int nLen=strlen((const char *)sString),i=0;
	while(i<nLen-1&&(sString[i]==161||sString[i]==163))
	{
		i+=2;
	}
	if(i<nLen)
		return false;
	return true;
}

//Binary search a value in a table which len is nTableLen
inline int BinarySearch(int nVal, int *nTable,int nTableLen)
{
	int nStart=0;
	int nEnd=nTableLen-1;
	int nMid;

	while(nStart <= nEnd)//Binary search
	{
		nMid=(nStart+nEnd)>>1;

		if(nTable[nMid]==nVal)
		{
			return nMid;//find it
		}
		else if(nTable[nMid]<nVal)
		{
			nStart=nMid+1;
		}
		else
		{
			nEnd=nMid-1;
		}
	}
	return -1;//Can not find it;
}



//
// sWord maybe is a foreign translation
//
inline bool IsForeign(char *sWord)
{
	int nForeignCount=GetForeignCharCount(sWord);
	int nCharCount= strlen(sWord);
	if(nCharCount > 2||nForeignCount >= 1*nCharCount/2)
		return true;
	return false;
};


//
//
// Decide whether the word is all  foreign translation
inline bool IsAllForeign(char *sWord)
{
	unsigned int nForeignCount = (unsigned int)GetForeignCharCount(sWord);
	if((nForeignCount << 1) == strlen(sWord))
		return true;
	return false;
}


//
//Decide whether the word is Chinese Number word
//eg:�ٷ�֮������������ϰ˵�ʮ�˷���
inline bool IsAllChineseNum(char *sWord)
{
	unsigned int  k; 
	char tchar[3];
	static const char ChineseNum[]="���һ�����������߰˾�ʮإ��ǧ����Ҽ��������½��ƾ�ʰ��Ǫ�á�������";//
	static const char sPrefix[]="��������";


	for(k = 0; k < strlen(sWord); k+=2)
	{
		strncpy(tchar,sWord+k,2) ;
		tchar[2]='\0';
		if(strncmp(sWord+k,"��֮",4)==0)//�ٷ�֮��
		{
			k+=2;
			continue;
		}
		
		if(!CC_Find(ChineseNum, tchar)&&!(k==0&&CC_Find(sPrefix, tchar)))
			return false;
	}
	return true;
}


//Get the count of char which is in sWord and in sCharSet
//Return the foreign type 
inline int GetForeignType(char *sWord)
{
	unsigned int nForeignCount,nCount,nType=TT_ENGLISH;
	nForeignCount=GetCharCount(TRANS_ENGLISH,sWord);//English char counnts
	nCount=GetCharCount(TRANS_RUSSIAN,sWord);//Russian char counnts
	if(nForeignCount<nCount)
	{
		nForeignCount=nCount;
		nType=TT_RUSSIAN;
	}
	nCount=GetCharCount(TRANS_JAPANESE,sWord);//Japan char counnts
	if(nForeignCount<nCount)
	{
		nForeignCount=nCount;
		nType=TT_JAPANESE;
	}
	return nType;
}



//Get the postfix
inline bool PostfixSplit(char *sWord, char *sWordRet, char *sPostfix)
{
	static const char sSinglePostfix[]=POSTFIX_SINGLE;
	static const char sMultiPostfix[][9]=POSTFIX_MUTIPLE;
	unsigned int nPostfixLen=0,nWordLen=strlen(sWord);
	int i=0;
	
	while(sMultiPostfix[i][0]!=0&&strncmp(sWord+nWordLen-strlen(sMultiPostfix[i]),sMultiPostfix[i],strlen(sMultiPostfix[i]))!=0)
	{//Try to get the postfix of an address
		i++;
	}
	strcpy(sPostfix,sMultiPostfix[i]);
	nPostfixLen=strlen(sMultiPostfix[i]);//Get the length of place postfix
	
	if(nPostfixLen==0)
	{
		sPostfix[2]=0;
		strncpy(sPostfix,sWord+nWordLen-2,2);
		if(CC_Find(sSinglePostfix,sPostfix))
			nPostfixLen=2;
	}
	
	strncpy(sWordRet,sWord,nWordLen-nPostfixLen);
	sWordRet[nWordLen-nPostfixLen]=0;//Get the place name which have erasing the postfix
	sPostfix[nPostfixLen]=0;
    return true;
}


//
//remove all leading space in the left and right
//
//return false ,if the string are all blank
inline bool TrimStringSpaces(char *sWord)
{
	unsigned char ch;
	int size = strlen(sWord);
	if(size == 0) return false;

	int start = 0,end = size-1;

	while(end >= 0 )
	{
		ch = (unsigned char)sWord[end];
		if(ch == 32)
		{
			end--;
			continue;
		}
		if(ch == 161)
		{
			if((ch = sWord[end-1]) == 161)
			{
				end -= 2;
				continue;
			}
		}
		break;
	}

	while(start >= 0)
	{
		ch = (unsigned char)sWord[start];
		if(ch == 32)//english whitespace
		{
			start++;
			continue;
		}
		if(ch == 161)
		{
			if((ch = sWord[start+1]) == 161)
			{
				start += 2;
				continue;
			}
		}
		break;
	}
	
	if(start >= 0 && end >= 0 && (start != 0 || end != size-1))
	{
		strncpy(sWord,sWord+start,end-start+1);
		sWord[end-start+1] = 0;
	}
	else if(start < 0 || end < 0)
	{
		sWord[0] = 0;
		return false;
	}
	return true;
}


inline  void memzeroquad(void *dest,int nums)
{
	_asm
	{
		cld
        mov edi,dest
		mov ecx,nums
		xor eax,eax
		rep stosd
	}
}


