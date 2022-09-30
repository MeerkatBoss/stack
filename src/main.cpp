#include <stdio.h>

// TODO: There's a way to avoid macro in this case (check note),
//       you could forward-declare a struct in header file:
//
// struct element;
//
// And it will not complain about incomplete type as long ,
// as you define it here, like this:
//
// struct element {
//     int value;
// };
//
// Or even remove forward declaration altogether, you
// still can define it before header file (as you kinda do)
//
// Worst case scenario -- typedef, just not macro. Avoid them!
//
// Note: I got it, you want to support "default" params,
//       I think I'd rather make a separate file with defaults:
//
// #include "stack-generic-element.h"
// #include "stack.h"
//
// You can even do something like:
// In stack-generic-element.h:
//     #define STACK_CUSTOM_ELEMENT // I know, i'm killing macros
//                                  // with other macros, not ideal
// In stack.h
//     #ifndef STACK_CUSTOM_ELEMENT
//         #error "Need to define custom stack element before usage!"
//     #endif
//
// And make user implement some kind of adapters:
//
// In stack-int.h (suggested file for int):
//     #define STACK_CUSTOM_ELEMENT // Marks this as element impl
//
//     // You get the idea
//     class element { ... };
//
// But of course, this STACK_CUSTOM_ELEMENT could serve more as a
// remainder, than bullet-proof system. 
//
// Note: Even for your defaulting system you're kind of only relying
// on just ELEMENT being a macro:
//
// #ifndef ELEMENT
//     // defaults...
//     // You could very well define structure here instead of macros,
//     // than you will even get redefinition check for free!
// #endif

#define ELEMENT long long

// TODO: This could easily be a const int:
#define ELEMENT_POISON (-1ll)


// TODO: same
#define PRINT_ELEMENT(element) printf("%lld", (element))

// TODO: thing
#define IS_POISON(element) ((element) == -1ll)

// TODO: Try to put your dependencies before everything else,
//       this way you can track unwanted dependencies easier:
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
