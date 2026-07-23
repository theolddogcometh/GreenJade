/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3495: shell product green extract (wave 3500).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_3500(uint32_t flags);
 *     - Return 1 if flags has the product shell green/ready bit set
 *       (bit0), else 0. Soft pure-data gate for the milestone 3500
 *       continuum; not a runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_3500  (alias)
 *   __libcgj_batch3495_marker = "libcgj-batch3495"
 *
 * Milestone 3500 exclusive continuum CREATE-ONLY (3491-3500). Unique
 * gj_shell_green_3500 surface only; no multi-def. Distinct from
 * gj_shell_green_3400 (batch3395), gj_shell_green_3300 (batch3295),
 * gj_shell_green_u (batch3097), and gj_libcgj_green_3500
 * (batch3496). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3495_marker[] = "libcgj-batch3495";

/* Product shell green/ready lamp bit (bit0). */
#define B3495_SHELL_GREEN_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if shell green bit is set in flags. */
static uint32_t
b3495_green(uint32_t uFlags)
{
	if ((uFlags & B3495_SHELL_GREEN_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_3500 - 1u iff product shell green/ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = shell green)
 *
 * Returns 1 when bit0 is set, else 0. Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_3500(uint32_t uFlags)
{
	(void)NULL;
	return b3495_green(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_3500(uint32_t uFlags)
    __attribute__((alias("gj_shell_green_3500")));
