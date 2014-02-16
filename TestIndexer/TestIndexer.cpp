#include "../Indexer/IndexWriter.h"
#include "../Indexer/Field.h"


#define TESTPATH "W:\\2007\\SearchEngine\\TestIndexer\\test\\"
#define MAX_TEXT_SIZE  1024*200


bool InitTestData(int iFile,char *buffer,int &nBufferSize)
{
	char fileName[260];
	strcpy(fileName,TESTPATH);
	char temp[10];
	sprintf(temp,"%d.txt",iFile);

	strcat(fileName,temp);
	FILE* fp = fopen(fileName,"rb");
	if(fp)
	{
		nBufferSize = fread(buffer,sizeof(char),nBufferSize,fp);
		buffer[nBufferSize] = 0;
		fclose(fp);
		return true;
	}
	return false;
}

void TestWrite()
{
	CIndexWriter writer;
	writer.SetMergeFactor(10);
	writer.Open("test\\test.db");

	for(int i = 0; i <= 16;++i)
	{
		CDocument doc;
		doc.Add(CField("path","w:\\test\\test",CField::YES,CField::UN_TOKENIZED));

		char text[MAX_TEXT_SIZE];
		int  size = MAX_TEXT_SIZE;
		if(!InitTestData(i,text,size))
			continue;
		doc.Add(CField("content",text,CField::YES,CField::TOKENIZED));

		writer.AddDocument(doc);
	}
	writer.Close();
}




void main()
{
  
    TestWrite();
}