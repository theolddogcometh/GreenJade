/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <linux/if_tun.h> for libcgj. OpenSSH port-net.c
 * (HAVE_LINUX_IF_TUN_H, SSH_TUN_LINUX) needs IFF_TUN/TAP/NO_PI
 * and TUNSETIFF. ABI numbers match Linux x86_64 UAPI. Dual DoD B OPEN.
 */
#pragma once

#include <linux/if.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IFF_TUN
#define IFF_TUN   0x0001
#endif
#ifndef IFF_TAP
#define IFF_TAP   0x0002
#endif
#ifndef IFF_NO_PI
#define IFF_NO_PI 0x1000
#endif

/*
 * Linux _IOW('T', 202, int) on x86. TUNSETIFF overlays struct ifreq
 * (ifr_name + ifr_flags). Not a product tunnel claim.
 */
#ifndef TUNSETIFF
#define TUNSETIFF 0x400454cau
#endif
#ifndef TUNGETIFF
#define TUNGETIFF 0x800454d3u
#endif

#ifdef __cplusplus
}
#endif
