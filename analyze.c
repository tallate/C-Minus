/********************************************/
/* File: analyze.c                          */
/* Semantic analyzer implementation         */
/* for the C- compiler                      */
/* tallate                                  */
/********************************************/

#include "analyze.h"
#include "symtab.h"
#include "util.h"
#include "globals.h"

/* 变量的内存地址的计数器 */
// static int location = 0;

static int nestNum = 0;

static TreeNode *enclosingFunction = NULL;

/* typeError 打印类型错误信息
 */
static void typeError(TreeNode *t, char *message) {
    fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
    printf("%s\n", t->attr.name);
    Error = TRUE;
}

static int isArrType(TreeNode *decl) {
    return decl->nodeKind == DeclK && decl->kind.decl == ArrD;
}
static int isFunType(TreeNode *decl) {
    return decl->nodeKind == DeclK && decl->kind.decl == FunD;
}

static int typeEqual(ExpType target, ExpType cur) {
    return target == cur;
}
/* 检查函数调用的实参列表是否与其定义的形参列表一致 */
static int checkFunCall(TreeNode *funDecl, TreeNode *funCall) {
    if(! funDecl || ! funCall) {
        /* 传入了空指针 */
        return 0;
    }
    TreeNode *declArgList = funDecl->child[0];
    TreeNode *callArgList = funCall->child[0];
    while(declArgList && callArgList) {
        if(! typeEqual(declArgList->varType, callArgList->expType)) {
            return 0;
        }
        declArgList = declArgList->sibling;
        callArgList = callArgList->sibling;
    }
    if(declArgList || callArgList) {
        return 0;
    } else {
        return 1;
    }
}

/* traverse 语法树递归遍历的子程序
 * 以前序应用preProc，以后序应用postProc
 */
static void traverse(TreeNode *t,
                     void (* preProc) (TreeNode *),
                     void (* postProc) (TreeNode *)) {
    if(t) {
        preProc(t);
        /* 遍历所有子节点 */
        for(int i = 0; i < MAXCHILDREN; i++) {
            traverse(t->child[i], preProc, postProc);
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

/* insertNode 向符号表的当前scope插入一个标志符
 */
static void insertNode(TreeNode *t) {
    switch(t->nodeKind) {
    case DeclK:
        switch(t->kind.decl) {
        /* 在当前scope中插入一个标志符 */
        case VarD:
        /*****************************************/
            if(st_lookup(t->attr.name, 1)) {
                typeError(t, "Redeclaration");
                symbolTable->err = TRUE;
            }
            st_insert(t->attr.name, t, t->lineno, symbolTable->location++);
            break;

        case ArrD:
        /*****************************************/
            if(st_lookup(t->attr.name, 1)) {
                typeError(t, "Redeclaration");
                symbolTable->err = TRUE;
            }
            st_insert(t->attr.name, t, t->lineno, symbolTable->location);
            if(t->child[0]) {
                symbolTable->location += t->child[0]->attr.val;
            } else {
                /* 数组参数，传入的是地址 */
                symbolTable->location++;
            }
            break;

        case FunD:
        /*****************************************/
            if (st_lookup(t->attr.name, 1)) {
                typeError(t, "Redeclaration");
                symbolTable->err = TRUE;
            }
            st_insert(t->attr.name, t, t->lineno, symbolTable->location++);
            printSymTab(listing);
            st_augment();
            enclosingFunction = t;
            break;

        default:
            break;
        }
        break;
    case StmtK:
        switch(t->kind.stmt) {
        case ComS:
            printf("line: %d\n", t->lineno);
            printSymTab(listing);
            /* 不为0表示当前scope不是函数定义的复合语句中 */
            if(nestNum != 0) {
                st_augment();
                /* 初始地址继承自外部的符号表 */
                symbolTable->location = symbolTable->next->location;
            }
            nestNum++;
            break;
        default:
            break;
        }
    default:
        break;
    }
} /* insertNode */

/* checkNode 对当前树节点进行type checking
 */
static void checkNode(TreeNode *t) {
    BucketList *tmp;
    switch(t->nodeKind) {
    case DeclK:
        switch(t->kind.decl) {
        case VarD:
            // if(! symbolTable->err) {
                if (t->varType != Integer) {
                    typeError(t, "Var declared with non-integer type");
                    symbolTable->err = TRUE;
                }
            // }
            break;
        case ArrD:
            // if(! symbolTable->err) {
                if(t->varType != Integer) {
                    typeError(t, "Array declared with non-integer type");
                    symbolTable->err = TRUE;
                }
            // }
            break;
        case FunD:
            // if(! symbolTable->err) {
            // }
            break;
        default:
            break;
        }
        break;
    case StmtK:
        switch(t->kind.stmt) {
        case ExpS:
            break;
        case ComS:
            t->scope = st_abrade(); /* 保存对应scope用于代码生成 */
            /* 记录AR的最大偏移量 */
            if(t->scope->location > t->scope->maxOffset) {
                t->scope->maxOffset = t->scope->location;
            }
            if(symbolTable->next) { /* 说明不是函数域 */
                if(t->scope->location > symbolTable->maxOffset) {
                    symbolTable->maxOffset = t->scope->location;
                }
            }
            nestNum--;
            if(nestNum == 0) {
                enclosingFunction = NULL;
            }
            break;
        case SelS:
            break;
        case IteS:
            break;
        case RetS:
            if(t->child[0]) {
                if(enclosingFunction->funcReturnType == Void) {
                    typeError(t, "function with void return type returned non-void value");
                    symbolTable->err = TRUE;
                }
            } else {
                if(enclosingFunction->funcReturnType != Void) {
                    typeError(t, "function with non-void return type returned void");
                    symbolTable->err = TRUE;
                }
            }
            break;
        default:
            break;
        }
        break;
    case ExpK:
        switch(t->kind.exp) {
        case AssignE:
            if(t->child[0]->kind.exp != VarE && t->child[0]->kind.exp != ArrE) {
                typeError(t, "l-value is not var or array");
                symbolTable->err = 1;
            }
            t->attr.val = t->child[1]->attr.val;
            break;
        case OpE:
            if((t->child[0]->expType != Integer) ||
               (t->child[1]->expType != Integer)) {
                typeError(t, "Op applied to non-integer");
            }
            switch(t->attr.op) {
                /* relop（关系表达式） */
                case LT: case LTET: case MT:
                case MTET: case EQ: case NEQ:
                    t->expType = Boolean;
                    break;
                default:
                    t->expType = Integer;
                    break;
            }
            break;
        case ConstE:
            t->expType = Integer;
            break;
        case VarE:
            tmp = st_lookup(t->attr.name, -1);
            if(! tmp) {
                typeError(t, "Var Used without declaration");
                t->expType = Void;
            } else {
                t->expType = Integer;
            }
            break;
        case ArrE:
            tmp = st_lookup(t->attr.name, -1);
            if(! tmp) {
                typeError(t, "Array used without declaration");
                t->expType = Void;
            } else if(!isArrType(tmp->decl)) {
                typeError(t, "Subscripted value is not array");
                t->expType = Void;
            } else {
                t->expType = Integer;
            }
            break;
        case CallE:
            tmp = st_lookup(t->attr.name, -1);
            if(! tmp) {
                typeError(t, "Call withoud declaration");
                t->expType = Void;
            } else if(! isFunType(tmp->decl)) {
                typeError(t, "Called object is not a function");
                t->expType = Void;
            } else if(! checkFunCall(tmp->decl, t)) {
                typeError(t, "Call with false arguments");
                t->expType = Void;
            } else {
                t->expType = Integer;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
} /* checkNode */

void initSymbolTable() {
    st_augment();
    /* 预定义的输入函数 */
    TreeNode *inputFun = newDeclNode(FunD);
    inputFun->attr.name = copyString("input");
    inputFun->funcReturnType = Integer;
    inputFun->child[1] = newStmtNode(ComS);
    inputFun->child[1]->scope = (SymbolTable*) malloc(sizeof(SymbolTable));
    inputFun->child[1]->scope->location = inputFun->child[1]->scope->maxOffset = 0;
    st_insert("input", inputFun, -1, symbolTable->location++);
    /* 预定义的输出函数 */
    TreeNode *outputFun = newDeclNode(FunD);
    outputFun->attr.name = copyString("output");
    outputFun->funcReturnType = Void;
    outputFun->child[0] = newDeclNode(VarD);
    outputFun->child[0]->varType = Integer;
    outputFun->child[1] = newStmtNode(ComS);
    outputFun->child[1]->scope = (SymbolTable*) malloc(sizeof(SymbolTable));
    outputFun->child[1]->scope->location = outputFun->child[1]->scope->maxOffset = 1;
    st_insert("output", outputFun, -1, symbolTable->location++);
}

void analyze(TreeNode *syntaxTree) {
    initSymbolTable();
    traverse(syntaxTree, insertNode, checkNode);
    if(TraceAnalyze) {
        fprintf(listing, "\nSymbol table:\n");
        printSymTab(listing);
    }
}