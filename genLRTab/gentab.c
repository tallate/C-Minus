#include "gentab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FirstSet *newFirstSet() {
    FirstSet *firstSet = (FirstSet*) malloc(sizeof(FirstSet));
    firstSet->firstNum = 0;
    return firstSet;
}
Symbol *newSymbol(char *lexeme, SymType symType) {
    Symbol *symbol = (Symbol*) malloc(sizeof(Symbol));
    strcpy(symbol->lexeme, lexeme);
    symbol->symType = symType;
    symbol->firstSet = newFirstSet();
    return symbol;
}
Product *newProduct() {
    Product *product = (Product*) malloc(sizeof(Product));
    product->left = 0;
    /* 初始化为-1表示该位置没有symbol */
    memset(product->symbols, -1, sizeof(product->symbols));
    product->symbolNum = 0;
    return product;
}
Item *newItem(int product, int dotPos, int lookahead) {
    Item *item = (Item*) malloc(sizeof(Item));
    item->product = product;
    item->dotPos = dotPos;
    item->lookahead = lookahead;
    return item;
}
ItemList *newItemList(Item *item) {
    ItemList *itemList = (ItemList*) malloc(sizeof(ItemList));
    itemList->item = item;
    itemList->next = NULL;
    return itemList;
}
State *newState() {
    State *state = (State*) malloc(sizeof(State));
    state->itemNum = 0;
    state->itemsFirst = state->itemsLast = NULL;
    memset(state->shifts, 0, sizeof(state->shifts));
    memset(state->reduces, 0, sizeof(state->reduces));
    memset(state->gotos, 0, sizeof(state->gotos));
    return state;
}

void deleteSymbol(Symbol *symbol) {
    free(symbol->firstSet);
    free(symbol);
}
void deleteProduct(Product *product) {
    free(product);
}
void deleteItem(Item *item) {
    free(item);
}
void deleteItemList(ItemList *itemList) {
    deleteItem(itemList->item);
    free(itemList);
}
void deleteState(State *state) {
    ItemList *cur = state->itemsFirst, *next;
    while(cur) {
        next = cur->next;
        deleteItemList(cur);
        cur = next;
    }
    free(state);
}
void clear() {
    for(int staPos = 0; staPos < stateNum; staPos++) {
        deleteState(states[staPos]);
    }
    for(int symPos = 0; symPos < symbolNum; symPos++) {
        deleteSymbol(symbols[symPos]);
    }
    for(int proPos = 0; proPos < productNum; proPos++) {
        deleteProduct(products[proPos]);
    }
}

int getDotPosSymbol(Product *product, int dotPos) {
    return product->symbols[dotPos];
}
int getSymbol(char *lexeme) {
    for(int pos = 0; pos < symbolNum; pos++) {
        if(! strcmp(symbols[pos]->lexeme, lexeme)) {
            return pos;
        }
    }
    return -1;
}


void addSymbol(Symbol *symbol) {
    symbols[symbolNum++] = symbol;
}
void addProduct(Product *product) {
    products[productNum++] = product;
}
int addState(State *state) {
    states[stateNum++] = state;
    return stateNum - 1;
}

void addItem(State *state, Item *item) {
    /* 判断是否与已有的重复 */
    ItemList *cur = state->itemsFirst;
    while(cur) {
        if(equalItem(cur->item, item)) {
            return ;
        }
        cur = cur->next;
    }
    /* 没有重复，添加到末尾 */
    if(state->itemsFirst) {
        state->itemsLast->next = newItemList(item);
        state->itemsLast = state->itemsLast->next;
    } else {
        state->itemsFirst = state->itemsLast = newItemList(item);
    }
    state->itemNum++;
}

int equalState(State *state1, State *state2) {
    if(state1->itemNum != state2->itemNum) {
        return 0;
    }
    /* 对state1的每个item，从state2中找相同的一个 */
    ItemList *itemList1 = state1->itemsFirst;
    while(itemList1) {
        ItemList *itemList2 = state2->itemsFirst;
        while(itemList2 && ! equalItem(itemList1->item, itemList2->item)) {
            itemList2 = itemList2->next;
        }
        if(! itemList2) {
            /* itemList2到头也没找到 */
            return 0;
        }
        itemList1 = itemList1->next;
    }
    return 1;
}
int equalItem(Item *item1, Item *item2) {
    return item1->product == item2->product &&
           item1->dotPos == item2->dotPos &&
           item1->lookahead == item2->lookahead;
}

void init() {
    symbolNum = productNum = stateNum = 0;
    symbols[symbolNum++] = newSymbol("$", TRML);
    addFirst(0, 0);
    symbols[symbolNum++] = newSymbol("Z", NONTRML);
}

void readIn(char *fname) {
    FILE *fin = fopen(fname, "r");
    if(fin == NULL) {
        error_exit("open file failed", 1);
    }

    int nontrmlNum = 0;
    char buf[BUFSIZ], *bufp, symLexeme[MAX_LEXEME];
    int offset;
    /* 读取nonterminals */
    fgets(buf, BUFSIZ, fin);
    bufp = buf;
    while(sscanf(bufp, "%s%n", symLexeme, &offset) == 1) {
        addSymbol(newSymbol(symLexeme, NONTRML));
        nontrmlNum++;
        bufp += offset;
    }
    /* 扩充grammar */
    Product *product = newProduct();
    product->left = 1;
    product->symbols[0] = 2;
    product->symbolNum = 1;
    addProduct(product);
    /* 读取terminals */
    fgets(buf, BUFSIZ, fin);
    bufp = buf;
    while(sscanf(bufp, "%s%n", symLexeme, &offset) == 1) {
        addSymbol(newSymbol(symLexeme, TRML));
        bufp += offset;
        addFirst(getSymbol(symLexeme), getSymbol(symLexeme));
    }
    /* 读取first set */
    for(int i = 0; i < nontrmlNum; i++) {
        fgets(buf, BUFSIZ, fin);
        bufp = buf;
        sscanf(bufp, "%s%n", symLexeme, &offset);
        bufp += offset;
        int sym = getSymbol(symLexeme);
        while(sscanf(bufp, "%s%n", symLexeme, &offset) == 1) {
            addFirst(sym, getSymbol(symLexeme));
            bufp += offset;
        }
    }
    /* 读取products */
    while(fgets(buf, BUFSIZ, fin)) {
        bufp = buf;
        sscanf(bufp, "%s %*s%n", symLexeme, &offset);
        bufp += offset;
        int pos;
        if((pos = getSymbol(symLexeme)) == -1) {
            error_exit("unknown symbol", 2);
        }
        Product *product = newProduct();
        product->left = pos;
        while(sscanf(bufp, "%s%n", symLexeme, &offset) == 1) {
            bufp += offset;
            if((pos = getSymbol(symLexeme)) == -1) {
                error_exit("unknown symbol", 2);
            }
            product->symbols[product->symbolNum++] = pos;
        }
        addProduct(product);
    }
}

void addFirst(int sym, int first) {
    FirstSet *firstSet = symbols[sym]->firstSet;
    firstSet->firsts[firstSet->firstNum++] = first;
}

/* 接下来由一个state的kernal item生成其所有item */
void genItems(State *state) {
    ItemList *cur = state->itemsFirst;
    while(cur) {
        Item *item = cur->item;
        /* 该item的ditPos处的symbol的地址 */
        int symPos = getDotPosSymbol(products[item->product], item->dotPos);
        /* [Z -> B., $] 直接忽略 */
        /* [Z -> .B@, $] */
        /* [Z -> .b@, $] */
        if(symPos != -1) {
            /* 对该nonterminal的每个product及每个First(@$)加一条 */
            int nextSymPos = getDotPosSymbol(products[item->product], item->dotPos + 1);
            if(nextSymPos == -1) {
                /* 直接取$ */
                Item *closureItem;
                for(int pPos = 0; pPos < productNum; pPos++) {
                    if(products[pPos]->left == symPos) {
                        /* 对该symPos处的symbol的每一关联product */
                        closureItem = newItem(pPos, 0, item->lookahead);
                        addItem(state, closureItem);
                    }
                }
            } else {
                /* 取所有First(@) */
                Item *closureItem;
                for(int pPos = 0; pPos < productNum; pPos++) {
                    if(products[pPos]->left == symPos) {
                        /* 对该symPos处的symbol的每一个关联product */
                        FirstSet *firstSet = symbols[nextSymPos]->firstSet;
                        for(int fPos = 0; fPos < firstSet->firstNum; fPos++) {
                            /* 对该firstSet的每一项 */
                            closureItem = newItem(pPos, 0, firstSet->firsts[fPos]);
                            addItem(state, closureItem);
                        }
                    }
                }
            }
        }
        cur = cur->next;
    }
}
/* dfs生成states */
void genStatesByDfs(int statePos) {
    State *curState = states[statePos];
    for(int symPos = 0; symPos < symbolNum; symPos++) {
        /* 对每个symbol */
        State *nextState = newState();
        ItemList *itemList = curState->itemsFirst;
        while(itemList) {
            /* 遍历这个state的items */
            Item *curItem = itemList->item;
            int curSymbol = getDotPosSymbol(products[curItem->product], curItem->dotPos);
            /* 与该item的lookahead相等且dotPos在最後面，加入一条归约 */
            if(symPos == curItem->lookahead && curSymbol == -1) {
                /* 判断是否存在reduce-reduce conflict */
                if(curState->reduces[symPos]) {
                    fprintf(stderr, "\n\nreduce-reduce conflict: state%d, %d %d\n\n",
                            statePos, curState->reduces[symPos], curItem->product);
                }
                curState->reduces[symPos] = curItem->product;
            }
            /* 如果该item的点处的symbol是当前的symbol */
            if(symPos == curSymbol) {
                /* 则新添一个kernal item */
                addItem(nextState,
                        newItem(curItem->product, curItem->dotPos + 1, curItem->lookahead));
            }
            itemList = itemList->next;
        }
        /* 不存在该转换 */
        if(! nextState->itemsFirst) {
            deleteState(nextState);
            continue;
        }
        /* 为这个state生成closure item */
        genItems(nextState);
        /* 判断这个新的state是否已经存在 */
        int isExist = 0;
        for(int staPos = 0; staPos < stateNum; staPos++) {
            if(equalState(nextState, states[staPos])) {
                /* 已经存在 */
                deleteState(nextState);
                if(symbols[symPos]->symType == TRML) {
                    curState->shifts[symPos] = staPos;
                } else {
                    curState->gotos[symPos] = staPos;
                }
                isExist = 1;
                break;
            }
        }
        if(! isExist) {
            /* 该state不存在 */
            int nextStatePos = addState(nextState);
            if(symbols[symPos]->symType == TRML) {
                curState->shifts[symPos] = nextStatePos;
            } else {
                curState->gotos[symPos] = nextStatePos;
            }
            genStatesByDfs(nextStatePos);
        }

    }
}
/* 接下来，如何生成State0，然后genStates */
void genStates() {
    /* 生成初始状态 */
    State *state0 = newState();
    Item *initItem = newItem(0, 0, 0);
    addItem(state0, initItem);
    genItems(state0);
    int statePos = addState(state0);
    /* dfs生成所有状态 */
    genStatesByDfs(statePos);
}

/* 打印工具函数 */
void printFirstSet(FirstSet *firstSet) {
    int fsPos;
    for(fsPos = 0; fsPos < firstSet->firstNum; fsPos++) {
        printSymbol(firstSet->firsts[fsPos]);
        printf(" ");
    }
}
void printFirstSets() {
    int symPos;
    printf("FirstSets: \n");
    for(symPos = 0; symPos < symbolNum; symPos++) {
        printSymbol(symPos);
        printf(" ");
        printFirstSet(symbols[symPos]->firstSet);
        printf("\n");
    }
}
void printSymbol(int pos) {
    printf("%s ", symbols[pos]->lexeme);
    if(symbols[pos]->symType == TRML) {
        printf("terminal");
    } else {
        printf("nonterminal");
    }
}
void printSymbols() {
    int i;
    printf("Symbols: \n");
    for(i = 0; i < symbolNum; i++) {
        printSymbol(i);
        printf("\n");
    }
}
void printProducts() {
    int ppos, spos;
    printf("Products: \n");
    for(ppos = 0; ppos < productNum; ppos++) {
        printSymbol(products[ppos]->left);
        printf(" -> ");
        for(spos = 0; spos < products[ppos]->symbolNum; spos++) {
            printSymbol(products[ppos]->symbols[spos]);
            printf(" ");
        }
        printf("\n");
    }
}
void printItem(Item *item) {
    printf("[");
    Product *product = products[item->product];
    printSymbol(product->left);
    printf(" -> ");
    for(int spos = 0; spos < product->symbolNum; spos++) {
        if(spos == item->dotPos) {
            printf(".");
        }
        printSymbol(product->symbols[spos]);
        printf(" ");
    }
    if(item->dotPos == product->symbolNum) {
        printf(".");
    }
    printf(", ");
    printSymbol(item->lookahead);
    printf("]");
}
void printStates() {
    int i;
    printf("States: \n");
    for(i = 0; i < stateNum; i++) {
        printf("State%d\n", i);
        ItemList *itemList = states[i]->itemsFirst;
        while(itemList) {
            printItem(itemList->item);
            printf("\n");
            itemList = itemList->next;
        }
    }
}
/* 打印最终结果 */
void printShifts() {
    printf("%5s", "shift");
    for(int symPos = 0; symPos < symbolNum; symPos++) {
        if(symbols[symPos]->symType == TRML) {
            printf("%5s", symbols[symPos]->lexeme);
        }
    }
    printf("\n");
    for(int staPos = 0; staPos < stateNum; staPos++) {
        // printf("%5d", staPos);
        for(int sftPos = 0; sftPos < symbolNum; sftPos++) {
            if(symbols[sftPos]->symType == TRML) {
                printf("%5d,", states[staPos]->shifts[sftPos]);
            }
        }
        printf("\n");
    }
}
void printReduces() {
    printf("reduc");
    for(int symPos = 0; symPos < symbolNum; symPos++) {
        if(symbols[symPos]->symType == TRML) {
            printf("%5s", symbols[symPos]->lexeme);
        }
    }
    printf("\n");
    for(int staPos = 0; staPos < stateNum; staPos++) {
        // printf("%5d", staPos);
        for(int redPos = 0; redPos < symbolNum; redPos++) {
            if(symbols[redPos]->symType == TRML) {
                printf("%5d,", states[staPos]->reduces[redPos]);
            }
        }
        printf("\n");
    }
}
void printGotos() {
    printf("gotos");
    for(int symPos = 0; symPos < symbolNum; symPos++) {
        if(symbols[symPos]->symType == NONTRML) {
            printf("%5s", symbols[symPos]->lexeme);
        }
    }
    printf("\n");
    for(int staPos = 0; staPos < stateNum; staPos++) {
        // printf("%5d", staPos);
        for(int goPos = 0; goPos < symbolNum; goPos++) {
            if(symbols[goPos]->symType == NONTRML) {
                printf("%5d,", states[staPos]->gotos[goPos]);
            }
        }
        printf("\n");
    }
}

void judge_shift_reduce_conflict() {
    for(int staPos = 0; staPos < stateNum; staPos++) {
        for(int symPos = 0; symPos < symbolNum; symPos++) {
            if(symbols[symPos]->symType == TRML &&
                    states[staPos]->shifts[symPos] &&
                    states[staPos]->reduces[symPos]) {
                fprintf(stderr, "shift-reduce conflict: state%d %s\n", staPos, symbols[symPos]->lexeme);
            }
        }
    }
}