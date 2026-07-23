/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * htonl/htons/ntohl/ntohs (+ 64-bit soft htonll/ntohll) as exported symbols.
 * Host is little-endian (x86_64 / aarch64 LE product path): pure octet swap.
 * Big-endian compile: identity via __BYTE_ORDER__ when available.
 */
#include <stdint.h>

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && \
    (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define GJ_HOST_BE 1
#else
#define GJ_HOST_BE 0
#endif

static uint16_t
bswap16(uint16_t uV)
{
	return (uint16_t)((uV << 8) | (uV >> 8));
}

static uint32_t
bswap32(uint32_t uV)
{
	return ((uV & 0x000000ffu) << 24) | ((uV & 0x0000ff00u) << 8) |
	       ((uV & 0x00ff0000u) >> 8) | ((uV >> 24) & 0x000000ffu);
}

static uint64_t
bswap64(uint64_t uV)
{
	return ((uint64_t)bswap32((uint32_t)uV) << 32) |
	       (uint64_t)bswap32((uint32_t)(uV >> 32));
}

uint16_t
htons(uint16_t uV)
{
#if GJ_HOST_BE
	return uV;
#else
	return bswap16(uV);
#endif
}

uint16_t
ntohs(uint16_t uV)
{
	return htons(uV);
}

uint32_t
htonl(uint32_t uV)
{
#if GJ_HOST_BE
	return uV;
#else
	return bswap32(uV);
#endif
}

uint32_t
ntohl(uint32_t uV)
{
	return htonl(uV);
}

/* Soft 64-bit network order (common extension surface; not in POSIX). */
uint64_t
htonll(uint64_t uV)
{
#if GJ_HOST_BE
	return uV;
#else
	return bswap64(uV);
#endif
}

uint64_t
ntohll(uint64_t uV)
{
	return htonll(uV);
}
