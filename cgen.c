/****************************************/
/* File: cgen.c                         */
/* code generator implementation for    */
/* the C- compiler                      */
/* (run by TM machine)                  */
/* tallate                              */
/****************************************/

#include "globals.h"
#include "code.h"
#include "symtab.h"

static int tmpOffset = 0;

static int frameOffset = 0;

static int ofpFO = 0;
static int retFO = -1;
static int initFO = -2;

static void cGen(TreeNode *tree, int isAddress);

/* genDecl 为一个declaration节点生成代码 */
static void genDecl(TreeNode *tree) {
    BucketList *l = NULL;
    int loc;

    switch(tree->kind.decl) {
#if 0
没用，因为偏移量已经在semantic analysis部分计算过了（maxOffset）
        case VarD:
        /*****************************************/
            if(TraceCode) { emitComment("-> VarD"); }
            // l = st_lookup(tree->attr.name, 1);
            /* 不用判断l == NULL */
            if(symbolTable->nestLevel == 0) {
                /* 修改寄存器fp */
                emitRM("LDA", fp, -1, fp, "decrease the fp");
            }
            /* 普通变量的声明不需要生成代码 */
            if(TraceCode) { emitComment("<- VarD"); }
            break;

        case ArrD:
        /*****************************************/
            if(TraceCode) { emitComment("-> ArrD"); }
            if(symbolTable->nestLevel == 0) {
                if(tree->child[0]) {
                    /* 全局变量 */
                    emitRM("LDA", fp, - tree->child[0]->attr.val, fp, "decrease the fp");
                }
                /* 函数的数组参数的声明可以忽略（根本不会被cGen） */
            }
            if(TraceCode) { emitComment("<- ArrD"); }
            break;
#endif
        case FunD: /* callee */
        /*****************************************/
            if(TraceCode) { emitComment("-> FunD"); }
            /* 函数的入口地址 */
            loc = emitSkip(0);
            l = st_lookup(tree->attr.name, -1);
            l->decl->funcEntryLoc = loc;
            /* 在该函数中调用其他函数时，使用这个frameOffset来计算新AR的初始地址 */
            frameOffset = initFO - l->decl->child[1]->scope->maxOffset;
            // printf("name:%s offset:%d\n", l->decl->attr.name, l->decl->child[1]->scope->maxOffset);
            /* 函数的起始处，保存返回地址到retFO处 */
            emitRM("ST", ac, retFO, fp, "funD: store return address to retFO");
            /* gen code for fun-body */
            cGen(tree->child[1], FALSE);
            /* 当前frame的retFO处保存了该次调用的地址 */
            emitRM("LD", pc, retFO, fp, "funD: return");
            if(TraceCode) { emitComment("<- FunD"); }
            break;

        default:
            break;
    }

}


/* genStmt 为一个statement节点生成代码 */
static void genStmt(TreeNode *tree) {
    TreeNode *p1, *p2, *p3;
    int savedLoc1, savedLoc2, currentLoc;

    switch(tree->kind.stmt) {
        case ExpS:
        /*****************************************/
            if(TraceCode) { emitComment("-> ExpS"); }
            cGen(tree->child[0], FALSE);
            if(TraceCode) { emitComment("<- ExpS"); }
            break;

        case ComS:
        /*****************************************/
            if(TraceCode) { emitComment("-> ComS"); }
            /* 将局部作用域补回符号表 */
            tree->scope->next = symbolTable;
            symbolTable = tree->scope;
            /* gen code for decl-part */
            cGen(tree->child[0], FALSE);
            /* gen code for stmt-part */
            cGen(tree->child[1], FALSE);
            st_abrade(); /* 撤回符号表 */
            if(TraceCode) { emitComment("<- ComS"); }
            break;

        case SelS:
        /*****************************************/
            if(TraceCode) { emitComment("-> SelS"); }
            p1 = tree->child[0];
            p2 = tree->child[1];
            p3 = tree->child[2];
            /* gen code for test-part */
            cGen(p1, FALSE);
            savedLoc1 = emitSkip(1);
            emitComment("selS: jump to else belongs here");
            /* recurse on then part */
            cGen(p2, FALSE);
            savedLoc2 = emitSkip(1);
            emitComment("selS: jump to end belongs here");
            currentLoc = emitSkip(0);
            emitBackup(savedLoc1);
            emitRM_Abs("JEQ", ac, currentLoc, "selS: jmp to else");
            emitRestore();
            /* recurse on else part */
            cGen(p3, FALSE);
            currentLoc = emitSkip(0);
            emitBackup(savedLoc2);
            emitRM_Abs("LDA", pc, currentLoc, "selS: jmp to end");
            emitRestore();
            if(TraceCode) { emitComment("<- SelS"); }
            break;

        case IteS:
        /*****************************************/
            if(TraceCode) { emitComment("-> IteS"); }
            p1 = tree->child[0];
            p2 = tree->child[1];
            /* 保留while语句的初始位置 */
            savedLoc1 = emitSkip(0);
            /* gen code for test-part */
            cGen(p1, FALSE);
            /* 计算括号内值是否==0，跳到while语句末尾 */
            savedLoc2 = emitSkip(1);
            emitComment("iteS: jump to end belongs here");
            /* gen code for stmt-part */
            cGen(p2, FALSE);
            /* 跳到while语句起始处 */
            emitRM_Abs("LDA", pc, savedLoc1, "jmp to begin");
            emitComment("iteS: jump to begin belongs here");
            /* 补全savedLoc2处的jump语句 */
            currentLoc = emitSkip(0);
            emitBackup(savedLoc2);
            /**一个bug，之前误把ac写成pc******************************************/
            emitRM_Abs("JEQ", ac, currentLoc, "jmp to end");
            emitRestore();
            if(TraceCode) { emitComment("<- IteS"); }
            break;

        case RetS:
        /*****************************************/
            if(TraceCode) { emitComment("-> RetS"); }
            /* gen code for the return value, the result wile be load to ac */
            cGen(tree->child[0], FALSE);
            /* 当前frame的retFO处保存了该次调用的地址 */
            emitRM("LD", pc, retFO, fp, "retS: return");
            if(TraceCode) { emitComment("<- RetS"); }
            break;

        default:
            break;
    }
}

/* genExp 为一个expression节点生成代码 */
static void genExp(TreeNode *tree, int isAddress) {
    TreeNode *p1, *p2, *sibling;
    BucketList *l = NULL;
    int argNum = 0;
    int currentTmpOffset;

    switch(tree->kind.exp) {
    case AssignE:
    /*****************************************/
        if(TraceCode) { emitComment("-> AssignE"); }
        /* gen code for lhs，有可能是其相对address被保存到reg[ac] */
        cGen(tree->child[0], TRUE);
        // emitRO("OUT", ac, 0, 0, "output lhs");
        currentTmpOffset = tmpOffset++;
        emitRM("ST", ac, currentTmpOffset, tp, "assignE: push left");
        /* gen code for rhs，value */
        cGen(tree->child[1], FALSE);
        /* store lhs to ac1 */
        emitRM("LD", ac1, currentTmpOffset, tp, "assignE: load left");
        /* store rhs to lhs */
        emitRM("ST", ac, 0, ac1, "assignE: store right to left");
        if(TraceCode) { emitComment("<- assign"); }
        break;

    case OpE:
    /*****************************************/
        if(TraceCode) { emitComment("-> OpE"); }
        p1 = tree->child[0];
        p2 = tree->child[1];
        /* gen code for ac = left arg */
        cGen(p1, FALSE);
        /* gen code to push left operand */
        currentTmpOffset = tmpOffset++;
        emitRM("ST", ac, currentTmpOffset, tp, "opE: push left");
        /* gen code for ac = right operand */
        cGen(p2, FALSE);
        /* load left operand */
        emitRM("LD", ac1, currentTmpOffset, tp, "opE: load left");
        switch(tree->attr.op) {
        case PLUS:
            emitRO("ADD", ac, ac1, ac, "opE +");
            break;
        case MINUS:
            emitRO("SUB", ac, ac1, ac, "opE -");
            break;
        case TIMES:
            emitRO("MUL", ac, ac1, ac, "opE *");
            break;
        case OVER:
            emitRO("DIV", ac, ac1, ac, "opE /");
            break;
        case LT:
            emitRO("SUB", ac, ac1, ac, "op <");
            emitRM("JLT", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        case LTET:
            emitRO("SUB", ac, ac1, ac, "op <=");
            emitRM("JLE", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        case MT:
            emitRO("SUB", ac, ac1, ac, "op >");
            emitRM("JGT", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        case MTET:
            emitRO("SUB", ac, ac1, ac, "op >=");
            emitRM("JGE", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        case EQ:
            emitRO("SUB", ac, ac1, ac, "op ==");
            emitRM("JEQ", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        case NEQ:
            emitRO("SUB", ac, ac1, ac, "op !=");
            emitRM("JNE", ac, 2, pc, "br if true");
            emitRM("LDC", ac, 0, ac, "false case");
            emitRM("LDA", pc, 1, pc, "unconditional jmp");
            emitRM("LDC", ac, 1, ac, "true case");
            break;
        default:
            emitComment("BUG: Unknown operator");
            break;
        } /* switch op */
        if(TraceCode) { emitComment("<- OpE"); }
        break; /* OpE */

    case ConstE:
    /*****************************************/
        if(TraceCode) { emitComment("-> ConstE"); }
        /* gen code to load integer constant using LDC */
        emitRM("LDC", ac, tree->attr.val, 0, "constE: load const");
        if(TraceCode) { emitComment("<- ConstE"); }
        break; /* ConstE */

    case VarE:
    /****************************************/
        if(TraceCode) { emitComment("-> VarE"); }
        emitComment("varE: address of arr");
        /* 计算变量的地址 */
        l = st_lookup(tree->attr.name, -1);
        /* 全局变量对应声明的嵌套层次为0，局部变量的大于0 */
        if(l->nestLevel == 0) {
            emitRM("LDA", ac, - l->memloc, gp, "varE: cal address in global");
        } else {
            emitRM("LDA", ac, initFO - l->memloc, fp, "varE: cal address in local");
        }
        /* 若标志isAddress为FALSE说明不是在赋值表达式的左边 */
        if(! isAddress) {
            emitComment("varE: if ! isAddress load the value");
            emitRM("LD", ac, 0, ac, "varE: load value to ac");
            // printf("%s\n", tree->attr.name);
            // emitRO("OUT", ac, 0, 0, "asd");
        }
        if(TraceCode) { emitComment("<- VarE"); }
        break; /* VarE */

    case ArrE:
    /*****************************************/
        if(TraceCode) { emitComment("-> ArrE"); }
        emitComment("arrE: cal address of arr(base + index)");
        /* 计算数组表达式的下标（保存到reg[ac]） */
        cGen(tree->child[0], FALSE);
        /* 计算数组的基址 */
        l = st_lookup(tree->attr.name, -1);
        /* 全局变量对应声明的嵌套层次为0，局部变量的大于0 */
        if(l->nestLevel == 0) {
            emitRM("LDA", ac1, - l->memloc, gp, "arrE: cal the base address in global");
        } else {
            emitRM("LDA", ac1, initFO - l->memloc, fp, "arrE: cal the base address in local");
        }
        /* 若引用的是函数的数组参数 */
        if(! l->decl->child[0]) {
            // printf("引用了数组参数name:%s line:%d\n", l->decl->attr.name, l->decl->lineno);
            emitRM("LD", ac1, 0, ac1, "arrE: redirect");
        }
        /* 计算绝对地址，保存到reg[ac] */
        /**一个Bug，ADD->SUB，注意函数调用栈是自顶向下生长的***********************************************/
        emitRO("SUB", ac, ac1, ac, "arrE: cal the absolute address");
        if(! isAddress) {
            /* 加载值 */
            emitComment("arrE: if ! isAddress load the value");
            emitRM("LD", ac, 0, ac, "arrE: put value in ac");
        }
        if(TraceCode) { emitComment("<- ArrE"); }
        break; /* ArrE */

    case CallE: /* caller */
    /*****************************************/
        if(TraceCode) { emitComment("-> CallE"); }
        /* compute arguments */
        emitComment("callE: cal every arguments");
        l = st_lookup(tree->attr.name, -1); /* 函数的声明节点 */
        p1 = tree->child[0];
        while(p1) {
            /**BUG：因为cGen是连着sibling一块生成的，所以不能直接cGen(p1)，要先断开sibling************************/
            sibling = p1->sibling;
            p1->sibling = NULL;
            /* 判断参数是否引用自一个数组 */
            if(p1->kind.exp == VarE) {
                BucketList *varTerm = st_lookup(p1->attr.name, -1); /* 参数引用的变量对应的声明节点 */
                if(varTerm->decl->kind.decl == ArrD && varTerm->decl->child[0]) { /* 该标志符的定义是数组且不是传递过来的数组参数 */
                    // printf("use-line:%d var-name:%s decl-line:%d\n", tree->lineno, varTerm->decl->attr.name, varTerm->decl->lineno);
                    cGen(p1, TRUE);
                } else {
                    cGen(p1, FALSE);
                }
            } else {
                /* 说明不是一个简单的标志符表达式 */
                cGen(p1, FALSE);
            }
            // emitRO("OUT", ac, 0, 0, "output arg");
            emitRM("ST", ac, initFO + frameOffset - (argNum++), fp, "callE: store argument");
            p1->sibling = sibling;
            p1 = p1->sibling;
        }
        /* store ofp and ret */
        emitRM("ST", fp, ofpFO + frameOffset, fp, "store current fp");
        emitRM("LDA", fp, ofpFO + frameOffset, fp, "push new frame");
        // emitRO("OUT", fp, 0, 0, "output current fp");
        /* 将函数调用的返回地址保存到ac，待遇到return语句后跳转 */
        emitRM("LDA", ac, 1, pc, "save return in ac");
        /* get address of fun-entry and jump */
        emitRM("LDC", pc, l->decl->funcEntryLoc, 0, "jump to function entry");
        emitRM("LD", fp, ofpFO, fp, "pop current frame");
        if(TraceCode) { emitComment("<- CallE"); }
        break; /* CallE */

    default:
        break;
    } /* switch exp */
}

/* cGen 递归地遍历语法树，生成代码
 */
static void cGen(TreeNode *tree, int isAddress) {
    if(tree) {
        switch(tree->nodeKind) {
        case DeclK:
            genDecl(tree);
            break;
        case StmtK:
            genStmt(tree);
            break;
        case ExpK:
            genExp(tree, isAddress);
            break;
        default:
            break;
        }
        cGen(tree->sibling, FALSE);
    }
}

void gen_input_output() {
    int loc;
    BucketList *l = NULL;
    /* gen code for input */
    if(TraceCode) { emitComment("-> Input"); }
    /* store the entry address of the function */
    loc = emitSkip(0);
    l = st_lookup("input", -1);
    l->decl->funcEntryLoc = loc;
    /* 函数的起始处，保存返回地址到retFO */
    emitRM("ST", ac, retFO, fp, "input: store return address to retFO");
    /* 函数体 */
    emitRO("IN", ac, 0, 0, "input: read integer value");
    emitRM("LD", pc, retFO, fp, "input: return");
    if(TraceCode) { emitComment("<- Input"); }

    /* gen code for output */
    if(TraceCode) { emitComment("-> Output"); }
    /* store the entry address of the function */
    loc = emitSkip(0);
    l = st_lookup("output", -1);
    l->decl->funcEntryLoc = loc;
    /* 函数的起始处，保存返回地址到retFO */
    emitRM("ST", ac, retFO, fp,  "output: store return address to retFO");
    /* 函数体先将参数加载到ac，再print */
    emitRM("LD", ac, initFO, fp, "output: load first arg to ac");
    emitRO("OUT", ac, 0, 0, "output: print");
    emitRM("LD", pc, retFO, fp, "output: return");
    if(TraceCode) { emitComment("<- Output"); }
}

void codeGen(TreeNode *syntaxTree, char *codeFile) {
    int savedLoc, endLoc;
    BucketList *l = NULL;

    printf("\nGenerating Code...\n");
    char *s = malloc(strlen(codeFile) + 7);
    strcpy(s, "File: ");
    strcat(s, codeFile);
    emitComment("C- Compilation to TM Code");
    emitComment(s);
    /* generate standard prelude */
    emitComment("Standard prelude");
    emitRM("LD", gp, 0, ac, "load gp with maxaddress");
    emitRM("LDA", fp, - symbolTable->location, gp, "copy offset to fp");
    emitRM("ST", ac, 0, ac, "clear location 0");
    savedLoc = emitSkip(3);
    emitComment("End of standard prelude");
    /* generate code for C- program */
    gen_input_output();
    cGen(syntaxTree, FALSE);

    /* call main */
    endLoc = emitSkip(0);
    emitBackup(savedLoc);
    /* 函数的起始处，保存返回地址到retFO处 */
    emitRM("LDC", ac, endLoc, 0, "main: load return address to ac");
    emitRM("ST", ac, retFO, fp, "main: store return address to retFO");
    l = st_lookup("main", -1);
    emitRM("LDC", pc, l->decl->funcEntryLoc, 0, "jump to main");
    emitRestore();

    /* finish */
    emitComment("End of execution.");
    emitRO("HALT", 0, 0, 0, "");
    free(s);
    printf("\nGenerate Code Finished\n");
}