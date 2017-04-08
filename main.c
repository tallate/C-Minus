/********************************************/
/* File: main.c                             */
/* Main program for C- compiler             */
/* tallate                                  */
/********************************************/

#include "globals.h"

/* set TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE
/* set TRUE to get a compiler without codegen */
#define NO_CODE FALSE

#include "util.h"
#include "scan.h"
#if !NO_PARSE
#include "parse.h"
#include "analyze.h"

#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* 分配全局变量 */
int lineno = 0;
FILE *source;
FILE *listing;
FILE *code;

/* 分配并初始化tracing flags */
int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

int main(int argc, char *argv[]) {
    TreeNode *syntaxTree;
    char pgm[20]; /* source code file name */
    if(argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    /* 输入文件 */
    strcpy(pgm, argv[1]);
    if(strchr(pgm, '.') == NULL) {
        strcat(pgm, ".c");
    }
    source = fopen(pgm, "r");
    if(source == NULL) {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }
    /* listing到屏幕 */
    listing = stdout;
    fprintf(listing, "\nC- COMPILATION: %s\n", pgm);
#if NO_PARSE
    while(getToken() != ENDFILE);
#else
    syntaxTree = parse();
    if(TraceParse) {
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }
#if !NO_ANALYZE
    if(! Error) {
        fprintf(listing, "\nBuilding Symbol Table and Checking Types...\n");
        analyze(syntaxTree);
        fprintf(listing, "\nBuild Symbol Table and Check Types Finished\n");
    }
#if !NO_CODE
    if(! Error) {
        char *codeFile;
        int fnlen = strcspn(pgm, ".");
        codeFile = (char*) calloc(fnlen + 4, sizeof(char));
        strncpy(codeFile, pgm, fnlen);
        strcat(codeFile, ".tm");
        code = fopen(codeFile, "w");
        if(code == NULL) {
            printf("Unable to open %s\n", codeFile);
            exit(1);
        }
        codeGen(syntaxTree, codeFile);
        fclose(code);
        free(codeFile); /* 不释放也没关系，因为程序都要退出了 */
    }
#endif
#endif
#endif
    return 0;
}


