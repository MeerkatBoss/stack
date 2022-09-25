#ifndef CUSTOM_STACK_H
#define CUSTOM_STACK_H

#include <stddef.h>
#include <stdio.h>

#ifndef ELEMENT
#define ELEMENT void*
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

const canary_t CANARY = 0xD1AB011CA1C0C0A5ULL;    /* DIABOLICAL COCOAS*/

enum PoisonFlags
{ 
    PSN_NO_POISON       = 000,
    PSN_UNSET           = 001,
    PSN_NO_MEMORY       = 002,
    PSN_CORRUPTED_SIZE  = 004,
    PSN_CORRUPTED_CAP   = 010,
    PSN_CORRUPTED_DATA  = 020,
    PSN_DEAD_CANARY     = 040
};

/**
 * @brief 
 * Element or poison
 */
struct MaybeElement
{
    _ON_CANARY(canary_t canary_start;)
               ELEMENT value;
               int poison_flags;
    _ON_CANARY(canary_t canary_end;)
};

_ON_DEBUG_INFO(
    struct _debug_info
    {
        const char *name;
        const char *func_name;
        const char *file_name;
        size_t      line_num;
    };
)

struct Stack
{
    _ON_CANARY(     canary_t        canary_start;)
                    MaybeElement*   data;
                    size_t          size;
                    size_t          capacity;
    _ON_DEBUG_INFO( _debug_info     _debug;)
    _ON_CANARY(     canary_t        canary_end;)
};

MaybeElement    MaybeElementCtor        (ELEMENT value, int poison_flags = 0);
_ON_CANARY(int  MaybeElementCheckCanary (MaybeElement* mb_element);)

int _StackCtor(Stack* stack,
               const char* name,
               const char* func_name,
               const char* file_name,
               size_t line_num);

#define StackCtor(stack) _StackCtor(stack, #stack,\
                                    __PRETTY_FUNCTION__,\
                                    __FILE__, __LINE__);

void          StackDtor     (Stack* stack);
int           StackPush     (Stack* stack, ELEMENT value);
MaybeElement* StackPop      (Stack* stack);
int          _StackCheck    (Stack* stack);
void          StackDump     (Stack* stack);

#endif