/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7453: linkat FOLLOW|EMPTY composite mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_linkat_both_mask_7453(void);
 *     - Returns soft AT_SYMLINK_FOLLOW | AT_EMPTY_PATH mask (0x1400).
 *   uint32_t __gj_linkat_both_mask_7453  (alias)
 *   __libcgj_batch7453_marker = "libcgj-batch7453"
 *
 * Exclusive continuum CREATE-ONLY (7451-7460: linkat flags stubs —
 * symlink_follow, empty_path, both_mask, known_flags, known_flags_ok,
 * has_known, both_u, mask_u, any_u, continuum + batch_id_7460).
 * Unique gj_linkat_both_mask_7453 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7453_marker[] = "libcgj-batch7453";

/*
 * Soft linkat both-flags composite (compile-time tags only):
 *   AT_SYMLINK_FOLLOW (0x400)
 *   AT_EMPTY_PATH     (0x1000)
 * Soft value 0x1400. Not a kernel probe.
 */
#define B7453_AT_SYMLINK_FOLLOW ((uint32_t)0x00000400u)
#define B7453_AT_EMPTY_PATH     ((uint32_t)0x00001000u)
#define B7453_LINKAT_BOTH_MASK \
	(B7453_AT_SYMLINK_FOLLOW | B7453_AT_EMPTY_PATH)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7453_both_mask(void)
{
	return B7453_LINKAT_BOTH_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_linkat_both_mask_7453 - soft FOLLOW|EMPTY composite mask.
 *
 * Always returns 0x1400. Soft pure-data product tag; does not call
 * linkat. No parent wires.
 */
uint32_t
gj_linkat_both_mask_7453(void)
{
	(void)NULL;
	return b7453_both_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_linkat_both_mask_7453(void)
    __attribute__((alias("gj_linkat_both_mask_7453")));
