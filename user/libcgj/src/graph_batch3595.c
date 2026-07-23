/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3595: shell product green extract (wave 3600).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_3600(uint32_t flags);
 *     - Return 1 if flags has the product shell green/ready bit set
 *       (bit0), else 0. Soft pure-data gate for the milestone 3600
 *       continuum; not a runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_3600  (alias)
 *   __libcgj_batch3595_marker = "libcgj-batch3595"
 *
 * Milestone 3600 exclusive continuum CREATE-ONLY (3591-3600). Unique
 * gj_shell_green_3600 surface only; no multi-def. Distinct from
 * gj_shell_green_3550 (batch3545), gj_shell_green_3500 (batch3495),
 * gj_shell_green_3400 (batch3395), and gj_libcgj_green_3600
 * (batch3596). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3595_marker[] = "libcgj-batch3595";

/* Product shell green/ready lamp bit (bit0). */
#define B3595_SHELL_GREEN_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if shell green bit is set in flags. */
static uint32_t
b3595_green(uint32_t uFlags)
{
	if ((uFlags & B3595_SHELL_GREEN_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_3600 - 1u iff product shell green/ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = shell green)
 *
 * Returns 1 when bit0 is set, else 0. Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_3600(uint32_t uFlags)
{
	(void)NULL;
	return b3595_green(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_3600(uint32_t uFlags)
    __attribute__((alias("gj_shell_green_3600")));
