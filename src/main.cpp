#include <stdio.h>

#include "safe_stack.h"
#include "utils.h"

int main()
{
    /*Big size because we'll need to shrink later */
    const size_t initial_size = 65;
    const size_t end_size     = 16;
    puts("Hello, stack!"); // Hello!
    unsigned int err = 0;
    printf("Can read 5: %d\n", CanReadPointer((const char*)5));
    //printf("%hhx", *(const char*)5);

    SafeStack* stack = SafeStackCtor();

    puts("Push started\n");
    for(size_t i = 0; i < initial_size; i++)
    {
        SafeStackPush(stack, (int)i, &err);
        if (err) goto end;
    }
    puts("Push ended\n");

    puts("Pop started\n");
    for (size_t i = initial_size; i > end_size + 1; i--)
    {
        SafeStackPop(stack, &err);
        if (err) goto end;
    }
    puts("Pop ended\n");
    
    SafeStackPop(stack, &err);
    if (err) goto end;


end:
    SafeStackDtor(stack);
    return (int) err;
}
