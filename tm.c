
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/******* const *******/
#define IADDR_SIZE 1024
#define DADDR_SIZE 1024
#define NO_REGS 8
#define PC_REG 7

#define LINESIZE 121
#define WORDSIZE 20

/******* type *******/
typedef enum {
    opclRR,     /* reg operands r, s, t */
    opclRM,     /* reg r, mem d + s */
    opclRA      /* reg r, int d + s */
} OPCLASS;

typedef enum {
    /* RR instructions */
    opHALT,     /* RR   halt, operands must be zero */
    opIN,       /* RR   read into reg(r); s and r are ignored */
    opOUT,      /* RR   write from reg(r), s and t are ignored */
    opADD,      /* RR   reg(r) = reg(s) + reg(t) */
    opSUB,      /* RR   reg(r) = reg(s) - reg(t) */
    opMUL,      /* RR   reg(r) = reg(s) * reg(t) */
    opDIV,      /* RR   reg(r) = reg(s) / reg(t) */
    opRRLim,    /* limit of RR opcodes */

    /* RM instructions */
    opLD,       /* RM   reg(r) = mem(d + reg(s)) */
    opST,       /* RM   mem(d + reg(s)) = reg(r) */
    opRMLim,    /* limit of RM opcodes */

    /* RA instructions */
    opLDA,      /* RA   reg(r) = d + reg(s) */
    opLDC,      /* RA   reg(r) = d; reg(s) is ignored */
    opJLT,      /* RA   if reg(r) < 0 then reg(7) = d + reg(s) */
    opJLE,      /* RA   if reg(r) <= 0 ... */
    opJGT,      /* RA   if reg(r) > 0 ... */
    opJGE,      /* RA   if reg(r) >= 0 ... */
    opJEQ,      /* RA   if reg(r) == 0 ... */
    opJNE,      /* RA   if reg(r) != 0 ... */
    opRALim,    /* limit of RA opcodes */
} OPCODE;

typedef enum {
    srOKAY,
    srHALT,
    srIMEM_ERR,
    srDMEM_ERR,
    srZERODIVIDE
} STEPRESULT;

typedef struct {
    int iop;
    int iarg1;
    int iarg2;
    int iarg3;
} INSTRUCTION;

/******* vars *******/
int iloc = 0;
int dloc = 0;
int icountflag = FALSE;
int traceflag = FALSE;

INSTRUCTION iMem[IADDR_SIZE];
int dMem[DADDR_SIZE];
int reg[NO_REGS];

char *opCodeTab[] =
        { "HALT", "IN", "OUT", "ADD", "SUB", "MUL", "DIV", "????", /* RR opcodes */
          "LD", "ST", "????", /* RM opcodes */
          "LDA", "LDC", "JLT", "JLE", "JGT", "JGE", "JEQ", "JNE", "????" /* RA opcodes */
        };

char *stepResultTab[] =
        { "OK", "Halted",  "Instruction Memory Fault",
          "Data Memory Fault", "Division by 0"
        };

char pgmName[20];
FILE *pgm;

char in_Line[LINESIZE];
int lineLen;
int inCol;
int num;
char word[WORDSIZE];
char ch;
int done;

/*****************************/
int opClass(int c) {
    if(c <= opRRLim) { return opclRR; }
    else if(c < opRMLim) { return opclRM; }
    else { return opclRA; }
} /* opClass */

/*****************************/
void writeInstruction(int loc) {
    printf("%5d: ", loc);
    if((loc >= 0) && (loc < IADDR_SIZE)) {
        printf("%6s%3d,", opCodeTab[iMem[loc].iop], iMem[loc].iarg1);
        switch(opClass(iMem[loc].iop)) {
            case opclRR:
                printf("%1d, %1d", iMem[loc].iarg2, iMem[loc].iarg3);
            case opclRM:
            case opclRA:
                printf("%3d(%1d)", iMem[loc].iarg2, iMem[loc].iarg3);
                break;
            default:
                break;
        }
        printf("\n");
    }
} /* writeInstruction */

/*****************************/
void getCh(void) {
    if(++inCol < lineLen) {
        ch = in_Line[inCol];
    } else {
        ch = ' ';
    }
} /* getCh */

/*****************************/
/* nonBlank 移动到此行下一个非空处
 */
int nonBlank(void) {
    while((inCol < lineLen) &&
            (in_Line[inCol] == ' ' || in_Line[inCol] == '\t')) {
        inCol++;
    }
    if(inCol < lineLen) {
        ch = in_Line[inCol];
        return TRUE;
    } else {
        ch = ' ';
        return FALSE;
    }
} /* nonBlank */

/*****************************/
/* getNum 读取一个数字（如果是一个表达式则计算其值）
 */
int getNum(void) {
    int sign;
    int term;
    int success = FALSE;
    num = 0;
    do {
        sign = 1;
        while(nonBlank() && ((ch == '+') || (ch == '-'))) {
            success = FALSE;
            if(ch == '-') { sign = -sign; }
            getCh();
        }
        term = 0;
        nonBlank();
        while(isdigit(ch)) {
            success = TRUE;
            term = term * 10 + (ch - '0');
            getCh();
        }
        num += term * sign;
    } while(nonBlank() && ((ch == '+') || (ch == '-')));
    return success;
} /* getNum */

/*****************************/
/* getWord 从in_Line读取下一个单词到word
 */
int getWord(void) {
    int isNonEmpty = FALSE;
    int length = 0;
    if(nonBlank()) {
        while(isalnum(ch)) {
            if(length < WORDSIZE - 1) { word[length++] = ch; }
            getCh();
        }
        word[length] = '\0';
        isNonEmpty = (length != 0);
    }
    return isNonEmpty;
} /* getWord */

/*****************************/
int skipCh(char c) {
    int temp = FALSE;
    if(nonBlank() && (ch == c)) {
        getCh();
        temp = TRUE;
    }
    return temp;
} /* skipCh */

/*****************************/
int atEOL(void) {
    return (! nonBlank());
} /* atEOL */

/*****************************/
/* error 打印错误信息
 */
int error(char *msg, int lineno, int instNo) {
    printf("Line %d", lineno);
    if(instNo >= 0) { printf(" (Instruction %d)", instNo); }
    printf("    %s\n", msg);
    return FALSE;
} /* error */

/*****************************/
/* readInstructions 读入所有代码
 */
int readInstructions(void) {

    /* init */
    for(int regNo = 0; regNo < NO_REGS; regNo++) {
        reg[regNo] = 0;
    }
    dMem[0] = DADDR_SIZE - 1;
    for(int loc = 1; loc < DADDR_SIZE; loc++) {
        dMem[loc] = 0;
    }
    for(int loc = 0; loc < IADDR_SIZE; loc++) {
        iMem[loc].iop = opHALT;
        iMem[loc].iarg1 = 0;
        iMem[loc].iarg2 = 0;
        iMem[loc].iarg3 = 0;
    }
    /* 读取指令到文件末尾 */
    int lineNo = 0;
    int loc;
    OPCODE op;
    int arg1, arg2, arg3;
    while(! feof(pgm)) {
        fgets(in_Line, LINESIZE - 2, pgm);
        inCol = 0;
        lineNo++;
        lineLen = strlen(in_Line) - 1;
        if(in_Line[lineLen] == '\n') { in_Line[lineLen] = '\0'; }
        else { in_Line[++lineLen] = '\0'; }
        if(nonBlank() && (in_Line[inCol] != '*')) {
            if(! getNum()) { return error("Bad location", lineNo, -1); }
            loc = num;
            if(loc > IADDR_SIZE) { return error("Location too large", lineNo, loc); }
            if(! skipCh(':')) { return error("Missing colon", lineNo, loc); }
            if(! getWord()) { return error("Missing opcode", lineNo, loc); }
            op = opHALT;
            while((op < opRALim) &&
                    (strncmp(opCodeTab[op], word, 4) != 0)) {
                op++;
            }
            if(strncmp(opCodeTab[op], word, 4) != 0) { return error("Illegal opcode", lineNo, loc); }
            switch(opClass(op)) {
            case opclRR:
            /*********************/
                if((! getNum()) || (num < 0) || (num >= NO_REGS)) {
                    return error("Bad first register", lineNo, loc);
                }
                arg1 = num;
                if(! skipCh(',')) { return error("Missing comma", lineNo, loc); }
                if((! getNum()) || (num < 0) || (num >= NO_REGS)) {
                    return error("Bad second register", lineNo, loc);
                }
                arg2 = num;
                if(! skipCh(',')) { return error("Missing comma", lineNo, loc); }
                if((! getNum()) || (num < 0) || (num >= NO_REGS)) {
                    return error("Bad third register", lineNo, loc);
                }
                arg3 = num;
                break;

            case opclRM:
            case opclRA:
            /*********************/
                if((! getNum()) || (num < 0) || (num >= NO_REGS)) {
                    return error("Bad first register", lineNo, loc);
                }
                arg1 = num;
                if(! skipCh(',')) { return error("Missing comma", lineNo, loc); }
                if(! getNum()) { return error("Bad displacement", lineNo, loc); }
                arg2 = num;
                if(! skipCh('(') && ! skipCh(',')) { return error("Missing LParen", lineNo, loc); }
                if((! getNum()) || (num < 0) || (num >= NO_REGS)) {
                    return error("Bad second register", lineNo, loc);
                }
                arg3 = num;
                break;
            }
            iMem[loc].iop = op;
            iMem[loc].iarg1 = arg1;
            iMem[loc].iarg2 = arg2;
            iMem[loc].iarg3 = arg3;
        }
    }
    return TRUE;
} /* readInstructions */

/*****************************/
/* stepTM 处理一条代码
 */
STEPRESULT stepTM(void) {
    INSTRUCTION currentInstruction;
    int pc;
    int r, s, t, d;
    int ok;

    pc = reg[PC_REG];
    if((pc < 0) || (pc > IADDR_SIZE)) { return srIMEM_ERR; }
    reg[PC_REG] = pc + 1;
    currentInstruction = iMem[pc];
    switch(opClass(currentInstruction.iop)) {
    case opclRR:
    /*************************/
        r = currentInstruction.iarg1;
        s = currentInstruction.iarg2;
        t = currentInstruction.iarg3;
        break;

    case opclRM:
    /*************************/
        r = currentInstruction.iarg1;
        s = currentInstruction.iarg3;
        d = currentInstruction.iarg2 + reg[s];
        if((d < 0) || (d > DADDR_SIZE)) {
            return srDMEM_ERR;
        }
        break;

    case opclRA:
    /*************************/
        r = currentInstruction.iarg1;
        s = currentInstruction.iarg3;
        d = currentInstruction.iarg2 + reg[s];
        break;
    } /* switch */

    switch(currentInstruction.iop) {
    /* RR instructions */
    case opHALT:
        printf("HALT: %1d, %1d, %1d\n", r, s, t);
        return srHALT;
        /* break; */

    case opIN:
        do {
            printf("Enter value for IN instruction: ");
            fflush(stdin);
            fgets(in_Line, LINESIZE, stdin);
            lineLen = strlen(in_Line) - 1;
            inCol = 0;
            ok = getNum();
            if(! ok) { printf("Illegal value\n"); }
            else { reg[r] = num; }
        } while(! ok);
        break;
    case opOUT:
        printf("OUT instruction prints: %d\n", reg[r]);
        break;

    case opADD: reg[r] = reg[s] + reg[t]; break;
    case opSUB: reg[r] = reg[s] - reg[t]; break;
    case opMUL: reg[r] = reg[s] * reg[t]; break;
    case opDIV:
        if(reg[t] != 0) { reg[r] = reg[s] / reg[t]; }
        else { return srZERODIVIDE; }
        break;

    /* RM instructions */
    case opLD: reg[r] = dMem[d]; break;
    case opST: dMem[d] = reg[r]; break;

    /* RA instructions */
    case opLDA: reg[r] = d; break;
    case opLDC: reg[r] = currentInstruction.iarg2; break;
    case opJLT: if(reg[r] < 0) { reg[PC_REG] = d; } break;
    case opJLE: if(reg[r] <= 0) { reg[PC_REG] = d; } break;
    case opJGT: if(reg[r] > 0) { reg[PC_REG] = d; } break;
    case opJGE: if(reg[r] >= 0) { reg[PC_REG] = d; } break;
    case opJEQ: if(reg[r] == 0) { reg[PC_REG] = d; } break;
    case opJNE: if(reg[r] != 0) { reg[PC_REG] = d; } break;

    /* end of legal instructions */
    } /* switch */
    return srOKAY;
} /* stepTM */

/*****************************/
/* doCommand 读取一条命令
 */
int doCommand(void) {
    char cmd;
    int stepcnt = 0;
    int printcnt;
    int stepResult;

    do {
        printf("Enter command: ");
        fflush(stdin);
        fgets(in_Line, LINESIZE, stdin);
        inCol = 0;
    } while(! getWord());
    /* 去掉末尾读入的\n */
    lineLen = strlen(in_Line) - 1;
    if(in_Line[lineLen] == '\n') {
        in_Line[lineLen] = '\0';
    }

    cmd = word[0];
    switch(cmd) {
        case 'h':
            printf("Commands are:\n");
            printf("    s(tep <n> \t"\
                    "Execute n (default 1) TM instructions\n");
            printf("    g(o \t"\
                    "Execute TM instructions until HALT\n");
            printf("    r(egs \t"\
                    "Print the contents of the registers\n");
            printf("    i(Mem <b <n>> \t"\
                    "Print n iMem locations starting at b\n");
            printf("    d(Mem <b <n>> \t"\
                    "Print n dMem locations starting at b\n");
            printf("    t(race \t"\
                    "Toggle instruction trace\n");
            printf("    p(rint \t"\
                    "Toggle print of total instructions executed"\
                    " ('go' only)\n");
            printf("    c(lear \t"\
                    "Reset simulator for new execution of program\n");
            printf("    q(uit \t"\
                    "Terminate the simulation\n");
            break;

        case 't':
        /*************************/
            traceflag = ! traceflag;
            printf("Tracing now ");
            if(traceflag) { printf("on.\n"); }
            else { printf("off.\n"); }
            break;

        case 'p':
        /**************************/
            icountflag = ! icountflag;
            printf("Printing instruction count now ");
            if(icountflag) { printf("on.\n"); }
            else { printf("off.\n"); }
            break;

        case 's':
        /***************************/
            if(atEOL()) { stepcnt = 1; }
            else if(getNum()) { stepcnt = abs(num); }
            else { printf("Step count?\n"); }
            break;

        case 'g':
            stepcnt = 1;
            break;

        case 'r':
        /***************************/
            for(int i = 0; i < NO_REGS; i++) {
                printf("%1d: %4d    ", i, reg[i]);
                if((i % 4) == 3) { printf("\n"); }
            }
            break;

        case 'i':
        /***************************/
            printcnt = 1;
            if(getNum()) {
                iloc = num;
                if(getNum()) { printcnt = num; }
            }
            if(! atEOL()) {
                printf("Instruction locations?\n");
            } else {
                while((iloc >= 0) && (iloc < IADDR_SIZE)
                        && (printcnt > 0)) {
                    writeInstruction(iloc);
                    iloc++;
                    printcnt--;
                }
            }
            break;

        case 'd':
        /***************************/
            printcnt = 1;
            if(getNum()) {
                dloc = num;
                if(getNum()) { printcnt = num; }
            }
            if(! atEOL()) {
                printf("Data location?\n");
            } else {
                while((dloc >= 0) && (dloc < DADDR_SIZE) &&
                        (printcnt > 0)) {
                    printf("%5d: %5d\n", dloc, dMem[dloc]);
                    dloc++;
                    printcnt--;
                }
            }
            break;

        case 'c':
        /***************************/
            iloc = 0;
            dloc = 0;
            stepcnt = 0;
            for(int regNo = 0; regNo < NO_REGS; regNo++) {
                reg[regNo] = 0;
            }
            dMem[0] = DADDR_SIZE - 1;
            for(int loc = 1; loc < DADDR_SIZE; loc++) {
                dMem[loc] = 0;
            }
            break;

        case 'q':
            return FALSE;
            /* break; */

        default:
            printf("Command %c unknown.\n", cmd);
            break;
    } /* switch */

    stepResult = srOKAY;
    if(stepcnt > 0) {
        if(cmd == 'g') {
            stepcnt = 0;
            while(stepResult == srOKAY) {
                iloc = reg[PC_REG];
                if(traceflag) { writeInstruction(iloc); }
                stepResult = stepTM();
                stepcnt++;
            }
            if(icountflag) {
                printf("Number of instructions executed = %d\n", stepcnt);
            }
        } else {
            while((stepcnt > 0) && (stepResult == srOKAY)) {
                iloc = reg[PC_REG];
                if(traceflag) { writeInstruction(iloc); }
                stepResult = stepTM();
                stepcnt--;
            }
        }
        printf("%s\n", stepResultTab[stepResult]);
    }
    return TRUE;
} /* doCommand */

/*****************************/
int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    strcpy(pgmName, argv[1]);
    if(strchr(pgmName, '.') == NULL) {
        strcat(pgmName, ".tm");
    }
    pgm = fopen(pgmName, "r");
    if(pgm == NULL) {
        printf("file '%s' not found\n", pgmName);
        exit(1);
    }

    /* read the program */
    if(! readInstructions()) {
        exit(1);
    }
    /* switch input file to terminal */
    /* reset( input ); */
    /* read-eval-print */
    printf("TM simulation (enter h for help)...\n");
    do {
        done = ! doCommand();
    } while(! done);
    printf("Simulation done.\n");
    return 0;
}

