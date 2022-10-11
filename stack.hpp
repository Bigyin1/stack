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

/// @brief Main Stack structure
struct stack_s;

typedef struct stack_s stack_s;

typedef __uint64_t elem_t;

/// @brief Stack initialization must be performed before usage.
/// @param stack pointer to pointer to stack structure.
/// @param capacity // Prefferred capacity. Min capacity is 8 elements.
/// @param log // file to log in stack dump
/// @return
stackError stackInit(stack_s **stack, size_t capacity, FILE *log);

/// @brief Frees stack buffer and stack structure
/// @param stack
/// @return
stackError stackFree(stack_s *stack);

/// @brief Increases stack's capacity by two
/// @param stack
/// @return
stackError stackResizeUp(stack_s *stack);

/// @brief Decreases stack's capacity by two
/// @param stack
/// @return
stackError stackResizeDown(stack_s *stack);

/// @brief
/// @param stack
/// @param el
/// @return
stackError stackPush(stack_s *stack, const elem_t el);

/// @brief
/// @param stack
/// @param el
/// @return
stackError stackPop(stack_s *stack, elem_t *el);

/// @brief
/// @param stack
/// @param el
/// @return
stackError stackPeek(const stack_s *stack, elem_t *el);

/// @brief
/// @param stack
/// @param out
/// @return
stackError stackDump(const stack_s *stack, FILE *out);

/// @brief
/// @param stack
/// @return
size_t stackGetSize(const stack_s *stack);

/// @brief
/// @param stack
/// @return
size_t stackGetCapacity(const stack_s *stack);


#endif
