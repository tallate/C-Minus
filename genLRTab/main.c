#include <stdio.h>
#include <stdlib.h>
#include "gentab.h"

#define TEST 1

int main(int ac, char *av[]) {
    if(ac != 2) {
        error_exit("usage: gentab fname", 1);
    }

    init();
    readIn(av[1]);
    genStates();

#if TEST
    printSymbols();
    printFirstSets();
    printProducts();
    printStates();
#endif

    printShifts();
    printReduces();
    printGotos();

    judge_shift_reduce_conflict();

    clear();

    printf("\n————done————\n");
    return 0;
}
