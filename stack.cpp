/** @file */

#include <assert.h>
#include <stdlib.h>
#include "stack.hpp"


#define CHECK_BAD_POINTER(ptr) if (ptr == NULL) return E_BAD_POINTER;

static const size_t minCapacity = 8;
static const canary_t stackCanaryLeft =       0xBCBDBAB;
static const canary_t stackCanaryRight =      0xBCBAAAC;
static const canary_t stackDataCanaryLeft =   0xCAABDFF;
static const canary_t stackDataCanaryRight =  0xEEEBDAA;


static void setDataCanary(size_t capacity, char *data){
    assert(data != NULL);

    *(canary_t *)data = stackDataCanaryLeft;
    *(canary_t *)(data + sizeof(canary_t) + capacity * sizeof(elem_t)) = stackDataCanaryRight;
}

static stackError verifyCanary(const stack_s *stack){
    assert(stack != NULL);

    if (stack->canaryLeft != stackCanaryLeft || stack->canaryRight != stackCanaryRight)
        return E_STACK_CORRUPTED;

    char *data = (char *)stack->elems - sizeof(canary_t);

    canary_t leftCanary = *(canary_t *)data;
    canary_t rightCanary = *(canary_t *)(data + sizeof(canary_t) + capacity * sizeof(elem_t));

    if (leftCanary != stackDataCanaryLeft || rightCanary != stackDataCanaryRight)
        return E_STACK_CORRUPTED;

    return E_OK;
}

static stackError stackRealloc(stack_s *stack, size_t newCapacity){
    CHECK_BAD_POINTER(stack)
    CHECK_BAD_POINTER(stack->elems)

    if (newCapacity < minCapacity)
        newCapacity = minCapacity;

    if (newCapacity == stack->capacity)
        return E_OK;

    char *buf = (char *)realloc(stack->elems, 2 * sizeof(canary_t) + sizeof(elem_t) * newCapacity);
    if (buf == NULL)
        return E_ALLOC;

    stack->elems = (elem_t *)(buf + sizeof(canary_t));

    stack->capacity = newCapacity;
    setDataCanary(stack->capacity, buf);

    if (stack->capacity < stack->sz)
        stack->sz = stack->capacity;

    return E_OK;
}


stackError stackInit(stack_s *stack, size_t capacity){
    CHECK_BAD_POINTER(stack)

    if (capacity < minCapacity)
        capacity = minCapacity;

    char *buf = (char *)calloc(capacity, sizeof(elem_t) + 2 * sizeof(canary_t));
    if (buf == NULL)
        return E_ALLOC;

    stack->elems = (elem_t *)(buf + sizeof(canary_t));

    stack->canaryLeft = stackCanaryLeft;
    stack->canaryRight = stackCanaryRight;
    setDataCanary(stack->capacity, buf);

    return E_OK;
}


stackError stackFree(stack_s *stack){
    CHECK_BAD_POINTER(stack)

    free((char *)stack->elems - sizeof(canary_t));

    return E_OK;
}

stackError stackResizeUp(stack_s *stack){
    CHECK_BAD_POINTER(stack)

    return stackRealloc(stack, stack->capacity * 2);
}

stackError stackResizeDown(stack_s *stack){
    CHECK_BAD_POINTER(stack)

    return stackRealloc(stack, stack->capacity / 2);
}


stackError stackPush(stack_s *stack, const elem_t el){
    CHECK_BAD_POINTER(stack)

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
    CHECK_BAD_POINTER(stack)

    if (stack->sz == 0)
        return E_EMPTY;


    *el = stack->elems[--stack->sz];

    return E_OK;
}

stackError stackPeek(const stack_s *stack, elem_t *el){
    CHECK_BAD_POINTER(stack)

    if (stack->sz == 0)
        return E_EMPTY;


    *el = stack->elems[stack->sz - 1];

    return E_OK;
}


stackError stackDump(const stack_s *stack, FILE *out){
    CHECK_BAD_POINTER(stack)
    CHECK_BAD_POINTER(stack)

    fprintf(out, "Stack Info\n\n");

    fprintf(out, "Stack size: %zu\nStack capacity: %zu\n", stack->sz, stack->capacity);
    fprintf(out, "Stack structure address: %p\nStack data address: %p\n", stack, stack->elems);

    return E_OK;
}
