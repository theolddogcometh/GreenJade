/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2298: dual-license flag predicate.
 *
 * Surface (unique symbols):
 *   int gj_license_dual_p(uint32_t flags);
 *     - Return 1 if flags has both dual-license bits set: bit0 (MIT)
 *       and bit1 (Apache-2.0), i.e. (flags & 0x3) == 0x3. Else 0.
 *       Soft product SPDX dual-license presence check; not a legal
 *       authority and does not parse LICENSE text.
 *   int __gj_license_dual_p  (alias)
 *   __libcgj_batch2298_marker = "libcgj-batch2298"
 *
 * Milestone 2300 exclusive product helpers (2291-2300). Unique
 * gj_license_dual_p surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2298_marker[] = "libcgj-batch2298";

/* Bit0 = MIT, bit1 = Apache-2.0; both required for dual license. */
#define B2298_MIT_BIT      0x1u
#define B2298_APACHE_BIT   0x2u
#define B2298_DUAL_MASK    (B2298_MIT_BIT | B2298_APACHE_BIT)

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if both dual-license bits are set in flags. */
static int
b2298_dual_p(uint32_t uFlags)
{
	if ((uFlags & B2298_DUAL_MASK) == B2298_DUAL_MASK) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_license_dual_p - 1 iff flags encodes both MIT and Apache-2.0.
 *
 * flags: license presence bitmask (bit0=MIT, bit1=Apache-2.0)
 *
 * Returns 1 when (flags & 0x3) == 0x3, else 0. Extra high bits are
 * ignored. Does not call libc.
 */
int
gj_license_dual_p(uint32_t uFlags)
{
	(void)NULL;
	return b2298_dual_p(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_license_dual_p(uint32_t uFlags)
    __attribute__((alias("gj_license_dual_p")));
