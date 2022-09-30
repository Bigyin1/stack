/** @file */

#include <assert.h>
#include <stdlib.h>
#include "stack.hpp"


#define CHECK_INITIALIZED(stack) {if (stack->initialized == false) return E_NOT_INITIALIZED;}

#define CHECK_BAD_POINTER(ptr) {if ((ptr) == NULL) return E_BAD_POINTER;}

#define CHECK_CANARY(stack)              \
{                                        \
    stackError err = verifyCanary(stack);\
    if (err != E_OK){                   \
        stackDump(stack, NULL);         \
        return err;                     \
    }                                    \
}

#define CHECK_HASH(stack)              \
{                                      \
    stackError err = verifyHash(stack);\
   if (err != E_OK){                    \
        stackDump(stack, NULL);        \
        return err;                     \
    }                                    \
}



static const size_t minCapacity = 8;

static const canary_t stackCanaryLeft =       0xDEDEDED;
static const canary_t stackCanaryRight =      0xBCBAAAC;
static const canary_t stackDataCanaryLeft =   0xCAABDFF;
static const canary_t stackDataCanaryRight =  0xEEEBDAA;

const elem_t STACK_POISON = (elem_t)0xABDEEAF0;

static hash_t calcHash(const char *data, size_t sz){
    assert(data != NULL);
    assert(sz != 0);

    hash_t hash = 0;

    for (size_t i = 0; i < sz; i++){
        hash += (hash_t)data[i];
        hash += 111;
        hash += sz * i;
    }

    return hash;
}

static hash_t calcStackHash(const stack_s *stack){
    assert(stack != NULL);

    hash_t hash = 0;

    hash += calcHash((const char *)stack->elems, stack->capacity * sizeof(elem_t));

    hash += calcHash((const char *)&stack->capacity, sizeof(stack->capacity));
    hash += calcHash((const char *)&stack->sz, sizeof(stack->sz));
    hash += calcHash((const char *)&stack->elems, sizeof(stack->elems));
    hash += calcHash((const char *)&stack->initialized, sizeof(stack->initialized));
    hash += calcHash((const char *)&stack->log, sizeof(stack->log));

    return hash;
}


static stackError verifyHash(const stack_s *stack){
    assert(stack != NULL);

    if (calcStackHash(stack) != stack->stackHash)
        return E_STACK_CORRUPTED;

    return E_OK;
}


static void setDataCanary(char *data, size_t capacity){
    assert(data != NULL);

    *(canary_t *)data = stackDataCanaryLeft;
    *(canary_t *)(data + sizeof(canary_t) + capacity * sizeof(elem_t)) = stackDataCanaryRight;
}


static stackError verifyCanary(const stack_s *stack){
    assert(stack != NULL);

    if (stack->canaryLeft != stackCanaryLeft || stack->canaryRight != stackCanaryRight)
        return E_STACK_CORRUPTED;

    char *data = (char *)stack->elems - sizeof(canary_t);

    canary_t leftDataCanary = *(canary_t *)data;
    canary_t rightDataCanary = *(canary_t *)(data + sizeof(canary_t) + stack->capacity * sizeof(elem_t));

    if (leftDataCanary != stackDataCanaryLeft || rightDataCanary != stackDataCanaryRight)
        return E_STACK_CORRUPTED;

    return E_OK;
}

static stackError stackRealloc(stack_s *stack, size_t newCapacity){
    assert(stack != NULL);
    assert(stack->elems != NULL);

    if (newCapacity < minCapacity)
        newCapacity = minCapacity;

    if (newCapacity == stack->capacity)
        return E_OK;

    char *buf = (char *)realloc((char *)stack->elems - sizeof(canary_t), 2 * sizeof(canary_t) + sizeof(elem_t) * newCapacity);
    if (buf == NULL)
        return E_ALLOC;

    stack->elems = (elem_t *)(buf + sizeof(canary_t));

    stack->capacity = newCapacity;
    setDataCanary(buf, stack->capacity);

    if (stack->capacity < stack->sz)
        stack->sz = stack->capacity;

    stack->stackHash = calcStackHash(stack);

    return E_OK;
}


stackError stackInit(stack_s *stack, size_t capacity, FILE *log){
    CHECK_BAD_POINTER(stack)

    if (stack->initialized)
        return E_OK;

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


stackError stackDump(const stack_s *stack, FILE *out){
    CHECK_BAD_POINTER(stack)
    CHECK_INITIALIZED(stack)

    if (out == NULL)
        out = stack->log;

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
    fprintf(out, "Stack left canary: \t\t0x%X\n", stack->canaryLeft);
    fprintf(out, "Stack right canary: \t\t0x%X\n", stack->canaryRight);
#endif

    return E_OK;
}
