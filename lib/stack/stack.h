#ifndef CUSTOM_STACK_IMPL
#define CUSTOM_STACL_IMPL

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
 * `ELEMENT`, `ELEMENT_POISON`, `IS_POISON(element)` and `PRINT_ELEMENT(element)`
 * macros
 * 
 * @warning DO NOT include this header in other headers.
 * This header DOES NOT support separate compilation
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "_stack_interface.h"

_ON_STACK_CHECK(

    /**
     * @brief 
     * Check stack integrity
     * 
     * @param[in] stack `Stack` instance
     * @param[in] func  calling fuction name
     * @param[in] file  calling file name
     * @param[in] line  calling line number
     * @return zero upon success, some combination of
     * `ErrorFlags` otherwise
     */
    int    _StackCheck      (const Stack* stack,
                            const char*  func,
                            const char*  file,
                                size_t line);

    /**
     * @brief 
     * Check stack integrity
     * 
     * @param[in] stack `Stack` instance
     * @return zero upon success, some combination of
     * `ErrorFlags` otherwise
     */
    #define StackCheck(      stack) _StackCheck(stack,\
                                                __PRETTY_FUNCTION__,\
                                                __FILE__, __LINE__);
    /**
     * @brief 
     * Print `Stack` contents
     * 
     * @param[in] stack `Stack` instance
     * @param[in] func  calling fuction name
     * @param[in] file  calling file name
     * @param[in] line  calling line number
     */
    void   _StackDump       (const Stack* stack,
                            const char*  func,
                            const char*  file,
                                size_t line,
                                int    errs);

    /**
     * @brief 
     * Print `Stack` contents
     * 
     * @param[in] stack `Stack` instance
     */
    #define StackDump(       stack, errs) _StackDump(stack,\
                                                __PRETTY_FUNCTION__,\
                                                __FILE__, __LINE__, errs);
)

/**
 * @brief 
 * Stack capacity growth coefficient
 */
const double _stack_growth = 1.7;

/**
 * @brief 
 * Ensure stack is ready to accept new element
 * @param[inout] stack `Stack` instance
 * @return zero upon success, non-zero otherwise
 */
int         _StackEnsurePushable(Stack* stack);

/**
 * @brief 
 * Ensure stack has no eccess capacity
 * @param[inout] stack `Stack` instance
 * @return zero upon success, non-zero otherwise
 */
int         _StackTryShrink     (Stack* stack);

/**
 * @brief 
 * (Re)allocate array with size `new_size`,
 * add canaries before array start and after
 * its end.
 * @param[inout] old_array pointer to beginning of an
 * array, returned by call to `_ReallocWithCanary` or
 * `NULL`
 * @param[in] old_size length of old array. This parameter
 * is ignored if `old_array` is `NULL`
 * @param[in] new_size required new length of array
 * @return pointer to beginning of allocated array
 * or `NULL` upon failure
 * 
 * @note If `old_array` is not `NULL` coresponding memory
 * will be freed.
 * 
 * @warning If `old_array` is not `NULL` and it
 * was not returned by previous call to `_ReallocWithCanary`,
 * the result is undefined
 */
ELEMENT*    _ReallocWithCanary  (ELEMENT* old_array,
                                size_t old_size,
                                size_t new_size);
int _StackCtor(Stack* stack,
                const char* name,
                const char* func_name,
                const char* file_name,
                size_t line_num)
{
    const size_t default_cap = 16;
    ELEMENT* data = _ReallocWithCanary(NULL, 0, default_cap);

    if (!data)
        return -1;
    
    *stack = {
        _ON_CANARY(     .canary_start   = CANARY,)  //TODO: CANARY ^ (canary_t)stack
                        .data           = data,
                        .size           = 0,
                        .capacity       = default_cap,
        _ON_DEBUG_INFO(
                        ._debug         = {
                            .name       = name,
                            .func_name  = func_name,
                            .file_name  = file_name,
                            .line_num   = line_num
                        },
        )
        _ON_CANARY(     .canary_end     = CANARY)
    };
    
    return 0;
}

void    StackDtor       (Stack* stack)
{
    _ON_STACK_CHECK(
        int errs = StackCheck(stack);
        if (errs)
        {
            StackDump(stack, errs);
            return;
        }
    )
    free(stack->data);
    *stack = {};
}

int     StackPush       (Stack* stack, ELEMENT value)
{
    _ON_STACK_CHECK(
        int errs = StackCheck(stack);
        if (errs)
        {
            StackDump(stack, errs);
            return errs;
        }
    )
    int can_push = _StackEnsurePushable(stack);
    if (!can_push) return STK_NO_MEMORY;
    
    stack->data[stack->size++] = value;
    return STK_NO_ERROR;
}

int     StackPop        (Stack* stack)
{
    _ON_STACK_CHECK(
        int errs = StackCheck(stack);
        if (errs)
        {
            StackDump(stack, errs);
            return errs;
        }
    )

    if (stack->size == 0) return STK_EMPTY;

    stack->data[--stack->size] = ELEMENT_POISON;
    return STK_NO_ERROR;
}

ELEMENT StackPopCopy    (Stack* stack, int* err = NULL)
{
    _ON_STACK_CHECK(
        int errs = StackCheck(stack);
        if (errs)
        {
            StackDump(stack, errs);
            if (err) *err = errs;
            return ELEMENT_POISON;
        }
    )

    if (stack->size == 0)
    {
        if (err) *err = STK_EMPTY;
        return ELEMENT_POISON;
    }
    
    ELEMENT result = stack->data[stack->size - 1];
    stack->data[--stack->size] = ELEMENT_POISON;
    if (err) *err = STK_NO_ERROR;
    return result;
}

ELEMENT* StackPeek      (Stack* stack, int* err = NULL)
{
    _ON_STACK_CHECK(
        int errs = StackCheck(stack);
        if (errs)
        {
            StackDump(stack, errs);
            if (err) *err = errs;
            return NULL;
        }
    )

    if (err) *err = STK_NO_ERROR;
    return stack->data + stack->size;
}

ELEMENT StackPeekCopy   (const Stack* stack, int* err = NULL)
{
    _ON_STACK_CHECK(
        int errs = StackCheck(stack);
        if (errs)
        {
            StackDump(stack, errs);
            if (err) *err = errs;
            return NULL;
        }
    )

    if (err) *err = STK_NO_ERROR;
    return stack->data[stack->size];
}

_ON_STACK_CHECK(
    int _StackCheck(const Stack* stack,
                    const char*  func,
                    const char*  file,
                        size_t line)
    {
        int flags = STK_NO_ERROR;

        _ON_CANARY(
            if (stack->canary_start != CANARY ||
                stack->canary_end   != CANARY)
                flags |= STK_DEAD_CANARY;
        )

        if ((long long)stack->size < 0 || stack->size > stack->capacity)
            flags |= STK_CORRUPTED_SIZE;

        if ((long long)stack->capacity < 0)
            flags |= STK_CORRUPTED_CAP;

        if (stack->data == NULL)
            return flags | STK_CORRUPTED_DATA;
        
        if ((flags & STK_CORRUPTED_SIZE) ||
            (flags & STK_CORRUPTED_CAP))
            return flags;

        canary_t start = ((canary_t*) stack->data)[-1];
        canary_t end   =  (canary_t)  stack->data [stack->capacity];

        if (start != CANARY || end != CANARY)
            return flags | STK_CORRUPTED_DATA;

        //TODO: check if data is readable
        for (size_t i = 0; i < stack->size; i++)
            if (IS_POISON(stack->data[i]))
                return flags | STK_CORRUPTED_DATA;

        for (size_t i = stack->size; i < stack->capacity; i++)
            if (!IS_POISON(stack->data[i]))
                return flags | STK_CORRUPTED_DATA;
        
        return flags;
    }

    void _StackDump(const Stack* stack,
                    const char*  func,
                    const char*  file,
                        size_t line,
                        int    errs)  //TODO: add logging, use LOG_MESSAGE instead of printf
    {
        printf("Dumping stack[%p] (status: %s)\n"
            "\tin %s:%zu in file \'%s\'\n",
                stack, errs ? "CORRUPTED" : "ok",
                func, line, file);
        
        _ON_DEBUG_INFO(
        printf("Stack \'%s\' declared in %s on line %zu, file \'%s\'\n",
                stack->_debug.name,
                stack->_debug.func_name,
                stack->_debug.line_num,
                stack->_debug.file_name);
        )

        if (errs)
            printf("Error flags: %llo\n", errs);


        _ON_CANARY(
            printf("Canary state:\n"
                "\tstart: %#16llx\n"
                "\tend  : %#16llx\n",
                stack->canary_start,
                stack->canary_end);
        )

        printf("Elements stored: %zu\n"
            "Total capacity : %zu\n",
            stack->size,
            stack->capacity);
        
        printf("Data[%p]:\n", stack->data);
        //TODO: check if data is readable
        printf("\t canary: %#16llx\n", ((long long*) stack->data)[-1]);
        for (size_t i = 0; i < stack->capacity; i++)
        {
            printf("\t");
            if (i < stack->size)
                printf("*");
            else
                printf(" ");
            
            printf("[%zu]: ", i);
            
            if (IS_POISON(stack->data[i]))
                printf("POISON");
            else
                PRINT_ELEMENT(stack->data[i]);
            
            printf("\n");
        }
        printf("\t canary: %#16llx\n", (long long)(stack->data)[stack->size]);
        printf("\n");
    }
)

ELEMENT* _ReallocWithCanary(ELEMENT* old_array,
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
    if (!new_data)
        return -1;
    
    stack->data     = new_data;
    stack->capacity = new_capacity;

    return 0;
}

int _StackTryShrink(Stack* stack)
{
    size_t capacity_limit = round(stack->size * _stack_growth*_stack_growth);

    if (stack->capacity < capacity_limit)
        return 0;

    size_t new_capacity = round(stack->size * _stack_growth);
    
    ELEMENT* new_data = _ReallocWithCanary(
                                        stack->data,
                                        stack->capacity,
                                           new_capacity);
    if (!new_data)
        return -1;
    
    stack->data     = new_data;
    stack->capacity = new_capacity;

    return 0;
}

#endif