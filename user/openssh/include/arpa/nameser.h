/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <arpa/nameser.h> for OpenSSH-portable getrrsetbyname (no
 * HAVE_GETRRSETBYNAME). Host POSIX: glibc. DUT: libcgj has no nameser.h
 * and must not include_next glibc (conflicts with libcgj resolv.h).
 */
#pragma once
#if __STDC_HOSTED__
# include_next <arpa/nameser.h>
#else

#include <stdint.h>
#include <endian.h>

#ifndef HFIXEDSZ
# define HFIXEDSZ 12
#endif
#ifndef INT16SZ
# define INT16SZ 2
#endif
#ifndef INT32SZ
# define INT32SZ 4
#endif
#ifndef PACKETSZ
# define PACKETSZ 512
#endif
#ifndef MAXDNAME
# define MAXDNAME 1025
#endif
#ifndef MAXCDNAME
# define MAXCDNAME 255
#endif

#ifndef C_IN
# define C_IN  1
# define C_ANY 255
#endif
#ifndef T_A
# define T_A     1
# define T_NS    2
# define T_CNAME 5
# define T_SOA   6
# define T_PTR   12
# define T_MX    15
# define T_TXT   16
# define T_AAAA  28
# define T_RRSIG 46
# define T_ANY   255
#endif
#ifndef NOERROR
# define NOERROR  0
# define FORMERR  1
# define SERVFAIL 2
# define NXDOMAIN 3
# define NOTIMP   4
# define REFUSED  5
#endif

/*
 * BIND nameser_compat HEADER (12-byte wire). memcpy onto this from
 * the DNS header; bitfield order matches little-endian Linux LP64.
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

#endif
