/************************************************/
/* File: symtab.h                               */
/* Symbol table interface for the C- compiler   */
/* tallate                                      */
/************************************************/

#ifndef _SYMTAB_H
#define _SYMTAB_H

#include <stdio.h>
#include "globals.h"

/* SIZE 为hash table的大小
 */
#define SIZE 211

/* SHIFT 为hash function中两项间的乘数
 */
#define SHIFT 4


/* LineList 记录变量被引用的行号
 */
typedef struct LineListRec {
    int lineno;
    struct LineListRec *next;
} LineList;

/* BucketList 记录每个变量
 * 包括其id、所处嵌套层次、声明的位置、出现行号、内存位置
 */
typedef struct BucketListRec {
    char *name;
    LineList *lines;
    int nestLevel;
    TreeNode *decl;
    int memloc;
    struct BucketListRec *next;
} BucketList;

/* SeparateTable 代表每个声明scope
 */
typedef struct SymbolTable {
    BucketList *hashTable[SIZE];
    /* 该scope中分配的相对地址， */
    int location;
    int maxOffset; /* 考虑复合语句的嵌套，最多的变量偏移 */
    /* 嵌套层次 */
    int nestLevel;
    int err;
    struct SymbolTable *next;
} SymbolTable;

/* 多scope符号表，在symbol.c中定义 */
extern SymbolTable *symbolTable;

/* st_insert 将行号和内存地址插入符号表(loc = memory location)
 * 只有第一次有效，之后的忽略（？）
 */
void st_insert(char *name, TreeNode *dec, int lineno, int loc);

/* st_lookup 返回变量对应的BucketList
 * 若找到则返回其指针
 * 若scopes用完或者depth用完则终止，并返回NULL（depth赋值为-1表示搜索所有scope）
 */
BucketList *st_lookup(char *name, int depth);

/* st_augment 在原symbolTable的基础上增加一个scope
 */
void st_augment();

/* st_abrade 提取一个最近的scope并返回
 */
SymbolTable *st_abrade();

/* printSymTab 打印格式化的符号表到listing
 * 打印当前scope的符号表
 */
void printSymTab(FILE *listing);

#endif // _SYMTAB_H
