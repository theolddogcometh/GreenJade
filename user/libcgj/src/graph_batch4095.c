/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4095: shell product green extract (wave 4100).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_4100(uint32_t flags);
 *     - Return 1 if flags has the product shell green/ready bit set
 *       (bit0), else 0. Soft pure-data gate for the milestone 4100
 *       continuum; not a runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_4100  (alias)
 *   __libcgj_batch4095_marker = "libcgj-batch4095"
 *
 * Milestone 4100 exclusive continuum CREATE-ONLY (4091-4100). Unique
 * gj_shell_green_4100 surface only; no multi-def. Distinct from
 * gj_shell_green_3950 (batch3945), gj_shell_green_3900 (batch3895),
 * gj_shell_green_3700 (batch3695), and gj_libcgj_green_4100
 * (batch4096). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4095_marker[] = "libcgj-batch4095";

/* Product shell green/ready lamp bit (bit0). */
#define B4095_SHELL_GREEN_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if shell green bit is set in flags. */
static uint32_t
b4095_green(uint32_t uFlags)
{
	if ((uFlags & B4095_SHELL_GREEN_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_4100 - 1u iff product shell green/ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = shell green)
 *
 * Returns 1 when bit0 is set, else 0. Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_4100(uint32_t uFlags)
{
	(void)NULL;
	return b4095_green(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_4100(uint32_t uFlags)
    __attribute__((alias("gj_shell_green_4100")));
