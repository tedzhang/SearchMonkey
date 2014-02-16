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
** Internal interface definitions for SQLite.
**
** @(#) $Id: sqliteInt.h,v 1.523 2006/07/26 13:43:31 drh Exp $
*/
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _SQLITEINT_H_
#define _SQLITEINT_H_
#include <windows.h>


/*
** Extra interface definitions for those who need them
*/
#ifdef SQLITE_EXTRA
# include "sqliteExtra.h"
#endif

/*
** Many people are failing to set -DNDEBUG=1 when compiling SQLite.
** Setting NDEBUG makes the code smaller and run faster.  So the following
** lines are added to automatically set NDEBUG unless the -DSQLITE_DEBUG=1
** option is set.  Thus NDEBUG becomes an opt-in rather than an opt-out
** feature.
*/
#if !defined(NDEBUG) && !defined(SQLITE_DEBUG) 
# define NDEBUG 1
#endif

/*
** These #defines should enable >2GB file support on Posix if the
** underlying operating system supports it.  If the OS lacks
** large file support, or if the OS is windows, these should be no-ops.
**
** Large file support can be disabled using the -DSQLITE_DISABLE_LFS switch
** on the compiler command line.  This is necessary if you are compiling
** on a recent machine (ex: RedHat 7.2) but you want your code to work
** on an older machine (ex: RedHat 6.0).  If you compile on RedHat 7.2
** without this option, LFS is enable.  But LFS does not exist in the kernel
** in RedHat 6.0, so the code won't work.  Hence, for maximum binary
** portability you should omit LFS.
**
** Similar is true for MacOS.  LFS is only supported on MacOS 9 and later.
*/
#ifndef SQLITE_DISABLE_LFS
# define _LARGE_FILE       1
# ifndef _FILE_OFFSET_BITS
#   define _FILE_OFFSET_BITS 64
# endif
# define _LARGEFILE_SOURCE 1
#endif

#ifndef SQLITE_OMIT_SHARED_CACHE
#define SQLITE_OMIT_SHARED_CACHE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>


#define SQLITE_OK           0   /* Successful result */
/* beginning-of-error-codes */
#define SQLITE_ERROR        1   /* SQL error or missing database */
#define SQLITE_INTERNAL     2   /* NOT USED. Internal logic error in SQLite */
#define SQLITE_PERM         3   /* Access permission denied */
#define SQLITE_ABORT        4   /* Callback routine requested an abort */
#define SQLITE_BUSY         5   /* The database file is locked */
#define SQLITE_LOCKED       6   /* A table in the database is locked */
#define SQLITE_NOMEM        7   /* A malloc() failed */
#define SQLITE_READONLY     8   /* Attempt to write a readonly database */
#define SQLITE_INTERRUPT    9   /* Operation terminated by sqlite3_interrupt()*/
#define SQLITE_IOERR       10   /* Some kind of disk I/O error occurred */
#define SQLITE_CORRUPT     11   /* The database disk image is malformed */
#define SQLITE_NOTFOUND    12   /* NOT USED. Table or record not found */
#define SQLITE_FULL        13   /* Insertion failed because database is full */
#define SQLITE_CANTOPEN    14   /* Unable to open the database file */
#define SQLITE_PROTOCOL    15   /* Database lock protocol error */
#define SQLITE_EMPTY       16   /* Database is empty */
#define SQLITE_SCHEMA      17   /* The database schema changed */
#define SQLITE_TOOBIG      18   /* NOT USED. Too much data for one row */
#define SQLITE_CONSTRAINT  19   /* Abort due to contraint violation */
#define SQLITE_MISMATCH    20   /* Data type mismatch */
#define SQLITE_MISUSE      21   /* Library used incorrectly */
#define SQLITE_NOLFS       22   /* Uses OS features not supported on host */
#define SQLITE_AUTH        23   /* Authorization denied */
#define SQLITE_FORMAT      24   /* Auxiliary database format error */
#define SQLITE_RANGE       25   /* 2nd parameter to sqlite3_bind out of range */
#define SQLITE_NOTADB      26   /* File opened that is not a database file */
#define SQLITE_ROW         100  /* sqlite3_step() has another row ready */
#define SQLITE_DONE        101  /* sqlite3_step() has finished executing */


/*
** These are the allowed values for the eTextRep argument to
** sqlite3_create_collation and sqlite3_create_function.
*/
#define SQLITE_UTF8           1
#define SQLITE_UTF16LE        2
#define SQLITE_UTF16BE        3
#define SQLITE_UTF16          4    /* Use native byte order */
#define SQLITE_ANY            5    /* sqlite3_create_function only */
#define SQLITE_UTF16_ALIGNED  8    /* sqlite3_create_collation only */


/*
** The maximum number of in-memory pages to use for the main database
** table and for temporary tables. Internally, the MAX_PAGES and 
** TEMP_PAGES macros are used. To override the default values at
** compilation time, the SQLITE_DEFAULT_CACHE_SIZE and 
** SQLITE_DEFAULT_TEMP_CACHE_SIZE macros should be set.
*/
#ifdef SQLITE_DEFAULT_CACHE_SIZE
# define MAX_PAGES SQLITE_DEFAULT_CACHE_SIZE
#else
# define MAX_PAGES   2000
#endif
#ifdef SQLITE_DEFAULT_TEMP_CACHE_SIZE
# define TEMP_PAGES SQLITE_DEFAULT_TEMP_CACHE_SIZE
#else
# define TEMP_PAGES   500
#endif

/*
** OMIT_TEMPDB is set to 1 if SQLITE_OMIT_TEMPDB is defined, or 0
** afterward. Having this macro allows us to cause the C compiler 
** to omit code used by TEMP tables without messy #ifndef statements.
*/
#ifdef SQLITE_OMIT_TEMPDB
#define OMIT_TEMPDB 1
#else
#define OMIT_TEMPDB 0
#endif

/*
** If the following macro is set to 1, then NULL values are considered
** distinct when determining whether or not two entries are the same
** in a UNIQUE index.  This is the way PostgreSQL, Oracle, DB2, MySQL,
** OCELOT, and Firebird all work.  The SQL92 spec explicitly says this
** is the way things are suppose to work.
**
** If the following macro is set to 0, the NULLs are indistinct for
** a UNIQUE index.  In this mode, you can only have a single NULL entry
** for a column declared UNIQUE.  This is the way Informix and SQL Server
** work.
*/
#define NULL_DISTINCT_FOR_UNIQUE 1

/*
** The maximum number of attached databases.  This must be at least 2
** in order to support the main database file (0) and the file used to
** hold temporary tables (1).  And it must be less than 32 because
** we use a bitmask of databases with a u32 in places (for example
** the Parse.cookieMask field).
*/
#define MAX_ATTACHED 10

/*
** The maximum value of a ?nnn wildcard that the parser will accept.
*/
#define SQLITE_MAX_VARIABLE_NUMBER 999

/*
** The "file format" number is an integer that is incremented whenever
** the VDBE-level file format changes.  The following macros define the
** the default file format for new databases and the maximum file format
** that the library can read.
*/
#define SQLITE_MAX_FILE_FORMAT 4
#ifndef SQLITE_DEFAULT_FILE_FORMAT
# define SQLITE_DEFAULT_FILE_FORMAT 1
#endif

/*
** Provide a default value for TEMP_STORE in case it is not specified
** on the command-line
*/
#ifndef TEMP_STORE
# define TEMP_STORE 1
#endif

/*
** GCC does not define the offsetof() macro so we'll have to do it
** ourselves.
*/
#ifndef offsetof
#define offsetof(STRUCTURE,FIELD) ((int)((char*)&((STRUCTURE*)0)->FIELD))
#endif

/*
** Check to see if this machine uses EBCDIC.  (Yes, believe it or
** not, there are still machines out there that use EBCDIC.)
*/
#if 'A' == '\301'
# define SQLITE_EBCDIC 1
#else
# define SQLITE_ASCII 1
#endif

/*
** Integers of known sizes.  These typedefs might change for architectures
** where the sizes very.  Preprocessor macros are available so that the
** types can be conveniently redefined at compile-type.  Like this:
**
**         cc '-DUINTPTR_TYPE=long long int' ...
*/
#ifndef UINT32_TYPE
# define UINT32_TYPE unsigned int
#endif
#ifndef UINT16_TYPE
# define UINT16_TYPE unsigned short int
#endif
#ifndef INT16_TYPE
# define INT16_TYPE short int
#endif
#ifndef UINT8_TYPE
# define UINT8_TYPE unsigned char
#endif
#ifndef INT8_TYPE
# define INT8_TYPE signed char
#endif
#ifndef INT32_TYPE
#define INT32_TYPE	int
#endif
typedef UINT32_TYPE u32;           /* 4-byte unsigned integer */
typedef UINT16_TYPE u16;           /* 2-byte unsigned integer */
typedef INT16_TYPE	i16;            /* 2-byte signed integer */
typedef UINT8_TYPE	u8;             /* 1-byte unsigned integer */
typedef UINT8_TYPE	i8;             /* 1-byte signed integer */
typedef LONGLONG	i64;
typedef ULONGLONG	u64;
typedef double		LONGDOUBLE_TYPE;
typedef ULONGLONG	sqlite_uint64;

/*
** Macros to determine whether the machine is big or little endian,
** evaluated at runtime.
*/
extern const int sqlite3one;
#define SQLITE_BIGENDIAN    (*(char *)(&sqlite3one)==0)
#define SQLITE_LITTLEENDIAN (*(char *)(&sqlite3one)==1)

/*
** Defer sourcing vdbe.h and btree.h until after the "u8" and 
** "BusyHandler typedefs.
*/
#include "btree.h"
#include "pager.h"

#define SQLITE_MEMDEBUG 0

#ifdef SQLITE_MEMDEBUG
/*
** The following global variables are used for testing and debugging
** only.  They only work if SQLITE_MEMDEBUG is defined.
*/
extern int sqlite3_nMalloc;      /* Number of sqliteMalloc() calls */
extern int sqlite3_nFree;        /* Number of sqliteFree() calls */
extern int sqlite3_iMallocFail;  /* Fail sqliteMalloc() after this many calls */
extern int sqlite3_iMallocReset; /* Set iMallocFail to this when it reaches 0 */

extern void *sqlite3_pFirst;         /* Pointer to linked list of allocations */
extern int sqlite3_nMaxAlloc;        /* High water mark of ThreadData.nAlloc */
extern int sqlite3_mallocDisallowed; /* assert() in sqlite3Malloc() if set */
extern int sqlite3_isFail;           /* True if all malloc calls should fail */
extern const char *sqlite3_zFile;    /* Filename to associate debug info with */
extern int sqlite3_iLine;            /* Line number for debug info */

#define ENTER_MALLOC (sqlite3_zFile = __FILE__, sqlite3_iLine = __LINE__)
#define sqliteMalloc(x)          (ENTER_MALLOC, sqlite3Malloc(x,1))
#define sqliteMallocRaw(x)       (ENTER_MALLOC, sqlite3MallocRaw(x,1))
#define sqliteRealloc(x,y)       (ENTER_MALLOC, sqlite3Realloc(x,y))
#define sqliteStrDup(x)          (ENTER_MALLOC, sqlite3StrDup(x))
#define sqliteStrNDup(x,y)       (ENTER_MALLOC, sqlite3StrNDup(x,y))
#define sqliteReallocOrFree(x,y) (ENTER_MALLOC, sqlite3ReallocOrFree(x,y))

#else

#define ENTER_MALLOC 0
#define sqliteMalloc(x)          sqlite3Malloc(x,1)
#define sqliteMallocRaw(x)       sqlite3MallocRaw(x,1)
#define sqliteRealloc(x,y)       sqlite3Realloc(x,y)
#define sqliteStrDup(x)          sqlite3StrDup(x)
#define sqliteStrNDup(x,y)       sqlite3StrNDup(x,y)
#define sqliteReallocOrFree(x,y) sqlite3ReallocOrFree(x,y)

#endif

#define sqliteFree(x)          sqlite3FreeX(x)
#define sqliteAllocSize(x)     sqlite3AllocSize(x)


/*
** The root-page of the master database table.
*/
#define MASTER_ROOT       1

/*
** The name of the schema table.
*/
#define SCHEMA_TABLE(x)  ((!OMIT_TEMPDB)&&(x==1)?TEMP_MASTER_NAME:MASTER_NAME)

/*
** A convenience macro that returns the number of elements in
** an array.
*/
#define ArraySize(X)    (sizeof(X)/sizeof(X[0]))


/*
** An instance of the following structure stores a database schema.
*/

/*
** Allowed values for the DB.flags field.
**
** The DB_SchemaLoaded flag is set after the database schema has been
** read into internal hash tables.
**
** DB_UnresetViews means that one or more views have column names that
** have been filled out.  If the schema changes, these column names might
** changes and so the view will need to be reset.
*/
#define DB_SchemaLoaded    0x0001  /* The schema has been loaded */
#define DB_UnresetViews    0x0002  /* Some views have defined column names */
#define DB_Empty           0x0004  /* The file is empty (length 0 bytes) */

#define SQLITE_UTF16NATIVE (SQLITE_BIGENDIAN?SQLITE_UTF16BE:SQLITE_UTF16LE)




/*
** A macro to discover the encoding of a database.
*/
#define ENC(db) ((db)->aDb[0].pSchema->enc)

/*
** Possible values for the sqlite.flags and or Db.flags fields.
**
** On sqlite.flags, the SQLITE_InTrans value means that we have
** executed a BEGIN.  On Db.flags, SQLITE_InTrans means a statement
** transaction is active on that particular database file.
*/
#define SQLITE_VdbeTrace      0x00000001  /* True to trace VDBE execution */
#define SQLITE_InTrans        0x00000008  /* True if in a transaction */
#define SQLITE_InternChanges  0x00000010  /* Uncommitted Hash table changes */
#define SQLITE_FullColNames   0x00000020  /* Show full column names on SELECT */
#define SQLITE_ShortColNames  0x00000040  /* Show short columns names */
#define SQLITE_CountRows      0x00000080  /* Count rows changed by INSERT, */
                                          /*   DELETE, or UPDATE and return */
                                          /*   the count using a callback. */
#define SQLITE_NullCallback   0x00000100  /* Invoke the callback once if the */
                                          /*   result set is empty */
#define SQLITE_SqlTrace       0x00000200  /* Debug print SQL as it executes */
#define SQLITE_VdbeListing    0x00000400  /* Debug listings of VDBE programs */
#define SQLITE_WriteSchema    0x00000800  /* OK to update SQLITE_MASTER */
#define SQLITE_NoReadlock     0x00001000  /* Readlocks are omitted when 
                                          ** accessing read-only databases */
#define SQLITE_IgnoreChecks   0x00002000  /* Do not enforce check constraints */
#define SQLITE_ReadUncommitted 0x00004000 /* For shared-cache mode */
#define SQLITE_LegacyFileFmt  0x00008000  /* Create new databases in format 1 */
#define SQLITE_FullFSync      0x00010000  /* Use full fsync on the backend */
#define SQLITE_LoadExtension  0x00020000  /* Enable load_extension */

/*
** Possible values for the sqlite.magic field.
** The numbers are obtained at random and have no special meaning, other
** than being distinct from one another.
*/
#define SQLITE_MAGIC_OPEN     0xa029a697  /* Database is open */
#define SQLITE_MAGIC_CLOSED   0x9f3c2d33  /* Database is closed */
#define SQLITE_MAGIC_BUSY     0xf03b7906  /* Database currently in use */
#define SQLITE_MAGIC_ERROR    0xb5357930  /* An SQLITE_MISUSE error occurred */

/*
** Possible values for FuncDef.flags
*/
#define SQLITE_FUNC_LIKE   0x01  /* Candidate for the LIKE optimization */
#define SQLITE_FUNC_CASE   0x02  /* Case-sensitive LIKE-type function */
#define SQLITE_FUNC_EPHEM  0x04  /* Ephermeral.  Delete with VDBE */

/*
** A "Collating Sequence" is defined by an instance of the following
** structure. Conceptually, a collating sequence consists of a name and
** a comparison routine that defines the order of that sequence.
**
** There may two seperate implementations of the collation function, one
** that processes text in UTF-8 encoding (CollSeq.xCmp) and another that
** processes text encoded in UTF-16 (CollSeq.xCmp16), using the machine
** native byte order. When a collation sequence is invoked, SQLite selects
** the version that will require the least expensive encoding
** translations, if any.
**
** The CollSeq.pUser member variable is an extra parameter that passed in
** as the first argument to the UTF-8 comparison function, xCmp.
** CollSeq.pUser16 is the equivalent for the UTF-16 comparison function,
** xCmp16.
**
** If both CollSeq.xCmp and CollSeq.xCmp16 are NULL, it means that the
** collating sequence is undefined.  Indices built on an undefined
** collating sequence may not be read or written.
*/
struct CollSeq {
  char *zName;         /* Name of the collating sequence, UTF-8 encoded */
  u8 enc;              /* Text encoding handled by xCmp() */
  u8 type;             /* One of the SQLITE_COLL_... values below */
  void *pUser;         /* First argument to xCmp() */
  int (*xCmp)(void*,int, const void*, int, const void*);
};

/*
** Allowed values of CollSeq flags:
*/
#define SQLITE_COLL_BINARY  1  /* The default memcmp() collating sequence */
#define SQLITE_COLL_NOCASE  2  /* The built-in NOCASE collating sequence */
#define SQLITE_COLL_REVERSE 3  /* The built-in REVERSE collating sequence */
#define SQLITE_COLL_USER    0  /* Any other user-defined collating sequence */

/*
** A sort order can be either ASC or DESC.
*/
#define SQLITE_SO_ASC       0  /* Sort in ascending order */
#define SQLITE_SO_DESC      1  /* Sort in ascending order */

/*
** Column affinity types.
**
** These used to have mnemonic name like 'i' for SQLITE_AFF_INTEGER and
** 't' for SQLITE_AFF_TEXT.  But we can save a little space and improve
** the speed a little by number the values consecutively.  
**
** But rather than start with 0 or 1, we begin with 'a'.  That way,
** when multiple affinity types are concatenated into a string and
** used as the P3 operand, they will be more readable.
**
** Note also that the numeric types are grouped together so that testing
** for a numeric type is a single comparison.
*/
#define SQLITE_AFF_TEXT     'a'
#define SQLITE_AFF_NONE     'b'
#define SQLITE_AFF_NUMERIC  'c'
#define SQLITE_AFF_INTEGER  'd'
#define SQLITE_AFF_REAL     'e'

#define sqlite3IsNumericAffinity(X)  ((X)>=SQLITE_AFF_NUMERIC)

/*
** Test to see whether or not a table is a virtual table.  This is
** done as a macro so that it will be optimized out when virtual
** table support is omitted from the build.
*/
#ifndef SQLITE_OMIT_VIRTUALTABLE
#  define IsVirtual(X) ((X)->isVirtual)
#else
#  define IsVirtual(X) 0
#endif

/*
** SQLite supports many different ways to resolve a contraint
** error.  ROLLBACK processing means that a constraint violation
** causes the operation in process to fail and for the current transaction
** to be rolled back.  ABORT processing means the operation in process
** fails and any prior changes from that one operation are backed out,
** but the transaction is not rolled back.  FAIL processing means that
** the operation in progress stops and returns an error code.  But prior
** changes due to the same operation are not backed out and no rollback
** occurs.  IGNORE means that the particular row that caused the constraint
** error is not inserted or updated.  Processing continues and no error
** is returned.  REPLACE means that preexisting database rows that caused
** a UNIQUE constraint violation are removed so that the new insert or
** update can proceed.  Processing continues and no error is reported.
**
** RESTRICT, SETNULL, and CASCADE actions apply only to foreign keys.
** RESTRICT is the same as ABORT for IMMEDIATE foreign keys and the
** same as ROLLBACK for DEFERRED keys.  SETNULL means that the foreign
** key is set to NULL.  CASCADE means that a DELETE or UPDATE of the
** referenced table row is propagated into the row that holds the
** foreign key.
** 
** The following symbolic values are used to record which type
** of action to take.
*/
#define OE_None     0   /* There is no constraint to check */
#define OE_Rollback 1   /* Fail the operation and rollback the transaction */
#define OE_Abort    2   /* Back out changes but do no rollback transaction */
#define OE_Fail     3   /* Stop the operation but leave all prior changes */
#define OE_Ignore   4   /* Ignore the error. Do not do the INSERT or UPDATE */
#define OE_Replace  5   /* Delete existing record, then do INSERT or UPDATE */

#define OE_Restrict 6   /* OE_Abort for IMMEDIATE, OE_Rollback for DEFERRED */
#define OE_SetNull  7   /* Set the foreign key value to NULL */
#define OE_SetDflt  8   /* Set the foreign key value to its default */
#define OE_Cascade  9   /* Cascade the changes */

#define OE_Default  99  /* Do whatever the default action is */




/*
** These macros can be used to test, set, or clear bits in the 
** Expr.flags field.
*/
#define ExprHasProperty(E,P)     (((E)->flags&(P))==(P))
#define ExprHasAnyProperty(E,P)  (((E)->flags&(P))!=0)
#define ExprSetProperty(E,P)     (E)->flags|=(P)
#define ExprClearProperty(E,P)   (E)->flags&=~(P)

/*
** The bitmask datatype defined below is used for various optimizations.
*/
typedef unsigned int Bitmask;

/*
** The results of a select can be distributed in several ways.
*/
#define SRT_Union        1  /* Store result as keys in an index */
#define SRT_Except       2  /* Remove result from a UNION index */
#define SRT_Discard      3  /* Do not save the results anywhere */

/* The ORDER BY clause is ignored for all of the above */
#define IgnorableOrderby(X) (X<=SRT_Discard)

#define SRT_Callback     4  /* Invoke a callback with each row of result */
#define SRT_Mem          5  /* Store result in a memory cell */
#define SRT_Set          6  /* Store non-null results as keys in an index */
#define SRT_Table        7  /* Store result as data with an automatic rowid */
#define SRT_EphemTab     8  /* Create transient tab and store like SRT_Table */
#define SRT_Subroutine   9  /* Call a subroutine to handle results */
#define SRT_Exists      10  /* Store 1 if the result is not empty */

#ifdef SQLITE_OMIT_VIRTUALTABLE
  #define IN_DECLARE_VTAB 0
#else
  #define IN_DECLARE_VTAB (pParse->declareVtab)
#endif

/*
** Bitfield flags for P2 value in OP_Insert and OP_Delete
*/
#define OPFLAG_NCHANGE   1    /* Set to update db->nChange */
#define OPFLAG_LASTROWID 2    /* Set to update db->lastRowid */
#define OPFLAG_ISUPDATE  4    /* This OP_Insert is an sql UPDATE */

/*
** The SQLITE_CORRUPT_BKPT macro can be either a constant (for production
** builds) or a function call (for debugging).  If it is a function call,
** it allows the operator to set a breakpoint at the spot where database
** corruption is first detected.
*/
#ifdef SQLITE_DEBUG
  extern int sqlite3Corrupt(void);
# define SQLITE_CORRUPT_BKPT sqlite3Corrupt()
#else
# define SQLITE_CORRUPT_BKPT SQLITE_CORRUPT
#endif


int sqlite3StrICmp(const char *, const char *);
int sqlite3StrNICmp(const char *, const char *, int);
int sqlite3IsNumber(const char*, int*, u8);

void *sqlite3Malloc(int,int);
void *sqlite3MallocRaw(int,int);
void sqlite3Free(void*);
void *sqlite3Realloc(void*,int);
char *sqlite3StrDup(const char*);
char *sqlite3StrNDup(const char*, int);
# define sqlite3CheckMemory(a,b)
void sqlite3ReallocOrFree(void**,int);
void sqlite3FreeX(void*);
void *sqlite3MallocX(int);
int sqlite3AllocSize(void *);

char *sqlite3MPrintf(const char*, ...);
char *sqlite3VMPrintf(const char*, va_list);
void sqlite3DebugPrintf(const char*, ...);
void *sqlite3TextToPtr(const char*);
void sqlite3SetString(char **, ...);
int sqlite3KeywordCode(const unsigned char*, int);
int sqlite3InitCallback(void*, int, char**, char**);

int sqlite3ArrayAllocate(void**,int,int);
void sqlite3Randomness(int, void*);

int sqlite3AtoF(const char *z, double*);
char *sqlite3_snprintf(int,char*,const char*,...);
int sqlite3GetInt32(const char *, int*);
int sqlite3FitsIn64Bits(const char *);
int sqlite3utf16ByteLen(const void *pData, int nChar);
int sqlite3utf8CharLen(const char *pData, int nByte);
int sqlite3ReadUtf8(const unsigned char *);
int sqlite3PutVarint(unsigned char *, u64);
int sqlite3GetVarint(const unsigned char *, u64 *);
int sqlite3GetVarint32(const unsigned char *, u32 *);
int sqlite3VarintLen(u64 v);
int sqlite3atoi64(const char*, i64*);
void *sqlite3HexToBlob(const char *z);
const char *sqlite3ErrStr(int);
int sqlite3ReadUniChar(const char *zStr, int *pOffset, u8 *pEnc, int fold);

char *sqlite3utf16to8(const void*, int);
int sqlite3GetToken(const unsigned char *, int *);
const char *sqlite3TestErrorName(int);
void sqlite3SchemaFree(void *);
int sqlite3MallocFailed(void);
void sqlite3FailedMalloc(void);

void *sqlite3_realloc(void *pOld, int nByte);


#ifdef SQLITE_MEMDEBUG
  void sqlite3MallocDisallow(void);
  void sqlite3MallocAllow(void);
  int sqlite3TestMallocFail(void);
#else
  #define sqlite3TestMallocFail() 0
  #define sqlite3MallocDisallow()
  #define sqlite3MallocAllow()
#endif

#endif


#ifdef __cplusplus
}
#endif

