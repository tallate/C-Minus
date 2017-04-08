/************************************/
/* File: cgen.h                     */
/* code generator interface to the  */
/* C- compiler                      */
/* tallate                          */
/************************************/

#ifndef _CGEN_H
#define _CGEN_H

#include "globals.h"

/* codeGen 遍历syntaxTree，生成目标代码到codeFile
 */
void codeGen(TreeNode *syntaxTree, char *codeFile);

#endif //_CGEN_H
