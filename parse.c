/********************************************************/
/* File: parse.c                                        */
/* The parser implementation for the C- compiler        */
/* 使用recursive-descent实现的parser，结果遇到了很多冲突    */
/* 赋值表达式和简单表达式相同的前缀还没有解决                */
/* tallate                                              */
/********************************************************/

#include "globals.h"
#include "scan.h"
#include "util.h"

/* lookahead token */
static TokenType token;

/* function prototypes for recursive calls */
/* declarations */
static TreeNode *declaration_list(void);
static TreeNode *declaration(void);
static TreeNode *params(void);
static TreeNode *param_list(void); /* 函数声明内部的参数列表 */
static TreeNode *param(void);
/* statements */
static TreeNode *compound_stmt(void);
static TreeNode *local_declarations(void); /* compound_stmt内部的声明串 */
static TreeNode *statement_list(void); /* compound_stmt内部的语句串 */
static TreeNode *statement(void); /* 表示一个语句 */
static TreeNode *expression_stmt(void);
static TreeNode *selection_stmt(void);
static TreeNode *iteration_stmt(void);
static TreeNode *return_stmt(void);
/* expressions */
static TreeNode *expression(void);
static TreeNode *simple_expression(void);
static TreeNode *additive_expression(void);
static TreeNode *term(void);
static TreeNode *factor(void);
static TreeNode *var_call(void); /* var与call含有公共前缀 */
static TreeNode *args(void); /* 忽略了arg-list */

static void syntaxError(char *message) {
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

static void match(TokenType expected) {
    if(token == expected) {
        /* 继续lookahead */
        token = getToken();
    } else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString); /* tokenString在scan中定义 */
        fprintf(listing, "        ");
    }
}

/* 匹配一个类型(void、int) */
static ExpType matchType() {
    ExpType type = Void;
    if(token == VOID) {
        match(VOID);
        type = Void;
    } else if(token == INT) {
        match(INT);
        type = Integer;
    } else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        fprintf(listing, "        ");
        token = getToken();
    }
    return type;
}

/* 声明列表 */
TreeNode *declaration_list(void) {
    TreeNode *root = declaration();
    TreeNode *last = root; /* 指向最后一个兄弟节点 */
    while(token != ENDFILE) {
        TreeNode *next = declaration();
        if(next) {
            /* 将新识别的子树作为新的last */
            if(!root) {
                root = last = next;
            } else {
                last->sibling = next;
                last = next;
            }
        }
    }
    return root;
}

/* 一个声明（函数、变量、数组） */
TreeNode *declaration(void) {
    TreeNode *root = NULL;
    ExpType type = Void; /* 定义的类型声明符 */
    char *name = NULL; /* 定义的标志符 */
    /* 识别类型声明符 */
    type = matchType();
    /* 识别id */
    if(token == ID) {
        name = copyString(tokenString);
        match(ID);
    } else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
    }
    /* 根据token判断是var还是fun */
    switch(token) {
    case SEMI:
        /* 变量 */
        root = newDeclNode(VarD);
        if(root) {
            root->varType = type;
            root->attr.name = name;
        }
        match(SEMI);
        break;
    case LSPAREN:
        /* 数组 */
        root = newDeclNode(ArrD);
        if(root) {
            root->varType = type;
            root->attr.name = name;
        }
        match(LSPAREN);
        if(root) { root->child[0] = expression(); }
        match(RSPAREN);
        match(SEMI);
        break;
    case LPAREN:
        /* 函数 */
        root = newDeclNode(FunD);
        if(root) {
            root->funcReturnType = type;
            root->attr.name = name;
        }
        match(LPAREN);
        if(root) { root->child[0] = params(); }
        match(RPAREN);
        if(root) {
            root->child[1] = compound_stmt();
        }
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return root;
}

TreeNode *params(void) {
    TreeNode *root = NULL;
    ExpType type = Void; /* 类型声明符 */
    char *name = NULL;
    /* 类型声明符 */
    type = matchType();
    /* 根据token判断是否存在第一个声明 */
    if(token == ID) {
        name = copyString(tokenString);
        match(ID);
        if(token == LSPAREN) {
            match(LSPAREN);
            /* 根据语法的定义不能填下标 */
            match(RSPAREN);
            /* 数组参数 */
            root = newDeclNode(ArrD);
            root->varType = type;
            root->attr.name = name;
        } else {
            /* 变量参数 */
            root = newDeclNode(VarD);
            root->varType = type;
            root->attr.name = name;
        }
        /* 识别后面的参数列表 */
        if(root) { root->sibling = param_list(); }
    }
    return root;
}

TreeNode *param_list(void) {
    TreeNode *root = NULL, *last = NULL;
    while(token == COMM) {
        TreeNode *next = NULL;
        match(COMM);
        next = param();
        if(next) {
            if(!root) {
                root = last = next;
            } else {
                last->sibling = next;
                last = next;
            }
        }
    }
    return root;
}

TreeNode *param(void) {
    TreeNode *root = NULL;
    ExpType type = matchType();
    char *name = NULL;
    if(token == ID) {
        name = copyString(tokenString);
        match(ID);
        if(token == LSPAREN) {
            match(LSPAREN);
            match(RSPAREN);
            /* 数组 */
            root = newDeclNode(ArrD);
            root->varType = type;
            root->attr.name = name;
        } else {
            /* 变量 */
            root = newDeclNode(VarD);
            root->varType = type;
            root->attr.name = name;
        }
    } else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
    }
    return root;
}

/* 复合语句 */
TreeNode *compound_stmt(void) {
    TreeNode *root = NULL;
    match(LCPAREN);
    root = newStmtNode(ComS);
    if(token != RCPAREN) {
        if(token == INT || token == VOID) {
            if(root) { root->child[0] = local_declarations(); }
        }
        if(token != RCPAREN) {
            if(root) { root->child[1] = statement_list(); }
        }
    }
    match(RCPAREN);
    return root;
}

TreeNode *local_declarations(void) {
    TreeNode *root = NULL, *last = NULL;
    while(token == INT || token == VOID) {
        TreeNode *next = declaration(); /* 实际上是var_declaration */
        if(next != NULL) {
            /* 将新识别的声明接到最后 */
            if(root == NULL) {
                root = last = next;
            } else {
                last->sibling = next;
                last = next;
            }
        }
    }
    return root;
}

TreeNode *statement_list(void) {
    TreeNode *root = NULL, *last = NULL;
    while(token != RCPAREN) { /* 因为语句串是在compound_stmt内的，所以碰到RCPAREN就结束了 */
        TreeNode *next = statement();
        if(next != NULL) {
            /* 将新识别的语句接到最后 */
            if(root == NULL) {
                root = last = next;
            } else {
                last->sibling = next;
                last = next;
            }
        }
    }
    return root;
}

TreeNode *statement(void) {
    TreeNode *root = NULL;
    switch(token) {
        case LCPAREN:
            /* 复合表达式 */
            root = compound_stmt();
            break;
        case IF:
            /* if表达式 */
            root = selection_stmt();
            break;
        case WHILE:
            /* 循环表达式 */
            root = iteration_stmt();
            break;
        case RETURN:
            /* 返回表达式 */
            root = return_stmt();
            break;
        default:
            /* 表达式语句 */
            root = expression_stmt();
            break;
    }
    return root;
}

/* 剩余4个语句 */
TreeNode *expression_stmt(void) {
    TreeNode *root = newStmtNode(ExpS);
    if(token != SEMI && root) {
        root->child[0] = expression();
    }
    match(SEMI);
    return root;
}
TreeNode *selection_stmt(void) {
    TreeNode *root = newStmtNode(SelS);
    match(IF);
    match(LPAREN);
    if(root) { root->child[0] = expression(); }
    match(RPAREN);
    if(root) { root->child[1] = statement(); }
    if(token == ELSE) {
        match(ELSE);
        if(root) { root->child[2] = statement(); }
    }
    return root;
}
TreeNode *iteration_stmt(void) {
    TreeNode *root = newStmtNode(IteS);
    match(WHILE);
    match(LPAREN);
    if(root) { root->child[0] = expression(); }
    match(RPAREN);
    if(root) { root->child[1] = statement(); }
    return root;
}
TreeNode *return_stmt(void) {
    TreeNode *root = newStmtNode(RetS);
    match(RETURN);
    if(token != SEMI) {
        if(root) { root->child[0] = expression(); }
    }
    match(SEMI);
    return root;
}

/* 表达式，即赋值表达式或简单表达式 */
TreeNode *expression(void) {
    /* 使用leftfactoring合并assign-expression和simple-expression */
    TreeNode *root = NULL;
    if(token == ID) {
        /* 可能是赋值表达式或者简单表达式，若是赋值表达式则只返回一个var并且token会是ASIGN */
        root = simple_expression();
        if(token == ASIGN) {
            /* 说明是赋值表达式 */
            TreeNode *newRoot = newExpNode(AssignE);
            if(newRoot) {
                newRoot->attr.op = token;
                newRoot->child[0] = root;
                root = newRoot;
            }
            match(ASIGN);
            if(root) {
                root->child[1] = expression();
            }
        }
    } else {
        /* 说明是简单表达式 */
        root = simple_expression();
    }
    return root;
}

/* 简单表达式，即比较、算数、标志符、常数、函数调用 */
static TreeNode *simple_expression(void) {
    TreeNode *root = additive_expression();
    if(token == LTET || token == LT ||
            token == MT || token == MTET ||
            token == EQ || token == NEQ) {
        TreeNode *newRoot = newExpNode(OpE);
        if(newRoot) {
            newRoot->child[0] = root;
            newRoot->attr.op = token;
            root = newRoot;
        }
        match(token);
        if(root) {
            root->child[1] = additive_expression();
        }
    }
    return root;
}

static TreeNode *additive_expression(void) {
    TreeNode *root = term();
    while(token == PLUS || token == MINUS) {
        /* 将+ -符号当作新的根 */
        TreeNode *newRoot = newExpNode(OpE);
        if(newRoot) {
            newRoot->child[0] = root;
            newRoot->attr.op = token;
            root = newRoot;
        }
        match(token);
        if(root) {
            root->child[1] = term();
        }
    }
    return root;
}
static TreeNode *term(void) {
    TreeNode *root = factor();
    while(token == TIMES || token == OVER) {
        /* 将* /符号当作新的根 */
        TreeNode *newRoot = newExpNode(OpE);
        if(newRoot) {
            newRoot->child[0] = root;
            newRoot->attr.op = token;
            root = newRoot;
        }
        match(token);
        if(root) {
            root->child[1] = factor();
        }
    }
    return root;
}
static TreeNode *factor(void) {
    TreeNode *root = NULL;
    switch(token) {
    case LPAREN:
        match(LPAREN);
        root = expression();
        match(RPAREN);
        break;
    case NUM:
        root = newExpNode(ConstE);
        if(root) { root->attr.val = atoi(tokenString); }
        match(NUM);
        break;
    case ID:
        root = var_call();
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
    }
    return root;
}

/* 函数调用、数组、标志符表达式 */
TreeNode *var_call(void) {
    TreeNode *root = NULL;
    char *name = copyString(tokenString);
    match(ID);
    if(token == LPAREN) {
        /* 函数调用表达式 */
        root = newExpNode(CallE);
        root->attr.name = name;
        match(LPAREN);
        root->child[0] = args();
        match(RPAREN);
    } else if(token == LSPAREN) {
        /* 数组表达式 */
        root = newExpNode(ArrE);
        root->attr.name = name;
        match(LSPAREN);
        root->child[0] = expression();
        match(RSPAREN);
    } else {
        /* 标志符表达式 */
        root = newExpNode(VarE);
        root->attr.name = name;
    }
    return root;
}
TreeNode *args(void) {
    TreeNode *root = NULL;
    if(token != RPAREN) {
        root = expression();
        TreeNode *last = root;
        while(token == COMM) {
            match(COMM);
            TreeNode *next = expression();
            /* 将新识别的兄弟表达式连到最后 */
            if(next != NULL) {
                if(root == NULL) {
                    root = last = next;
                } else {
                    last->sibling = next;
                    last = next;
                }
            }
        }
    }
    return root;
}

/**************************************/
/* the primary function of the parser */
/**************************************/
TreeNode *parse(void) {
    TreeNode *root;
    token = getToken();
    root = declaration_list();
    if(token != ENDFILE) {
        syntaxError("Code ends before file\n");
    }
    return root;
}
