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

#define ELEMENT int

// TODO: This could easily be a const int:
#define ELEMENT_POISON (-1)


// TODO: same
#define PRINT_ELEMENT(element) printf("%d", (element))

// TODO: thing
#define IS_POISON(element) ((element) == -1)

// TODO: Try to put your dependencies before everything else,
//       this way you can track unwanted dependencies easier:
#include "stack.h"

int main()
{
    puts("Hello, stack!"); // Hello!
    unsigned int err = 0; // TODO: Just define it when it's used

    Stack stack = {};
    StackCtor(&stack);

    for(int i = 0; i < 65; i++)
    { // TODO:         ^~ What is this magic number? Explain! 
        err = StackPush(&stack, i);
        if (err) goto end;
    }

    StackDump(&stack, 0);

    for (int i = 0; i < 16; i++)
    { // TODO:          ^~ What is this magic number? Explain! 
        err = StackPop(&stack);
        if (err) goto end;
    }
    
    StackDump(&stack, 0);

    for (int i = 0; i < 35; i++)
    { // TODO:          ^~ What is this magic number? Explain! 
        err = StackPop(&stack);
        if (err) goto end;
    }

    StackDump(&stack, 0);
    // TODO:          ^ Check StackDump docs for critique of
    //                  this decision

end:
    StackDtor(&stack);
    return 0; // TODO: you can indicate fail with error code,
              //       you literally have it in "err"
}
