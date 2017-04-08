/************************************/
/* File: code.h                     */
/* Code emitting utilities for the  */
/* C- compiler and interface to the */
/* TM machine                       */
/* tallate                          */
/************************************/

#ifndef _CODE_H
#define _CODE_H

/* 保存当前指令的寄存器 */
#define pc 7
#define gp 6
#define fp 5
#define tp 4
#define ac 0
#define ac1 1

/* emitRO emits a register-only TM instruction
 * op = opcode
 * r = target register
 * s,t = source register
 * c = print if TraceCode is TRUE
 */
void emitRO(char *op, int r, int s, int t, char *c);

/* emitRM emits a register-memory TM instruction
 * op = opcode
 * r = target register
 * d = offset
 * s = base register
 * c = print if TraceCode is TRUE
 */
void emitRM(char *op, int r, int d, int s, char *c);

/* emitSkip 跳过howMany条指令
 * 返回当前指令地址
 */
int emitSkip(int howMany);

/* emitBackup 倒退到之前跳过的一条指令地址
 */
void emitBackup(int loc);

/* emitRestore 恢复
 */
void emitRestore(void);

/* emitComment 打印一行注释
 */
void emitComment(char *c);

/* emitRM_Abs 当emit一条RM指令时将绝对引用转换为相对pc的引用
 * op = opcode
 * r = target register
 * a = absolute location in memory
 * c = print if TraceCode is TRUE
 */
void emitRM_Abs(char *op, int r, int a, char *c);

#endif //_CODE_H