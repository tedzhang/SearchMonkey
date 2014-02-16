
#ifndef _ANALYSIS_DLL_H
#define _ANALYSIS_DLL_H

#ifdef SEGMENTER_EXPORTS
#define ANALYSIS_API __declspec(dllexport)
#else
#define ANALYSIS_API __declspec(dllimport)
#endif


#define ATTR_STOPWORD 0X1

typedef void AnalysisCallBack(const char* pKeyword, unsigned int nStartPos, unsigned short nWordIndex, float fScore, int nAttr );

class CResult;
class CDictionary;

class ANALYSIS_API CAnalysis {
public:
	static int	initAnalysis(const char* szDictRoot);
	static void	releaseAnalysis();

	static int	analysisText(const char* pText, int nTextSize, AnalysisCallBack* pCallBackFuct,bool bKeepLogicalword);
	static int	analysisFile(const char* pText, int nTextSize, AnalysisCallBack* pCallBackFuct);
private:
	static CResult* m_pAnalysis;
	static bool		m_bInitFlag;
public:
};

//
//Usage
//
//////////////////////////////////////////////////////////////////////////////////////////////
/*   CCnDictionary dict;
	char coreDictPath[256];
	strcpy(coreDictPath,"F:\\temp\\Data\\");
	strcat(coreDictPath,"coreDict.dct");
	bool bSuccess = false;
	if(dict.OpenDictFile(coreDictPath))
	{
	bSuccess = dict.AddWord("超女","n",10);
	dict.AddWord("卡巴斯基","n",100);
	dict.AddWord("新东方","n",100);
	dict.AddWord("渔家庄","n",10000);
	dict.AddWord("李鹏","nr",100);
	dict.DelWord("渔家庄","n");
	}
	dict.SaveDictFile(coreDictPath);
*/

class ANALYSIS_API CCnDictionary
{
public:
	CCnDictionary();
	~CCnDictionary();
	bool OpenDictFile( char *sFileName);
	//
	//default 词性是名词
	//@param strWord 要加入的词
	//@param POS 为词性,见如下说明,详细的见文档
	//  n 名词
	//  nr 人名
	//  ns 地名
	//  nt 机构团体名
	//	nz 其它专名
	//	nl 名词性惯用语
	//	ng 名词性语素
	//  t  时间词
	//  s 处所词
	/*	f 方位词
		v 动词
		a 形容词
		b 区别词
		r 代词
		m 数词
		mq 数量词
		q 量词
		d 副词
		p 介词
		u 助词
		e 叹词
		y 语气词
		o 拟声词

		x 字符串
		xx 非语素字
		xu 网址URL
		w 标点符号
		wkz 左括号，全角：（ 〔  ［  ｛  《 【  〖 〈   半角：( [ { <
		wky 右括号，全角：） 〕  ］ ｝ 》  】 〗 〉 半角： ) ] { >
		wyz 左引号，全角：“ ‘ 『  
		wyy 右引号，全角：” ’ 』 
		wj 句号，全角：。
		ww 问号，全角：？ 半角：?
		wt 叹号，全角：！ 半角：!
		wd 逗号，全角：， 半角：,
		wf 分号，全角：； 半角： ;
	wn 顿号，全角：、
		wm 冒号，全角：： 半角： :
	ws 省略号，全角：……  …
		wp 破折号，全角：——   －－   ——－   半角：---  ----
		wb 百分号千分号，全角：％ ‰   半角：%
		wh 单位符号，全角：￥ ＄ ￡  °  ℃  半角：$*/
	//@param nFrequence 为0(不包括） 到 2079997(包括）之间的一个数

	bool AddWord(char *strWord,const char* sPOS,int nFrequence =2079997);


    //从词典中删除一个词
	//
	//需要指明词和词性,同上说明
	bool DelWord(char *strWord,const char* sPOS);

	//
	//
    //保存词典文件,sFileName可以和 OpenDictFile的参数一样,表示
	//覆盖原来的词典,也可以选择把新词典保存到另外的文件里面
	bool SaveDictFile( char *sFileName);
private:
	CDictionary *m_pDictionary;


};

#endif


