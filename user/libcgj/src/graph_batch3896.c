/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3896: libcgj product green extract (wave 3900).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_green_3900(uint32_t flags);
 *     - Return 1 if flags has the product libcgj green/ready bit set
 *       (bit0), else 0. Soft pure-data gate for the milestone 3900
 *       continuum; not a runtime dlopen or soname probe.
 *   uint32_t __gj_libcgj_green_3900  (alias)
 *   __libcgj_batch3896_marker = "libcgj-batch3896"
 *
 * Milestone 3900 exclusive continuum CREATE-ONLY (3891-3900). Unique
 * gj_libcgj_green_3900 surface only; no multi-def. Distinct from
 * gj_libcgj_green_3700 (batch3696), gj_libcgj_green_3600 (batch3596),
 * gj_libcgj_green_3550 (batch3546), and gj_shell_green_3900
 * (batch3895). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3896_marker[] = "libcgj-batch3896";

/* Product libcgj green/ready lamp bit (bit0). */
#define B3896_LIBCGJ_GREEN_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if libcgj green bit is set in flags. */
static uint32_t
b3896_green(uint32_t uFlags)
{
	if ((uFlags & B3896_LIBCGJ_GREEN_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_green_3900 - 1u iff product libcgj green/ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = libcgj green)
 *
 * Returns 1 when bit0 is set, else 0. Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_green_3900(uint32_t uFlags)
{
	(void)NULL;
	return b3896_green(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_green_3900(uint32_t uFlags)
    __attribute__((alias("gj_libcgj_green_3900")));
