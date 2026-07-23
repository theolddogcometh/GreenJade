/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6845: close_range known-flags mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cr_known_mask_6845(void);
 *     - Returns CLOSE_RANGE_UNSHARE|CLOSE_RANGE_CLOEXEC (0x6). Soft
 *       catalog of currently documented close_range flag bits.
 *   uint32_t __gj_cr_known_mask_6845  (alias)
 *   __libcgj_batch6845_marker = "libcgj-batch6845"
 *
 * Exclusive continuum CREATE-ONLY (6841-6850: close_range flags stubs —
 * unshare, cloexec, has_unshare, has_cloexec, known_mask, has_any,
 * flags_ok, plain, both, batch_id_6850). Unique gj_cr_known_mask_6845
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6845_marker[] = "libcgj-batch6845";

/* Documented close_range(2) flag bits (uapi as of CLOSE_RANGE_CLOEXEC). */
#define B6845_CLOSE_RANGE_UNSHARE ((uint32_t)0x00000002u)
#define B6845_CLOSE_RANGE_CLOEXEC ((uint32_t)0x00000004u)
#define B6845_KNOWN_MASK \
	(B6845_CLOSE_RANGE_UNSHARE | B6845_CLOSE_RANGE_CLOEXEC)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6845_known_mask(void)
{
	return B6845_KNOWN_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cr_known_mask_6845 - soft mask of documented close_range flags.
 *
 * Always returns 6 (UNSHARE|CLOEXEC). Pure catalog; no parent wires.
 */
uint32_t
gj_cr_known_mask_6845(void)
{
	(void)NULL;
	return b6845_known_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cr_known_mask_6845(void)
    __attribute__((alias("gj_cr_known_mask_6845")));
