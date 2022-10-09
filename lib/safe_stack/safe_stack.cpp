#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#define USE_CUSTOM_ELEMENT
typedef struct
{
    int value;
    int is_poison;
} element_t; // TODO: add space!
const element_t element_poison = {.value = 0, .is_poison = 1};
// TODO:                          ^ add                     ^ space!
inline void PrintElement(element_t element) { printf("%d", element.value); }
inline int IsPoison(element_t element){ return element.is_poison; }

#define STK_PROT_LEVEL 03

#include "stack.h"
#include "safe_stack.h"
#include "logger.h"

struct SafeStack
{
    canary_t canary_start;
    Stack stack;
    canary_t canary_end;
};

static hash_t GetHashKey_()
{
    hash_t result = 0;
    int fd = open("/dev/random", O_RDONLY);
    read(fd, &result, sizeof(&result));
    close(fd);
    result = GetHash(&result, sizeof(result));
    return result;
}

static const hash_t HASH_KEY = GetHashKey_();

static inline SafeStack* SafeStackDecrypt_(SafeStack* safe_stack)
{
    LOG_ASSERT(MSG_ERROR, safe_stack != NULL, return NULL);
    safe_stack = (SafeStack*)((hash_t)safe_stack ^ HASH_KEY);
    LOG_ASSERT(MSG_ERROR,
        CanReadPointer(safe_stack),
        {
            log_message(MSG_ERROR, "Corrupted stack ptr %p (%#016llx^%#016llx)",
                safe_stack,
                (hash_t)safe_stack ^ HASH_KEY, HASH_KEY);
            return NULL;
        });

    LOG_ASSERT(MSG_ERROR,
        safe_stack->canary_start == (CANARY ^ HASH_KEY),
        {
            log_message(MSG_ERROR, "Dead canary %#016llx", safe_stack->canary_start);
            return NULL;
        }); // TODO: maybe extract logging + return NULL?
    LOG_ASSERT(MSG_ERROR,
        safe_stack->canary_end   == (CANARY ^ HASH_KEY),
        { // TODO:                  ^~~~~~~~~~~~~~~~~ Do this ones, and save result
            log_message(MSG_ERROR, "Dead canary %#016llx", safe_stack->canary_end);
            return NULL;
        });
    return safe_stack;
}

SafeStack* SafeStackCtor()
{
    SafeStack* safe_stack = (SafeStack*) calloc(1, sizeof(*safe_stack));
    int res = 0;
    LOG_CATCH_ERROR({res = StackCtor(&safe_stack->stack);}, res == 0, return NULL);
    //              ^ TODO: space?                       ^
    safe_stack->canary_start = CANARY ^ HASH_KEY;
    safe_stack->canary_end   = CANARY ^ HASH_KEY; // TODO: Why twice? Use sequenced assignment
    return (SafeStack*)((hash_t)safe_stack ^ HASH_KEY);
}


void SafeStackDtor(SafeStack* safe_stack)
{
    safe_stack = SafeStackDecrypt_(safe_stack);
    if (!safe_stack) return;
    StackDtor(&safe_stack->stack);
    free(safe_stack);
}

int SafeStackPop(SafeStack* safe_stack, unsigned int *err)
{
    safe_stack = SafeStackDecrypt_(safe_stack);
    if (!safe_stack)
    {
        TRY_ASSIGN_PTR(err, STK_BAD_PTR);
        return 0;
    };
    return StackPopCopy(&safe_stack->stack, err).value;
}

int SafeStackPush(SafeStack* safe_stack, int value, unsigned int *err)
{
    safe_stack = SafeStackDecrypt_(safe_stack);
    if (!safe_stack)
    {
        TRY_ASSIGN_PTR(err, STK_BAD_PTR); // TODO: very common thing, e.g. if (err) { propagate error + return special value }
                                          //       try extracting!
        return 0;
    };
    unsigned int flags = StackPush(&safe_stack->stack, {.value = value, .is_poison = 0});
    TRY_ASSIGN_PTR(err, flags);
    return value;
}

int SafeStackPeek(SafeStack* safe_stack, unsigned int *err)
{
    safe_stack = SafeStackDecrypt_(safe_stack);
    if (!safe_stack)
    {
        TRY_ASSIGN_PTR(err, STK_BAD_PTR);
        return 0;
    }
    return StackPeek(&safe_stack->stack, err)->value;
}

void SafeStackDump(SafeStack* safe_stack)
{
    safe_stack = SafeStackDecrypt_(safe_stack);
    StackDump(&safe_stack->stack);
}

