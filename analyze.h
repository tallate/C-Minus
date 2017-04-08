/************************************************/
/* File: analyze.h                              */
/* Semantic analyzer interface for C- compiler  */
/* tallate                                      */
/************************************************/

#include "globals.h"

#ifndef _ANALYZE_H
#define _ANALYZE_H



/* analyze 语义分析，包括符号表构建和类型检查
 */
void analyze(TreeNode *);

#endif // _ANALYZE_H
