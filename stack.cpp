/** @file */

#include <assert.h>
#include <stdlib.h>
#include "inc/stack.hpp"


static const size_t minCapacity = 8;

static stackError stackRealloc(stack_s *stack, size_t newCapacity){
    assert(stack != NULL);

    if (newCapacity < minCapacity)
        newCapacity = minCapacity;

    if (newCapacity == stack->capacity)
        return E_OK;

    char *buf = (char *)realloc((char *)stack->elems - sizeof(canary_t), 2 * sizeof(canary_t) + sizeof(elem_t) * newCapacity);
    if (buf == NULL)
        return E_ALLOC;

    stack->elems = (elem_t *)(buf + sizeof(canary_t));

    if (newCapacity > stack->capacity){
        for (size_t i = stack->capacity; i < newCapacity; i++)
            stack->elems[i] = STACK_POISON;
    } else {
        for (size_t i = newCapacity; i < stack->capacity; i++)
            stack->elems[i] = STACK_POISON;
    }

    stack->capacity = newCapacity;
    setDataCanary(buf, stack->capacity);

    if (stack->capacity < stack->sz)
        stack->sz = stack->capacity;

    stack->stackHash = calcStackHash(stack);

    return E_OK;
}


stackError stackInit(stack_s **st, size_t capacity, FILE *log){
    CHECK_BAD_POINTER(st)

    stack_s *stack = (stack_s *)calloc(1, sizeof(stack_s));
    if (stack == NULL)
        return E_ALLOC;

    *st = stack;

    if (capacity < minCapacity)
        capacity = minCapacity;

    char *buf = (char *)calloc(capacity, sizeof(elem_t) + 2 * sizeof(canary_t));
    if (buf == NULL)
        return E_ALLOC;

    stack->capacity = capacity;
    stack->elems = (elem_t *)(buf + sizeof(canary_t));

    stack->canaryLeft = stackCanaryLeft;
    stack->canaryRight = stackCanaryRight;
    setDataCanary(buf, stack->capacity);

    if (log == NULL)
        log = stderr;

    stack->log = log;
    stack->initialized = true;

    for (size_t i = 0; i < stack->capacity; i++)
        stack->elems[i] = STACK_POISON;

    stack->stackHash = calcStackHash(stack);

    return E_OK;
}


stackError stackFree(stack_s *stack){
    CHECK_BAD_POINTER(stack)
    CHECK_INITIALIZED(stack)

    for (size_t i = 0; i < stack->capacity; i++)
        stack->elems[i] = STACK_POISON;

    free((char *)stack->elems - sizeof(canary_t));

    stack->canaryLeft   = 0xABCD;
    stack->canaryRight  = 0xFFEC;
    stack->elems        = NULL;
    stack->log          = NULL;
    stack->capacity     = 0xDEDD;
    stack->sz           = 0xAAAF;

    stack->initialized = false;

    free(stack);

    return E_OK;
}

stackError stackResizeUp(stack_s *stack){
    CHECK_BAD_POINTER(stack)
    CHECK_INITIALIZED(stack)
    CHECK_CANARY(stack)
    CHECK_HASH(stack)

    return stackRealloc(stack, stack->capacity * 2);
}

stackError stackResizeDown(stack_s *stack){
    CHECK_BAD_POINTER(stack)
    CHECK_INITIALIZED(stack)
    CHECK_CANARY(stack)
    CHECK_HASH(stack)

    return stackRealloc(stack, stack->capacity / 2);
}


stackError stackPush(stack_s *stack, const elem_t el){
    CHECK_BAD_POINTER(stack)
    CHECK_INITIALIZED(stack)
    CHECK_CANARY(stack)
    CHECK_HASH(stack)

    stackError err = E_OK;

    if (stack->sz == stack->capacity) {
        err = stackResizeUp(stack);
        if (err != E_OK)
            return err;
    }


    stack->elems[stack->sz++] = el;

    stack->stackHash = calcStackHash(stack);

    return E_OK;
}


stackError stackPop(stack_s *stack, elem_t *el){
    CHECK_BAD_POINTER(stack)
    CHECK_INITIALIZED(stack)
    CHECK_CANARY(stack)
    CHECK_HASH(stack)

    if (stack->sz == 0)
        return E_EMPTY;


    *el = stack->elems[--stack->sz];

    stack->stackHash = calcStackHash(stack);

    return E_OK;
}

stackError stackPeek(const stack_s *stack, elem_t *el){
    CHECK_BAD_POINTER(stack)
    CHECK_INITIALIZED(stack)
    CHECK_CANARY(stack)
    CHECK_HASH(stack)

    if (stack->sz == 0)
        return E_EMPTY;

    *el = stack->elems[stack->sz - 1];

    return E_OK;
}

size_t stackGetSize(const stack_s *stack){
    return stack->sz;
}

size_t stackGetCapacity(const stack_s *stack){
    return stack->capacity;
}

stackError stackDump(const stack_s *stack, FILE *out){
    CHECK_BAD_POINTER(stack)
    CHECK_INITIALIZED(stack)

    if (out == NULL)
        out = stack->log;

    fprintf(out, "\n-------------------------\n");
    fprintf(out, "Stack Info:\n\n");

    fprintf(out, "Stack size: %zu\nStack capacity: %zu\n\n", stack->sz, stack->capacity);

    for (size_t i = 0; i < stack->capacity; i++){
        fprintf(out, "%08zx  ", i * sizeof(elem_t));

        uint8_t *elem = (uint8_t *)(stack->elems + i);
        for (size_t b = 0; b < sizeof(elem_t); b++){
            fprintf(out, "%02x ", elem[b]);
        }
        fputc('\n', out);
    }

#if _DEBUG
    fputc('\n', out);
    fprintf(out, "Stack structure address: \t%p\nStack data address: \t\t%p\n", stack, stack->elems);
    fprintf(out, "Stack left canary: \t\t0x%llX\n", stack->canaryLeft);
    fprintf(out, "Stack right canary: \t\t0x%llX\n", stack->canaryRight);

    char *data = (char *)stack->elems - sizeof(canary_t);
    canary_t leftDataCanary = *(canary_t *)data;
    canary_t rightDataCanary = *(canary_t *)(data + sizeof(canary_t) + stack->capacity * sizeof(elem_t));

    fprintf(out, "Stack Data left canary: \t0x%llX\n", leftDataCanary);
    fprintf(out, "Stack Data right canary: \t0x%llX\n", rightDataCanary);
#endif
    fprintf(out, "\n-------------------------\n");
    return E_OK;
}
