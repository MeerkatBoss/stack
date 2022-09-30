#ifndef SAFE_STACK_H
#define SAFE_STACK_H

struct SafeStack;

SafeStack* SafeStackCtor();

void SafeStackDtor(SafeStack* safe_stack);

int SafeStackPop(SafeStack* safe_stack, unsigned int *err);

int SafeStackPush(SafeStack* safe_stack, int value, unsigned int *err);

#endif