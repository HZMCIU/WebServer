#ifndef NET_ENDIAN_H
#define NET_ENDIAN_H

#include <cstdint>
#include <endian.h>

inline uint32_t host32ToNetwork(uint32_t n)
{
    return htobe32(n); 
}

inline uint32_t networkToHost32(uint32_t n)
{
    return be32toh(n);
}

inline uint16_t host16ToNetwork(uint16_t n)
{
    return htobe16(n);
}

inline uint16_t networkToHost16(uint16_t n)
{
    return be16toh(n);
}
#endif
