#pragma  once 
#include <string.h> //for strcpy 

namespace QueryParser
{
	typedef enum
	{
		kEnd    = 0,
		kID     = 1,  //words(chinese or English)
		kAnd    = 2,  //and operator
		kOr     = 3,  //or operator
		kNot    = 4,  //not operator
		kLParen = 5,  //"("
		kRParen = 6,  //")"
		kQuote  = 7,  //"""
		kPlus   = 8,  //"+"
		kMinus  = 9,  //"-"
		kExclam = 10, //"!"
	} TokenType;



	class CToken
	{
	public:
		enum { MAX_TOKEN_SIZE = 255};
		TokenType  GetTokenType()               { return m_tokenType;}
		void       SetTokenType(TokenType type) { m_tokenType = type;}
		int        GetStartPos()                { return m_pos_start;}
		int        GetEndPos()                  { return m_pos_end  ;}
		const char*GetText()const               { return m_text; }
		void       SetStartPos(int nStartPos)   { m_pos_start = nStartPos;}
		void       SetEndPos(int nEndPos)       { m_pos_end = nEndPos;}
		void       SetText(const char *buffer)  { strcpy(m_text,buffer);}
		void       SetText(const char ch)       { m_text[0] = ch;m_text[1]= '\0';}


		CToken::CToken(const char  *strText,TokenType type, int startPos,int endPos)
			:m_tokenType(type)
			,m_pos_start(startPos)
			,m_pos_end(endPos)
		{
			strcpy(m_text,strText);
		}

		CToken::CToken(const char ch,TokenType type,int startpos)
			:m_tokenType(type)
			,m_pos_start(startpos)
			,m_pos_end(startpos)
		{
			m_text[0] = ch;
			m_text[1] = '\0';
		}

		CToken() { }
	private:

		char        m_text[MAX_TOKEN_SIZE];	
		TokenType   m_tokenType;
		int         m_pos_start;
		int         m_pos_end;
	};

}
