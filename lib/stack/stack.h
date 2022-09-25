#ifndef CUSTOM_STACK_H
#define CUSTOM_STACK_H

/**
 * @file stack.h
 * @author MeerkatBoss
 * @brief Stack data structure
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * @note For usage with any desired type define
 * `ELEMENT`, `ELEMENT_POISON` and `PRINT_ELEMENT(element)`
 * macros
 * 
 * @warning DO NOT include this header in other headers.
 * This header DOES NOT support separate compilation
 * 
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef ELEMENT
/**
 * @brief 
 * Stack element type
 */
#define ELEMENT                 void*

/**
 * @brief 
 * Invalid element value
 */
#define ELEMENT_POISON          NULL

/**
 * @brief 
 * Print given element
 * 
 * @param[in] element printed element
 */
#define PRINT_ELEMENT(element)  printf("%p", element)
#endif

#ifndef NSTACK_CANARY
    #define _ON_CANARY(...) __VA_ARGS__
    #define _NO_CANARY(...)
#else
    #define _ON_CANARY(...)
    #define _NO_CANARY(...) __VA_ARGS__
#endif

#ifndef NSTACK_DEBUG_INFO
    #define _ON_DEBUG_INFO(...) __VA_ARGS__
    #define _NO_DEBUG_INFO(...)
#else
    #define _ON_DEBUG_INFO(...)
    #define _NO_DEBUG_INFO(...) __VA_ARGS__
#endif

typedef unsigned long long canary_t;

const canary_t CANARY = 0xD1AB011CA1C0C0A5ULL;  /* DIABOLICAL COCOAS*/

enum ErrorFlags
{ 
    STK_NO_ERROR        = 000,
    STK_EMPTY           = 001,
    STK_NO_MEMORY       = 002,
    STK_CORRUPTED_SIZE  = 004,
    STK_CORRUPTED_CAP   = 010,
    STK_CORRUPTED_DATA  = 020,
    STK_DEAD_CANARY     = 040
};

_ON_DEBUG_INFO(
    /**
     * @brief 
     * Debug information about variable
     */
    struct _debug_info
    {
        const char *name;       /* variable name */
        const char *func_name;  /* declaring function name */
        const char *file_name;  /* declaring file name */
        size_t      line_num;   /* declaration line */
    };
)

/**
 * @brief 
 * LIFO data structure
 */
struct Stack
{
    _ON_CANARY(     canary_t        canary_start;)
                    ELEMENT*        data;           /* stored elements */
                    size_t          size;           /* stored elements count*/
                    size_t          capacity;       /* maximum capacity */
    _ON_DEBUG_INFO( _debug_info     _debug;)        
    _ON_CANARY(     canary_t        canary_end;)
};

/**
 * @brief 
 * Construct `Stack` instance from parameters
 * @param[out] stack     constructed instance
 * @param[in]  name      [debug-only] variable name
 * @param[in]  func_name [debug-only] declaring function name
 * @param[in]  file_name [debug-only] declaring file name
 * @param[in]  line_num  [debug-only] declaration line
 * @return zero upon successful construction, non-zero otherwise
 */
int     _StackCtor      (Stack* stack,
                        const char* name,
                        const char* func_name,
                        const char* file_name,
                        size_t line_num);

/**
 * @brief 
 * Construct `Stack`
 * 
 * @param[out] stack constructed instance
 */
#define StackCtor       (stack) _StackCtor(stack, #stack,\
                                    __PRETTY_FUNCTION__,\
                                    __FILE__, __LINE__);

/**
 * @brief 
 * Clean up `Stack` instance. Free associated resources
 * 
 * @param[inout] stack instance to be cleaned up
 */
void    StackDtor       (Stack* stack);

/**
 * @brief
 * Add element to stack
 * 
 * @param[inout] stack `Stack` instance
 * @param[in] value    value to be added
 * @return zero upon success, some combination of
 * `ErrorFlags` otherwise
 */
int     StackPush       (Stack* stack, ELEMENT value);

/**
 * @brief 
 * Remove top element from stack
 * 
 * @param[inout] stack `Stack` instance
 * @return zero upon success, some combination of
 * `ErrorFlags` otherwise
 */
int     StackPop        (Stack* stack);

/**
 * @brief
 * Remove top element from stack
 * 
 * @param[inout] stack `Stack` instance
 * @param[out] err Error code, i.e some combination of
 * `ErrorFlags`. Parameter is ignored if set to NULL
 * @return Removed value
 */
ELEMENT StackPopCopy    (Stack* stack, int* err = NULL);

/**
 * @brief
 * Get top element from stack
 * 
 * @param[inout] stack `Stack` instance
 * @param[out] err Error code, i.e some combination of
 * `ErrorFlags`. Parameter is ignored if set to NULL
 * @return Pointer to element in stack
 * 
 * @warning Pointer invalidates after call to `StackPop`
 * with the same `Stack` instance
 */
ELEMENT* StackPeek      (Stack* stack, int* err = NULL);

/**
 * @brief
 * Get top element from stack
 * 
 * @param[inout] stack `Stack` instance
 * @param[out] err Error code, i.e some combination of
 * `ErrorFlags`. Parameter is ignored if set to NULL
 * @return Element value
 */
ELEMENT StackPeekCopy   (const Stack* stack, int* err = NULL);  



/**
 * @brief 
 * Check stack integrity
 * 
 * @param[in] stack `Stack` instance
 * @return zero upon success, some combination of
 * `ErrorFlags` otherwise
 */
int    _StackCheck      (const Stack* stack);

/**
 * @brief
 * Print stack contents
 * 
 * @param[in] stack `Stack` instance
 */
void   _StackDump       (const Stack* stack);

const double _stack_growth = 1.7;
int         _StackEnsurePushable(Stack* stack);
int         _StackTryShrink     (Stack* stack);
ELEMENT*    _ReallocWithCanary  (ELEMENT* old_array,
                                size_t old_size,
                                size_t new_size);

ELEMENT* _ReallocWithCanary  (ELEMENT* old_array,
                                size_t old_size,
                                size_t new_size)
{
    const size_t canary_size = sizeof(canary_t);
    if (old_array == NULL) old_size = 0;

    /* Allocate result */
    ELEMENT* result = (ELEMENT*)(
        (char*)realloc(
            /* Calculate real array start*/
            old_array
                ? (char*)old_array - canary_size    /* get real beginning */
                : NULL,                             /* allocate new array */
            /* Ensure there is enough space for canaries */
            new_size*sizeof(ELEMENT) + 2*canary_size)
        + canary_size);
    
    if (result == NULL)
        return NULL;

    /* Fill new elements (if any) with poison*/
    for (size_t i = old_size; i < new_size; i++)
        result[i] = ELEMENT_POISON;

    /* Set canaries before and after array*/
    ((canary_t*) result)[-1]        = CANARY;
    *(canary_t*)(result + new_size) = CANARY;

    return result;
}

int _StackEnsurePushable(Stack* stack)
{
    if (stack->size < stack->capacity)
        return 0;
    
    size_t new_capacity = round(stack->capacity * _stack_growth);
    
    ELEMENT* new_data = _ReallocWithCanary(
                                        stack->data,
                                        stack->capacity,
                                           new_capacity);
    if (new_data == NULL)
        return -1;
    
    stack->data     = new_data;
    stack->capacity = new_capacity;

    return 0;
}


#endif