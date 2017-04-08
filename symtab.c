/********************************************************/
/* File: symtab.c                                       */
/* Symbol table implementationfor the C- compiler       */
/* symbol table is implemented as a chained hash table  */
/* tallate                                              */
/********************************************************/

#include "globals.h"
#include "symtab.h"

SymbolTable *symbolTable;

/* 删除LineList链表
 */
void deleteLineList(LineList *list) {
    LineList *next = NULL;
    while(list) {
        next = list->next;
        free(list);
        list = next;
    }
}
/* 删除符号表的一个篮子
 */
void deleteBucketList(BucketList *list) {
    BucketList *next = NULL;
    while(list) {
        next = list->next;
        free(list->name);
        deleteLineList(list->lines);
        list = next;
    }
}

/* hash function
 */
static int hash(char *key) {
    int temp = 0;
    int i;
    for(i = 0; key[i] != '\0'; i++) {
        temp = ((temp << SHIFT) + key[i]) % SIZE;
    }
    return temp;
}

/* st_insert 将行号和内存地址插入符号表(loc = memory location)
 * (先在当前scope查找标志符，若未找到则插入并返回1，否则忽略并返回0)
 * 直接插入好了，至于重复声明的问题就交给type checker好了
 */
void st_insert(char *name, TreeNode *dec, int lineno, int loc) {
    int h = hash(name);
    BucketList *l = (BucketList*) malloc(sizeof(BucketList));
    l->name = name;
    l->lines = (LineList*) malloc(sizeof(LineList));
    l->nestLevel = symbolTable->nestLevel; /* 符号表的每一项的嵌套层次 */
    l->decl = dec;
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    /* 插入到表头 */
    l->next = symbolTable->hashTable[h];
    symbolTable->hashTable[h] = l;
}

/* st_lookup 返回变量的内存地址
 * 从当前scope向外查找标志符，若找到则返回其地址，否则返回-1
 * 顶多查找depth层
 */
BucketList *st_lookup(char *name, int depth) {
    int h = hash(name);
    SymbolTable *currentScope = symbolTable;
    /* 从内层scope向外层查找 */
    while(currentScope && depth--) {
        BucketList *l = currentScope->hashTable[h];
        while(l) {
            if(strcmp(name, l->name) == 0) {
                return l;
            }
            l = l->next;
        }
        currentScope = currentScope->next;
    }
    return NULL;
}

void st_augment() {
    SymbolTable *newScope = (SymbolTable*) malloc(sizeof(SymbolTable));
    memset(newScope->hashTable, 0, sizeof(newScope->hashTable));
    newScope->location = newScope->maxOffset = 0;
    newScope->err = 0;
    newScope->next = symbolTable;
    symbolTable = newScope;
    if(symbolTable->next) {
        symbolTable->nestLevel = symbolTable->next->nestLevel + 1;
    } else {
        symbolTable->nestLevel = 0;
    }
}

SymbolTable *st_abrade() {
    SymbolTable *oldScope = symbolTable;
    symbolTable = oldScope->next;
    oldScope->next = NULL;
    return oldScope;
}

void printSymTab(FILE *listing) {
    for(SymbolTable *table = symbolTable; table; table = table->next) {
        printf("nestLevel: %d\n", table->nestLevel);
        fprintf(listing, "Variable Name   Location   Line Numbers\n");
        fprintf(listing, "-------------   --------   ------------\n");
        for(int i = 0; i < SIZE; i++) {
            BucketList *l = table->hashTable[i];
            /* 打印符号表的每一项 */
            while(l) {
                LineList *lineList = l->lines;
                fprintf(listing, "%-14s   ", l->name);
                fprintf(listing, "%-8d   ", l->memloc);
                while(lineList) {
                    fprintf(listing, "%4d ", lineList->lineno);
                    lineList = lineList->next;
                }
                fprintf(listing, "\n");
                l = l->next;
            }
        }
        printf("\n");
    }
}
