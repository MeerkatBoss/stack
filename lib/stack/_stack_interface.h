#ifndef CUSTOM_STACK_H
#define CUSTOM_STACK_H

/**
 * @file _stack_interface.h
 * @author MeerkatBoss
 * @brief Stack data structure function definitions
 * @version 0.1
 * @date 2022-09-25
 * 
 * @warning This header is internal, It IS NOT supposed
 * to be included outside the scope of the library.
 * 
 */

#include <stddef.h>

#include "utils.h"

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

    #define IS_POISON(element)      ((element) == NULL)

    /**
     * @brief 
     * Print given element
     * 
     * @param[in] element printed element
     */
    #define PRINT_ELEMENT(element)  printf("%p", (element))
#endif

#define STK_CANARY_PROT 01
#define STK_HASH_PROT   02
#define STK_DEBUG_INFO  04 

#ifndef STK_PROT_LEVEL
#define STK_PROT_LEVEL STK_CANARY_PROT | STK_HASH_PROT | STK_DEBUG_INFO
#endif

#if (STK_PROT_LEVEL & STK_CANARY_PROT)
    #define _ON_CANARY(...) __VA_ARGS__
    #define _NO_CANARY(...)
#else
    #define _ON_CANARY(...)
    #define _NO_CANARY(...) __VA_ARGS__
#endif

#if (STK_PROT_LEVEL & STK_HASH_PROT)
    #define _ON_HASH(...) __VA_ARGS__
    #define _NO_HASH(...)
#else
    #define _ON_HASH(...)
    #define _NO_HASH(...) __VA_ARGS__
#endif

#if (STK_PROT_LEVEL & STK_DEBUG_INFO)
    #define _ON_DEBUG_INFO(...) __VA_ARGS__
    #define _NO_DEBUG_INFO(...)
#else
    #define _ON_DEBUG_INFO(...)
    #define _NO_DEBUG_INFO(...) __VA_ARGS__
#endif

#ifndef NSTACK_CHECK
    #define _ON_STACK_CHECK(...) __VA_ARGS__
    #define _NO_STACK_CHECK(...)
#else
    #define _ON_STACK_CHECK(...)
    #define _NO_STACK_CHECK(...) __VA_ARGS__
#endif

_ON_CANARY(
    typedef unsigned long long canary_t;

    const canary_t CANARY = 0xD1AB011CA1C0C0A5ULL;  /* DIABOLICAL COCOAS*/
)

_ON_HASH(
    typedef unsigned long long hash_t;
)

enum ErrorFlags
{ 
    STK_NO_ERROR        = 0000,
    STK_EMPTY           = 0001,
    STK_NO_MEMORY       = 0002,
    STK_CORRUPTED_SIZE  = 0004,
    STK_CORRUPTED_CAP   = 0010,
    STK_CORRUPTED_DATA  = 0020,
    STK_DEAD_CANARY     = 0040,
    STK_WRONG_HASH      = 0100,
    STK_BAD_DATA_PTR    = 0200,
    STK_BAD_PTR         = 0400
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
    _ON_HASH(       hash_t          _hash;)
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
 * 
 * @return zero upon successful construction, non-zero otherwise
 */
#define StackCtor(       stack) _StackCtor(stack,\
                                    #stack + (*#stack == '&'),\
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
unsigned int StackPush  (Stack* stack, ELEMENT value);

/**
 * @brief 
 * Remove top element from stack
 * 
 * @param[inout] stack `Stack` instance
 * @return zero upon success, some combination of
 * `ErrorFlags` otherwise
 */
unsigned int StackPop   (Stack* stack);

/**
 * @brief
 * Remove top element from stack
 * 
 * @param[inout] stack `Stack` instance
 * @param[out] err Error code, i.e some combination of
 * `ErrorFlags`. Parameter is ignored if set to NULL
 * @return Removed value
 */
ELEMENT StackPopCopy    (Stack* stack, unsigned int* err);

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
ELEMENT* StackPeek      (const Stack* stack, unsigned int* err);

/**
 * @brief
 * Get top element from stack
 * 
 * @param[inout] stack `Stack` instance
 * @param[out] err Error code, i.e some combination of
 * `ErrorFlags`. Parameter is ignored if set to NULL
 * @return Element value
 */
ELEMENT StackPeekCopy   (const Stack* stack, unsigned int* err);  

#endif