

#include "StopwordFilter.h"
#include "Utility.h"

CStopwordFilter::STOPWORDSLIST CStopwordFilter::m_list;




bool CStopwordFilter::Load(const char* stopwordfile)
{
   if(m_list.size() > 0 ) return true;

   FILE *fp = fopen(stopwordfile,"rb");
   if(fp)
   {
	   char buffer[1024*10];
	   int read_bytes = fread(buffer,sizeof(char),1024*10,fp);
	   fclose(fp);
	   int ipos = 0,i = 0;
	   char line[255];
	   while(i < read_bytes)
	   {
	      if(buffer[i] != '\r'&& buffer[i]!='\n')
		  { 
              line[ipos++] = buffer[i];
			  i++;
		  }
		  else
		  {
			  line[ipos] = 0;
			  if(!TrimStringSpaces(line))
			  { 
				  i++; 
				  ipos=0; 
				  continue;
			  }
			  m_list.insert(strdup(line));
			  ipos = 0;
			  i++;
		  }
	   }
	  
	   return true;
   }
   return false;
}