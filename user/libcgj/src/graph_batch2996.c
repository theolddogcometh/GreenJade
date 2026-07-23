/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2996: libcgj product ready extract (uint form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_ready_u(uint32_t flags);
 *     - Return 1 if flags has the product libcgj ready bit set
 *       (bit0), else 0. Soft pure-data gate; not a runtime dlopen
 *       or soname probe. Unsigned form.
 *   uint32_t __gj_libcgj_ready_u  (alias)
 *   __libcgj_batch2996_marker = "libcgj-batch2996"
 *
 * Milestone 3000 exclusive continuum CREATE-ONLY (2991-3000). Unique
 * gj_libcgj_ready_u surface only; no multi-def. Distinct from
 * gj_libcgj_ready_p (batch2595 int form), gj_libcgj_wave_ok (batch2696),
 * gj_libcgj_soname_ok_u (batch2399), and gj_shell_ready_u (batch2995).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2996_marker[] = "libcgj-batch2996";

/* Product libcgj ready lamp bit (bit0). */
#define B2996_LIBCGJ_READY_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if libcgj ready bit is set in flags. */
static uint32_t
b2996_ready_u(uint32_t uFlags)
{
	if ((uFlags & B2996_LIBCGJ_READY_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_ready_u - 1u iff product libcgj ready bit is set.
 *
 * flags: product readiness bitmask (bit0 = libcgj ready)
 *
 * Returns 1 when bit0 is set, else 0. Does not parse soname strings
 * and does not call libc. No parent wires.
 */
uint32_t
gj_libcgj_ready_u(uint32_t uFlags)
{
	(void)NULL;
	return b2996_ready_u(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_ready_u(uint32_t uFlags)
    __attribute__((alias("gj_libcgj_ready_u")));
