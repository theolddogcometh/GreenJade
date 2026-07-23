/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2995: shell product ready extract (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_ready_u(uint32_t flags);
 *     - Return 1 if flags has the product shell ready bit set
 *       (bit0), else 0. Soft pure-data gate; not a runtime probe of
 *       greenjade-shell. Unsigned form.
 *   uint32_t __gj_shell_ready_u  (alias)
 *   __libcgj_batch2995_marker = "libcgj-batch2995"
 *
 * Milestone 3000 exclusive continuum CREATE-ONLY (2991-3000). Unique
 * gj_shell_ready_u surface only; no multi-def. Distinct from
 * gj_shell_ready_p (batch2596 int form), gj_shell_wave_ok (batch2695),
 * gj_shell_product_ok (batch2497), and gj_libcgj_ready_u (batch2996).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2995_marker[] = "libcgj-batch2995";

/* Product shell ready lamp bit (bit0). */
#define B2995_SHELL_READY_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if shell ready bit is set in flags. */
static uint32_t
b2995_ready_u(uint32_t uFlags)
{
	if ((uFlags & B2995_SHELL_READY_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_ready_u - 1u iff product shell ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = shell ready)
 *
 * Returns 1 when bit0 is set, else 0. Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_ready_u(uint32_t uFlags)
{
	(void)NULL;
	return b2995_ready_u(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_ready_u(uint32_t uFlags)
    __attribute__((alias("gj_shell_ready_u")));
