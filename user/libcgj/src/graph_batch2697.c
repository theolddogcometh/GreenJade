/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2697: product path soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_product_path_ok(uint32_t flags);
 *     - Return 1 if flags has the product path ready bit set (bit0),
 *       else 0. Soft pure-data gate for install / rootfs / export path
 *       readiness; not a runtime filesystem probe.
 *   int __gj_product_path_ok  (alias)
 *   __libcgj_batch2697_marker = "libcgj-batch2697"
 *
 * Milestone 2700 exclusive product helpers (2691-2700). Unique
 * gj_product_path_ok surface only; no multi-def. Distinct from
 * gj_product_bar3_mask (batch2597 constant), gj_install_ready_bits
 * (batch2598 extract), gj_libcgj_ready_p (batch2595), and
 * gj_shell_ready_p (batch2596).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2697_marker[] = "libcgj-batch2697";

/* Product path ready lamp bit (bit0). */
#define B2697_PATH_READY_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if product path ready bit is set in flags. */
static int
b2697_path_ok(uint32_t uFlags)
{
	if ((uFlags & B2697_PATH_READY_BIT) != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_path_ok - 1 iff product path ready bit is set.
 *
 * flags: product path readiness bitmask (bit0 = path ready)
 *
 * Returns 1 when bit0 is set, else 0. Does not walk filesystems and
 * does not call libc.
 */
int
gj_product_path_ok(uint32_t uFlags)
{
	(void)NULL;
	return b2697_path_ok(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_product_path_ok(uint32_t uFlags)
    __attribute__((alias("gj_product_path_ok")));
