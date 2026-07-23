/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped net/if.h (subset). Not GNU glibc.
 */
#pragma once

#include <sys/socket.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IF_NAMESIZE 16

#define IFF_UP          0x1
#define IFF_BROADCAST   0x2
#define IFF_LOOPBACK    0x8
#define IFF_RUNNING     0x40
#define IFF_MULTICAST   0x1000

struct if_nameindex {
    unsigned int if_index;
    char        *if_name;
};

unsigned int if_nametoindex(const char *szIfname);
char        *if_indextoname(unsigned int uIndex, char *szIfname);
struct if_nameindex *if_nameindex(void);
void         if_freenameindex(struct if_nameindex *p);

#ifdef __cplusplus
}
#endif
