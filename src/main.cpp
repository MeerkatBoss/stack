#include <stdio.h>

#define USE_CUSTOM_ELEMENT
typedef long long element_t;
const long long element_poison = -1ll;
inline void PrintElement(element_t element) { printf("%lld", element); }
inline long long IsPoison(element_t element){ return element == -1ll; }

#include "stack.h"


int main()
{
    /*Big size because we'll need to shrink later */
    const size_t initial_size = GetCapacityLimit_(default_cap_) + 1;
    const size_t end_size     = default_cap_ - 1;
    puts("Hello, stack!"); // Hello!
    unsigned int err = 0;

    Stack stack = {};
    StackCtor(&stack);

    for(size_t i = 0; i < initial_size; i++)
    {
        err = StackPush(&stack, (long long)i);
        if (err) goto end;
    }

    StackAssert(&stack);

    for (size_t i = initial_size; i > end_size + 1; i--)
    {
        err = StackPop(&stack);
        if (err) goto end;
    }
    
    StackAssert(&stack);
    
    err = StackPop(&stack);
    if (err) goto end;

    StackAssert(&stack);

end:
    StackDtor(&stack);
    return (int) err;
}
