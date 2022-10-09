#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#include "utils.h" // TODO: Just split this in "hashing.h" and like "memory-prober.h"
                   //       Calling this "utils" and smashing it together is a lazy way. You can do better)

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

static jmp_buf jump_buffer = {};
static volatile int is_bad_ptr; // TODO: What is this?

static void segfault_handler(int sig)
{
    signal(SIGSEGV, SIG_DFL);
    //is_bad_ptr = 1;
    longjmp(jump_buffer, 1);
}

int CanReadPointer(const void *ptr)
{
    if (!ptr)
        return 0;
    
    int fd = open("/dev/random", O_RDWR); // TODO: Using entropy for such task... How could you?!
    int res = write(fd, ptr, 1); // TODO: does writing 0 bytes work? I can't remember
    close(fd);

    return res >= 0;

/*
    if (setjmp(jump_buffer) == 0)
    {
        signal(SIGSEGV, segfault_handler);
        char c = *(const char*) ptr;
        signal(SIGSEGV, SIG_DFL);
        return 1;
    }
    return 0;*/
}
