#include <stdio.h>

#include "safe_stack.h"
#include "utils.h"
#include "logger.h"

int main()
{
    add_default_file_logger();

    /*Big size because we'll need to shrink later */
    const size_t initial_size = 65;
    const size_t end_size     = 16;
    puts("Hello, stack!"); // Hello!
    unsigned int err = 0;
    int tmp = 0;

    //LOG_PRINT_TRACE({tmp = CanReadPointer((const char*)5);}, "tmp = %d", tmp);

    SafeStack* stack = SafeStackCtor();

    log_message(MSG_TRACE, "Started push in main()");
    for(size_t i = 0; i < initial_size; i++)
    {
        SafeStackPush(stack, (int)i, &err);
        if (err) goto end;
    }
    log_message(MSG_TRACE, "Ended push in main()");

    log_message(MSG_TRACE, "Started pop in main()");
    for (size_t i = initial_size; i > end_size + 1; i--)
    {
        SafeStackPop(stack, &err);
        if (err) goto end;
    }
    log_message(MSG_TRACE, "Ended pop in main()");
    
    SafeStackPop(stack, &err);
    if (err) goto end;


end:
    stack = (SafeStack*)"12345";
    SafeStackDtor(stack);
    LOG_ASSERT(MSG_WARNING, err == 0, return (int)err);
    return 0;
}
