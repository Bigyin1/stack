/** @file */

#include <assert.h>
#include <stdlib.h>
#include "stack.hpp"

static const size_t minCapacity = 8;


static stackError stackRealloc(stack_s *stack, size_t newCapacity){
    assert(stack != NULL);
    assert(stack->elems != NULL);

    if (newCapacity < minCapacity)
        newCapacity = minCapacity;

    if (newCapacity == stack->capacity)
        return E_OK;

    void *nData = realloc(stack->elems, sizeof(elem_t) * newCapacity);
    if (nData == NULL)
        return E_ALLOC;

    stack->elems = (elem_t *)nData;
    stack->capacity = newCapacity;

    if (stack->capacity < stack->sz)
        stack->sz = stack->capacity;

    return E_OK;
}


stackError stackInit(stack_s *stack, size_t capacity){
    assert(stack != NULL);

    if (capacity < minCapacity)
        capacity = minCapacity;


    stack->elems = (elem_t *)calloc(capacity, sizeof(elem_t));
    if (stack->elems == NULL)
        return E_ALLOC;

    return E_OK;
}


stackError stackFree(stack_s *stack){
    assert(stack != NULL);

    free(stack->elems);

    return E_OK;
}

stackError stackResizeUp(stack_s *stack){
    assert(stack != NULL);

    return stackRealloc(stack, stack->capacity * 2);
}

stackError stackResizeDown(stack_s *stack){
    assert(stack != NULL);

    return stackRealloc(stack, stack->capacity / 2);
}


stackError stackPush(stack_s *stack, const elem_t el){
    assert(stack != NULL);

    stackError err = E_OK;

    if (stack->sz == stack->capacity) {
        err = stackResizeUp(stack);
        if (err != E_OK)
            return err;
    }


    stack->elems[stack->sz++] = el;
    return E_OK;
}


stackError stackPop(stack_s *stack, elem_t *el){
    assert(stack != NULL);

    if (stack->sz == 0)
        return E_EMPTY;


    *el = stack->elems[--stack->sz];

    return E_OK;
}

stackError stackPeek(const stack_s *stack, elem_t *el){
    assert(stack != NULL);

    if (stack->sz == 0)
        return E_EMPTY;


    *el = stack->elems[stack->sz - 1];

    return E_OK;
}


stackError stackDump(const stack_s *stack, FILE *out){
    assert(stack != NULL);
    assert(out != NULL);

    fprintf(out, "Stack Info\n\n");

    fprintf(out, "Stack size: %zu\nStack capacity: %zu\n", stack->sz, stack->capacity);
    fprintf(out, "Stack structure address: %p\nStack data address: %p\n", stack, stack->elems);

    return E_OK;
}
