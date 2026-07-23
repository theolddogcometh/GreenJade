/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3245: shell product ok extract (3250 wave).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_ok_3250(uint32_t flags);
 *     - Return 1 if flags has the product shell ready/ok bit set
 *       (bit0), else 0. Soft pure-data gate for the milestone 3250
 *       continuum; not a runtime probe of greenjade-shell. Unsigned
 *       form, wave-tagged symbol name.
 *   uint32_t __gj_shell_ok_3250  (alias)
 *   __libcgj_batch3245_marker = "libcgj-batch3245"
 *
 * Milestone 3250 exclusive continuum CREATE-ONLY (3241-3250). Unique
 * gj_shell_ok_3250 surface only; no multi-def. Distinct from
 * gj_shell_ok_u (batch3196), gj_shell_green_u (batch3097),
 * gj_shell_ready_u (batch2995), and gj_libcgj_ok_3250 (batch3246).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3245_marker[] = "libcgj-batch3245";

/* Product shell ready/ok lamp bit (bit0). */
#define B3245_SHELL_OK_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if shell ok bit is set in flags. */
static uint32_t
b3245_ok_u(uint32_t uFlags)
{
	if ((uFlags & B3245_SHELL_OK_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_ok_3250 - 1u iff product shell ready/ok bit is set.
 *
 * flags: product readiness bitmask (bit0 = shell ok)
 *
 * Returns 1 when bit0 is set, else 0. Wave-tagged distinct surface for
 * MILESTONE 3250. Does not call libc or probe greenjade-shell.
 * No parent wires.
 */
uint32_t
gj_shell_ok_3250(uint32_t uFlags)
{
	(void)NULL;
	return b3245_ok_u(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_ok_3250(uint32_t uFlags)
    __attribute__((alias("gj_shell_ok_3250")));
