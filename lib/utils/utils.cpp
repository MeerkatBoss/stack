#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include "utils.h"

hash_t GetHash(const void* data, size_t length)
{
    static const hash_t base = 269; /* prime number near 256 */
    static const hash_t add  = 31;  /* small prime number */
    hash_t result = 0;

    const unsigned char* bytes = (const unsigned char*)data;
    for (size_t i = 0; i < length; i++)
        result = (result * base) + bytes[i] + add;
    
    return result;
}

static int is_bad_ptr;

static void segfault_handler(int signal)
{
    is_bad_ptr = 1;
}

int CanReadPointer(const void *ptr)
{
    if (!ptr)
        return 0;
    struct sigaction action = {};
    struct sigaction old_action = {};
    action.sa_handler = segfault_handler;
    sigaction(SIGSEGV, &action, &old_action);
    is_bad_ptr = 0;
    char c = *(const char*) ptr;
    sigaction(SIGSEGV, &old_action, &action);
    return is_bad_ptr == 0;
}
