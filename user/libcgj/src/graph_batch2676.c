/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2676: product bar3 set install bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_set_install(uint32_t mask);
 *     - Return mask with the install readiness bit set (bit3 | 0x8).
 *       Soft pure-data update; does not mutate storage or probe ESP.
 *   uint32_t __gj_bar3_set_install  (alias)
 *   __libcgj_batch2676_marker = "libcgj-batch2676"
 *
 * Milestone 2680 exclusive product bar3 bitmask helpers (2671-2680).
 * Unique gj_bar3_set_install surface only; no multi-def. Pair with
 * gj_bar3_install_bit (batch2671 extract) and gj_bar3_all_ready_p
 * (batch2675).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2676_marker[] = "libcgj-batch2676";

/* Install readiness bit within product bar3 mask (bit3). */
#define B2676_INSTALL_BIT  0x8u

/* ---- freestanding helpers ---------------------------------------------- */

/* Set install readiness bit in mask. */
static uint32_t
b2676_set_install(uint32_t uMask)
{
	return uMask | B2676_INSTALL_BIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_set_install - set install readiness bit on bar3 mask.
 *
 * mask: product bar3 readiness bitmask
 *
 * Returns mask | 0x8. Idempotent if bit3 already set. Does not call
 * libc.
 */
uint32_t
gj_bar3_set_install(uint32_t uMask)
{
	(void)NULL;
	return b2676_set_install(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_set_install(uint32_t uMask)
    __attribute__((alias("gj_bar3_set_install")));
