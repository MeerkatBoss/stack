#include <unistd.h>

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

int CanReadPointer(const void *ptr)
{
    return ptr != NULL && write(STDOUT_FILENO, ptr, 0) >= 0;
    // TODO: Haha, old classic, respectable! (not a TODO)
    //       But I've seen this trick a lot more with read, I wonder why that might be.
}
