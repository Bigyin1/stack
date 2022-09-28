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
    E_OK,       // No error occured
    E_ALLOC,    // Memory allocation error
    E_EMPTY,
};

typedef enum stackError stackError;

typedef int elem_t;

/**
 * @brief
 */
struct stack_s
{
    elem_t  *elems;
    size_t  sz;
    size_t  capacity;

};

typedef struct stack_s stack_s;


stackError stackInit(stack_s *stack, size_t capacity);

stackError stackFree(stack_s *stack);

stackError stackResizeUp(stack_s *stack);

stackError stackResizeDown(stack_s *stack);

stackError stackPush(stack_s *stack, const elem_t el);

stackError stackPop(stack_s *stack, elem_t *el);

stackError stackPeek(const stack_s *stack, elem_t *el);

stackError stackDump(const stack_s *stack, FILE *out);





#endif
