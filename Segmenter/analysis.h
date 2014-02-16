
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
	bSuccess = dict.AddWord("��Ů","n",10);
	dict.AddWord("����˹��","n",100);
	dict.AddWord("�¶���","n",100);
	dict.AddWord("���ׯ","n",10000);
	dict.AddWord("����","nr",100);
	dict.DelWord("���ׯ","n");
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
	//default ����������
	//@param strWord Ҫ����Ĵ�
	//@param POS Ϊ����,������˵��,��ϸ�ļ��ĵ�
	//  n ����
	//  nr ����
	//  ns ����
	//  nt ����������
	//	nz ����ר��
	//	nl �����Թ�����
	//	ng ����������
	//  t  ʱ���
	//  s ������
	/*	f ��λ��
		v ����
		a ���ݴ�
		b �����
		r ����
		m ����
		mq ������
		q ����
		d ����
		p ���
		u ����
		e ̾��
		y ������
		o ������

		x �ַ���
		xx ��������
		xu ��ַURL
		w ������
		wkz �����ţ�ȫ�ǣ��� ��  ��  ��  �� ��  �� ��   ��ǣ�( [ { <
		wky �����ţ�ȫ�ǣ��� ��  �� �� ��  �� �� �� ��ǣ� ) ] { >
		wyz �����ţ�ȫ�ǣ��� �� ��  
		wyy �����ţ�ȫ�ǣ��� �� �� 
		wj ��ţ�ȫ�ǣ���
		ww �ʺţ�ȫ�ǣ��� ��ǣ�?
		wt ̾�ţ�ȫ�ǣ��� ��ǣ�!
		wd ���ţ�ȫ�ǣ��� ��ǣ�,
		wf �ֺţ�ȫ�ǣ��� ��ǣ� ;
	wn �ٺţ�ȫ�ǣ���
		wm ð�ţ�ȫ�ǣ��� ��ǣ� :
	ws ʡ�Ժţ�ȫ�ǣ�����  ��
		wp ���ۺţ�ȫ�ǣ�����   ����   ������   ��ǣ�---  ----
		wb �ٷֺ�ǧ�ֺţ�ȫ�ǣ��� ��   ��ǣ�%
		wh ��λ���ţ�ȫ�ǣ��� �� ��  ��  ��  ��ǣ�$*/
	//@param nFrequence Ϊ0(�������� �� 2079997(������֮���һ����

	bool AddWord(char *strWord,const char* sPOS,int nFrequence =2079997);


    //�Ӵʵ���ɾ��һ����
	//
	//��Ҫָ���ʺʹ���,ͬ��˵��
	bool DelWord(char *strWord,const char* sPOS);

	//
	//
    //����ʵ��ļ�,sFileName���Ժ� OpenDictFile�Ĳ���һ��,��ʾ
	//����ԭ���Ĵʵ�,Ҳ����ѡ����´ʵ䱣�浽������ļ�����
	bool SaveDictFile( char *sFileName);
private:
	CDictionary *m_pDictionary;


};

#endif


