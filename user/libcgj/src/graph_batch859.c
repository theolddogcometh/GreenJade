/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch859: IPv4 loopback predicate (127.0.0.0/8, host order).
 *
 * Surface (unique symbols):
 *   int gj_ipv4_is_loopback(uint32_t addr);
 *     — 1 if high octet is 127 (host-order), else 0.
 *   int __gj_ipv4_is_loopback  (alias)
 *   __libcgj_batch859_marker = "libcgj-batch859"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch859_marker[] = "libcgj-batch859";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ipv4_is_loopback — 1 if addr is in 127.0.0.0/8 (host order).
 *
 * Host-order test: top octet == 127 ⇔ (addr >> 24) == 0x7f.
 * Includes 127.0.0.1 and the full 127/8 block.
 */
int
gj_ipv4_is_loopback(uint32_t u32Addr)
{
	return ((u32Addr >> 24) == 0x7fu) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ipv4_is_loopback(uint32_t u32Addr)
    __attribute__((alias("gj_ipv4_is_loopback")));
