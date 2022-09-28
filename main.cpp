#include "stack.hpp"



int main() {

    stack_s st = {0};

    stackInit(&st, 0);

    stackPush(&st, 1);
    stackPush(&st, 1);
    stackPush(&st, 1);

    stackDump(&st, stdout);

    stackFree(&st);

}
