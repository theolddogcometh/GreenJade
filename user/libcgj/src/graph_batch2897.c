/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2897: product libcgj soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_libcgj_ok_u(uint32_t flags);
 *     - Return 1 if flags has the product libcgj ready bit set (bit0),
 *       else 0. Soft pure-data gate for libcgj product readiness; not a
 *       runtime dlopen or soname probe. Unsigned 0/1 form.
 *   uint32_t __gj_product_libcgj_ok_u  (alias)
 *   __libcgj_batch2897_marker = "libcgj-batch2897"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_product_libcgj_ok_u surface only; no multi-def. Distinct from
 * gj_libcgj_wave_ok (batch2696 span), gj_libcgj_ready_p (batch2595),
 * and gj_libcgj_soname_ok_u (batch2399).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2897_marker[] = "libcgj-batch2897";

/* Product libcgj ready lamp bit (bit0). */
#define B2897_LIBCGJ_READY_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if product libcgj ready bit is set in flags. */
static uint32_t
b2897_libcgj_ok(uint32_t uFlags)
{
	if ((uFlags & B2897_LIBCGJ_READY_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_libcgj_ok_u - 1 iff product libcgj ready bit is set.
 *
 * flags: product libcgj readiness bitmask (bit0 = libcgj ready)
 *
 * Returns 1 when bit0 is set, else 0. Does not parse soname strings
 * and does not call libc.
 */
uint32_t
gj_product_libcgj_ok_u(uint32_t uFlags)
{
	(void)NULL;
	return b2897_libcgj_ok(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_libcgj_ok_u(uint32_t uFlags)
    __attribute__((alias("gj_product_libcgj_ok_u")));
