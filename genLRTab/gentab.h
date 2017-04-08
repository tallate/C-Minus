#ifndef GENLRTAB_GENTAB_H
#define GENLRTAB_GENTAB_H

#include <openacc.h>

#define MAX_LEXEME 100
#define MAX_SYMBOL 100
#define MAX_PRODUCT 100
#define MAX_STATE 100

#define error_exit(m, x) { fprintf(stderr, m); exit(x); }

typedef enum SymType {
    TRML, NONTRML
} SymType;

typedef struct FirstSet {
    int firsts[MAX_SYMBOL];
    int firstNum;
} FirstSet;

typedef struct Symbol {
    char lexeme[MAX_LEXEME];
    SymType symType;
    FirstSet *firstSet;
} Symbol;

typedef struct Product {
    int left;
    int symbols[MAX_SYMBOL];
    int symbolNum;
} Product;

typedef struct Item {
    int product;
    int dotPos;
    int lookahead;
} Item;

typedef struct ItemList {
    Item *item;
    struct ItemList *next;
} ItemList;

typedef struct State {
    ItemList *itemsFirst;
    ItemList *itemsLast;
    int itemNum;
    int shifts[MAX_SYMBOL]; /* states的地址 */
    int reduces[MAX_SYMBOL]; /* products的地址 */
    int gotos[MAX_SYMBOL]; /* states的地址 */
} State;

/* 全局变量 */
static Symbol *symbols[MAX_SYMBOL] = { 0 };
static Product *products[MAX_PRODUCT] = { 0 };
static State *states[MAX_STATE] = { 0 };

int symbolNum, productNum, stateNum;

/* 构造器
 */
FirstSet *newFirstSet();
Symbol *newSymbol(char *lexeme, SymType symType);
Product *newProduct();
Item *newItem(int product, int dotPos, int lookahead);
ItemList *newItemList(Item *item);
State *newState();

/* 资本主义改造器
 */
void deleteSymbol(Symbol *symbol);
void deleteProduct(Product *product);
void deleteItem(Item *item);
void deleteItemList(ItemList *itemList);
void deleteState(State *state);
void clear();

/* init 初始化全局变量
 */
void init();
/* readIn 读取文件
 * 文件格式在input.txt给出
 */
void readIn(char *fname);

void addSymbol(Symbol *symbol);
void addProduct(Product *product);
int addState(State *state);

/* getDotPosSymbol 返回product的点处的symbol的地址
 * 找到返回其位置，否则返回-1
 */
int getDotPosSymbol(Product *product, int dotPos);
/* getSymbol 由一个symbol的字面量获得其地址
 * 若找到返回其地址（symbols下标），否则返回-1
 */
int getSymbol(char *lexeme);

/* addFirst 添加一个first到一个symbol的first set
 */
void addFirst(int sym, int first);

/* addItem 将一个item添加到state的itemList的末尾
 * 注意可能存在重复的item，直接忽略
 */
void addItem(State *state, Item *item);

/* equalState 判断两个state是否相等
 * 即判断其item数量是否相等，每个item是否都相同
 */
int equalState(State *state1, State *state2);
/* equalItem 判断两个item是否相等
 * 即判断其product、dotPos、lookahead是否都相等
 */
int equalItem(Item *item1, Item *item2);

/* genItems 为一个state生成所有item
 * 该state起始必须包含所有kernal item
 */
void genItems(State *state);
/* genStatesByDfs dfs生成所有states
 */
void genStatesByDfs(int statePos);
/* genStates 生成所有状态到全局变量states中
 * 先生成state0,然后调用genStatesByDfs生成其余states
 */
void genStates();

/* 测试用函数
 */
void printFirstSet(FirstSet *firstSet);
void printFirstSets();
void printSymbol(int pos);
void printSymbols();
void printProducts();
void printItem(Item *item);
void printStates();

/* 打印结果
 */
void printShifts();
void printReduces();
void printGotos();

/* judge_shift_reduce_conflict 测试是否存在shift-reduce冲突
 */
void judge_shift_reduce_conflict();

#endif //GENLRTAB_GENTAB_H
