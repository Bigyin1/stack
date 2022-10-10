#include <assert.h>
#include "../inc/stack.hpp"


const canary_t stackCanaryLeft =       0xDEDEDED;
const canary_t stackCanaryRight =      0xBCBAAAC;
const canary_t stackDataCanaryLeft =   0xCAABDFF;
const canary_t stackDataCanaryRight =  0xEEEBDAA;

const elem_t STACK_POISON = (elem_t)0xEDEEDEF0FF;

hash_t calcHash(const char *data, size_t sz){
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

hash_t calcStackHash(const stack_s *stack){
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


stackError verifyHash(const stack_s *stack){
    assert(stack != NULL);

    if (calcStackHash(stack) != stack->stackHash)
        return E_STACK_CORRUPTED;

    return E_OK;
}

void setDataCanary(char *data, size_t capacity){
    assert(data != NULL);

    *(canary_t *)data = stackDataCanaryLeft;
    *(canary_t *)(data + sizeof(canary_t) + capacity * sizeof(elem_t)) = stackDataCanaryRight;
}


stackError verifyCanary(const stack_s *stack){
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
