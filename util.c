/********************************************/
/* File: util.c                             */
/* Utility function implementation          */
/* for the C- compiler                      */
/* tallate                                  */
/********************************************/

#include "globals.h"
#include "util.h"

/* 打印一个token及其lexeme到listing文件
 */
void printToken(TokenType token, const char *tokenString) {
    switch(token) {
    /* book-keeping tokens */
    case ENDFILE: fprintf(listing, "EOF\n"); break;
    case ERROR: fprintf(listing, "ERROR: %s\n", tokenString); break;
    /* reserved words */
    case IF:
    case ELSE:
    case INT:
    case RETURN:
    case VOID:
    case WHILE:
        fprintf(listing, "reserved word: %s\n", tokenString);
        break;
    /* special symbols */
    case PLUS: fprintf(listing, "+\n"); break;
    case MINUS: fprintf(listing, "-\n"); break;
    case TIMES: fprintf(listing, "*\n"); break;
    case OVER: fprintf(listing, "/\n"); break;
    case LT: fprintf(listing, "<\n"); break;
    case LTET: fprintf(listing, "<=\n"); break;
    case MT: fprintf(listing, ">\n"); break;
    case MTET: fprintf(listing, ">=\n"); break;
    case EQ: fprintf(listing, "==\n"); break;
    case NEQ: fprintf(listing, "!=\n"); break;
    case ASIGN: fprintf(listing, "=\n"); break;
    case SEMI: fprintf(listing, ";\n"); break;
    case COMM: fprintf(listing, ",\n"); break;
    case LPAREN: fprintf(listing, "(\n"); break;
    case RPAREN: fprintf(listing, ")\n"); break;
    case LSPAREN: fprintf(listing, "[\n"); break;
    case RSPAREN: fprintf(listing, "]\n"); break;
    case LCPAREN: fprintf(listing, "{\n"); break;
    case RCPAREN: fprintf(listing, "}\n"); break;
    /* others */
    case ID:
        fprintf(listing, "ID, name = %s\n", tokenString);
        break;
    case NUM:
        fprintf(listing, "NUM, val = %s\n", tokenString);
        break;
    default:
        fprintf(listing, "Unknown token: %d\n", token);
    }
}

/* newDeclNode creates a new declaration
 * node for the syntax tree construction
 */
TreeNode *newDeclNode(DeclKind kind) {
    TreeNode *node = (TreeNode*) malloc(sizeof(TreeNode));
    int i;
    if(node == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    } else {
        /* 对节点初始化 */
        for(i = 0; i < MAXCHILDREN; i++) {
            node->child[i] = NULL;
        }
        node->sibling = NULL;
        node->nodeKind = DeclK;
        node->kind.decl = kind;
        node->scope = NULL;
        node->lineno = lineno;
        node->funcEntryLoc = 0;
    }
    return node;
}

TreeNode *newStmtNode(StmtKind kind) {
    TreeNode *node = (TreeNode*) malloc(sizeof(TreeNode));
    int i;
    if(node == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    } else {
        for(i = 0; i < MAXCHILDREN; i++) {
            node->child[i] = NULL;
        }
        node->sibling = NULL;
        node->nodeKind = StmtK;
        node->kind.stmt = kind;
        node->scope = NULL;
        node->lineno = lineno;
        node->funcEntryLoc = 0;
    }
    return node;
}

TreeNode *newExpNode(ExpKind kind) {
    TreeNode *node = (TreeNode*) malloc(sizeof(TreeNode));
    int i;
    if(node == NULL) {
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    } else {
        for(i = 0; i < MAXCHILDREN; i++) {
            node->child[i] = NULL;
        }
        node->sibling = NULL;
        node->nodeKind = ExpK;
        node->kind.exp = kind;
        node->scope = NULL;
        node->lineno = lineno;
        node->funcEntryLoc = 0;
    }
}

/* copyString 拷贝一个字符串并为其分配空间
 */
char *copyString(char *s) {
    if(s == NULL) { return NULL; }

    char *newString = NULL;
    newString = malloc(strlen(s) + 1);
    if(newString == NULL) {
        fprintf(listing, "Out of memory error at line %dn", lineno);
    } else {
        strcpy(newString, s);
    }
    return newString;
}


/* indentno 标识当前缩进空间
 */
static int indentno = 0;

#define INDENT (indentno += 2)
#define UNINDENT (indentno -= 2)

/* printSpace 通过打印空格来缩进
 */
static void printSpace(void) {
    int i;
    for(i = 0; i < indentno; i++) {
        fprintf(listing, " ");
    }
}

/* printTree 打印一棵syntax tree到listing file
 * 适当的缩进代表子树
 */
void printTree(TreeNode *tree) {
    INDENT;
    while(tree != NULL) {
        printSpace();
        if(tree->nodeKind == DeclK) {
            switch(tree->kind.decl) {
            case VarD:
                fprintf(listing, "Var: %s\n", tree->attr.name);
                break;
            case ArrD:
                fprintf(listing, "Array: %s\n", tree->attr.name);
                break;
            case FunD:
                fprintf(listing, "Fun: %s\n", tree->attr.name);
                break;
            default:
                fprintf(listing, "Unknown DeclNode kind\n");
                break;
            }
        } else if(tree->nodeKind == StmtK) {
            switch(tree->kind.stmt) {
            case ExpS:
                fprintf(listing, "Expression\n");
                break;
            case ComS:
                fprintf(listing, "Compound\n");
                break;
            case SelS:
                fprintf(listing, "Select\n");
                break;
            case IteS:
                fprintf(listing, "Iteration\n");
                break;
            case RetS:
                fprintf(listing, "Return\n");
                break;
            default:
                fprintf(listing, "Unknown StmtNode kind\n");
                break;
            }
        } else if(tree->nodeKind == ExpK) {
            switch(tree->kind.exp) {
            case AssignE:
                fprintf(listing, "Assign: ");
                printToken(tree->attr.op, "\0");
                break;
            case OpE:
                fprintf(listing, "Op: ");
                printToken(tree->attr.op, "\0");
                break;
            case ConstE:
                fprintf(listing, "Const: %d\n", tree->attr.val);
                break;
            case VarE:
                fprintf(listing, "Var: %s\n", tree->attr.name);
                break;
            case ArrE:
                fprintf(listing, "Array: %s\n", tree->attr.name);
                break;
            case CallE:
                fprintf(listing, "Call: %s\n", tree->attr.name);
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind\n");
                break;
            }
        } else {
            fprintf(listing, "Unknown node kind\n");
        }
        /* 递归调用child和sibling */
        int i;
        for(i = 0; i < MAXCHILDREN; i++) {
            printTree(tree->child[i]);
        }
        tree = tree->sibling;
    }
    UNINDENT;
}

