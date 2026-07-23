/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2596: shell product ready predicate.
 *
 * Surface (unique symbols):
 *   int gj_shell_ready_p(uint32_t flags);
 *     - Return 1 if flags has the product shell ready bit set
 *       (bit0), else 0. Soft pure-data gate; not a runtime probe of
 *       greenjade-shell.
 *   int __gj_shell_ready_p  (alias)
 *   __libcgj_batch2596_marker = "libcgj-batch2596"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_shell_ready_p surface only; no multi-def. Distinct from
 * gj_libcgj_ready_p (batch2595), gj_shell_product_ok (batch2497),
 * gj_shell_continuum_ok (batch2397), and gj_shell_ver_ok_hint
 * (batch2295).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2596_marker[] = "libcgj-batch2596";

/* Product shell ready lamp bit (bit0). */
#define B2596_SHELL_READY_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if shell ready bit is set in flags. */
static int
b2596_ready_p(uint32_t uFlags)
{
	if ((uFlags & B2596_SHELL_READY_BIT) != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_ready_p - 1 iff product shell ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = shell ready)
 *
 * Returns 1 when bit0 is set, else 0. Does not call libc or probe
 * greenjade-shell.
 */
int
gj_shell_ready_p(uint32_t uFlags)
{
	(void)NULL;
	return b2596_ready_p(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shell_ready_p(uint32_t uFlags)
    __attribute__((alias("gj_shell_ready_p")));
