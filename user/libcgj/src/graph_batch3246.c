/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3246: libcgj product ok extract (3250 wave).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libcgj_ok_3250(uint32_t flags);
 *     - Return 1 if flags has the product libcgj ready/ok bit set
 *       (bit0), else 0. Soft pure-data gate for the milestone 3250
 *       continuum; not a runtime dlopen or soname probe. Unsigned
 *       form, wave-tagged symbol name.
 *   uint32_t __gj_libcgj_ok_3250  (alias)
 *   __libcgj_batch3246_marker = "libcgj-batch3246"
 *
 * Milestone 3250 exclusive continuum CREATE-ONLY (3241-3250). Unique
 * gj_libcgj_ok_3250 surface only; no multi-def. Distinct from
 * gj_libcgj_ok_u (batch3197), gj_libcgj_green_u (batch3098),
 * gj_libcgj_ready_u (batch2996), and gj_shell_ok_3250 (batch3245).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3246_marker[] = "libcgj-batch3246";

/* Product libcgj ready/ok lamp bit (bit0). */
#define B3246_LIBCGJ_OK_BIT  0x1u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1u if libcgj ok bit is set in flags. */
static uint32_t
b3246_ok_u(uint32_t uFlags)
{
	if ((uFlags & B3246_LIBCGJ_OK_BIT) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libcgj_ok_3250 - 1u iff product libcgj ready/ok bit is set.
 *
 * flags: product readiness bitmask (bit0 = libcgj ok)
 *
 * Returns 1 when bit0 is set, else 0. Wave-tagged distinct surface for
 * MILESTONE 3250. Does not parse soname strings and does not call
 * libc. No parent wires.
 */
uint32_t
gj_libcgj_ok_3250(uint32_t uFlags)
{
	(void)NULL;
	return b3246_ok_u(uFlags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libcgj_ok_3250(uint32_t uFlags)
    __attribute__((alias("gj_libcgj_ok_3250")));
