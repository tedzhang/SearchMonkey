#pragma  once 

#include "Global.h"
#include <vector>

class CScoreDoc
{
public:
	CScoreDoc(UINT docID,float score):nDocID(docID),fScore(score)
	{

	}
	UINT  GetDocID() const  { return nDocID;}
	float GetScore() const  { return fScore;}
private:
	UINT  nDocID;
	float fScore;
};

typedef std::vector<CScoreDoc*> ScoreDocs;