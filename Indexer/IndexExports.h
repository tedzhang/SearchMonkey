#pragma  once 

#ifdef INDEXER_EXPORTS
#define INDEXREXPORT_API __declspec(dllexport)
#else
#define INDEXREXPORT_API __declspec(dllimport)
#endif