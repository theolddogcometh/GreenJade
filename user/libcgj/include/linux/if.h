/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <linux/if.h> for libcgj. OpenSSH port-net.c
 * (SYS_RDOMAIN_LINUX) needs IFNAMSIZ. Dual DoD B OPEN.
 */
#pragma once

#include <net/if.h>

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
#ifndef IFHWADDRLEN
#define IFHWADDRLEN 6
#endif
