#include "stack.hpp"



int main() {

    stack_s st = {0};

    stackError err = E_OK;
    err = stackInit(&st, 0, NULL);
    if (err != E_OK) {
        printf("error\n");

    }

    for (size_t i = 0; i < 33; i++)
    {
        err = stackPush(&st, 1);
        if (err != E_OK){
            printf("\nerror %d\n", err);
            stackFree(&st);
            return 0;
        }
    }

    stackDump(&st, NULL);

    stackFree(&st);

}
