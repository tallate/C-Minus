/********************************************/
/* File: util.h                             */
/* Utility functions for the C- compiler    */
/* tallate                                  */
/********************************************/

#ifndef _UTIL_H_
#define _UTIL_H_

#include "globals.h"

/* pringToken: 打印一个token和其lexeme到listing file */
void printToken(TokenType, const char*);

/* newDeclNode: 分配一个syntax tree的新declaration node */
TreeNode *newDeclNode(DeclKind);

/* newStmtNode: 分配一个syntax tree的新statement node */
TreeNode *newStmtNode(StmtKind);

/* newExpNode: 分配一个syntax tree的新expression node */
TreeNode *newExpNode(ExpKind);

/* copyString: 分配一个已有字符串的拷贝 */
char *copyString(char*);

/* printTree: 打印syntax tree到listing file，使用缩进来表示子树 */
void printTree(TreeNode*);

#endif
