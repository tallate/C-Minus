/************************************************/
/* File: scan.h                                 */
/* The scanner interface for the C- compiler    */
/* tallate                                      */
/************************************************/

#ifndef _SCAN_H_
#define _SCAN_H_

#include "globals.h"

/* token的最大长度 */
#define MAXTOKENLEN 40

/* 存储每个token的lexeme */
extern char tokenString[MAXTOKENLEN + 1];

/* 返回source中下一个token */
TokenType getToken(void);

#endif
