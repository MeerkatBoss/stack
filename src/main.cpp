#include <stdio.h>

#define ELEMENT int
#define ELEMENT_POISON (-1)
#define PRINT_ELEMENT(element) printf("%d", (element))
#define IS_POISON(element) ((element) == -1)
#include "stack.h"

int main()
{
    puts("Hello, stack!");
    unsigned int err = 0;

    Stack stack = {};
    StackCtor(&stack);

    for(int i = 0; i < 65; i++)
    {
        err = StackPush(&stack, i);
        if (err) goto end;
    }

    StackDump(&stack, 0);

    for (int i = 0; i < 16; i++)
    {
        err = StackPop(&stack);
        if (err) goto end;
    }
    
    StackDump(&stack, 0);

    for (int i = 0; i < 35; i++)
    {
        err = StackPop(&stack);
        if (err) goto end;
    }

    StackDump(&stack, 0);

end:
    StackDtor(&stack);
    return 0;
}