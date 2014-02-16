#pragma  once 

#ifdef SEARCHER_EXPORTS
#define SEARCHER_API __declspec(dllexport)
#else
#define SEARCHER_API __declspec(dllimport)
#endif