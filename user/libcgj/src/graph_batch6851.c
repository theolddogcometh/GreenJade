/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6851: openat2 RESOLVE_NO_XDEV resolve flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_oat2_resolve_no_xdev_6851(void);
 *     - Returns 0x00000001 (soft RESOLVE_NO_XDEV resolve flag bit).
 *       Pure-data catalog stub; not a live openat2 syscall.
 *   uint32_t __gj_oat2_resolve_no_xdev_6851  (alias)
 *   __libcgj_batch6851_marker = "libcgj-batch6851"
 *
 * Exclusive continuum CREATE-ONLY (6851-6860: openat2 resolve flags
 * stubs —
 * resolve_no_xdev_6851, resolve_no_magiclinks_6852,
 * resolve_no_symlinks_6853, resolve_beneath_6854, resolve_in_root_6855,
 * resolve_cached_6856, resolve_known_mask_6857, resolve_bit_count_6858,
 * resolve_flag_ok_6859, continuum + batch_id_6860). Unique surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6851_marker[] = "libcgj-batch6851";

/* Soft RESOLVE_NO_XDEV (block mount-point crossings during path resolve). */
#define B6851_RESOLVE_NO_XDEV  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6851_no_xdev(void)
{
	return B6851_RESOLVE_NO_XDEV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_oat2_resolve_no_xdev_6851 - soft RESOLVE_NO_XDEV resolve flag bit.
 *
 * Always returns 0x00000001. Pure-data stub for product openat2 resolve
 * flag catalogs; does not open paths or call openat2. No parent wires.
 */
uint32_t
gj_oat2_resolve_no_xdev_6851(void)
{
	(void)NULL;
	return b6851_no_xdev();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_oat2_resolve_no_xdev_6851(void)
    __attribute__((alias("gj_oat2_resolve_no_xdev_6851")));
