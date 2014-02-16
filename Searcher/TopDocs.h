#pragma  once 

#include "ScoreDoc.h"
 class CTopDocs 
 {
 public:
	
	 ScoreDocs scoreDocs;
	
	 float GetMaxScore()            { return fMaxScore;}
	 void  SetMaxScore(float score) { fMaxScore = score;}
	 int   GetTotalHits()           { return totalHits;}

	 CTopDocs(int totalHits,float maxScore):totalHits(totalHits),fMaxScore(maxScore)
	 {
	 }
	 ~CTopDocs()
	 {
       int nsize = scoreDocs.size();
	   for(int i = 0; i < nsize;++i)
	   {
		   delete scoreDocs[i];
	   }
	 }
 private:
	  float fMaxScore;
	  int  totalHits;
 };
	
