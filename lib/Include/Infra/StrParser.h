
#ifndef __STR_PARSER_H__
#define __STR_PARSER_H__

class CStrParser
{
public:
	static char* memMem(char* pBuf, int len, char* pSub, int subLen);

	static char* skipCut(char** pBuf, const char *delimiters);
};


#endif //__STR_PARSER_H__
