/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <net/route.h> for libcgj. OpenSSH servconf.c
 * (HAVE_NET_ROUTE_H). Linux DUT uses SYS_RDOMAIN_LINUX, not
 * OpenBSD rt_tableinfo. Dual DoD B OPEN.
 */
#pragma once

#include <stdint.h>
#include <sys/socket.h>

#ifndef RTF_UP
#define RTF_UP       0x0001
#define RTF_GATEWAY  0x0002
#define RTF_HOST     0x0004
#define RTF_REJECT   0x0200
#define RTF_DYNAMIC  0x0010
#define RTF_MODIFIED 0x0020
#endif

#ifndef RT_CLASS_UNSPEC
#define RT_CLASS_UNSPEC  0
#define RT_CLASS_DEFAULT 253
#define RT_CLASS_MAIN    254
#define RT_CLASS_LOCAL   255
#endif

#ifndef RTA_DST
#define RTA_DST    1
#define RTA_SRC    2
#define RTA_IIF    3
#define RTA_OIF    4
#define RTA_GATEWAY 5
#define RTA_PRIORITY 6
#define RTA_PREFSRC 7
#define RTA_METRICS 8
#define RTA_MULTIPATH 9
#define RTA_FLOW   11
#define RTA_CACHEINFO 12
#define RTA_TABLE  15
#endif
