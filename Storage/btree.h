 /*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This header file defines the interface that the sqlite B-Tree file
** subsystem.  See comments in the source code for a detailed description
** of what each interface routine does.
**
** @(#) $Id: btree.h,v 1.71 2006/06/27 16:34:57 danielk1977 Exp $
*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _BTREE_H_
#define _BTREE_H_

#include "pager.h"
#include "sqliteInt.h"
/* TODO: This definition is just included so other modules compile. It
** needs to be revisited.
*/
#define SQLITE_N_BTREE_META 10



/*
** Page type flags.  An ORed combination of these flags appear as the
** first byte of every BTree page.
*/
#define PTF_INTKEY    0x01
#define PTF_ZERODATA  0x02
#define PTF_LEAFDATA  0x04
#define PTF_LEAF      0x08


/*
** Forward declarations of structure
*/
typedef struct Btree Btree;
typedef struct BtCursor BtCursor;
typedef struct BtShared BtShared;
/* Forward declarations */
typedef struct MemPage MemPage;
typedef struct BtLock BtLock;

/*
** As each page of the file is loaded into memory, an instance of the following
** structure is appended and initialized to zero.  This structure stores
** information about the page that is decoded from the raw file page.
**
** The pParent field points back to the parent page.  This allows us to
** walk up the BTree from any leaf to the root.  Care must be taken to
** unref() the parent page pointer when this page is no longer referenced.
** The pageDestructor() routine handles that chore.
*/
struct MemPage {
  u8 isInit;           /* True if previously initialized. MUST BE FIRST! */
  u8 idxShift;         /* True if Cell indices have changed */
  u8 nOverflow;        /* Number of overflow cell bodies in aCell[] */
  u8 intKey;           /* True if intkey flag is set */
  u8 leaf;             /* True if leaf flag is set */
  u8 zeroData;         /* True if table stores keys only */
  u8 leafData;         /* True if tables stores data on leaves only */
  u8 hasData;          /* True if this page stores data */
  u8 hdrOffset;        /* 100 for page 1.  0 otherwise */
  u8 childPtrSize;     /* 0 if leaf==1.  4 if leaf==0 */
  u16 maxLocal;        /* Copy of Btree.maxLocal or Btree.maxLeaf */
  u16 minLocal;        /* Copy of Btree.minLocal or Btree.minLeaf */
  u16 cellOffset;      /* Index in aData of first cell pointer */
  u16 idxParent;       /* Index in parent of this node */
  u16 nFree;           /* Number of free bytes on the page */
  u16 nCell;           /* Number of cells on this page, local and ovfl */
  struct _OvflCell {   /* Cells that will not fit on aData[] */
    u8 *pCell;          /* Pointers to the body of the overflow cell */
    u16 idx;            /* Insert this cell before idx-th non-overflow cell */
  } aOvfl[5];
  BtShared *pBt;       /* Pointer back to BTree structure */
  u8 *aData;           /* Pointer back to the start of the page */
  Pgno pgno;           /* Page number for this page */
  MemPage *pParent;    /* The parent of this page.  NULL for root */
};

/*
** The in-memory image of a disk page has the auxiliary information appended
** to the end.  EXTRA_SIZE is the number of bytes of space needed to hold
** that extra information.
*/
#define EXTRA_SIZE sizeof(MemPage)


/*
** Btree.inTrans may take one of the following values.
**
** If the shared-data extension is enabled, there may be multiple users
** of the Btree structure. At most one of these may open a write transaction,
** but any number may have active read transactions. Variable Btree.pDb 
** points to the handle that owns any current write-transaction.
*/
#define TRANS_NONE  0
#define TRANS_READ  1
#define TRANS_WRITE 2



/*
** Everything we need to know about an open database
*/
struct BtShared {
  Pager *pPager;        /* The page cache */
  BtCursor *pCursor;    /* A list of all open cursors */
  MemPage *pPage1;      /* First page of the database */
  u8 inStmt;            /* True if we are in a statement subtransaction */
  u8 readOnly;          /* True if the underlying file is readonly */
  u8 maxEmbedFrac;      /* Maximum payload as % of total page size */
  u8 minEmbedFrac;      /* Minimum payload as % of total page size */
  u8 minLeafFrac;       /* Minimum leaf payload as % of total page size */
  u8 pageSizeFixed;     /* True if the page size can no longer be changed */
#ifndef SQLITE_OMIT_AUTOVACUUM
  u8 autoVacuum;        /* True if database supports auto-vacuum */
#endif
  u16 pageSize;         /* Total number of bytes on a page */
  u16 usableSize;       /* Number of usable bytes on each page */
  int maxLocal;         /* Maximum local payload in non-LEAFDATA tables */
  int minLocal;         /* Minimum local payload in non-LEAFDATA tables */
  int maxLeaf;          /* Maximum local payload in a LEAFDATA table */
  int minLeaf;          /* Minimum local payload in a LEAFDATA table */
//  BusyHandler *pBusyHandler;   /* Callback for when there is lock contention */
  u8 inTransaction;     /* Transaction state */
  int nRef;             /* Number of references to this structure */
  int nTransaction;     /* Number of open transactions (read + write) */
  void *pSchema;        /* Pointer to space allocated by sqlite3BtreeSchema() */
  void (*xFreeSchema)(void*);  /* Destructor for BtShared.pSchema */
#ifndef SQLITE_OMIT_SHARED_CACHE
  BtLock *pLock;        /* List of locks held on this shared-btree struct */
  BtShared *pNext;      /* Next in ThreadData.pBtree linked list */
#endif
};

/*
** An instance of the following structure is used to hold information
** about a cell.  The parseCellPtr() function fills in this structure
** based on information extract from the raw disk page.
*/
typedef struct CellInfo CellInfo;
struct CellInfo {
  u8 *pCell;     /* Pointer to the start of cell content */
  i64 nKey;      /* The key for INTKEY tables, or number of bytes in key */
  u32 nData;     /* Number of bytes of data */
  u16 nHeader;   /* Size of the cell content header in bytes */
  u16 nLocal;    /* Amount of payload held locally */
  u16 iOverflow; /* Offset to overflow page number.  Zero if no overflow */
  u16 nSize;     /* Size of the cell content on the main b-tree page */
};

/*
** A cursor is a pointer to a particular entry in the BTree.
** The entry is identified by its MemPage and the index in
** MemPage.aCell[] of the entry.
*/
struct BtCursor {
  Btree *pBtree;            /* The Btree to which this cursor belongs */
  BtCursor *pNext, *pPrev;  /* Forms a linked list of all cursors */
  int (*xCompare)(void*,int,const void*,int,const void*); /* Key comp func */
  void *pArg;               /* First arg to xCompare() */
  Pgno pgnoRoot;            /* The root page of this tree */
  MemPage *pPage;           /* Page that contains the entry */
  int idx;                  /* Index of the entry in pPage->aCell[] */
  CellInfo info;            /* A parse of the cell we are pointing at */
  u8 wrFlag;                /* True if writable */
  u8 eState;                /* One of the CURSOR_XXX constants (see below) */
#ifndef SQLITE_OMIT_SHARED_CACHE
  void *pKey;      /* Saved key that was cursor's last known position */
  i64 nKey;        /* Size of pKey, or last integer key */
  int skip;        /* (skip<0) -> Prev() is a no-op. (skip>0) -> Next() is */
#endif
};

/*
** If defined as non-zero, auto-vacuum is enabled by default. Otherwise
** it must be turned on for each database using "PRAGMA auto_vacuum = 1".
*/
#ifndef SQLITE_DEFAULT_AUTOVACUUM
  #define SQLITE_DEFAULT_AUTOVACUUM 0
#endif

/* Btree handle */
struct Btree {
//  sqlite3 *pSqlite;
  BtShared *pBt;
  u8 inTrans;            /* TRANS_NONE, TRANS_READ or TRANS_WRITE */
};



/* The flags parameter to sqlite3BtreeOpen can be the bitwise or of the
** following values.
**
** NOTE:  These values must match the corresponding PAGER_ values in
** pager.h.
*/
#define BTREE_OMIT_JOURNAL  1  /* Do not use journal.  No argument */
#define BTREE_NO_READLOCK   2  /* Omit readlocks on readonly files */
#define BTREE_MEMORY        4  /* In-memory DB.  No argument */


int sqlite3BtreeOpen(
  const char *zFilename,   /* Name of database file to open */
  Btree **,                /* Return open Btree* here */
  int flags                /* Flags */
);

int sqlite3BtreeClose(Btree*);
//int sqlite3BtreeSetBusyHandler(Btree*,BusyHandler*);
int sqlite3BtreeSetCacheSize(Btree*,int);
int sqlite3BtreeSetSafetyLevel(Btree*,int, int);
int sqlite3BtreeSyncDisabled(Btree*);
int sqlite3BtreeSetPageSize(Btree*,int, int);
int sqlite3BtreeGetPageSize(Btree*);
int sqlite3BtreeGetReserve(Btree*);
int sqlite3BtreeSetAutoVacuum(Btree *, int);
int sqlite3BtreeGetAutoVacuum(Btree *);

int sqlite3BtreeBeginTrans(Btree*, int);
int sqlite3BtreeCommit(Btree*);
int sqlite3BtreeRollback(Btree*);

int sqlite3BtreeBeginStmt(Btree*);
int sqlite3BtreeCommitStmt(Btree*);
int sqlite3BtreeRollbackStmt(Btree*);

int sqlite3BtreeCreateTable(Btree*, int*, int flags);
int sqlite3BtreeIsInTrans(Btree*);
int sqlite3BtreeIsInStmt(Btree*);
int sqlite3BtreeIsInReadTrans(Btree*);

//?????
int sqlite3BtreeSync(Btree*, const char *zMaster);
void *sqlite3BtreeSchema(Btree *, int, void(*)(void *));
int sqlite3BtreeSchemaLocked(Btree *);
int sqlite3BtreeLockTable(Btree *, int, u8);

const char *sqlite3BtreeGetFilename(Btree *);
const char *sqlite3BtreeGetDirname(Btree *);
//日志文件
const char *sqlite3BtreeGetJournalname(Btree *);
//将一个btree中的数据拷贝到另外一个btree,如果失败,将回滚
int sqlite3BtreeCopyFile(Btree *, Btree *);

/* The flags parameter to sqlite3BtreeCreateTable can be the bitwise OR
** of the following flags:
*/
#define BTREE_INTKEY     1    /* Table has only 64-bit signed integer keys */
#define BTREE_ZERODATA   2    /* Table has keys only - no data */
#define BTREE_LEAFDATA   4    /* Data stored in leaves only.  Implies INTKEY */

//删除一个表
int sqlite3BtreeDropTable(Btree*, int, int*);
//清除一个表中的所有数据
int sqlite3BtreeClearTable(Btree*, int);
//其他数据,究竟做什么用,还不知道
int sqlite3BtreeGetMeta(Btree*, int idx, u32 *pValue);
int sqlite3BtreeUpdateMeta(Btree*, int idx, u32 value);

int sqlite3BtreeCursor(
  Btree*,                              /* BTree containing table to open */
  int iTable,                          /* Index of root page */
  int wrFlag,                          /* 1 for writing.  0 for read-only */
  int(*)(void*,int,const void*,int,const void*),  /* Key comparison function */
  void*,                               /* First argument to compare function */
  BtCursor **ppCursor                  /* Returned cursor */
);


//比较函数:void: not used, int: first key size, void*:first key, int:second keysize, void*: second key
void sqlite3BtreeSetCompare(
  BtCursor *,
  int(*)(void*,int,const void*,int,const void*),
  void*
);

//光标处理
int sqlite3BtreeCloseCursor(BtCursor*);
int sqlite3BtreeMoveto(BtCursor*, const void *pKey, i64 nKey, int *pRes);
int sqlite3BtreeDelete(BtCursor*);
int sqlite3BtreeInsert(BtCursor*, const void *pKey, i64 nKey,
                                  const void *pData, int nData);
int sqlite3BtreeFirst(BtCursor*, int *pRes);
int sqlite3BtreeLast(BtCursor*, int *pRes);
int sqlite3BtreeNext(BtCursor*, int *pRes);
//检查关标指向的位置是否为空
int sqlite3BtreeEof(BtCursor*);
//获取光标指向的页面的标志
int sqlite3BtreeFlags(BtCursor*);
//光标向前移动,成功pRes=0;如果光标一开始指向第一个位置,返回1;
int sqlite3BtreePrevious(BtCursor*, int *pRes);
//获取光标处key的大小
int sqlite3BtreeKeySize(BtCursor*, i64 *pSize);
//获取光标所在的一部分key的值,offset为开始偏移量,amt为长度
int sqlite3BtreeKey(BtCursor*, u32 offset, u32 amt, void*);

//得到光标的的key,data的数据,pAmt为可以提供的长度,当前cell的长度，而不是保存的整个数据的长度
const void *sqlite3BtreeKeyFetch(BtCursor*, int *pAmt);
const void *sqlite3BtreeDataFetch(BtCursor*, int *pAmt);

//得到关标处的数据的长度
int sqlite3BtreeDataSize(BtCursor*, u32 *pSize);
//得到关标处的数据,offset:为起始偏移量,amt为数据长度
int sqlite3BtreeData(BtCursor*, u32 offset, u32 amt, void*);

//检查btree的完整性
char *sqlite3BtreeIntegrityCheck(Btree*, int *aRoot, int nRoot);
//获取btree的paper
struct Pager *sqlite3BtreePager(Btree*);


#ifdef SQLITE_TEST
int sqlite3BtreeCursorInfo(BtCursor*, int*, int);
void sqlite3BtreeCursorList(Btree*);
#endif

#ifdef SQLITE_DEBUG
int sqlite3BtreePageDump(Btree*, int, int recursive);
#else
#define sqlite3BtreePageDump(X,Y,Z) SQLITE_OK
#endif

#endif /* _BTREE_H_ */


#ifdef __cplusplus
}
#endif
