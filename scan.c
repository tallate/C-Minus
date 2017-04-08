/**************************************************/
/* File: scan.c                                   */
/* the scanner implementation for the C- compiler */
/* tallate                                        */
/**************************************************/

#include "scan.h"
#include "util.h"

/* state table */
typedef enum {
    /* start accepting state */
    START, DONE,
    /* compare asign state */
    INLESS, INMORE, INASIGN, INNEQ,
    /* comment */
    INSLASH, INCOMMENT, INASTER,
    /* others */
    INID, INNUM,
} StateType;

/* reserved table */
static struct {
    char *str;
    TokenType tok;
} reservedWords[MAXRESERVED]
= { { "else", ELSE }, { "if", IF },
{ "int", INT }, { "return", RETURN },
{ "void", VOID }, { "while", WHILE } };

/* identifier或reserved word的lexeme */
char tokenString[MAXTOKENLEN + 1];

/* 读取下一个字符
 * 1.若缓冲区为空，则填充一次缓冲区，
 * 2.不为空，currentPos++，并
 */
#define MAXBUFSIZ 256
char buf[MAXBUFSIZ];
int curPos = 0;
int curSize = 0;
/* 返回buf中下一个非空字符，若buf耗尽则重新读入一行 */
static char getNextChar(void){
    if(!(curPos < curSize)){
        /* 新读入一行 */
        lineno++;
        if(fgets(buf, MAXBUFSIZ - 1, source)) {
            if(EchoSource) {
                fprintf(listing, "%4d: %s", lineno, buf);
            }
            curSize = strlen(buf);
            curPos = 0;
            return buf[curPos++];
        }
        /* 文件尾或发生错误 */
        return EOF;
    }
    return buf[curPos++];
}
/* 将一个字符回退到buf */
static void ungetNextChar(void){
    /* 会不会发生curPos此时为0的情况？不会 */
    curPos--;
}

/* 当token为ID时，在reservedWords表中找它是否为某保留字 */
TokenType reservedLookup(char *str){
    int pos;
    for(pos = 0; pos < MAXRESERVED; pos++){
        if(strcmp(reservedWords[pos].str, str) == 0){
            return reservedWords[pos].tok;
        }
    }
    // 不是保留字，默认为还是ID
    return ID;
}

/* 若读到( = !=等特殊符号，要通过这个函数来记录，目的是为了简化 */
void recordSpecialSymbol(char *tokenString, TokenType token){
    switch(token){
    case PLUS: sprintf(tokenString, "%s", "+"); break;
    case MINUS: sprintf(tokenString, "%s", "-"); break;
    case TIMES: sprintf(tokenString, "%s", "*"); break;
    case OVER: sprintf(tokenString, "%s", "/"); break;
    case LT: sprintf(tokenString, "%s", "<"); break;
    case LTET: sprintf(tokenString, "%s", "<="); break;
    case MT: sprintf(tokenString, "%s", ">"); break;
    case MTET: sprintf(tokenString, "%s", ">="); break;
    case EQ: sprintf(tokenString, "%s", "=="); break;
    case NEQ: sprintf(tokenString, "%s", "!="); break;
    case ASIGN: sprintf(tokenString, "%s", "="); break;
    case SEMI: sprintf(tokenString, "%s", ";"); break;
    case COMM: sprintf(tokenString, "%s", ","); break;
    case LPAREN: sprintf(tokenString, "%s", "("); break;
    case RPAREN: sprintf(tokenString, "%s", ")"); break;
    case LSPAREN: sprintf(tokenString, "%s", "["); break;
    case RSPAREN: sprintf(tokenString, "%s", "]"); break;
    case LCPAREN: sprintf(tokenString, "%s", "{"); break;
    case RCPAREN: sprintf(tokenString, "%s", "}"); break;
    }
}

/* 返回下一个token的类型 */
TokenType getToken(void){
    /* 最终返回值 */
    TokenType currentToken;
    /* DFA总是从START开始 */
    StateType state = START;
    /* tokenString中下一个字符要存储的位置 */
    int tokenStringIndex = 0;
    /* 每次用getNextChar()读取一个字符 */
    char nextc;

    while(state != DONE){
        nextc = getNextChar();
        
        switch(state){
        case START:
            if(isalpha(nextc)){
                /* 保存到tokenString */
                state = INID;
                break;
            } 
            else if(isdigit(nextc)){
                /* 保存到tokenString */
                state = INNUM;
                break;
            }
            /* 读到space的情况，忽略 */
            else if((nextc == ' ') || (nextc == '\t') || (nextc == '\n')){
                break;
            }

            switch(nextc){
            /* 读到文件尾 */
            case EOF: currentToken = ENDFILE; state = DONE; break;
            /* 确定单字符 */
            case '(': currentToken = LPAREN; state = DONE; break;
            case ')': currentToken = RPAREN; state = DONE; break;
            case '[': currentToken = LSPAREN; state = DONE; break;
            case ']': currentToken = RSPAREN; state = DONE; break;
            case '{': currentToken = LCPAREN; state = DONE; break;
            case '}': currentToken = RCPAREN; state = DONE; break;
            case '+': currentToken = PLUS; state = DONE; break;
            case '-': currentToken = MINUS; state = DONE; break;
            case '*': currentToken = TIMES; state = DONE; break;
            case ';': currentToken = SEMI; state = DONE; break;
            case ',': currentToken = COMM; state = DONE; break;
            /* 非确定 */
            case '<': currentToken = LT; state = INLESS; break;
            case '>': currentToken = MT; state = INMORE; break;
            case '=': currentToken = ASIGN; state = INASIGN; break;
            case '/': currentToken = OVER; state = INSLASH; break;
            /* 特殊 */
            case '!': state = INNEQ; break;
            /* 无法识别的字符 */
            default: currentToken = ERROR; state = DONE; break;
            }
            break;
        case INLESS:
            if(nextc == '=') {
                currentToken = LTET;
            } else {
                ungetNextChar();
            }
            state = DONE;
            break;
        case INMORE:
            if(nextc == '=') {
                currentToken = MTET;
            } else {
                ungetNextChar();
            }
            state = DONE;
            break;
        case INASIGN:
            if(nextc == '=') {
                currentToken = EQ;
            } else {
                ungetNextChar();
            }
            state = DONE;
            break;
        case INNEQ:
            if(nextc == '=') {
                currentToken = NEQ;
            } else {
                ungetNextChar();
                currentToken = ERROR;
            }
            state = DONE;
            break;
        /* 注释部分 */
        case INSLASH:
            if(nextc == '*') {
                state = INCOMMENT;
            } else {
                ungetNextChar();
                state = DONE;
            }
            break;
        case INCOMMENT:
            if(nextc == '*') {
                state = INASTER;
            }
            break;
        case INASTER:
            if(nextc == '/') {
                state = START;
            } else {
                state = INCOMMENT;
            }
            break;
        case INID:
            if(!isalpha(nextc)) {
                ungetNextChar();
                currentToken = ID;
                state = DONE;
            }
            break;
        case INNUM:
            if(!isdigit(nextc)) {
                ungetNextChar();
                currentToken = NUM;
                state = DONE;
            }
            break;
        case DONE:
        default:/* 不可能发生的情况 */
            fprintf(stderr, "Scanner Bug: state = %d\n", state);
            currentToken = ERROR;
            state = DONE;
            break;
        }

        if((state == INID || state == INNUM) && tokenStringIndex <= MAXTOKENLEN) {
            /* 状态机循环读入id或num */
            tokenString[tokenStringIndex++] = nextc;
        }
        /* 不管token是不是ID这里都没有错 */
        if(state == DONE) {
            if(currentToken != ID && currentToken != NUM) {
                recordSpecialSymbol(tokenString, currentToken);
            }
            tokenString[tokenStringIndex] = '\0';
            if(currentToken == ID){
                currentToken = reservedLookup(tokenString);
            }
        }
    }
    if(TraceScan) {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
}
