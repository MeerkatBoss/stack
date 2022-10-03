#ifndef SAFE_STACK_H
#define SAFE_STACK_H

/**
 * @brief 
 * Safe `Stack` wrapper for type `int`
 */
struct SafeStack;

/**
 * @brief 
 * Construct `SafeStack` instance
 * @return Constructed instance pointer
 */
SafeStack* SafeStackCtor();

/**
 * @brief 
 * Destroy `SafeStack` instance. Free associated resources
 * @param[inout] safe_stack `SafeStack` instance
 */
void SafeStackDtor(SafeStack* safe_stack);

/**
 * @brief 
 * Remove top value from `SafeStack`
 * @param[inout] safe_stack `SafeStack` instance
 * @param[out] err Error flags. Ignored if set to `NULL`
 * @return Removed value
 */
int SafeStackPop(SafeStack* safe_stack, unsigned int *err);

/**
 * @brief 
 * Add value to stack
 * @param[inout] safe_stack `SafeStack` instance
 * @param[in] value Added value
 * @param[out] err Error flags. Ignored if set to `NULL`
 * @return `value` upon success, 0 otherwise
 */
int SafeStackPush(SafeStack* safe_stack, int value, unsigned int *err);

/**
 * @brief 
 * Get top value from stack
 * @param[inout] safe_stack `SafeStack` instance
 * @param[out] err Error flags. Ignored if set to `NULL`
 * @return Top value
 */
int SafeStackPeek(SafeStack* safe_stack, unsigned int *err);

/**
 * @brief 
 * Print contents of stack
 * @param[inout] safe_stack `SafeStack` instance
 */
void SafeStackDump(SafeStack* safe_stack);

#endif