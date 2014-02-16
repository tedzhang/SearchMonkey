
#include "UnknowWord.h"
#include <string.h>
#include <math.h>



//Unknown word recognition
//pWordSegResult:word Segmentation result;graphOptimum: The optimized segmentation graph
//graphSeg: The original segmentation graph
bool CUnknowWord::Recognition(PWORD_RESULT pWordSegResult, CDynamicArray &graphOptimum,CSegGraph &graphSeg,CDictionary &dictCore)
{	
	int nStartPos=0,j=0,nAtomStart,nAtomEnd,nPOSOriginal;
	ELEMENT_TYPE dValue;
	m_roleTag.POSTagging(pWordSegResult,dictCore,m_dict);
	//Tag the segmentation with unknown recognition roles according the core dictionary and unknown recognition dictionary
	for(int i=0;i<m_roleTag.m_nUnknownIndex;i++)
	{
		while( j < graphSeg.m_nAtomCount&&nStartPos<m_roleTag.m_nUnknownWords[i][0])
		{
			nStartPos+=graphSeg.m_nAtomLength[j++];
		}
		nAtomStart=j;
		while(j < graphSeg.m_nAtomCount&&nStartPos<m_roleTag.m_nUnknownWords[i][1])
		{
			nStartPos+=graphSeg.m_nAtomLength[j++];
		}
		nAtomEnd=j;
		if(nAtomStart<nAtomEnd)
		{
			graphOptimum.GetElementInfo(nAtomStart,nAtomEnd,dValue,nPOSOriginal);
			if(dValue>m_roleTag.m_dWordsPossibility[i])//Set the element with less frequency
			{
			   	graphOptimum.SetElement(nAtomStart,nAtomEnd,m_roleTag.m_dWordsPossibility[i],m_nPOS,NULL);
			}
		}
	}
	return true;
}
//Load unknown recognition dictionary
//Load context
//type: Unknown words type (including person,place,transliterion and so on)
bool CUnknowWord::Configure(char *sConfigFile,enum TAG_TYPE type)
{
	char sFilename[MAX_PATH];
	
	//Load the unknown recognition dictionary
	strcpy(sFilename,sConfigFile);
	strcat(sFilename,".dct");
	m_dict.Load(sFilename);
	
	//Load the unknown recognition context
	strcpy(sFilename,sConfigFile);
	strcat(sFilename,".ctx");
	m_roleTag.LoadContext(sFilename);
	
	//Set the tagging type
	m_roleTag.SetTagType(type);
	switch(type)
	{
		case TT_PERSON:
			m_nPOS=-'n'*256-'r';
			break;
		case TT_TRANS://Set the special flag for transliterations
			m_nPOS=-'t'*256-'t';
			break;
		case TT_PLACE:
			m_nPOS=-'n'*256-'s';
			break;
		default :
			m_nPOS=0;
			break;
	}
	return true;
}

//Judge whether the name is a given name
bool CUnknowWord::IsGivenName(char *sName)
{
	char sFirstChar[3],sSecondChar[3];
	ELEMENT_TYPE dGivenNamePossibility=0,dSingleNamePossibility=0;
	if(strlen(sName)!=4)
		return false;	
	
	strncpy(sFirstChar,sName,2);
	sFirstChar[2]=0;
	strncpy(sSecondChar,sName+2,2);
	sSecondChar[2]=0;

	//The possibility of P(Wi|Ti)
	dGivenNamePossibility+=log((double)m_dict.GetFrequency(sFirstChar,2)+1.0)-log(m_roleTag.m_context.GetFrequency(0,2)+1.0);
	dGivenNamePossibility+=log((double)m_dict.GetFrequency(sSecondChar,3)+1.0)-log(m_roleTag.m_context.GetFrequency(0,3)+1.0);
	//The possibility of conversion from 2 to 3
	dGivenNamePossibility+=log(m_roleTag.m_context.GetContextPossibility(0,2,3)+1.0)-log(m_roleTag.m_context.GetFrequency(0,2)+1.0);
	
	//The possibility of P(Wi|Ti)
	dSingleNamePossibility+=log((double)m_dict.GetFrequency(sFirstChar,1)+1.0)-log(m_roleTag.m_context.GetFrequency(0,1)+1.0);
	dSingleNamePossibility+=log((double)m_dict.GetFrequency(sSecondChar,4)+1.0)-log(m_roleTag.m_context.GetFrequency(0,4)+1.0);
	//The possibility of conversion from 1 to 4
	dSingleNamePossibility+=log(m_roleTag.m_context.GetContextPossibility(0,1,4)+1.0)-log(m_roleTag.m_context.GetFrequency(0,1)+1.0);

	if(dSingleNamePossibility>=dGivenNamePossibility)//ÕÅÕð||m_dict.GetFrequency(sFirstChar,1)/m_dict.GetFrequency(sFirstChar,2)>=10
	//The possibility being a single given name is more than being a 2-char given name
		return false;
	return true;
}
