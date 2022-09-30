#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#define USE_CUSTOM_ELEMENT
typedef struct
{
    int value;
    int is_poison;
} element_t;
const element_t element_poison = {.value = 0, .is_poison = 1};
inline void PrintElement(element_t element) { printf("%d", element.value); }
inline int IsPoison(element_t element){ return element.is_poison; }

#define STK_PROT_LEVEL 03

#include "stack.h"
#include "safe_stack.h"

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
    read(fd, &result, sizeof(result));
    close(fd);
    result = GetHash(&result, sizeof(result));
    return result;
}

static const hash_t HASH_KEY = GetHashKey_();

static inline SafeStack* SafeStackDecrypt_(SafeStack* safe_stack)
{
    safe_stack = (SafeStack*)((hash_t)safe_stack ^ HASH_KEY);
    assert(CanReadPointer(safe_stack) && "WRONG STACK PTR");
    assert(safe_stack->canary_start == (CANARY ^ HASH_KEY) && "DEAD CANARY");
    assert(safe_stack->canary_end   == (CANARY ^ HASH_KEY) && "DEAD CANARY");
    return safe_stack;
}

SafeStack* SafeStackCtor()
{
    SafeStack* safe_stack = (SafeStack*) calloc(1, sizeof(*safe_stack));
    int res = StackCtor(&safe_stack->stack);
    safe_stack->canary_start = CANARY ^ HASH_KEY;
    safe_stack->canary_end   = CANARY ^ HASH_KEY;
    assert(res == 0);
    return (SafeStack*)((hash_t)safe_stack ^ HASH_KEY);
}


void SafeStackDtor(SafeStack* safe_stack)
{
    safe_stack = SafeStackDecrypt_(safe_stack);
    StackDtor(&safe_stack->stack);
    free(safe_stack);
}

int SafeStackPop(SafeStack* safe_stack, unsigned int *err)
{
    safe_stack = SafeStackDecrypt_(safe_stack);
    return StackPopCopy(&safe_stack->stack, err).value;
}

int SafeStackPush(SafeStack* safe_stack, int value, unsigned int *err)
{
    safe_stack = SafeStackDecrypt_(safe_stack);
    unsigned int flags = StackPush(&safe_stack->stack, {.value = value, .is_poison = 0});
    TRY_ASSIGN_PTR(err, flags);
    return value;
}

