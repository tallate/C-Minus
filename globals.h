/************************************************/
/* File: globals.h                              */
/* Global types and vars for C- compiler        */
/* must come before other include files         */
/* tallate                                      */
/************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = 保留字的数量 */
#define MAXRESERVED 6

/* 代表每个token的类型 */
typedef enum{
    /* book-keeping tokens */
    ENDFILE, ERROR,
    /* reserved words */
    ELSE, IF, INT, RETURN, VOID, WHILE,
    /* special symbols */
    PLUS, MINUS, TIMES, OVER,
    LT, LTET, MT, MTET, EQ, NEQ, ASIGN, /* ASIGN对应= EQ对应== */
    SEMI, COMM, /* ; , */
    LPAREN, RPAREN, LSPAREN, RSPAREN, LCPAREN, RCPAREN, // parenthesis
    /* others */
    ID, NUM
    /* 注释符没有列出来，因为是被忽略的，根本没有用 */
} TokenType;

extern FILE *source; /* 指向输入文件 */
extern FILE *listing; /* 输出文件 */
extern FILE *code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */


/*****************************************************/
/************* syntax tree for parsing ***************/
/*****************************************************/

/* 节点有三大类 */
typedef enum { DeclK, StmtK, ExpK } NodeKind;
/* 声明有两种 */
typedef enum { VarD, ArrD, FunD } DeclKind;
/* 语句有五种 */
typedef enum { ExpS, ComS, SelS, IteS, RetS } StmtKind;
/* 表达式有五种 */
typedef enum { AssignE, OpE, ConstE, VarE, ArrE, CallE } ExpKind;

/* 声明的标识符或表达式的类型，用于type checking */
typedef enum { Void, Integer, Boolean } ExpType;

/* 每个节点最多有3个儿子，出现在if-else语句中 */
#define MAXCHILDREN 3

typedef struct treeNode {
    struct treeNode *child[MAXCHILDREN];
    struct treeNode *sibling;

    /* 节点类型 */
    NodeKind nodeKind;
    union { DeclKind decl; StmtKind stmt; ExpKind exp; } kind;

    /* 声明或表达式的属性 */
    union {
        TokenType op; /* operator-exp */
        int val; /* 常量表达式、数组表达式（或声明）的下标 */
        char *name; /* 标志符表达式、声明 */
    } attr;

    /* 在每个新层次的起始处（子树根）保存对应的scope */
    struct SymbolTable *scope;
    /* 函数声明给出的函数返回值类型 */
    ExpType funcReturnType;
    /* 变量声明中给出的类型 */
    ExpType varType;
    /* 表达式类型 */
    ExpType expType;

    /* type checking记录，记录一个变量对应的声明节点 */
    // struct treeNode *decl;
    /* 函数的入口代码地址 */
    int funcEntryLoc;

    /* 根出现位置 */
    int lineno;
} TreeNode;

/***********************/
/* Flags for tracing */
/***********************/

/* 当EchoSource = TRUE，
 * 在parsing时源程序会回显到listing文件中并带上行号
 */
extern int EchoSource;

/* 当TraceScan = TRUE，
 * token的信息会和识别的token一起显示到listing文件中
 */
extern int TraceScan;

/* 当TraceParse = TRUE，
 * syntax tree将会打印到listing文件（适当的缩进）
 */
extern int TraceParse;

/* 当TraceAnalyze = TRUE，
 * symbol table的inserts和lookups将会在listing文件中显示
 */
extern int TraceAnalyze;

/* 当TraceCode = TRUE，
 * 生成代码时注释将会被写入TM code file中
 */
extern int TraceCode;

/* 当Error = TRUE，当error出现时阻止之后的passes */
extern int Error;

#endif
