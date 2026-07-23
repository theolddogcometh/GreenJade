/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2891: product bar3 install soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_install_ok_u(uint32_t mask);
 *     - Return 1 if product bar3 mask has the install readiness bit set
 *       (bit3 / 0x8), else 0. Soft pure-data gate; not a runtime ESP
 *       or install-image probe. Unsigned 0/1 form.
 *   uint32_t __gj_bar3_install_ok_u  (alias)
 *   __libcgj_batch2891_marker = "libcgj-batch2891"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_bar3_install_ok_u surface only; no multi-def. Distinct from
 * gj_bar3_install_bit (batch2671 uint extract), gj_bar3_set_install
 * (batch2676), and gj_install_ready_bits (batch2598).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2891_marker[] = "libcgj-batch2891";

/* Install readiness bit within product bar3 mask (bit3). */
#define B2891_INSTALL_BIT  0x8u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if install readiness bit is set in mask. */
static uint32_t
b2891_install_ok(uint32_t uMask)
{
	if ((uMask & B2891_INSTALL_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_install_ok_u - 1 iff bar3 install readiness bit is set.
 *
 * mask: product bar3 readiness bitmask (bit3 = install slot)
 *
 * Returns 1 when bit3 is set, else 0. Soft pure-data only; does not
 * probe install media. Does not call libc.
 */
uint32_t
gj_bar3_install_ok_u(uint32_t uMask)
{
	(void)NULL;
	return b2891_install_ok(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_install_ok_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_install_ok_u")));
