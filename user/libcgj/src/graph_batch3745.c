/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3745: shell product green extract (wave 3750).
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_green_3750(uint32_t flags);
 *     - Return 1 if flags has the product shell green/ready bit set
 *       (bit0), else 0. Soft pure-data gate for the milestone 3750
 *       continuum; not a runtime probe of greenjade-shell.
 *   uint32_t __gj_shell_green_3750  (alias)
 *   __libcgj_batch3745_marker = "libcgj-batch3745"
 *
 * Milestone 3750 exclusive continuum CREATE-ONLY (3741-3750). Unique
 * gj_shell_green_3750 surface only; no multi-def. Distinct from
 * gj_shell_green_3700 (batch3695), gj_shell_green_3600 (batch3595),
 * gj_shell_green_3500 (batch3495), and gj_libcgj_green_3750
 * (batch3746). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3745_marker[] = "libcgj-batch3745";

/* Product shell green/ready lamp bit (bit0). */
#define B3745_SHELL_GREEN_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if shell green bit is set in flags. */
static uint32_t
b3745_green(uint32_t uFlags)
{
	if ((uFlags & B3745_SHELL_GREEN_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_green_3750 - 1u iff product shell green/ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = shell green)
 *
 * Returns 1 when bit0 is set, else 0. Does not call libc or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_shell_green_3750(uint32_t uFlags)
{
	(void)NULL;
	return b3745_green(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_green_3750(uint32_t uFlags)
    __attribute__((alias("gj_shell_green_3750")));
