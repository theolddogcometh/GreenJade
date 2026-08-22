/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <arpa/nameser.h> BIND HEADER for OpenSSH getrrsetbyname
 * (HAVE_GETRRSETBYNAME unset). Dual DoD B OPEN.
 */
#pragma once

#include <stdint.h>
#include <endian.h>

#ifndef HFIXEDSZ
#define HFIXEDSZ 12
#endif
#ifndef INT16SZ
#define INT16SZ 2
#endif
#ifndef INT32SZ
#define INT32SZ 4
#endif
#ifndef PACKETSZ
#define PACKETSZ 512
#endif
#ifndef MAXDNAME
#define MAXDNAME 1025
#endif
#ifndef MAXCDNAME
#define MAXCDNAME 255
#endif

#ifndef C_IN
#define C_IN  1
#define C_ANY 255
#endif
#ifndef T_A
#define T_A     1
#define T_NS    2
#define T_CNAME 5
#define T_SOA   6
#define T_PTR   12
#define T_MX    15
#define T_TXT   16
#define T_AAAA  28
#define T_RRSIG 46
#define T_ANY   255
#endif
#ifndef NOERROR
#define NOERROR  0
#define FORMERR  1
#define SERVFAIL 2
#define NXDOMAIN 3
#define NOTIMP   4
#define REFUSED  5
#endif

/*
 * BIND nameser_compat HEADER (12-byte wire). memcpy onto this from
 * the DNS header. Bitfield order matches little-endian Linux LP64.
 */
typedef struct {
    unsigned id : 16;
#if BYTE_ORDER == BIG_ENDIAN
    unsigned qr : 1;
    unsigned opcode : 4;
    unsigned aa : 1;
    unsigned tc : 1;
    unsigned rd : 1;
    unsigned ra : 1;
    unsigned unused : 1;
    unsigned ad : 1;
    unsigned cd : 1;
    unsigned rcode : 4;
#else
    unsigned rd : 1;
    unsigned tc : 1;
    unsigned aa : 1;
    unsigned opcode : 4;
    unsigned qr : 1;
    unsigned rcode : 4;
    unsigned cd : 1;
    unsigned ad : 1;
    unsigned unused : 1;
    unsigned ra : 1;
#endif
    unsigned qdcount : 16;
    unsigned ancount : 16;
    unsigned nscount : 16;
    unsigned arcount : 16;
} HEADER;

#ifndef NS_INT16SZ
#define NS_INT16SZ INT16SZ
#endif
#ifndef NS_INT32SZ
#define NS_INT32SZ INT32SZ
#endif
#ifndef NS_HFIXEDSZ
#define NS_HFIXEDSZ HFIXEDSZ
#endif
#ifndef NS_PACKETSZ
#define NS_PACKETSZ PACKETSZ
#endif
#ifndef NS_MAXDNAME
#define NS_MAXDNAME MAXDNAME
#endif

#ifndef NS_GET16
#define NS_GET16(s, cp) do { \
    const unsigned char *_pGet16 = (const unsigned char *)(cp); \
    (s) = ((unsigned)(_pGet16[0]) << 8) | (unsigned)(_pGet16[1]); \
    (cp) = (void *)(_pGet16 + NS_INT16SZ); \
} while (0)
#endif
#ifndef NS_GET32
#define NS_GET32(l, cp) do { \
    const unsigned char *_pGet32 = (const unsigned char *)(cp); \
    (l) = ((unsigned long)(_pGet32[0]) << 24) | \
          ((unsigned long)(_pGet32[1]) << 16) | \
          ((unsigned long)(_pGet32[2]) << 8) | \
          (unsigned long)(_pGet32[3]); \
    (cp) = (void *)(_pGet32 + NS_INT32SZ); \
} while (0)
#endif
#ifndef NS_PUT16
#define NS_PUT16(s, cp) do { \
    unsigned char *_pPut16 = (unsigned char *)(cp); \
    unsigned _uPut16 = (unsigned)(s); \
    _pPut16[0] = (unsigned char)((_uPut16 >> 8) & 0xffu); \
    _pPut16[1] = (unsigned char)(_uPut16 & 0xffu); \
    (cp) = (void *)(_pPut16 + NS_INT16SZ); \
} while (0)
#endif
#ifndef NS_PUT32
#define NS_PUT32(l, cp) do { \
    unsigned char *_pPut32 = (unsigned char *)(cp); \
    unsigned long _uPut32 = (unsigned long)(l); \
    _pPut32[0] = (unsigned char)((_uPut32 >> 24) & 0xffu); \
    _pPut32[1] = (unsigned char)((_uPut32 >> 16) & 0xffu); \
    _pPut32[2] = (unsigned char)((_uPut32 >> 8) & 0xffu); \
    _pPut32[3] = (unsigned char)(_uPut32 & 0xffu); \
    (cp) = (void *)(_pPut32 + NS_INT32SZ); \
} while (0)
#endif
#ifndef GETSHORT
#define GETSHORT NS_GET16
#endif
#ifndef GETLONG
#define GETLONG NS_GET32
#endif
#ifndef PUTSHORT
#define PUTSHORT NS_PUT16
#endif
#ifndef PUTLONG
#define PUTLONG NS_PUT32
#endif
