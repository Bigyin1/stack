/** @file */
#ifndef STACK_HPP
#define STACK_HPP

#include <stddef.h>
#include <stdio.h>

/**
 * @brief Holds error number.
 */
enum stackError
{
    E_OK,               // No error occured
    E_ALLOC,            // Memory allocation error
    E_EMPTY,            // Dtack is empty
    E_STACK_CORRUPTED,  // Security breach occured
    E_BAD_POINTER,      // NULL pointer was passed to stack method
    E_NOT_INITIALIZED,  // Stack is not initialized
};

typedef enum stackError stackError;

typedef int elem_t;
typedef unsigned long int hash_t;

typedef unsigned int canary_t;

/**
 * @brief
 */
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


stackError stackInit(stack_s *stack, size_t capacity, FILE *log);

stackError stackFree(stack_s *stack);

stackError stackResizeUp(stack_s *stack);

stackError stackResizeDown(stack_s *stack);

stackError stackPush(stack_s *stack, const elem_t el);

stackError stackPop(stack_s *stack, elem_t *el);

stackError stackPeek(const stack_s *stack, elem_t *el);

stackError stackDump(const stack_s *stack, FILE *out);





#endif
