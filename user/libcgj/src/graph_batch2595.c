/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2595: libcgj product ready predicate.
 *
 * Surface (unique symbols):
 *   int gj_libcgj_ready_p(uint32_t flags);
 *     - Return 1 if flags has the product libcgj ready bit set
 *       (bit0), else 0. Soft pure-data gate; not a runtime dlopen
 *       or soname probe.
 *   int __gj_libcgj_ready_p  (alias)
 *   __libcgj_batch2595_marker = "libcgj-batch2595"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_libcgj_ready_p surface only; no multi-def. Distinct from
 * gj_libcgj_soname_ok_u (batch2399 major check) and
 * gj_soname_major_ok (batch2297 band).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2595_marker[] = "libcgj-batch2595";

/* Product libcgj ready lamp bit (bit0). */
#define B2595_LIBCGJ_READY_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if libcgj ready bit is set in flags. */
static int
b2595_ready_p(uint32_t uFlags)
{
	if ((uFlags & B2595_LIBCGJ_READY_BIT) != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_ready_p - 1 iff product libcgj ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = libcgj ready)
 *
 * Returns 1 when bit0 is set, else 0. Does not parse soname strings
 * and does not call libc.
 */
int
gj_libcgj_ready_p(uint32_t uFlags)
{
	(void)NULL;
	return b2595_ready_p(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_libcgj_ready_p(uint32_t uFlags)
    __attribute__((alias("gj_libcgj_ready_p")));
