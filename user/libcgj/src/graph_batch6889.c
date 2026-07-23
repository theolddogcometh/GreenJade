/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6889: statx mask STATX_ALL soft baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mask_all_6889(void);
 *     - Returns soft STATX_ALL baseline (0x00000FFF):
 *       BASIC_STATS | BTIME.
 *   uint32_t gj_statx_mask_all_ok_6889(uint32_t mask);
 *     - Return 1 if mask covers the STATX_ALL baseline (has-all).
 *   uint32_t __gj_statx_mask_all_6889  (alias)
 *   uint32_t __gj_statx_mask_all_ok_6889  (alias)
 *   __libcgj_batch6889_marker = "libcgj-batch6889"
 *
 * Exclusive continuum CREATE-ONLY (6881-6890: statx mask stubs).
 * Unique gj_statx_mask_*_6889 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6889_marker[] = "libcgj-batch6889";

/*
 * Soft STATX_ALL baseline (common userspace "everything basic + btime"):
 *   TYPE|MODE|NLINK|UID|GID|ATIME|MTIME|CTIME|INO|SIZE|BLOCKS|BTIME
 * Compile-time tag only; not a kernel probe. Soft value 0x00000FFF.
 * (Note: STATX_MNT_ID is outside STATX_ALL in this subset header.)
 */
#define B6889_STATX_ALL  0x00000FFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6889_all(void)
{
	return B6889_STATX_ALL;
}

static uint32_t
b6889_all_ok(uint32_t u32Mask)
{
	uint32_t u32Need = B6889_STATX_ALL;

	return ((u32Mask & u32Need) == u32Need) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mask_all_6889 - soft STATX_ALL baseline mask.
 *
 * Always returns 0x00000FFF. Soft pure-data product tag; does not call
 * the statx syscall. No parent wires.
 */
uint32_t
gj_statx_mask_all_6889(void)
{
	(void)NULL;
	return b6889_all();
}

/*
 * gj_statx_mask_all_ok_6889 - mask covers STATX_ALL baseline bits.
 *
 * mask: soft statx bitmask (e.g. stx_mask after a filled call)
 *
 * Returns 1 if all baseline bits present; else 0.
 */
uint32_t
gj_statx_mask_all_ok_6889(uint32_t u32Mask)
{
	return b6889_all_ok(u32Mask);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_statx_mask_all_6889(void)
    __attribute__((alias("gj_statx_mask_all_6889")));

uint32_t __gj_statx_mask_all_ok_6889(uint32_t u32Mask)
    __attribute__((alias("gj_statx_mask_all_ok_6889")));
