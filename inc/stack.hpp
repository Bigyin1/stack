/** @file */
#ifndef STACK_PRIVATE_HPP
#define STACK_PRIVATE_HPP

#include <stddef.h>
#include <stdio.h>
#include "../stack.hpp"

typedef unsigned long int hash_t;

typedef long long unsigned int canary_t;

/// @brief
struct stack_s
{
    canary_t canaryLeft;
    FILE *log;

    elem_t  *elems;
    size_t  sz;
    size_t  capacity;

    bool initialized;

    hash_t  stackHash;
    canary_t canaryRight;
};

typedef struct stack_s stack_s;


extern const canary_t stackCanaryLeft;
extern const canary_t stackCanaryRight;
extern const canary_t stackDataCanaryLeft;
extern const canary_t stackDataCanaryRight;

extern const elem_t STACK_POISON;

stackError verifyHash(const stack_s *stack);

void setDataCanary(char *data, size_t capacity);

stackError verifyCanary(const stack_s *stack);

hash_t calcHash(const char *data, size_t sz);

hash_t calcStackHash(const stack_s *stack);



#define CHECK_INITIALIZED(stack) {if (stack->initialized == false) return E_NOT_INITIALIZED;}

#define CHECK_BAD_POINTER(ptr) {if ((ptr) == NULL) return E_BAD_POINTER;}

#define CHECK_CANARY(stack)              \
{                                        \
    stackError err = verifyCanary(stack);\
    if (err != E_OK){                    \
        return err;                      \
    }                                    \
}

#define CHECK_HASH(stack)               \
{                                       \
    stackError err = verifyHash(stack); \
   if (err != E_OK){                    \
        return err;                     \
    }                                   \
}

#endif
