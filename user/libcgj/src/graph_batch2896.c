/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2896: product shell soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_shell_ok_u(uint32_t flags);
 *     - Return 1 if flags has the product shell ready bit set (bit0),
 *       else 0. Soft pure-data gate for greenjade-shell product
 *       readiness; not a runtime shell spawn or path probe. Unsigned
 *       0/1 form.
 *   uint32_t __gj_product_shell_ok_u  (alias)
 *   __libcgj_batch2896_marker = "libcgj-batch2896"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_product_shell_ok_u surface only; no multi-def. Distinct from
 * gj_shell_wave_ok (batch2695 span), gj_shell_ready_p (batch2596),
 * and gj_shell_product_ok (batch2497).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2896_marker[] = "libcgj-batch2896";

/* Product shell ready lamp bit (bit0). */
#define B2896_SHELL_READY_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if product shell ready bit is set in flags. */
static uint32_t
b2896_shell_ok(uint32_t uFlags)
{
	if ((uFlags & B2896_SHELL_READY_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_shell_ok_u - 1 iff product shell ready bit is set.
 *
 * flags: product shell readiness bitmask (bit0 = shell ready)
 *
 * Returns 1 when bit0 is set, else 0. Does not spawn shells and does
 * not call libc.
 */
uint32_t
gj_product_shell_ok_u(uint32_t uFlags)
{
	(void)NULL;
	return b2896_shell_ok(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_shell_ok_u(uint32_t uFlags)
    __attribute__((alias("gj_product_shell_ok_u")));
