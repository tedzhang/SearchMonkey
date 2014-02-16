#include "../Searcher/IndexReader.h"
#include "../Searcher/IndexSearcher.h"
#include "../Segmenter/analysis.h"
#include "../Searcher/Analyzer.h"
#include "../Searcher/QueryParser.h"
#include "../Searcher/ResultList.h"
#include "../Searcher/Hits.h"
#include "../Indexer/Document.h"
using namespace QueryParser;
#include <iostream>
#include <string>
#include <time.h>

#define TESTPATH "W:\\2007\\SearchEngine\\TestIndexer\\test\\test.db"
#define DICT_PATH "F:\\temp\\Data\\"
void TestSearch()
{

	if(!CIndexReader::Open(TESTPATH))
		return;
	CAnalysis::initAnalysis(DICT_PATH);
	while(true)
	{
		std::string input;
		std::cout <<"Query:";
		std::cin >> input;

		CQueryParser parser;
		CQuery *pQuery = parser.Parse(input.c_str(),"content",new CChineseAnalyzer);
		if(!pQuery)
			return ;
		
		CIndexReader reader(pQuery);

		CIndexSearcher searcher(&reader);

		clock_t start = clock();
		
		CHits *pHits = searcher.Search(pQuery);

		clock_t end = clock();
		
		int ndoccount = 0;
		if(!pHits )
			ndoccount = 0;
		else
			ndoccount = pHits->GetLength();
		std::cout << "match "<<ndoccount <<" documents spending "<<(end-start)<<"ms"<<std::endl;

		int HITS_PER_PAGE = 10;
		for (int start = 0; start < pHits->GetLength(); start += HITS_PER_PAGE) 
		{
			int end = std::min(pHits->GetLength(), start + HITS_PER_PAGE);
			for (int i = start; i < end; i++) 
			{

				std::cout << "doc=" << pHits->GetDocID(i)<<" score="<<pHits->GetScore(i);

				CDocument* doc = pHits->GetDoc(i);
				std::string path = doc->Get("path");
				std::cout << path<<std::endl;
			}
		}
	}

	CIndexReader::Close();
}


void main()
{
	TestSearch();
}