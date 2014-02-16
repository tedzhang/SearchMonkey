
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


#define POSTFIX_SINGLE "坝邦堡杯城池村单岛道堤店洞渡队法峰府冈港阁宫沟国海号河湖环集江奖礁角街井郡坑口矿里岭楼路门盟庙弄牌派坡铺旗桥区渠泉人山省市水寺塔台滩坛堂厅亭屯湾文屋溪峡县线乡巷型洋窑营屿语园苑院闸寨站镇州庄族陂庵町"
#define POSTFIX_MUTIPLE {"半岛","草原","城市","大堤","大公国","大桥","地区","帝国","渡槽","港口","高速公路","高原","公路","公园","共和国","谷地","广场","国道","海峡","胡同","机场","集镇","教区","街道","口岸","码头","煤矿","牧场","农场","盆地","平原","丘陵","群岛","沙漠","沙洲","山脉","山丘","水库","隧道","特区","铁路","新村","雪峰","盐场","盐湖","渔场","直辖市","自治区","自治县","自治州",""}

#define  TRANS_ENGLISH   "·—阿埃艾爱安昂敖奥澳笆芭巴白拜班邦保堡鲍北贝本比毕彼别波玻博勃伯泊卜布才采仓查差柴彻川茨慈次达大戴代丹旦但当道德得的登迪狄蒂帝丁东杜敦多额俄厄鄂恩尔伐法范菲芬费佛夫福弗甫噶盖干冈哥戈革葛格各根古瓜哈海罕翰汗汉豪合河赫亨侯呼胡华霍基吉及加贾坚简杰金京久居君喀卡凯坎康考柯科可克肯库奎拉喇莱来兰郎朗劳勒雷累楞黎理李里莉丽历利立力连廉良列烈林隆卢虏鲁路伦仑罗洛玛马买麦迈曼茅茂梅门蒙盟米蜜密敏明摩莫墨默姆木穆那娜纳乃奈南内尼年涅宁纽努诺欧帕潘畔庞培佩彭皮平泼普其契恰强乔切钦沁泉让热荣肉儒瑞若萨塞赛桑瑟森莎沙山善绍舍圣施诗石什史士守斯司丝苏素索塔泰坦汤唐陶特提汀图土吐托陀瓦万王旺威韦维魏温文翁沃乌吾武伍西锡希喜夏相香歇谢辛新牙雅亚彦尧叶依伊衣宜义因音英雍尤于约宰泽增詹珍治中仲朱诸卓孜祖佐伽娅尕腓滕济嘉津赖莲琳律略慕妮聂裴浦奇齐琴茹珊卫欣逊札哲智兹芙汶迦珀琪梵斐胥黛"
#define  TRANS_RUSSIAN   "·阿安奥巴比彼波布察茨大德得丁杜尔法夫伏甫盖格哈基加坚捷金卡科可克库拉莱兰勒雷里历利连列卢鲁罗洛马梅蒙米姆娜涅宁诺帕泼普奇齐乔切日萨色山申什斯索塔坦特托娃维文乌西希谢亚耶叶依伊以扎佐柴达登蒂戈果海赫华霍吉季津柯理琳玛曼穆纳尼契钦丘桑沙舍泰图瓦万雅卓兹"
#define  TRANS_JAPANESE  "安奥八白百邦保北倍本比滨博步部彩菜仓昌长朝池赤川船淳次村大代岛稻道德地典渡尔繁饭风福冈高工宫古谷关广桂贵好浩和合河黑横恒宏后户荒绘吉纪佳加见健江介金今进井静敬靖久酒菊俊康可克口梨理里礼栗丽利立凉良林玲铃柳隆鹿麻玛美萌弥敏木纳南男内鸟宁朋片平崎齐千前浅桥琴青清庆秋丘曲泉仁忍日荣若三森纱杉山善上伸神圣石实矢世市室水顺司松泰桃藤天田土万望尾未文武五舞西细夏宪相小孝新星行雄秀雅亚岩杨洋阳遥野也叶一伊衣逸义益樱永由有佑宇羽郁渊元垣原远月悦早造则泽增扎宅章昭沼真政枝知之植智治中忠仲竹助椎子佐阪坂堀荻菅薰浜濑鸠筱"
//Translation type
#define  TT_ENGLISH  0
#define  TT_RUSSIAN  1
#define  TT_JAPANESE  2
//Seperator type
#define  SEPERATOR_C_SENTENCE "。！？：；…"
#define  SEPERATOR_C_SUB_SENTENCE "、，（）“”‘’"
#define  SEPERATOR_E_SENTENCE "!?:;"
#define  SEPERATOR_E_SUB_SENTENCE ",()\042'"
#define  SEPERATOR_LINK "\n\r 　"
//Sentence begin and ending string
#define SENTENCE_BEGIN	"始##始"
#define SENTENCE_END	"末##末"


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
		if(!strstr("±+—-＋",sChar))
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
		if(CC_Find("∶·．",sChar)||sChar[0]=='.')
		{//98．1％
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
		if(CC_Find("∶·．",sChar)||sChar[0]=='.')
		{//98．1％
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
		if(!CC_Find("百千万亿佰仟％‰",sChar)&&sChar[0]!='%')
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
//eg:百分之五点六的人早上八点十八分起床
inline bool IsAllChineseNum(char *sWord)
{
	unsigned int  k; 
	char tchar[3];
	static const char ChineseNum[]="零○一二三四五六七八九十廿百千万亿壹贰叁肆伍陆柒捌玖拾佰仟∶·．／点";//
	static const char sPrefix[]="几第数两";


	for(k = 0; k < strlen(sWord); k+=2)
	{
		strncpy(tchar,sWord+k,2) ;
		tchar[2]='\0';
		if(strncmp(sWord+k,"分之",4)==0)//百分之五
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


